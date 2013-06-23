#ifndef APP_H
#define APP_H

namespace cs350
{
  class Timer;
  class Camera;
  class MeshManager;
  class DebugDraw;

  struct Frustrum
  {
    Plane f[6];
    bool checkInside(AABB& box);
  };

  class DXAPP
  {
  public:
    DXAPP(HINSTANCE inst);
    ~DXAPP();

    HINSTANCE getHinst() const;
    HWND getWnd() const;

    void initApp();
    void update(float dt);
    void draw();
    int run();
    LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);

  protected:
    void initDevice();
    void initWindow();
    void onResize();
    void buildEffects();
    void buildVertLayouts();
    void updatePerFrameVariables();
    void updateFPS();
    void updateKeyStates(float dt);
    void drawShapes();
    void drawDebugInfo();
    void drawTrisFromKD();
    void drawDBVH();
    void prepDrawTris();
    void drawCoordSystem();

    //device stuff
    HINSTANCE mainInst;
    HWND mainWnd;

    bool paused;
    bool minimized;
    bool maximized;
    bool resizing;
    bool lMouseDown;
    bool drawNorms;

    D3D10_DRIVER_TYPE       driverType;
    ID3D10Device*           device;
    IDXGISwapChain*         pSwapChain;
    ID3D10Texture2D*        depthStencilBuffer;
    ID3D10RenderTargetView* pRenderTargetView;
    ID3D10DepthStencilView* depthStencilView;
    ID3DX10Font*            font;
    ID3D10Effect*           effect;
    ID3D10EffectTechnique*  tech;
    ID3D10EffectMatrixVariable* camMat;
    ID3D10EffectMatrixVariable* camProjMat;
    ID3D10EffectMatrixVariable* mtwMtx;
    ID3D10InputLayout* vertexLayout;

    ID3D10RasterizerState*  wireFrame;
    ID3D10RasterizerState*  solidFill;

    Timer timer;

    std::wstring frameStats;

    D3DXCOLOR clearColor;
    int winWidth;
    int winHeight;

    //Actual rendering stuff
    D3DXMATRIX mView;
    D3DXMATRIX mProj;
    D3DXMATRIX mWVP;
    D3DXMATRIX cullMtx;

    bool wired;

    float fov_;
    float aRat_;
    float nP_;
    float fP_;

    float mDx;
    float mDy;

    D3DXVECTOR3 camTarget, camUp, camRight, camPos;

    MeshManager* shapeMgr;
    DebugDraw* debugDraw;
    ID3D10Effect*           lineEffect;
    ID3D10EffectTechnique*  lineTech;
    GeometryLibrary* geomLib;
    Frustrum frustrum;
    ID3D10Effect*           triEffect;
    ID3D10EffectTechnique*  triTech;
    ID3D10EffectMatrixVariable* triCam;
    ID3D10EffectMatrixVariable* triCamProj;

    MPortal portal;
    Object *selectedObj, *otherObj;
  };
}


#endif