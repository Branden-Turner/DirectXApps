/*! ====================================================================================================================
  @file graphics.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Jun.03.2011

  @brief 
    More cool graphics stuff goes here

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_GRAPHICS_H
#define VIRTUS_GRAPHICS_H

#include "dx9.h"
#include "dx9shaders.h"
#include "cmpgfx.h"
 

namespace Grimelins {

    class DX9Device;
    class DX9RenderTarget;
    class DX9ResourceManager;
    class DX9DebugDraw;
    class Camera;
    class UISystem;
    class DevConsole;
    class Shape;
    class BoneBinding;
    class ParticleManager;
    struct AnimQuad;
    class BillBoard;

    class ModelManager
    {
      friend class gfxModel;
      friend class Graphics;
      public:
        ModelManager(DX9Device *device);
        ~ModelManager();
        void Update(f32 dt);
        Model* LoadModel( const char *file, u1 extraAnims = false );
        Model* LoadModelAnimsFromFile( const char *file );
        Model* LoadModelWithAnims( std::vector<Text>& filenames );
        void DrawModels( f32 dt, Matrix4 const& cam, Matrix4 const& camproj, u1 skeleton = false);
        void SetTransluscentRenderState(DXDevice* dev);
        

      private:
        DX9Device *mDevice_;
        std::vector<Model*> models_;
        void LoadStaticModel(Model &model);
        void LoadSkinnedModel(Model &model, Text& animName);
        void LoadSimpleModel(Model &model);
        void SetUpForDraw(Model &model, VertexType type);
        void Draw(Model &model, f32 dt, Matrix4 const& cam, Matrix4 const& camproj, u1 skeleton = false);
        Matrix4& GetModelToBoneSpaceTransform(Skeleton& skeleton, u32 boneIndex);
        void SetTextureSampling(DXDevice* dev);
        void SetOpaqueRenderState(DXDevice* dev);
        
        void SetShaderAssociation(const Model& model);

        //Going to be used as an internal temporary for storing the file to read from.
        FILE *fp_;

        DX9PixelShader*  skinnedps;
        DX9VertexShader* skinnedvs;

        DX9PixelShader*  staticps; 
        DX9VertexShader* staticvs; 

        DX9PixelShader*  simpleps; 
        DX9VertexShader* simplevs; 

        DX9PixelShader* modelps;
        DX9VertexShader* modelvs;
    };

    enum LightType
    {
      POINT, 
      SPOT,
      DIRECTIONAL,
      LIGHT_TYPE_COUNT
    };

    struct Light
    {
      Light();
      Light( vec3& pos, u32 _type, vec4& col = vec4(0.0f, 0.0f, 0.0f, 1.0f), vec3& view = vec3(1.0f, 0.0f, 1.0f), 
             f32 falloff = 1.0f, f32 inner = 15.0f, f32 outer = 30.0f, 
             f32 att = 1.0f, vec3& attCoeffs = vec3(1.0f, 0.1f, 0.0f), f32 startAngle = 0.0f);
      ~Light();

      //Basic properties
      vec3 pos;
      vec3 target;
      u32  type;

      vec3 viewVec;
      vec4 color;

      //Spot properties
      f32 spotFallOff;
      f32 innerAngle;
      f32 outerAngle;

      //Attenuation
      f32 attenuation;
      vec3 attenCoeffs;

      //For rotating the lights
      f32 startingAngle;
    };

    SCRIPT_HeaderDefine(Graphics);
    class Graphics : public ISystem, public IListener
    {

    public: //ANT Section
      ANT__Bar(bar);
    public:
      Graphics(void);
      virtual ~Graphics(void);

      // ISystem interface
    public:
      virtual void vLoadSystem(void);
      virtual void vUpdateSystem(void);
      virtual void vUnloadSystem(void);
      virtual hstring vGetSystemID(void) const { return mIDName; }
      virtual void vRegisterComponent(TypeID id, Component* component);
      virtual void vUnregisterComponent(TypeID id, Component* component);

      // ISubscriber interface
    private:
      virtual bool handleEvent(hstring const& message, RawStore const& args, MessageType type, f32 timestamp);

    private:
      void InitializeSystem( s32 winHandle );
      void DrawSkybox( void );
      void BuildProjection( f32 aspectRatio, f32 fov, f32 nplane, f32 fplane );
      void UpdateMatrices( void );    
      void RenderScene();
      void RenderSceneFromTexture(IDirect3DTexture9* texture);

      //Shape drawing stuff
      void DrawShapes(f32 dt);
      void AddShapeToDraw(DrawnShape& shape);
      void ClearShapes(void);
      void DrawShape(f32 dt, DrawnShape& shape);
      void UpdateComponents();
      void DrawShapeComponents();

      //other model drawing methods
      void DrawOpaqueModels();
      void DrawAlphaModels();

      //Render targets
      void GetRenderTargets();
      void CreateRenderTargetTexture( DX9RenderTarget*& target, IDirect3DTexture9*& texture, u32 dimX, u32 dimY, u32 dimZ = 1, D3DFORMAT fmt = D3DFMT_A32B32G32R32F );

      //Billboarding functionality
      void BillBoardSpherical(DrawnShape& shape);
      void BillBoardCylindrical(DrawnShape& shape);
      void updateBillboards();
      void drawBillboard(f32 dt, DrawnShape& shape);

      void addLight( Light& light );
      void killLight( vec3& pos );
      void updateLights();

    public:
      DX9Device* GetDevice(void);
      UISystem* GetUISystem(void);

      void DrawShape(vec3& pos, vec3& extents, Matrix4& orient, ShapeType shape, Color color, u1 solid = true);
      void ChangeAnim(Text& modelName, Text& animName);
      void ChangeAnimNow(Text& modelName, Text& animName);
      void ToggleCamMode();
      void ChangeRes(u32 width, u32 height);
      vec4 GetSceneAmbience();
      void SetSceneAmbience( const vec4& ambience );
      void RenderUI();
      void AddResolutionMode(u32 width, u32 height);
      u32 GetNextResolution(void);
      u32 GetCurrentResolution(void);
      u32 GetNumResolutionsAvail(void);
      u1 hasResolution(u32 width, u32 height);
      void setResolution(u32 width, u32 height);
      void Roll( Object& obj, f32 angle );
      void Pitch( Object& obj, f32 angle );
      void Yaw( Object& obj, f32 angle);
      Camera* getSceneCamera(void);
      BoneBinding getBoneBinding( Text const& name );
      ScriptVec3 getBoneWorldPos( Text const& name );
      void drawLine( ScriptVec3 p1, ScriptVec3 p2, u32 color );
      float getDT();
      void killDevice();
      void explosion( ScriptVec3 pos );
      void shakeCam( f32 intensity, f32 duration );
      void dust( ScriptVec3 pos );
      void sparks( ScriptVec3 pos1, ScriptVec3 pos2, ScriptVec3 dir, u32 numSparkstreams );
      void sparkStream( ScriptVec3 pos, ScriptVec3 dir );

      //billboarded quads w/ w/out animated textures    
      u32 makeBillboard( Text& texture, ScriptVec3& pos, f32 width, f32 height, u1 bob = false );
      BillBoard* makeRawBillBoard( Text& texture, ScriptVec3& pos, f32 width, f32 height, u1 bob = false );
      void killBillboard( u32 id );
      vec3* lightPositions();
      vec3* lightDirections();
      vec4* lightColors();
      s32*  lightTypes();
      s32*  lightsInScene();
      f32*  spotInner();
      f32*  spotOuter();
      f32*  spotFallOff();
      vec3* attenCoeffs();
      s32   numLightsInScene();
      void setBillboardPos( vec3& pos, s32 bbID );


      DX9PixelShader*  particleps;     
      DX9VertexShader* particlevs; 
      ModelManager* mModelMgr;
      DefaultVSData mDefaultShader;


    friend class UISystem;
    friend class DevConsole;
    friend class gfxShape;
    friend class gfxModel;
    friend class ModelManager;
    friend class UIGroup;
    friend class Factory;
    friend struct AnimQuad;
    private:
      f32 thisFrameDt;
      hstring mIDName;
      Matrix4 mProjectionMat;
      Matrix4 mCameraMat;
      Matrix4 mCameraProjectionMat;
     
      DevConsole* mDevConsole;
      UISystem* mUISystem;

      Camera* mCamera;

      DX9DebugDraw*       mDebugDraw;
      DX9Device*          mDevice;
      DX9ResourceManager* mRM;
      DX9RenderTarget*    mBackBuffer;
      std::vector<DrawnShape> shapesToDraw;

      //Component Management
      typedef std::map<ComponentID, gfxModel*>::iterator mCompIt;
      typedef std::map<ComponentID, gfxShape*>::iterator sCompIt;
      std::map<ComponentID, gfxModel*> modelComponentRegistry;
      std::map<ComponentID, gfxShape*> shapeComponentRegistry;

      //Shaders/Textures used for the graphics draw functions
      DX9PixelShader*  skyps;
      DX9VertexShader* skyvs;
      Shape* skybox;
      DX9CubeTexture* skytexture;
      DX9PixelShader*  shapeps;     
      DX9VertexShader* shapevs; 
      DX9Texture*      shapetexBlue;
      DX9Texture*      shapetexRed; 

      typedef std::pair<u32, u32> WindowSize;
      std::vector<WindowSize> mSupportedResolutions;
      size_t mCurResolutionIndex;

      u32 loadReadyHack;


      DX9RenderTarget*      postProcessTar;
      IDirect3DTexture9*    postProcessTex;

      DX9RenderTarget*      glowBuffer;
      IDirect3DTexture9*    glowBufferTex;

      vec4 sceneAmbience;

      DX9PixelShader*  quadps;     
      DX9VertexShader* quadvs; 

      static u32 m3dQuadID;
      std::vector<DrawnShape> billboards;

      ParticleManager* mPManager;

      std::vector<Light> mLights;
      std::vector<vec3> mLightPositions;
      std::vector<vec3> mLightDirections;
      std::vector<s32> mLightTypes;
      std::vector<vec4> mLightColors;
      s32 numLights;
    };
} // namespace Virtus

#endif
