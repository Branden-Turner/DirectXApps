#include "precompiled.h"

namespace cs350
{

  LRESULT CALLBACK
    MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
  {
    static DXAPP* app = 0;

    switch( msg )
    {
    case WM_CREATE:
      {
        // Get the 'this' pointer we passed to CreateWindow via the lpParam parameter.
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        app = (DXAPP*)cs->lpCreateParams;
        return 0;
      }
    }

    // Don't start processing messages until after WM_CREATE.
    if( app )
      return app->msgProc(msg, wParam, lParam);
    else
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }

  DXAPP::DXAPP( HINSTANCE inst ) : camTarget(0.0f, 0.0f, -1.0f), camUp(0.0f,1.0f,0.0f), 
                                   camRight(1.0f, 0.0f, 0.0f), camPos(0.0f,0.0f,0.0f)
  {
    mainInst   = inst;
    mainWnd    = 0;
    paused     = false;
    minimized  = false;
    maximized  = false;
    resizing   = false;
    lMouseDown = false;
    drawNorms  = false;

    frameStats = L"";

    device              = 0;
    pSwapChain          = 0;
    depthStencilBuffer  = 0;
    pRenderTargetView   = 0;
    depthStencilView    = 0;
    depthStencilBuffer  = 0;
    font                = 0;
    vertexLayout        = 0;
    tech                = 0;
    effect              = 0;

    driverType      = D3D10_DRIVER_TYPE_HARDWARE;
    clearColor      = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
    winWidth        = 800;
    winHeight       = 600;
    wired           = false;
  }

  DXAPP::~DXAPP()
  {
    delete shapeMgr;
    ReleaseCOM(pRenderTargetView);
    ReleaseCOM(depthStencilView);
    ReleaseCOM(pSwapChain);
    ReleaseCOM(depthStencilBuffer);
    ReleaseCOM(device);
    ReleaseCOM(font);
    ReleaseCOM(effect)
    ReleaseCOM(lineEffect)
    ReleaseCOM(vertexLayout);   
  }

  void DXAPP::initApp()
  {
    initWindow();
    initDevice();
    debugDraw = new DebugDraw(device);
    buildEffects();
    buildVertLayouts();

    D3DX10_FONT_DESC fontDesc;
    fontDesc.Height          = 24;
    fontDesc.Width           = 0;
    fontDesc.Weight          = 0;
    fontDesc.MipLevels       = 1;
    fontDesc.Italic          = false;
    fontDesc.CharSet         = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality         = DEFAULT_QUALITY;
    fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
    wcscpy(fontDesc.FaceName, L"Times New Roman");

    D3DX10CreateFontIndirect(device, &fontDesc, &font);

    D3D10_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D10_RASTERIZER_DESC));
    rastDesc.CullMode = D3D10_CULL_NONE;
    rastDesc.FillMode = D3D10_FILL_WIREFRAME;
    rastDesc.FrontCounterClockwise = false;

    HR(device->CreateRasterizerState(&rastDesc, &wireFrame));

    rastDesc.CullMode = D3D10_CULL_BACK;
    rastDesc.FillMode = D3D10_FILL_SOLID;
    HR(device->CreateRasterizerState(&rastDesc, &solidFill));

    //Set up projection mtx
    aRat_ = (float)winWidth / (float) winHeight;
    fov_  = 0.65f;
    nP_   = 1.0f;
    fP_   = 5000.0f;
    
    geomLib = new GeometryLibrary(debugDraw, device);
    shapeMgr = new MeshManager(device, mtwMtx, geomLib);

    geomLib->buildDBVH();
    geomLib->getDBVH()->setTech(tech);
  }

  void DXAPP::update(float dt)
  {
    updateFPS();
    updateKeyStates(dt);
    updatePerFrameVariables();
  }

  void DXAPP::prepDrawTris()
  {
    //Restore default states
    device->OMSetDepthStencilState(0, 0);
    float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
    device->OMSetBlendState(0, blendFactors, 0xffffffff);
    device->IASetInputLayout(vertexLayout);
    device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Draw em wired or solid depending on what the user wants
    if(wired)
      device->RSSetState(wireFrame);
    else 
      device->RSSetState(solidFill);
  }

  void DXAPP::drawTrisFromKD()
  {
    prepDrawTris();

    D3D10_TECHNIQUE_DESC techDesc;
    triTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
      triTech->GetPassByIndex( p )->Apply(0);
      geomLib->getKDTree()->drawTris(frustrum);
    }
  }

  void DXAPP::drawDBVH()
  {
    prepDrawTris();
    geomLib->getDBVH()->draw();
  }

  void DXAPP::drawShapes()
  {
    prepDrawTris();

    //Grab the technique and do some passes.  Kill this with fire when you get time.
    //Screw the effects framework!
    D3D10_TECHNIQUE_DESC techDesc;
    tech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
      unsigned shapeNum = shapeMgr->numMeshes();
      for(unsigned i = 0; i < shapeNum; ++i)
      {
        shapeMgr->setMtw(i);
        tech->GetPassByIndex( p )->Apply(0);
        shapeMgr->drawMesh(i);
      }
    }
  }

  void DXAPP::drawDebugInfo()
  {
    //Draw any other special things we need (KD tree stuff, normals if necessary, etc)
    device->RSSetState(solidFill);
    
    debugDraw->prepDraw();

    D3D10_TECHNIQUE_DESC techDesc;
    lineTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
      lineTech->GetPassByIndex( p )->Apply(0);
      debugDraw->draw(); 
    }

    //Draw the text of the scene last as its pixels overwrite whatever is there.
    RECT R = {5, 5, 0, 0};
    font->DrawText(0, frameStats.c_str(), -1, &R, DT_NOCLIP, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
  }


  bool Frustrum::checkInside( AABB& box )
  {
    //Generate 8 points to test against the frustrum
    Pt points[8];

    //P1 is min, 
    points[0] = box.center - box.extents;

    //up to p2, 
    points[1] = points[0] + vec3(0, (box.extents.y * 2), 0);

    //then right to p3, 
    points[2] = points[1] + vec3((box.extents.x * 2), 0, 0);

    //then down to p4
    points[3] = points[2] - vec3(0, (box.extents.y * 2), 0);

    //back to p5
    points[4] = points[3] + vec3(0, 0, (box.extents.z * 2));

    //left to p6
    points[5] = points[4] - vec3((box.extents.x * 2), 0, 0);

    //up to p7
    points[6] = points[5] + vec3(0, (box.extents.y * 2), 0);

    //then right to p8
    points[7] = points[6] + vec3((box.extents.x * 2), 0, 0);

    //loop through all planes and check all points of this triangle against it.
    for(unsigned i = 0; i < 6; ++i)
    {
      //If all three points are outside any plane, cull the box
      if(((f[i].eqn.PlaneEqn(points[0])) >= 0) &&
        ( (f[i].eqn.PlaneEqn(points[1])) >= 0) &&
        ( (f[i].eqn.PlaneEqn(points[2])) >= 0) &&
        ( (f[i].eqn.PlaneEqn(points[3])) >= 0) &&
        ( (f[i].eqn.PlaneEqn(points[4])) >= 0) &&
        ( (f[i].eqn.PlaneEqn(points[5])) >= 0) &&
        ( (f[i].eqn.PlaneEqn(points[6])) >= 0) &&
        ( (f[i].eqn.PlaneEqn(points[7])) >= 0)   )
        return false;
    }

    //All 8 points are inside of all 6 planes
    return true;
  }


  void DXAPP::drawCoordSystem()
  {
    static vec3 o = vec3(0,0,0);
    static vec3 x = vec3(1,0,0);
    static vec3 y = vec3(0,1,0);
    static vec3 z = vec3(0,0,1);

    debugDraw->addLine(o, x * 500, vec4(1,0,0,1));
    debugDraw->addLine(o, y * 500, vec4(0,1,0,1));
    debugDraw->addLine(o, z * 500, vec4(0,0,1,1));
  }

  void DXAPP::draw()
  {
    //Clear depth and stencil buffer
    device->ClearRenderTargetView( pRenderTargetView, clearColor );
    device->ClearDepthStencilView( depthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);    

    //Draw all the shapes of the scene
    drawDBVH();

    portal = geomLib->initialPortal(geomLib->getDBVH()->selectedObj(), geomLib->getDBVH()->otherObj());
    geomLib->discovery(portal, geomLib->getDBVH()->selectedObj(), geomLib->getDBVH()->otherObj());
    geomLib->refinement(portal, geomLib->getDBVH()->selectedObj(), geomLib->getDBVH()->otherObj());
    debugDraw->drawPortal(portal);

    //Draw the x y and z directions
    drawCoordSystem();
 
    //Draw everything else we want to know about
    drawDebugInfo();

    pSwapChain->Present( 0, 0 );
  }

  void DXAPP::updatePerFrameVariables(  )
  {
    //Rebuild projection matrix in case we have a new height/width
    aRat_ = (float)winWidth / (float)winHeight;

    D3DXMatrixLookAtLH(&mView, &camPos, &camTarget, &camUp);
    D3DXMatrixPerspectiveFovLH(&mProj, fov_, aRat_, nP_, fP_);

    mWVP = mView * mProj;

    //Set the matrices up
    camProjMat->SetMatrix((float*)&mWVP);
    camMat->SetMatrix((float*)&mView);

    //Debug uses the same projection matrix
    debugDraw->camProj->SetMatrix((float*)&mWVP);
    triCam->SetMatrix((float*)&mView);
    triCamProj->SetMatrix((float*)&mWVP);
  }

  void DXAPP::updateFPS()
  {
    // Code computes the average frames per second, and also the 
    // average time it takes to render one frame.

    static int frameCnt = 0;
    static float t_base = 0.0f;

    frameCnt++;

    // Compute averages over one second period.
    if( (timer.getGameTime() - t_base) >= 1.0f )
    {
      float fps = (float)frameCnt; // fps = frameCnt / 1
      float mspf = 1000.0f / fps;

      std::wostringstream outs;   
      outs.precision(6);
      outs << L"FPS: " << fps << L"\n" 
        << "Milliseconds: Per Frame: " << mspf;
      frameStats = outs.str();

      // Reset for next average.
      frameCnt = 0;
      t_base  += 1.0f;
    }
  }


  const static float CAMSPEED = 150.0f;
  void DXAPP::updateKeyStates(float dt)
  {
    static float mPhi = PI * 0.25f;
    static float mTheta = 0.0f;

    if(lMouseDown)
    {
      mTheta += mDx * dt * 0.5f;
      mPhi   -= mDy * dt * 0.5f;
    }

    if( mPhi < 0.1f )	mPhi = 0.1f;
    if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

    float x =  5.0f * sinf(mPhi) * sinf(mTheta);
    float z = -5.0f * sinf(mPhi) * cosf(mTheta);
    float y =  5.0f * cosf(mPhi);

    // Build the view matrix.
    D3DXVECTOR3 pos(x, y, z);
    camTarget = camPos + pos;

    D3DXVECTOR3 dir = (camTarget - camPos);
    D3DXVec3Normalize(&dir,&dir);
    D3DXVec3Cross( &camRight, &dir, &camUp );
    
    // Update angles based on input to orbit camera around box.
    if(GetAsyncKeyState('W') & 0x8000)	camPos += dir * dt * CAMSPEED;
    if(GetAsyncKeyState('S') & 0x8000)  camPos -= dir * dt * CAMSPEED;   

    if(GetAsyncKeyState('F') & 0x8000)	wired = !wired;
    if(GetAsyncKeyState('N') & 0x8000)  drawNorms = !drawNorms;

    if(GetAsyncKeyState('V') & 0x8000) geomLib->getDBVH()->toggleDebug();
  }

//Init Stuff///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
  LRESULT DXAPP::msgProc( UINT msg, WPARAM wParam, LPARAM lParam )
  {

    static float x = 0.0f;
    static float y = 0.0f;
    static float moveSpeed = 150.0f;
    unsigned depth = 0; 

    switch( msg )
    {
      // WM_ACTIVATE is sent when the window is activated or deactivated.  
      // We pause the game when the window is deactivated and unpause it 
      // when it becomes active.  
    case WM_ACTIVATE:
      if( LOWORD(wParam) == WA_INACTIVE )
      {
        paused = true;
        timer.stop();
      }
      else
      {
        paused = false;
        timer.start();
      }
      return 0;

      // WM_SIZE is sent when the user resizes the window.  
    case WM_SIZE:
      // Save the new client area dimensions.
      winWidth  = LOWORD(lParam);
      winHeight = HIWORD(lParam);
      if( device )
      {
        if( wParam == SIZE_MINIMIZED )
        {
          paused = true;
          minimized = true;
          maximized = false;
        }
        else if( wParam == SIZE_MAXIMIZED )
        {
          paused = false;
          minimized = false;
          maximized = true;
          onResize();
        }
        else if( wParam == SIZE_RESTORED )
        {

          // Restoring from minimized state?
          if( minimized )
          {
            paused = false;
            minimized = false;
            onResize();
          }

          // Restoring from maximized state?
          else if( maximized )
          {
            paused = false;
            maximized = false;
            onResize();
          }
          else if( resizing )
          {
            // If user is dragging the resize bars, we do not resize 
            // the buffers here because as the user continuously 
            // drags the resize bars, a stream of WM_SIZE messages are
            // sent to the window, and it would be pointless (and slow)
            // to resize for each WM_SIZE message received from dragging
            // the resize bars.  So instead, we reset after the user is 
            // done resizing the window and releases the resize bars, which 
            // sends a WM_EXITSIZEMOVE message.
          }
          else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
          {
            onResize();
          }
        }
      }
      return 0;

    case WM_KEYDOWN:
      {
        if(wParam == VK_ESCAPE)
          DestroyWindow(mainWnd);

        if(geomLib->getDBVH())
        {
          if(wParam == VK_LEFT)
          {
            geomLib->getDBVH()->selectedObj().moveX( moveSpeed );
            //geomLib->getDBVH()->selectedObjectMoved();
          }

          if(wParam == VK_RIGHT)
          {
            geomLib->getDBVH()->selectedObj().moveX( -moveSpeed );
            //geomLib->getDBVH()->selectedObjectMoved();
          }

          if(wParam == VK_UP)
          {
            geomLib->getDBVH()->selectedObj().moveZ( moveSpeed );
            //geomLib->getDBVH()->selectedObjectMoved();
          }


          if(wParam == VK_DOWN)
          {
            geomLib->getDBVH()->selectedObj().moveZ( -moveSpeed );
            //geomLib->getDBVH()->selectedObjectMoved();
          }

          if(wParam == VK_SHIFT)
            geomLib->getDBVH()->selectedObj().moveY( -moveSpeed );
          if(wParam == VK_RETURN)
            geomLib->getDBVH()->selectedObj().moveY( moveSpeed );
        }
        return 0;

      }
      
    case WM_LBUTTONDOWN:
      lMouseDown = true;
      return 0;

    case WM_LBUTTONUP:
      lMouseDown = false;
      return 0;

    case WM_RBUTTONDOWN:
      if(geomLib->getKDTree())
      {
        depth = geomLib->getKDTree()->getMaxDrawDepth();
        geomLib->getKDTree()->setMaxDrawDepth(depth + 1);
      }
      if(geomLib->getDBVH())
      {
        geomLib->getDBVH()->changeSelectedObject();
      }
      return 0;

    case WM_MOUSEMOVE:
      mDx = x - GET_X_LPARAM(lParam); 
      mDy = y - GET_Y_LPARAM(lParam); 
      x   = (float)GET_X_LPARAM(lParam);
      y   = (float)GET_Y_LPARAM(lParam);
      return 0;

      // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
    case WM_ENTERSIZEMOVE:
      paused = true;
      resizing  = true;
      timer.stop();
      return 0;

      // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
      // Here we reset everything based on the new window dimensions.
    case WM_EXITSIZEMOVE:
      paused = false;
      resizing  = false;
      timer.start();
      onResize();
      return 0;

      // WM_DESTROY is sent when the window is being destroyed.
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;

      // The WM_MENUCHAR message is sent when a menu is active and the user presses 
      // a key that does not correspond to any mnemonic or accelerator key. 
    case WM_MENUCHAR:
      // Don't beep when we alt-enter.
      return MAKELRESULT(0, MNC_CLOSE);

      // Catch this message so to prevent the window from becoming too small.
    case WM_GETMINMAXINFO:
      ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
      ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
      return 0;
    }

    return DefWindowProc(mainWnd, msg, wParam, lParam);
  }


  void DXAPP::buildVertLayouts()
  {
    // Create the vertex input layout.
    static D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
    {
      {"POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(Vertex_Pos_Clr_Nrm_Tex, pos), D3D10_INPUT_PER_VERTEX_DATA, 0},
      {"COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex_Pos_Clr_Nrm_Tex, clr), D3D10_INPUT_PER_VERTEX_DATA, 0},
      {"NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(Vertex_Pos_Clr_Nrm_Tex, nrm), D3D10_INPUT_PER_VERTEX_DATA, 0},
      {"TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, offsetof(Vertex_Pos_Clr_Nrm_Tex, tex), D3D10_INPUT_PER_VERTEX_DATA, 0}
    };

    // Create the input layout
    D3D10_PASS_DESC PassDesc;
    HR(tech->GetPassByIndex(0)->GetDesc(&PassDesc));
    HR(device->CreateInputLayout(vertexDesc, 
                                 4, 
                                 PassDesc.pIAInputSignature, 
                                 PassDesc.IAInputSignatureSize,
                                 &vertexLayout));
  }

  static void CheckCompErrors(HRESULT h, ID3D10Blob* compError)
  {
    if(FAILED(h))
    {
      if( compError )
      {
        MessageBoxA(0, (char*)compError->GetBufferPointer(), 0, 0);
        ReleaseCOM(compError);
      }
      DXTrace(__FILE__, (DWORD)__LINE__, h, L"D3DX10CreateEffectFromFile", true);
    } 
  }

  void DXAPP::buildEffects()
  {
    DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#ifdef _DEBUG
    shaderFlags |= D3D10_SHADER_DEBUG;
    shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

    ID3D10Blob* compilationErrors = 0;
    HRESULT hr = 0;
    hr = D3DX10CreateEffectFromFile(L"source/basicLighting.fx", 0, 0, 
      "fx_4_0", shaderFlags, 0, device, 0, 0, &effect, &compilationErrors, 0);
    CheckCompErrors(hr, compilationErrors);

    hr = D3DX10CreateEffectFromFile(L"source/debugDraw.fx", 0, 0, 
      "fx_4_0", shaderFlags, 0, device, 0, 0, &lineEffect, &compilationErrors, 0);
    CheckCompErrors(hr, compilationErrors);

    hr = D3DX10CreateEffectFromFile(L"source/worldTris.fx", 0, 0, 
      "fx_4_0", shaderFlags, 0, device, 0, 0, &triEffect, &compilationErrors, 0);
    CheckCompErrors(hr, compilationErrors);

    tech = effect->GetTechniqueByName("LightTech");
    lineTech = lineEffect->GetTechniqueByName("LineTech");
    triTech = triEffect->GetTechniqueByName("TriTech");

    camMat             = effect->GetVariableByName("Camera")->AsMatrix();
    mtwMtx             = effect->GetVariableByName("ModelToWorld")->AsMatrix();
    camProjMat         = effect->GetVariableByName("CameraProjection")->AsMatrix();
    debugDraw->camProj = lineEffect->GetVariableByName("CameraProjection")->AsMatrix();
    triCam             = triEffect->GetVariableByName("Camera")->AsMatrix();
    triCamProj         = triEffect->GetVariableByName("CameraProjection")->AsMatrix();
  }

  void DXAPP::onResize()
  {
    //Release the buffers to create new ones of the proper size
    ReleaseCOM(pRenderTargetView);
    ReleaseCOM(depthStencilView);
    ReleaseCOM(depthStencilBuffer);

    //Create newly resized render target view.
    HR(pSwapChain->ResizeBuffers(1, winWidth, winHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
    ID3D10Texture2D* backBuffer;
    HR(pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<void**>(&backBuffer)));
    HR(device->CreateRenderTargetView(backBuffer, 0, &pRenderTargetView));
    ReleaseCOM(backBuffer);

    //Create the depth/stencil target view
    D3D10_TEXTURE2D_DESC stencilDesc;
    stencilDesc.Width = winWidth;
    stencilDesc.Height = winHeight;
    stencilDesc.MipLevels = 1;
    stencilDesc.ArraySize = 1;
    stencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    stencilDesc.SampleDesc.Count = 1;
    stencilDesc.SampleDesc.Quality = 0;
    stencilDesc.Usage = D3D10_USAGE_DEFAULT;
    stencilDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
    stencilDesc.CPUAccessFlags = 0;
    stencilDesc.MiscFlags = 0;

    HR(device->CreateTexture2D(&stencilDesc, 0, &depthStencilBuffer));
    HR(device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView));

    device->OMSetRenderTargets( 1, &pRenderTargetView, depthStencilView );

    // Setup the viewport
    D3D10_VIEWPORT vp;
    vp.Width = winWidth;
    vp.Height = winHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    device->RSSetViewports( 1, &vp );
  }

  HINSTANCE DXAPP::getHinst() const
  {
    return mainInst;
  }

  HWND DXAPP::getWnd() const
  {
    return mainWnd;
  }

  int DXAPP::run()
  {
    timer.reset();

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
      if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
      {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
      }
      else
      {
        timer.tick();

        if( !paused )
          update(timer.getTimeElapsed());	
        else
          Sleep(50);

        draw();
      }
    }

    return ( int )msg.wParam;
  }

  void DXAPP::initDevice()
  {
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

    //Create the swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = winWidth;
    sd.BufferDesc.Height = winHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    //Refresh at 60hz
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;

    //This will be where we draw a rendertarget
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount  = 1;
    sd.OutputWindow = mainWnd;

    //Multi-sampling (Default to none)
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    //Allow hardware to specify
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    HR(D3D10CreateDeviceAndSwapChain( NULL, 
      driverType,
      NULL, 
      createDeviceFlags,
      D3D10_SDK_VERSION, 
      &sd, &pSwapChain, &device ));

    onResize();
  }

  void DXAPP::initWindow()
  {
    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(WNDCLASS));

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = MainWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hIcon         = LoadIcon(0, IDI_APPLICATION); 
    wc.hCursor       = LoadCursor (0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    wc.lpszMenuName  = 0;
    wc.lpszClassName = L"WindowClass";

    if( !RegisterClass(&wc) )
    {
      MessageBox(0, L"RegisterClass FAILED", 0, 0);
      PostQuitMessage(0);
    }

    winWidth = 800;
    winHeight = 600;
    RECT rc = { 0, 0, winWidth, winHeight };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, false );
    mainWnd = CreateWindow( 
      L"WindowClass", 
      L"DX10 Rendering Framework", 
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 
      CW_USEDEFAULT, 
      rc.right - rc.left, 
      rc.bottom - rc.top, 
      NULL, 
      NULL, 
      mainInst,
      this );
    if( !mainWnd )
    {
      MessageBox(0, L"CreateWindow FAILED", 0, 0);
      PostQuitMessage(0);
    }

    ShowWindow( mainWnd, SW_SHOW );
    UpdateWindow( mainWnd );
  }
}
