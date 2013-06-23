/**********************************************************************/
/*
  
  \file
    model.cpp
  
  \author
    branden.turner
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    This is how we store models in engine after we've loaded them.
    
*/
/**********************************************************************/
#include "precompiled.h"
#include "model.h"
#include "dx9vertextype.h"
#include "dx9device.h"
#include "dx9buffers.h"
#include "dx9shaders.h"
#include "dx9resourcemanager.h"
#include "graphics.h"
#include "dx9debugdraw.h"

#include <ShellAPI.h>
#pragma comment(lib ,"Shell32.lib")

namespace Grimelins 
{

    Model::Model() : controller_(NULL), mesh_(NULL), pos(0,0,0), trans(0,0,0), scale(1.0f,1.0f,1.0f), dir(0,0,-1), up(0,1,0), 
                     right(dir.Cross(up)), alphaBlended(false), diffCol(0.0f, 0.0f, 0.0f, 1.0f), shinyCoeff(255.0f), bloomFactor(1.1f), scrollSpeed(0.0f)
    {}

    Model::Model( const Model& rhs ) :  mesh_(rhs.mesh_), pos(rhs.pos), trans(rhs.trans), scale(rhs.scale), rot(rhs.rot), dir(rhs.dir), up(rhs.up), right(rhs.right), 
                                        diff(rhs.diff), nrml(rhs.nrml), emmi(rhs.emmi), spec(rhs.spec), alpha(rhs.alpha), alphaBlended(rhs.alphaBlended), diffCol(rhs.diffCol), shinyCoeff(rhs.shinyCoeff)
    {
      if(rhs.controller_)
      {
        controller_ = new AnimationController();
        controller_->activeAnim_ = rhs.controller_->activeAnim_;
        controller_->animations_ = rhs.controller_->animations_;
        controller_->animTrackData_ = rhs.controller_->animTrackData_;
        controller_->animations_ = rhs.controller_->animations_;
        controller_->animLength_ = rhs.controller_->animLength_;
        controller_->animSpeed_ = rhs.controller_->animSpeed_;
        controller_->skeleton_ = rhs.controller_->skeleton_;
        controller_->boneMatrixBuffer_ = rhs.controller_->boneMatrixBuffer_;
        controller_->paused = rhs.controller_->paused;
        controller_->newAnim = false;
        controller_->nextAnim_ = NULL;
      }
      else
        controller_ = NULL;
    }

    Model::~Model()
    {
      if(controller_)
        delete controller_;
    }

    Model& Model::operator=( const Model& rhs )
    {
      if(rhs.controller_)
      {
        controller_ = new AnimationController();
        controller_->activeAnim_ = rhs.controller_->activeAnim_;
        controller_->animations_ = rhs.controller_->animations_;
        controller_->animTrackData_ = rhs.controller_->animTrackData_;
        controller_->animations_ = rhs.controller_->animations_;
        controller_->animLength_ = rhs.controller_->animLength_;
        controller_->animSpeed_ = rhs.controller_->animSpeed_;
        controller_->skeleton_ = rhs.controller_->skeleton_;
        controller_->boneMatrixBuffer_ = rhs.controller_->boneMatrixBuffer_;
        controller_->paused = rhs.controller_->paused;
        controller_->newAnim = false;
        controller_->nextAnim_ = NULL;
      }
      else
        controller_ = NULL;

      name_ = rhs.name_ + Text("_copy");

      mesh_ = rhs.mesh_;
      
      pos = rhs.pos;
      trans = rhs.trans;
      scale = rhs.scale;
      rot = rhs.rot;
      dir = rhs.dir;
      up = rhs.up;
      right = rhs.right;
      diff = rhs.diff;
      nrml = rhs.nrml;
      emmi = rhs.emmi;
      spec = rhs.spec;
      alpha = rhs.alpha;
      alphaBlended = rhs.alphaBlended;
      diffCol = rhs.diffCol;
      shinyCoeff = rhs.shinyCoeff;
      bloomFactor = rhs.bloomFactor;
      scrollSpeed = rhs.scrollSpeed;

      return *this;
    }

    void Model::scrollUVs()
    {
      Vertex_Mesh *vertPtr = reinterpret_cast<Vertex_Mesh*>(mesh_->vBufferData_);

      for(u32 i = 0; i < mesh_->vertexCount_; ++i)
      {
        Vertex_Mesh &vert = *vertPtr;

        vert.uv_.y += scrollSpeed * GRAPHICS->getDT();

        ++vertPtr;
      }

      //Set up vertex buffer for drawing the mesh.
      u32 vBuffSize;

      switch(mesh_->vertexType_)
      {
      case Skinned:
        vBuffSize = sizeof(Vertex_Mesh_Anim) * mesh_->vertexCount_;
        break;

      case Static:
        vBuffSize = sizeof(Vertex_Mesh) * mesh_->vertexCount_;
        break;

      case Simple:
        vBuffSize = sizeof(Vertex_XYZ_NRM_UV) * mesh_->vertexCount_;
        break;

      default:
        vBuffSize = 0;
        break;
      }

      void* vbp = NULL;
      mesh_->vertexBuffer_->Lock(vbp);

      std::memcpy(vbp, mesh_->vBufferData_, vBuffSize);

      mesh_->vertexBuffer_->Unlock();
    }


    ModelManager::ModelManager( DX9Device *device ) : mDevice_(device)
    {
      skinnedps = mDevice_->GetRM()->GetPixelShader("dx9skinnedmesh.ps");
      skinnedvs = mDevice_->GetRM()->GetVertexShader("dx9skinnedmesh.vs");

      staticps = mDevice_->GetRM()->GetPixelShader("dx9skinnedmesh.ps");
      staticvs = mDevice_->GetRM()->GetVertexShader("dx9staticmesh.vs");

      simpleps = mDevice_->GetRM()->GetPixelShader("dx9simplemesh.ps");
      simplevs = mDevice_->GetRM()->GetVertexShader("dx9simplemesh.vs");
    }


    ModelManager::~ModelManager()
    {
      for(u32 i = 0; i < models_.size(); ++i)
        if(models_[i])
          delete models_[i];
    }


    void ModelManager::Update( f32 dt )
    {
      //simply loop over all the models and call update on their animation controllers
      //keep in mind that an animation controller may be left null on a static mesh.
      for(u32 i = 0; i < models_.size(); ++i)
      {
        if(models_[i]->scrollSpeed)
          models_[i]->scrollUVs();
        if(models_[i] && models_[i]->controller_)
        {
          if(!models_[i]->controller_->paused)
            models_[i]->controller_->Update(dt);

          models_[i]->controller_->ProcessAnimationGraph();
        }
      }
    }

    Model* ModelManager::LoadModel( const char *file, u1 extraAnims )
    {
      Text filepath(CONFIG_PATH_MESH);
      filepath.append(file);
      Application::FilePath path(filepath.c_str());

      std::string extension;

      Text animName(path.FileName());
      animName.erase(animName.find_last_of('_'));

      Text modelName(animName);

      //if condition added by fitz, it is illegal to erase a string "From" NPOS
      if(modelName.find_first_of('_')!=Text::npos)
        modelName.erase(modelName.find_first_of('_'));

      if(path.Exists())
        extension = path.Extension();
      else
      {
        printDebug("No file with that name in either fbx or bin formats.");
        return NULL;
      }

      if(extension == "bin")
      {
        Text fileToOpen(CONFIG_PATH_MESH);
        fileToOpen.append(file);
        fp_ = fopen(fileToOpen.c_str(), "rb");
      }
      else
      {
        //TODO, take out this code and replace it with a message for the person to convert their 
        //file outside the engine before loading it.  This can be done late in the dev cycle.
        std::string filelocation("../bin/");
        filelocation.append(file);
        ShellExecute(0, "open", "fbxconverter.exe",  filelocation.c_str(), "..\\tools\\", 0);

        std::string output(CONFIG_PATH_MESH);
        output.append(path.Name().substr(1, path.Name().length()));
        output.append(".bin");

        fp_ = fopen(output.c_str(), "rb");
      }

      //What we're going to be filling out.
      Model* modelPtr = new Model();
      Model& model = *modelPtr;
      model.name_ = modelName;
      model.mesh_ = new Mesh();
      fread(&model.mesh_->vertexType_, sizeof(u32), 1, fp_);

      //Grab the vertex and index count
      fread(&model.mesh_->vertexCount_, sizeof(u32), 1, fp_);
      fread(&model.mesh_->indexCount_, sizeof(u32), 1, fp_);

      model.mesh_->triCount_ = model.mesh_->indexCount_ / 3;

      //Now we can load the model based on what type of vertex its using
      switch(model.mesh_->vertexType_)
      {
        case Static:
          LoadStaticModel(model);
        break;
        case Skinned:
          LoadSkinnedModel(model, animName);
        break;
        case Simple:
          LoadSimpleModel(model);
        break;
      }

      //If this isn't just a pass for extra animations, we're good to go.
      if(!extraAnims)
        models_.push_back(modelPtr);

      return modelPtr;
    }

    Model* ModelManager::LoadModelAnimsFromFile( const char *file )
    {
      Text filepath(CONFIG_PATH_MESH);
      filepath.append(file);
      Application::FilePath path(filepath.c_str());

      std::string extension;

      if(path.Exists())
        extension = path.Extension();
      else
      {
        printDebug("No file with that name for loading");
        return NULL;
      }

      Text fileToOpen(CONFIG_PATH_MESH);
      fileToOpen.append(file);

      std::vector<Text> files;
      FileStream f;
      f.newStream(fileToOpen);
      while(f.hasData())
        files.push_back(f.read<Text>());

      return LoadModelWithAnims(files);
    }

    Model* ModelManager::LoadModelWithAnims( std::vector<Text>& filenames )
    {
      //Load the first model normally,
      Model* finalModel = LoadModel( filenames[0].c_str() );

      //This will collect all the animations and then be deleted
      Model* animationHolder;

      //Load the animation data for the rest and just add it to the animation buffer of this model.
      for(u32 i = 1; i < filenames.size(); ++i)
      {
        animationHolder = LoadModel(filenames[i].c_str(), true);
        finalModel->controller_->animations_.push_back(animationHolder->controller_->activeAnim_);
        delete animationHolder;
      }

      return finalModel;
    }

    void ModelManager::LoadStaticModel(Model &model)
    {
      model.mesh_->vertSize_ = sizeof(Vertex_Mesh);
      model.mesh_->decl_ = Vertex_Mesh::Decl;

      model.mesh_->vBufferData_ = new u8 [model.mesh_->vertexCount_ * model.mesh_->vertSize_];
      model.mesh_->iBufferData_ = new s32 [model.mesh_->indexCount_];

      Vertex_Mesh *vertPtr = reinterpret_cast<Vertex_Mesh*>(model.mesh_->vBufferData_);

      //Now a for loop instead of a direct read, thanks directX, thdirectX
      for(u32 i = 0; i < model.mesh_->vertexCount_; ++i)
      {
        Vertex_Mesh &vert = *vertPtr;

        //Grab position, normal, uvs, tangent, bitangent
        fread(&vert.pos_,   sizeof(f32) * 3, 1, fp_);
        fread(&vert.nrm_,   sizeof(f32) * 3, 1, fp_);
        fread(&vert.uv_,    sizeof(f32) * 2, 1, fp_);
        fread(&vert.tan_,   sizeof(f32) * 3, 1, fp_);
        fread(&vert.bitan_, sizeof(f32) * 3, 1, fp_);

        ++vertPtr;
      }

      //Grab all the indices
      fread(model.mesh_->iBufferData_, sizeof(s32) * model.mesh_->indexCount_, 1, fp_);

      SetUpForDraw(model, model.mesh_->vertexType_);
    }

    void ModelManager::LoadSkinnedModel(Model &model, Text& animName)
    {
      model.mesh_->vertSize_ = sizeof(Vertex_Mesh_Anim);
      model.mesh_->decl_ = Vertex_Mesh_Anim::Decl;

      model.mesh_->vBufferData_ = new u8 [model.mesh_->vertexCount_ * model.mesh_->vertSize_];
      model.mesh_->iBufferData_ = new s32 [model.mesh_->indexCount_];

      Vertex_Mesh_Anim *vertPtr = reinterpret_cast<Vertex_Mesh_Anim*>(model.mesh_->vBufferData_);

      //Now a for loop instead of a direct read, thanks directX, thdirectX
      for(u32 i = 0; i < model.mesh_->vertexCount_; ++i)
      {
        Vertex_Mesh_Anim &vert = *vertPtr;

        //Grab position, normal, uvs, tangent, bitangent
        fread(&vert.pos_,   sizeof(f32) * 3, 1, fp_);
        fread(&vert.nrm_,   sizeof(f32) * 3, 1, fp_);
        fread(&vert.uv_,    sizeof(f32) * 2, 1, fp_);
        fread(&vert.tan_,   sizeof(f32) * 3, 1, fp_);
        fread(&vert.bitan_, sizeof(f32) * 3, 1, fp_);

        //Grab bone indices
        fread(vert.boneIndices_, sizeof(u8) * 4, 1, fp_);

        //Grab bone weights
        fread(vert.boneWeights_, sizeof(f32) * 4, 1, fp_);

        ++vertPtr;
      }

      //Grab all the indices
      fread(model.mesh_->iBufferData_, sizeof(s32) * model.mesh_->indexCount_, 1, fp_);

      SetUpForDraw(model, model.mesh_->vertexType_);

      //Now for bones n such.
      u32 boneCount;
      fread(&boneCount, sizeof(u32), 1, fp_);
      if(!boneCount)
      {
        fclose(fp_);
        return;
      }

      //At this point we know the model is skinned and animated, so we set up the
      //skeleton and animation controller.
      model.controller_ = new AnimationController();

      //Create a skeleton and fill it out
      shrdptr<Skeleton> skeleton = new Skeleton();
      skeleton->bones_.resize(boneCount);

      for(u32 i = 0; i < boneCount; ++i)
      { 
        //Read in each bone.
        fread(&skeleton->bones_[i].localPos_, sizeof(f32) * 3, 1, fp_); //read pos
        fread(&skeleton->bones_[i].localRot_, sizeof(f32) * 4, 1, fp_); //rotation
        fread(&skeleton->bones_[i].invTransform_, sizeof(f32) * 16, 1, fp_); //inverse bone transform
        fread(&skeleton->bones_[i].index_, sizeof(u32), 1, fp_); //index
        fread(&skeleton->bones_[i].parentIndex_, sizeof(s32), 1, fp_); //parent index

        //Read in the name
        u32 nameLen;
        fread(&nameLen, sizeof(u32), 1, fp_);
        s8 *name = new s8[nameLen + 1];
        fread(name, nameLen, 1, fp_);

        //Append a null
        name[nameLen] = 0;
        skeleton->bones_[i].name_ = name;

        //Clean up string we read into
        delete [] name;

        //Create the hierarchy of the skeleton, starting with setting this bone's index
        Bone &bone = skeleton->bones_[i];
        bone.index_ = i;

        //Is this a root bone?
        if(bone.parentIndex_ != -1)
          skeleton->bones_[bone.parentIndex_].children_.push_back( &bone );
        else
          skeleton->rootBones_.push_back( &bone );

        //And since I read in the inverse transform for the bind pose (model to bone space)
        //I should go ahead and get the bone to model space transform.
        bone.bindTransform_ = bone.invTransform_.Inverse();
      } 

      //Give our controller a reference to the skeleton and record it.
      model.controller_->skeleton_ = skeleton;

      //Now we know what sizes our track data will be and our bone matrix buffer.
      model.controller_->boneMatrixBuffer_.resize( skeleton->bones_.size() );
      model.controller_->animTrackData_.resize( skeleton->bones_.size() );

      //Grab all the animations for the animation controller.
      u32 animCount;
      fread(&animCount, sizeof(u32), 1, fp_);

      for(u32 i = 0; i < animCount; ++i)
      {
        shrdptr<Animation> anim = new Animation();

        //Read in the name
        u32 nameLen;
        fread(&nameLen, sizeof(u32), 1, fp_);
        s8 *name = new s8[nameLen + 1];
        fread(name, nameLen, 1, fp_);

        //Append a null
        name[nameLen] = 0;
        anim->name_ = name;

        //Clean up string we read into
        delete [] name;

        anim->name_ = animName;

        //Get duration
        fread(&anim->length_, sizeof(f32), 1, fp_);

        //Get the key frames
        u32 keyFrameVecCount;
        fread(&keyFrameVecCount, sizeof(u32), 1, fp_);

        anim->tracks_.resize(keyFrameVecCount);

        for(u32 j = 0; j < keyFrameVecCount; ++j)
        {
          u32 keyFrameCount;
          fread(&keyFrameCount, sizeof(u32), 1, fp_);
          anim->tracks_[j].resize(keyFrameCount);

          //May be unsafe and requires an additional for loop...
          fread(&anim->tracks_[j][0], sizeof(KeyFrame) * keyFrameCount, 1, fp_);
        }

        //Every animation added is a reference to an animation and is our new current animation
        model.controller_->animations_.push_back( anim );
        model.controller_->activeAnim_ = anim;

        //HARD CODING
        model.controller_->activeAnim_->length_ = anim->tracks_[anim->tracks_.size() - 1][anim->tracks_[anim->tracks_.size() - 1].size() - 1].time_;
      }
    }

    void ModelManager::LoadSimpleModel(Model &model)
    {
      model.mesh_->vertSize_ = sizeof(Vertex_XYZ_NRM_UV);
      model.mesh_->decl_     = Vertex_XYZ_NRM_UV::Decl;

      model.mesh_->vBufferData_ = new u8 [model.mesh_->vertexCount_ * model.mesh_->vertSize_];
      model.mesh_->iBufferData_ = new s32 [model.mesh_->indexCount_];

      Vertex_XYZ_NRM_UV *vertPtr = reinterpret_cast<Vertex_XYZ_NRM_UV*>(model.mesh_->vBufferData_);

      //Now a for loop instead of a direct read, thanks directX, thdirectX
      for(u32 i = 0; i < model.mesh_->vertexCount_; ++i)
      {
        Vertex_XYZ_NRM_UV &vert = *vertPtr;

        //Grab position, normal, and uvs
        fread(&vert.pos, sizeof(f32) * 3, 1, fp_);
        fread(&vert.nrm, sizeof(f32) * 3, 1, fp_);
        fread(&vert.tex, sizeof(f32) * 2, 1, fp_);

        ++vertPtr;
      }

      //Grab all the indices
      fread(model.mesh_->iBufferData_, sizeof(s32) * model.mesh_->indexCount_, 1, fp_);

      SetUpForDraw(model, model.mesh_->vertexType_);
    }

    void ModelManager::SetUpForDraw( Model &model, VertexType type )
    {
      //Set up vertex buffer for drawing the mesh.
      u32 vBuffSize;
      
      switch(type)
      {
        case Skinned:
          vBuffSize = sizeof(Vertex_Mesh_Anim) * model.mesh_->vertexCount_;
        break;

        case Static:
          vBuffSize = sizeof(Vertex_Mesh) * model.mesh_->vertexCount_;
        break;

        case Simple:
          vBuffSize = sizeof(Vertex_XYZ_NRM_UV) * model.mesh_->vertexCount_;
        break;

        default:
          vBuffSize = 0;
        break;
      }
      
      u32 iBuffSize = sizeof(u32) * model.mesh_->indexCount_;
       
      model.mesh_->vertexBuffer_ = mDevice_->GetRM()->GetVertexBuffer( vBuffSize, D3DUSAGE_WRITEONLY);
      model.mesh_->indexBuffer_ = mDevice_->GetRM()->GetIndexBuffer( iBuffSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32);

      void* vbp = NULL;
      model.mesh_->vertexBuffer_->Lock(vbp);

      std::memcpy(vbp, model.mesh_->vBufferData_, vBuffSize);

      model.mesh_->vertexBuffer_->Unlock();

      void* ibp = NULL;
      model.mesh_->indexBuffer_->Lock(ibp);

      std::memcpy(ibp, model.mesh_->iBufferData_, iBuffSize);

      model.mesh_->indexBuffer_->Unlock();
    }

    void ModelManager::DrawModels( f32 dt, Matrix4 const& cam, Matrix4 const& camproj, u1 skeleton )
    {
      for(u32 i = 0; i < models_.size(); ++i)
        Draw(*models_[i], dt, cam, camproj, skeleton);
    }

    static void GetTranslation(const Matrix4& mtx, vec3& vec)
    {
      vec.x = mtx.m30;
      vec.y = mtx.m31;
      vec.z = mtx.m32;
    }

    void ModelManager::SetTextureSampling(DXDevice* dev)
    {
      for(u32 i = 0; i < 4; ++i )
      {
        dev->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        dev->SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
        dev->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
      }
    }

    void ModelManager::SetOpaqueRenderState(DXDevice* dev)
    {
      dev->SetRenderState( D3DRS_ALPHABLENDENABLE, 0 );     
      dev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
      dev->SetRenderState( D3DRS_ALPHATESTENABLE, 0 );
      dev->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, 0 );
    }

    void ModelManager::SetTransluscentRenderState(DXDevice* dev)
    {
      dev->SetRenderState( D3DRS_ALPHABLENDENABLE, 1 );
      dev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
      dev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
      dev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
      dev->SetRenderState( D3DRS_ALPHATESTENABLE, 1 );
      dev->SetRenderState( D3DRS_ALPHAREF, 0x01 ); 
      dev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL ); 
      dev->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, 1 ); 
    }


    void ModelManager::SetShaderAssociation( const Model& model )
    {
      switch(model.mesh_->vertexType_)
      {
      case Skinned:
        modelps = skinnedps;
        modelvs = skinnedvs;
        break;

      case Static:
        modelps = staticps;
        modelvs = staticvs;
        break;

      case Simple:
      default:
        modelps = simpleps;
        modelvs = simplevs;
        break;
      }
    }



    void ModelManager::Draw( Model &model, f32 dt, Matrix4 const& cam, Matrix4 const& camproj, u1 skeleton )
    {
      //Build the model to world transformation of each model.
      Matrix4 mtow = Matrix4::Translation(model.pos) * Matrix4::AlignVector(model.right, model.up, -model.dir) * Matrix4::Scale(model.scale);
      

      //If this model has bones, we simply get the worldspace position of each bone.
      if(model.controller_)
      { 
        vec3 p1,p2;

        //Calculate the worldspace position of each bone and store it. (for bone attaching system)
        for(u32 i = 0; i < model.controller_->skeleton_->bones_.size(); ++i)
        {
          Bone &bone = model.controller_->skeleton_->bones_[i];
          GetTranslation(model.controller_->boneMatrixBuffer_[ bone.index_ ], p1);
          vec4 p1World(p1.x, p1.y, p1.z, 1.0f);
          bone.worldPos_ =  mtow.MulPnt(p1);
        }

        //If you want to draw the skeleton, draw it in worldspace.  (this doesn't have to be quick, since it's a debug option)
        if(skeleton)
        {    
          for(u32 i = 0; i < model.controller_->skeleton_->bones_.size(); ++i)
          {
            Bone &bone = model.controller_->skeleton_->bones_[i];
            for(u32 j = 0; j < bone.children_.size(); ++j)
            {    
              GetTranslation(model.controller_->boneMatrixBuffer_[ bone.index_ ], p1);
              GetTranslation(model.controller_->boneMatrixBuffer_[ bone.children_[j]->index_ ], p2);

              vec4 p1World(p1.x, p1.y, p1.z, 1.0f);
              vec4 p2World(p2.x, p2.y, p2.z, 1.0f);

              p1World = mtow * p1World;
              p2World = mtow * p2World;

              GRAPHICS->mDebugDraw->AddLine(vec3(p1World.x, p1World.y, p1World.z), vec3(p2World.x, p2World.y, p2World.z));
            }
          }

          return;
        }


      }

      mtow.SetTranspose();

      //For whatever type of model this is, set its respective shader
      SetShaderAssociation(model);

      DXDevice* mDXDevice = mDevice_->GetDevice();

      SetTextureSampling(mDXDevice);

      if(model.alphaBlended)
        SetTransluscentRenderState(mDXDevice);
      else
        SetOpaqueRenderState(mDXDevice);

      //Basic render state info for all models.
      mDXDevice->SetRenderState( D3DRS_ZENABLE, 1 );
      mDXDevice->SetRenderState( D3DRS_ZWRITEENABLE, 1 );
      mDXDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

      mDevice_->SetShader(modelvs);
      mDevice_->SetShader(modelps);
      mDevice_->SetVSData(&cam, 4, 4);
      mDevice_->SetVSData(&camproj, 4, 8);

      //Pass skin data to shader if necessary
      if(model.controller_ && model.mesh_->vertexType_ == Skinned)
      {
        std::vector<Matrix4> boneMatrices;
        boneMatrices.resize(model.controller_->skeleton_->bones_.size());
        for(u32 i = 0; i < boneMatrices.size(); ++i)
        {
          boneMatrices[i] = GetModelToBoneSpaceTransform(*model.controller_->skeleton_, i) * model.controller_->boneMatrixBuffer_[i];
          //We transpose our bone matrices so we can pass them in as a 4x3 matrix.  (the last column is 0001, so we don't need it)
          boneMatrices[i].SetTranspose();
          mDevice_->SetVSData(&boneMatrices[i], 3, 12 + (i * 3));
        }
      }

      //Simple lighting for the scene (directional)
      static float4 lightDir = float4(0.5f, 0.5f, 0.5f, 1.0f);

      mDevice_->SetPSData(&GRAPHICS->GetSceneAmbience(), 1, 0);
      mDevice_->SetPSData(&lightDir, 1, 1);
      mDevice_->SetPSData(&model.diffCol, 1, 2);

      mDevice_->SetPSData(GRAPHICS->lightPositions(), 8, 3);
      mDevice_->SetPSData(GRAPHICS->lightDirections(), 8, 11);
      mDevice_->SetPSData(GRAPHICS->lightColors(), 8, 19);
      mDevice_->SetPSData(&model.shinyCoeff, 1, 27);
      mDevice_->SetPSData(&model.bloomFactor, 1, 28);

      mDevice_->SetPSData(GRAPHICS->lightTypes(), 8, 0, true);
      mDevice_->SetPSData(GRAPHICS->lightsInScene(), 1, 8, true);

     
      mDevice_->SetTexture(model.diff.GetPtr(),  0);
      mDevice_->SetTexture(model.emmi.GetPtr(),  1);
      mDevice_->SetTexture(model.nrml.GetPtr(),  2);
      mDevice_->SetTexture(model.spec.GetPtr(),  3);
      mDevice_->SetTexture(model.alpha.GetPtr(), 4);
      
      mDevice_->SetBuffer(model.mesh_->vertexBuffer_, model.mesh_->vertSize_ );
      mDevice_->SetBuffer(model.mesh_->indexBuffer_);
      mDevice_->SetDecl(model.mesh_->decl_);
      
      mDevice_->SetVSData(&mtow, 4, 0);
      mDevice_->DrawIndexedTris(model.mesh_->triCount_, model.mesh_->vertexCount_);
    }

    Matrix4& ModelManager::GetModelToBoneSpaceTransform( Skeleton& skeleton, u32 boneIndex )
    {
      return skeleton.bones_[boneIndex].invTransform_;
    }

}//Virtus