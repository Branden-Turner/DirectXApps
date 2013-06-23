/*! ====================================================================================================================
  @file graphics.cpp

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @author Branden Turner <branden.turner@digipen.edu>
  @date Jun.03.2011

  @brief 
    Cool graphics engine goes here.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "syswindow.h"
#include "graphics.h"
#include "dx9device.h"
#include "dx9buffers.h"
#include "dx9debugdraw.h"
#include "dx9vertextype.h"
#include "dx9rendertargets.h"
#include "dx9resourcemanager.h"
#include "camera.h"
#include "uisystem.h"
#include "devconsole.h"
#include "dx9shapes.h"
#include "factory.h"
#include "comptransform.h"
#include "compbody.h"
#include "anim2d.h"
#include "gfxparticlemanager.h"

namespace Grimelins {


  Graphics* GRAPHICS;

  SCRIPT_ClassStart(Graphics);
  SCRIPT_CTOR_Disabled();
  SCRIPT_Method(hasResolution);
  SCRIPT_Method(setResolution);
  SCRIPT_Method(Roll);
  SCRIPT_Method(Roll);
  SCRIPT_Method(Pitch);
  SCRIPT_Method(Yaw);
  SCRIPT_Method(getSceneCamera);
  SCRIPT_Method(getBoneWorldPos);
  SCRIPT_Method(drawLine);
  SCRIPT_Method(getDT);
  SCRIPT_Method(explosion);
  SCRIPT_Method(shakeCam);
  SCRIPT_Method(dust);
  SCRIPT_ClassEnd();

  ANT__RETVAL ant_toggle_cam(void* ud)
  {
    GRAPHICS->ToggleCamMode();
  }

  u32 Graphics::m3dQuadID = 0;
  static s32 LIGHT_MAX = 10;

Graphics::Graphics(void)
  : mIDName(H("Graphics")),
    mUISystem(NULL),
    mCamera(NULL),
    mDebugDraw(NULL),
    mDevice(NULL),
    mRM(NULL),
    mBackBuffer(NULL)
{
}

Graphics::~Graphics(void)
{ 
}

ANT__CVAR_DECL();
cvar cvDebugDraw("debugdraw", "Toggle Debug Drawing", true);
cvar cvWireframe("wireframe", "Toggle wireframe drawing", false);
cvar cvFOV("fov", "Setting FOV in Radians (0.5-1.2 are reasonable values)", 0.75f, 0.5f, 1.2f);

void Graphics::vLoadSystem(void)
{
  gCore->Events.Subscribe( H("GFX_TOGGLE_FULLSCREEN"), this );
  gCore->Events.Subscribe( H("GFX_DEBUG_LINE"), this );
  gCore->Events.Subscribe( H("GFX_SHAPE"), this );
  gCore->Events.Subscribe( H("LoadReady"), this );
  gCore->Events.Subscribe( H("cam_zoom"), this );
  gCore->Events.Subscribe( H("cam_rot"), this );
  gCore->Events.Subscribe( H("cam_pan"), this );
  gCore->Events.Subscribe( H("gfx_addparticle"), this );
  gCore->Events.Subscribe( H("Vibration"), this );//sent from input whenever the controller vibration changes
  
  loadReadyHack = -1;
  InitializeSystem( (s32)Window::mWinHandle() );
  ANT__LISTEN(this);
}

void Graphics::CreateRenderTargetTexture( DX9RenderTarget*& target, IDirect3DTexture9*& texture, u32 dimX, u32 dimY, u32 dimZ, D3DFORMAT fmt )
{
  DXSurface* ppTar;
  HR(mDevice->GetDevice()->CreateTexture(dimX, dimY, dimZ, D3DUSAGE_RENDERTARGET, fmt, D3DPOOL_DEFAULT, &texture, NULL));
  texture->GetSurfaceLevel(0, &ppTar);
  target = new DX9RenderTarget(ppTar, mDevice->mBBFlags, mDevice->mBBColor, mDevice->mBBStencil, mDevice->mBBDepth);
}


void Graphics::GetRenderTargets()
{
  //Get the shadow map texture/render target

  //Grab screen dimensions for the proper targets
//   vec2 dim = mDevice->GetScreenDim();
// 
//   
// 
//   CreateRenderTargetTexture( postProcessTar, postProcessTex, (u32)dim.x, (u32)dim.y, 1, D3DFMT_A32B32G32R32F);
//   CreateRenderTargetTexture( glowBuffer, glowBufferTex, (u32)dim.x, (u32)dim.y, 1, D3DFMT_A32B32G32R32F);

//   DXSurface* bBuffer;
//   mDevice->GetDevice()->GetRenderTarget(0, &bBuffer);
//   mBackBuffer = new DX9RenderTarget(bBuffer, mDevice->mBBFlags, mDevice->mBBColor, mDevice->mBBStencil, mDevice->mBBDepth);
}


void Graphics::InitializeSystem( s32 winHandle )
{
  s32 winWidth  = CONFIG_GFX_DEFAULT_WINDOW_WIDTH;
  s32 winHeight = CONFIG_GFX_DEFAULT_WINDOW_HEIGHT;

  mDevice = new DX9Device();
  mDevice->CreateDevice( (Application::OSWINDOW)winHandle, winWidth, winHeight );

  mCamera     = new Camera();
  mDebugDraw  = new DX9DebugDraw(mDevice);
  mUISystem   = new UISystem(mDevice);
  mDevConsole = new DevConsole(this);
  mRM         = mDevice->GetRM();

  //Default scene ambience
  sceneAmbience = vec4(0.2f, 0.2f, 0.2f, 1.0f);
  
  f32 aspectRatio = SCAST(f32, winWidth) / SCAST(f32, winHeight);
  f32 fov = 0.95f;
  f32 nearPlane = 10.0f;
  f32 farPlane = 50000.0f;

  BuildProjection(aspectRatio, fov, nearPlane, farPlane);
  UpdateMatrices();
  
  mModelMgr = new ModelManager(mDevice);

  ErrorIf(GRAPHICS, "multiple instances of graphics");
  GRAPHICS = this;

  ANT__Initialize(mDevice->GetDevice());
  ANT__WindowSize(1024, 576);

  SCRIPT_RegisterGlobal(Graphics);

  if(CONFIG_GFX_START_FULLSCREEN)
    mDevice->ToggleFullScreen();

  skyps        = mDevice->GetRM()->GetPixelShader("dx9skybox.ps");
  skyvs        = mDevice->GetRM()->GetVertexShader("dx9skybox.vs");
  skybox       = mDevice->GetShapes()->GetShape(ShapeSkybox);
  skytexture   = mDevice->GetRM()->GetCubeTextureFromFile(CONFIG_PATH_TEXTURE "skybox_sunset.dds");
  shapeps      = mDevice->GetRM()->GetPixelShader("dx9billboardedquad.ps");
  shapevs      = mDevice->GetRM()->GetVertexShader("dx9billboardedquad.vs");
  shapetexBlue = mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "blue.jpg");
  shapetexRed  = mDevice->GetRM()->GetTextureFromFile(CONFIG_PATH_TEXTURE "red.jpg");
  particleps   = mDevice->GetRM()->GetPixelShader("dx9particle.ps");
  particlevs   = mDevice->GetRM()->GetVertexShader("dx9particle.vs");
  quadps       = mDevice->GetRM()->GetPixelShader("dx93dquad.ps");
  quadvs       = mDevice->GetRM()->GetVertexShader("dx93dquad.vs");
  mCurResolutionIndex = -1;

  mDevice->GetDeviceResolutions();
  if(mDevice->support1024)
    AddResolutionMode(1024, 768);
  if(mDevice->support1280)
    AddResolutionMode(1280, 720);
  if(mDevice->support1600)
    AddResolutionMode(1600, 900);
  if(mDevice->support1920)
    AddResolutionMode(1920, 1080);

  GetRenderTargets();

  mPManager = new ParticleManager();
  mPManager->Init(mDevice);
  mLights.reserve(LIGHT_MAX);

#ifndef _DEBUG
  ShowCursor(false);
#endif
  

  addLight(Light(vec3(-97,500,275), DIRECTIONAL, vec4(0.24f, 0.0f, 0.52f, 1.0f), vec3(0.0f, -1.0f, 0.0f)));
  addLight(Light(vec3(163,500,275), DIRECTIONAL, vec4(0.22f, 0.18f, 1.0f, 1.0f), vec3(0.4f, -0.4f, -0.2f)));
  addLight(Light(vec3(29,-800,474), DIRECTIONAL, vec4(0.5f, 0.05f, 0.05f, 1.0f), vec3(0.0f, 0.9f, 0.1f)));
  addLight(Light(vec3(320,50,450), POINT, vec4(1.0f, 0.12f, 0.0f, 1.0f) * 1.0f, vec3(0.5f, 0.5f, 0.5f)));
  addLight(Light(vec3(-320,50,450), POINT, vec4(1.0f, 0.12f, 0.0f, 1.0f)* 0.5f, vec3(0.5f, 0.5f, 0.5f)));
  addLight(Light(vec3(-320,50,-500), POINT, vec4(1.0f, 0.12f, 0.0f, 1.0f)* 0.5f, vec3(0.5f, 0.5f, 0.5f)));
  addLight(Light(vec3(320,50,-500), POINT, vec4(1.0f, 0.12f, 0.0f, 1.0f)* 0.5f, vec3(0.5f, 0.5f, 0.5f)));
  addLight(Light(vec3(-50,800,0), POINT, vec4(0.0f, 1.0f, 0.92f, 1.0f), vec3(0.5f, 0.5f, 0.5f)));
}

void Graphics::BuildProjection( f32 aspectRatio, f32 fov, f32 nplane, f32 fplane )
{
  mProjectionMat = Matrix4::PerspectiveFOV( aspectRatio, fov, nplane, fplane );
}

void Graphics::UpdateMatrices( void )
{
  mCameraMat = mCamera->getMatrix();
  vec2 const& dim = mDevice->GetScreenDim();

  BuildProjection( dim.x / dim.y, cvFOV, 10.0f, 50000.0f );

  mCameraProjectionMat = mProjectionMat * mCameraMat;

  mDefaultShader.Camera = mCameraMat.Transposed();
  mDefaultShader.Projection = mProjectionMat.Transposed();
  mDefaultShader.CameraProjection = mCameraProjectionMat.Transposed();
}

void Graphics::DrawSkybox( void )
{
  DXDevice* mDXDevice = mDevice->GetDevice();

  mDXDevice->SetRenderState( D3DRS_ZENABLE, 0 );
  mDXDevice->SetRenderState( D3DRS_ZWRITEENABLE, 0 );
  mDXDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

  mDevice->SetShader(skyvs);
  mDevice->SetShader(skyps);
  mDevice->SetVSData(&mDefaultShader.CameraProjection, 4);
  mDevice->SetTexture(skytexture);

  mDevice->SetBuffer(skybox->mVBuffer, sizeof(Vertex_XYZ) );
  mDevice->SetBuffer(skybox->mIBuffer);
  mDevice->SetDecl(skybox->mDecl);
  mDevice->DrawIndexedTris(skybox->mNumTris, skybox->mNumVerts);
}

void Graphics::BillBoardSpherical(DrawnShape& shape)
{
    vec3 lookAt, objToCamProj, objToCam, upAux;
    float angleCosine;

    // objToCamProj is the vector in world coordinates from the 
    // local origin to the camera projected in the XZ plane
    vec3 lensPos = (mCamera->getPosition() + mCamera->getEye());
    objToCamProj =  lensPos - shape.pos;
    objToCamProj.y = 0;
    objToCamProj.Normalize();

    // This is the original lookAt vector for the object 
    // in world coordinates (start it facing the opposite direction of the camera.)
    lookAt = -mCamera->getDir();

    //Grab the quad's up vector
    upAux = lookAt.Cross(objToCamProj);

    // compute the angle
    angleCosine = lookAt.Dot(objToCamProj);

    //Clamp the angle we get
    angleCosine = std::min(std::max(-0.9999f, angleCosine), 0.9999f);

    //Do cylindrical rotation
    shape.rotation = shape.rotation.RotationAxis(upAux, acos(angleCosine) * 180/3.14f * 0.016f);

    //Rotate up or down
    objToCam = (lensPos - shape.pos);
    objToCam.Normalize();

    angleCosine = objToCamProj.Dot(objToCam);

    angleCosine = std::min(std::max(-0.9999f, angleCosine), 0.9999f);

    if (objToCam.y > 0)
      shape.rotation *= shape.rotation.RotationAxis(vec3(-1,0,0), acos(angleCosine)  * 180/3.14f * 0.016f);
    else
      shape.rotation *= shape.rotation.RotationAxis(vec3(-1,0,0), -acos(angleCosine) * 180/3.14f * 0.016f);
}

void Graphics::BillBoardCylindrical( DrawnShape& shape )
{
  vec3 lookAt, objToCamProj, objToCam, upAux;
  float angleCosine;

  // objToCamProj is the vector in world coordinates from the 
  // local origin to the camera projected in the XZ plane
  vec3 lensPos = (mCamera->getPosition() + mCamera->getEye());
  objToCamProj =  lensPos - shape.pos;
  objToCamProj.y = 0;
  objToCamProj.Normalize();

  // This is the original lookAt vector for the object 
  // in world coordinates (start it facing the opposite direction of the camera.)
  lookAt = -mCamera->getDir();

  //Grab the quad's up vector
  upAux = lookAt.Cross(objToCamProj);

  // compute the angle
  angleCosine = lookAt.Dot(objToCamProj);

  //Clamp the angle we get
  angleCosine = std::min(std::max(-0.9999f, angleCosine), 0.9999f);

  //Do cylindrical rotation
  shape.rotation = shape.rotation.RotationAxis(upAux, acos(angleCosine) * 180/3.14f * 0.016f);
}

void Graphics::RenderScene()
{
  mDevice->BeginScene();

  if(loadReadyHack)
  {
    --loadReadyHack;
    if(loadReadyHack==0)
    {
      gCore->Events.Post(H("LoadUI"), MSG_DEFAULT);
      gCore->Events.Post(H("LoadObjectLibrary"), MSG_DEFAULT);
    }
  }
  else
  {
    DrawSkybox();
    DrawOpaqueModels();
    DrawAlphaModels();
    mPManager->Render(mDevice);
    DrawShapeComponents();
  }
  
  DrawShapes(thisFrameDt);

  //!!!! ANT must be last draw call in the scene!
   ANT__Draw();

  mDevice->EndScene();
}

void Graphics::RenderSceneFromTexture(IDirect3DTexture9* texture)
{
  static DX9PixelShader* quadps = mRM->GetPixelShader("dx9screenquad.ps");
  static DX9VertexShader* quadvs = mRM->GetVertexShader("dx9screenquad.vs");
  static Shape* quad = mDevice->GetShapes()->GetShape(ShapeQuad2D);

  mDevice->BeginScene();

  DXDevice* dev = mDevice->GetDevice();

  //Set up the device state to render to a fullscreen quad
  mUISystem->PrepareDraw();

  //Set up the shaders and vertex declarations
  mDevice->SetShader(quadvs);
  mDevice->SetShader(quadps);

  mDevice->SetBuffer(quad->mIBuffer);
  mDevice->SetBuffer(quad->mVBuffer, sizeof(Vertex_XY_UV) );
  mDevice->SetDecl(quad->mDecl);


  static float2 pos(0.0f, 0.0f);
  static float2 dim(2.0f, 2.0f);

  float4 PSVars(0.0f, 0.0f, 0.0f, 1.0f);

  float4 VSPos( pos.x, pos.y, dim.x, dim.y );

  dev->SetTexture(0, texture);

  mDevice->SetVSData(&VSPos, 1, 0);
  mDevice->SetPSData(&PSVars, 1, 0);

  mDevice->DrawIndexedTris(quad->mNumTris, quad->mNumVerts);


  mDevice->EndScene();
}

void Graphics::RenderUI()
{
  mDevice->BeginScene();
  mUISystem->PrepareDraw();
  mUISystem->Draw();
  mDevice->EndScene();
}


void Graphics::vUpdateSystem(void)
{
  if( mDevice->IsDeviceLost() )
    return;

  UpdateMatrices();

  thisFrameDt = GetSystemDT();

  mCamera->updateDT( thisFrameDt );
  mCamera->update();

  mDevConsole->Update( thisFrameDt );

  if(!gCore->GamePaused)
  {
    UpdateComponents();
    if(mPManager)
      mPManager->Update();
    updateLights();
  }

  //Shadow First Pass or any sort of light prepass goes here


  //Main pass.  Lighting, shadow mapping done here
  //   mDevice->SetRenderTarget(postProcessTar);
  //   mDevice->SetRenderTarget(glowBuffer, 1);
  //   mDevice->ClearRenderTarget();
  
 

  //Post processing steps
  mBackBuffer = mDevice->GetBackBuffer();
  mDevice->SetRenderTarget(mBackBuffer);
  mDevice->ClearRenderTarget();
  RenderScene();

 /* RenderSceneFromTexture(postProcessTex);*/

  //Render the UI normally after postprocessing
  RenderUI();

  //Copy the texture we've manipulated into the back buffer and present it.
  mDevice->Present();
  ClearShapes();
}

void Graphics::vUnloadSystem(void)
{
  ANT__Terminate();

  if(mDevice)
    mDevice->DestroyDevice();

  mPManager->Shutdown();
  SafeDelete(mPManager);

  SafeDelete(mModelMgr);
  SafeDelete(mDevConsole);
  SafeDelete(mUISystem);
  SafeDelete(mDebugDraw);
  SafeDelete(mCamera);
  SafeDelete(mDevice);

  GRAPHICS = NULL;
}


bool Graphics::handleEvent( hstring const& message, RawStore const& args, MessageType type, f32 timestamp )
{
  MSG_StartHandling();
  MSG_PeekIf("Vibration")
  {
    static const f32 oneFrame = 1.0f/60.0f;
    f32 amount = args.GetF32("avg");
    f32 h = args.GetF32("hf");
    f32 l = args.GetF32("lf");
    if(amount != 0.0f)
      mCamera->shake(amount, 0.2f);
  }

  MSG_ConsumeIf("GFX_TOGGLE_FULLSCREEN")
  {
    mDevice->ToggleFullScreen();
  }
  MSG_ConsumeIf("LoadReady")
  {
    loadReadyHack = 2;//wait 2 frames before allowing the loadhack to run
  }

  ANT__MSG_HANDLE()
  {
    ANT__Bar_Init(bar, "Graphics", false);
    ANT__Bar_AddButton_P(ANT__GetBarP(bar, gCore), "Graphics", ANT__Toggle, ANT__PBar(ANT__GetBarP(bar, GRAPHICS)), "group=Systems" );

    //setup hide button
    ANT__Bar_AddButton(bar, "HIDE", ANT__Hide, ANT__PBar(bar));
    ANT__Bar_AddSeperatorAny(bar);//================================================
    ANT__Bar_AddButton(bar, "Toggle Camera", ant_toggle_cam, NULL);
  }


  MSG_DoneHandling();
}

void Graphics::DrawShape(vec3& pos, vec3& extents, Matrix4& orient, ShapeType shape, Color color, u1 solid /*= true*/)
{
  DrawnShape shapeToDraw(pos, extents, orient, shape, color, solid);
  AddShapeToDraw(shapeToDraw);
}

void Graphics::DrawShape( f32 dt, DrawnShape& shape )
{
  DXDevice* mDXDevice = mDevice->GetDevice();

  mDXDevice->SetRenderState( D3DRS_ZENABLE, 1 );
  mDXDevice->SetRenderState( D3DRS_ZWRITEENABLE, 1 );
  mDXDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
  mDXDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, 1 );
  mDXDevice->SetRenderState( D3DRS_ALPHATESTENABLE, 0 );
  mDXDevice->SetRenderState( D3DRS_FILLMODE, ( !shape.solid ) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

  mDevice->SetShader(shapevs);
  mDevice->SetShader(shapeps);
  mDevice->SetVSData(&mDefaultShader.Camera, 4, 4);
  mDevice->SetVSData(&mDefaultShader.CameraProjection, 4, 8);

  static float4 lightDir = float4(0.5f, 0.5f, 0.5f, 1.0f);

  mDevice->SetPSData(&lightDir, 1, 0);

  if(shape.color == Blue)     mDevice->SetTexture(shapetexBlue);
  else                        mDevice->SetTexture(shapetexRed);

  static Shape* cubeShape = mDevice->GetShapes()->GetShape(ShapeCube);
  static Shape* sphereShape = mDevice->GetShapes()->GetShape(ShapeSphere);
  static Shape* planeShape = mDevice->GetShapes()->GetShape(ShapePlane);

  Shape* obj;
  switch(shape.type)
  {
    case ShapeCube:
    obj = cubeShape;
    break;

    case ShapeSphere:
    obj = sphereShape;
    break;

    case ShapePlane:
    obj = planeShape;
    break;

  default:
    obj = mDevice->GetShapes()->GetShape(shape.type);
    break;
  };


  mDevice->SetBuffer(obj->mVBuffer, sizeof(Vertex_XYZ_NRM_UV) );
  mDevice->SetBuffer(obj->mIBuffer);
  mDevice->SetDecl(obj->mDecl);

  Matrix4 mtow = Matrix4::Translation(shape.pos) * shape.rotation * Matrix4::Scale(shape.scale);

  mDevice->SetVSData(&mtow.Transposed(), 4, 0);
  mDevice->DrawIndexedTris(obj->mNumTris, obj->mNumVerts);
}

void Graphics::DrawShapes( f32 dt )
{
  for(u32 i = 0; i < shapesToDraw.size(); ++i)
    DrawShape(dt, shapesToDraw[i]);
}

void Graphics::AddShapeToDraw( DrawnShape& shape )
{
  shapesToDraw.push_back(shape);
}

void Graphics::ClearShapes( void )
{
  shapesToDraw.clear();
}

void Graphics::vRegisterComponent( TypeID id, Component* component )
{
  static const TypeID cModel = gCore->getTypeID<gfxModel>();
  static const TypeID cShape = gCore->getTypeID<gfxShape>();

   if(id == cModel)
   {
     gfxModel* modelComp = SCAST(gfxModel*, component);
     ErrorIf(modelComponentRegistry.find(modelComp->getInstanceID()) != modelComponentRegistry.end(), "Duplicate component registration");
     modelComponentRegistry[modelComp->getInstanceID()] = modelComp;
   }
   else if(id == cShape)
   {
     gfxShape* shapeComp = SCAST(gfxShape*, component);
     ErrorIf(shapeComponentRegistry.find(shapeComp->getInstanceID()) != shapeComponentRegistry.end(), "Duplicate component registration");
     shapeComponentRegistry[shapeComp->getInstanceID()] = shapeComp;
   }
   else
     printDebug("Trying to register component Graphics could care less about...");
}

void Graphics::vUnregisterComponent( TypeID id, Component* component )
{
  static const TypeID cModel = gCore->getTypeID<gfxModel>();
  static const TypeID cShape = gCore->getTypeID<gfxShape>();

  if(id == cModel)
    modelComponentRegistry.erase(component->getInstanceID());
  else if(id == cShape)
    shapeComponentRegistry.erase(component->getInstanceID());
  else
    printDebug("Trying to unregister component Graphics shouldn't know about...");
}

void Graphics::UpdateComponents()
{
  mCompIt mItBegin = modelComponentRegistry.begin(); 
  sCompIt sItBegin = shapeComponentRegistry.begin();
  mCompIt mItEnd = modelComponentRegistry.end(); 
  sCompIt sItEnd = shapeComponentRegistry.end();

  for( ; mItBegin != mItEnd; ++mItBegin)
    mItBegin->second->Update(thisFrameDt);

  for( ; sItBegin != sItEnd; ++sItBegin)
    sItBegin->second->ReadTransform();
}

void Graphics::ToggleCamMode()
{
  mCamera->toggleCameraMode();
}

void Graphics::DrawShapeComponents()
{
  //First add all the shape components to draw queue
  sCompIt sbegin = shapeComponentRegistry.begin();
  sCompIt send = shapeComponentRegistry.end();
  for(; sbegin != send; ++sbegin)
    shapesToDraw.push_back(sbegin->second->shape);
}

void Graphics::DrawOpaqueModels()
{
  //Draw all the registered models
  mCompIt begin = modelComponentRegistry.begin();
  mCompIt end = modelComponentRegistry.end();

  for(; begin != end; ++begin)
    if(!begin->second->model->alphaBlended)
      mModelMgr->Draw(*begin->second->model, thisFrameDt, mDefaultShader.Camera, mDefaultShader.CameraProjection);
}

static bool SortVSDepthLessThan( gfxModel* lhs, gfxModel* rhs )
{
  return lhs->vsPos.z < rhs->vsPos.z;
}

static bool SortVSDepthGreaterThan( gfxModel* lhs, gfxModel* rhs )
{
  return lhs->vsPos.z > rhs->vsPos.z;
}

void Graphics::DrawAlphaModels()
{
  //Draw all the registered models
  mCompIt begin = modelComponentRegistry.begin();
  mCompIt end = modelComponentRegistry.end();

  std::vector<gfxModel*> modelstodraw;

  //Gather all the alpha blended models.
  for(; begin != end; ++begin)
    if(begin->second->model->alphaBlended)
      modelstodraw.push_back(begin->second);  

  //Gotta draw them in back to front order wrt to the camera
  //Transform them to the view space.
  for(u32 i = 0; i < modelstodraw.size(); ++i)
  {
    vec3 viewPos = mCameraMat.MulPnt(modelstodraw[i]->pos);
    modelstodraw[i]->vsPos = vec3(viewPos.x, viewPos.y, viewPos.z);
  }

  //Sort them by Z depth (largest first, which is furthest away, or back.)
  std::sort(modelstodraw.begin(), modelstodraw.end(), SortVSDepthGreaterThan);

  //Draw this newly created list.
  for(u32 i = 0; i < modelstodraw.size(); ++i)
    mModelMgr->Draw(*modelstodraw[i]->model, thisFrameDt, mDefaultShader.Camera, mDefaultShader.CameraProjection );
}

void Graphics::ChangeRes( u32 width, u32 height )
{
  mDevice->ChangeResolution(width, height);
}


void Graphics::AddResolutionMode( u32 width, u32 height )
{
  if(mCurResolutionIndex == -1)
    mCurResolutionIndex = 0;
  mSupportedResolutions.push_back(WindowSize(width, height));
}

Grimelins::u32 Graphics::GetNextResolution( void )
{
  ++mCurResolutionIndex;
  if(mCurResolutionIndex==mSupportedResolutions.size())
    mCurResolutionIndex = 0;
  return mSupportedResolutions[mCurResolutionIndex].first;
}

Grimelins::u32 Graphics::GetCurrentResolution( void )
{
  return mCurResolutionIndex;
}

Grimelins::u32 Graphics::GetNumResolutionsAvail( void )
{
  return mSupportedResolutions.size();
}

Grimelins::u1 Graphics::hasResolution( u32 width, u32 height )
{
  for(size_t i = 0; i < mSupportedResolutions.size(); ++i)
    if(mSupportedResolutions[i].first == width && mSupportedResolutions[i].second == height)
      return true;
  return false;
}

void Graphics::setResolution( u32 width, u32 height )
{
  if(hasResolution(width, height))
    mDevice->ChangeResolution(width, height);
}


void Graphics::Roll( Object& obj, f32 angle )
{
  gfxModel* model = obj.getComponentT<gfxModel>();
  if(model)
    model->Roll(angle);
}

void Graphics::Pitch( Object& obj, f32 angle )
{
  gfxModel* model = obj.getComponentT<gfxModel>();
  if(model)
    model->Pitch(angle);
}

void Graphics::Yaw( Object& obj, f32 angle )
{
  gfxModel* model = obj.getComponentT<gfxModel>();
  if(model)
    model->Yaw(angle);;
}

Camera* Graphics::getSceneCamera( void )
{
  return mCamera;
}

BoneBinding Graphics::getBoneBinding( Text const& name )
{
  //Search through every registered model
  mCompIt begin = modelComponentRegistry.begin();
  mCompIt end = modelComponentRegistry.end();

  //Set up the binding with the object that this bone belongs to and its world position.
  BoneBinding binding;

  for(; begin != end; ++begin)
  {
    //If this model has a controller, look through all its bones for the name we want and return
    //that bone's world position.
    Model& modelRef = *(begin->second->model);
    if(modelRef.controller_)
    {
      for( u32 i = 0; i < modelRef.controller_->skeleton_->bones_.size(); ++i)
      {
        Bone& bone = modelRef.controller_->skeleton_->bones_[i];
        if(bone.name_ == name)
        {
          binding.pos = &bone.worldPos_;
          binding.link = begin->second->parent();
          return binding;
        }
      }
    }
  }
    
  //Otherwise just return the binding as blank.  They're null anyways.
  return binding;
}


ScriptVec3 Graphics::getBoneWorldPos( Text const& name )
{
  //Search through every registered model
  mCompIt begin = modelComponentRegistry.begin();
  mCompIt end = modelComponentRegistry.end();

  for(; begin != end; ++begin)
  {
    //If this model has a controller, look through all its bones for the name we want and return
    //that bone's world position.
    Model& modelRef = *(begin->second->model);
    if(modelRef.controller_)
    {
      for( u32 i = 0; i < modelRef.controller_->skeleton_->bones_.size(); ++i)
      {
        Bone& bone = modelRef.controller_->skeleton_->bones_[i];
        if(bone.name_ == name)
          return bone.worldPos_;
      }
    }
  }

  //Otherwise give them the origin.
  return ScriptVec3(0.0f, 0.0f, 0.0f);
}

vec4 Graphics::GetSceneAmbience()
{
  return sceneAmbience;
}

void Graphics::SetSceneAmbience( const vec4& ambience )
{
  sceneAmbience = ambience;
}

void Graphics::drawLine( ScriptVec3 p1, ScriptVec3 p2, unsigned color )
{
  mDebugDraw->AddLine( p1, p2, (Color)color );
}

float Graphics::getDT()
{
  return thisFrameDt;
}

void Graphics::updateBillboards()
{
  for(u32 i = 0; i < billboards.size(); ++i)
  {
    //update animations if needed
    if(billboards[i].anim)
    {
      AnimQuad* anim = RCAST(AnimQuad*, &billboards[i]);
      anim->update(thisFrameDt);
    }

    //Billboard based on mode of billboarding
    if(billboards[i].cylinder)
      BillBoardCylindrical(billboards[i]);
    else
      BillBoardSpherical(billboards[i]);
  }
}

DX9Device* Graphics::GetDevice( void )
{
  return mDevice;
}

UISystem* Graphics::GetUISystem( void )
{
  return mUISystem;
}

void Graphics::killDevice()
{
  mDevice->GetRM()->OnLostDevice();
  std::exit(0);
}

void Graphics::explosion( ScriptVec3 pos )
{
  ParticleSystem::INIT_INFO info(Text(CONFIG_PATH_TEXTURE "particle.png"));
  info.BlendMode = D3DBLEND_SRCALPHA;
  info.NumParticles = 1000;
  info.Device = mDevice;
  info.Position = pos;
  info.SystemName = "EXPLOSION";

  Explosion* explode = new Explosion(&info);
  mPManager->Add(explode);
}

void Graphics::shakeCam( f32 intensity, f32 duration )
{
  mCamera->shake(intensity, duration);
}

void Graphics::dust( ScriptVec3 pos )
{
  ParticleSystem::INIT_INFO info(Text(CONFIG_PATH_TEXTURE "dust.png"));
  info.BlendMode = D3DBLEND_SRCALPHA;
  info.NumParticles = 1500;
  info.Device = mDevice;
  info.Position = pos;
  info.SystemName = "DUST";

  Dust* dustSys = new Dust(&info);
  mPManager->Add(dustSys);
}

void Graphics::sparks( ScriptVec3 pos1, ScriptVec3 pos2, ScriptVec3 dir, u32 numSparkstreams )
{
  for(u32 i = 0; i < numSparkstreams; ++i)
  {
    vec3 genPos = vec3(40, 30, 40);
    sparkStream(genPos, dir);
  }
}

void Graphics::sparkStream( ScriptVec3 pos, ScriptVec3 dir )
{
  ParticleSystem::INIT_INFO info(Text(CONFIG_PATH_TEXTURE "spark.png"));
  info.BlendMode = D3DBLEND_SRCALPHA;
  info.NumParticles = 250;
  info.Device = mDevice;
  info.Position = pos;
  info.SystemName = "SPARKS";

  Sparks* sparksys = new Sparks(&info, vec3(dir), false);
  mPManager->Add(sparksys);
}

u32 Graphics::makeBillboard( Text& texture, ScriptVec3& pos, f32 width, f32 height, u1 bob )
{
  Text bbFile(CONFIG_PATH_TEXTURE);
  bbFile.append(texture.c_str());
  ParticleSystem::INIT_INFO info( bbFile );
  info.BlendMode = D3DBLEND_SRCALPHA;
  info.NumParticles = 1;
  info.Device = mDevice;
  info.Position = pos;
  info.SystemName = "BILLBOARD";

  BillBoard* bb = new BillBoard(&info, width, height, bob);
  mPManager->Add(bb);
  return bb->ID();
}

BillBoard* Graphics::makeRawBillBoard( Text& texture, ScriptVec3& pos, f32 width, f32 height, u1 bob /*= false */ )
{
  Text bbFile(CONFIG_PATH_TEXTURE);
  bbFile.append(texture.c_str());
  ParticleSystem::INIT_INFO info( bbFile );
  info.BlendMode = D3DBLEND_SRCALPHA;
  info.NumParticles = 1;
  info.Device = mDevice;
  info.Position = pos;
  info.SystemName = "BILLBOARD";

  BillBoard* bb = new BillBoard(&info, width, height, bob);
  mPManager->Add(bb);
  return bb;
}


void Graphics::killBillboard( u32 id )
{
  mPManager->KillBB(id);
}

void Graphics::setBillboardPos( vec3& pos, s32 bbID )
{
  mPManager->SetBBPos(bbID, pos);
}

void Graphics::addLight( Light& light )
{
  mLights.push_back(light);
  mLightPositions.push_back(light.pos);
  mLightDirections.push_back(light.viewVec);
  mLightTypes.push_back(light.type);
  mLightColors.push_back(light.color);
}

void Graphics::killLight( vec3& pos )
{
  for(u32 i = 0; i < mLights.size(); ++i)
    if(pos == mLights[i].pos)
    {
      mLights.erase(mLights.begin() + i);
      mLightPositions.erase(mLightPositions.begin() + i);
      mLightDirections.erase(mLightDirections.begin() + i);
      mLightTypes.erase(mLightTypes.begin() + i);
      mLightColors.erase(mLightColors.begin() + i);
      break;
    }
}

void Graphics::updateLights()
{
  //Transform to the proper space for passing into shaders
  for(u32 i = 0; i < mLights.size(); ++i)
  {
    mLightPositions[i]  = mCamera->getMatrix().MulPnt(mLights[i].pos) ;
    mLightDirections[i] = mCamera->getMatrix().MulVec(mLights[i].viewVec);
  }

  numLights = mLights.size();
}

vec3* Graphics::lightPositions()
{
  return &mLightPositions[0];
}

vec3* Graphics::lightDirections()
{
  return &mLightDirections[0];
}

s32* Graphics::lightTypes()
{
  return &mLightTypes[0];
}

vec4* Graphics::lightColors()
{
  return &mLightColors[0];
}

s32 Graphics::numLightsInScene()
{
  return numLights;
}

s32* Graphics::lightsInScene()
{
  return &numLights;
}

f32* Graphics::spotInner()
{
  return &mLights[0].innerAngle;
}

f32* Graphics::spotOuter()
{
  return &mLights[0].outerAngle;
}

f32* Graphics::spotFallOff()
{
  return &mLights[0].spotFallOff;
}

vec3* Graphics::attenCoeffs()
{
  return &mLights[0].attenCoeffs;
}




Light::Light() : type(POINT), 
                 color(0.5f, 0.5f, 0.5f, 1.0f), 
                 spotFallOff(1.0f), innerAngle(15.0f), outerAngle(30.0f), 
                 attenuation(1.0f), attenCoeffs(1.0f, 0.1f, 0.0f)
{}

Light::Light( vec3& pos, u32 _type, vec4& col /*= vec4(0.0f, 0.0f, 0.0f, 1.0f)*/, 
              vec3& view /*= vec3(1.0f, 0.0f, 1.0f)*/, f32 falloff /*= 1.0f*/, 
              f32 inner /*= 15.0f*/, f32 outer /*= 30.0f*/, f32 att /*= 1.0f*/, 
              vec3& attCoeffs /*= vec3(1.0f, 0.1f, 0.0f)*/, f32 startAngle /*= 0.0f*/ )
              : type(_type), color(col), viewVec(view), spotFallOff(falloff), innerAngle(inner),
                outerAngle(outer), attenuation(att), attenCoeffs(attCoeffs), startingAngle(startingAngle)
{

}

Light::~Light(){}

} // namespace Virtus