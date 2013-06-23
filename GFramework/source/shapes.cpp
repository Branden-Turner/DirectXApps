#include "precompiled.h"

namespace cs350
{


  void MeshManager::drawMeshes()
  {
    for(unsigned i = 0; i < meshes.size(); ++i)
      meshes[i]->draw();
  }


  void MeshManager::drawMesh( unsigned index )
  {
    meshes[index]->draw();
  }


  void MeshManager::updateMeshes()
  {
    for(unsigned i = 0; i < meshes.size(); ++i)
      meshes[i]->update();
  }


  MeshManager::MeshManager( ID3D10Device* dev, ID3D10EffectMatrixVariable* mtow, GeometryLibrary* gLib )
    :device(dev), mtw(mtow), lib(gLib)
  {
    unsigned subdiv, type;
    D3DXVECTOR3 pos, scale, axis;
    D3DXVECTOR4 clr;
    float angle;

    std::vector<std::string> strs;

    std::string line, token;
    std::ifstream myfile ("scene.txt");
    if (myfile.is_open())
    {
      while (  myfile.good()   )
      {
        //Grab a line from the file
        getline (myfile,line);

        if(myfile.good())
        {

          //Break down the line into a bunch of string streams
          while(line.size())
          {
            token = line.substr(0, line.find_first_of(',') );
            line.erase(0, token.size() + 1);
            strs.push_back(std::string(token));
          }

          type    = atoi(strs[0].c_str());
          subdiv  = atoi(strs[1].c_str());
          pos.x   = (float)atof(strs[2].c_str());
          pos.y   = (float)atof(strs[3].c_str());
          pos.z   = (float)atof(strs[4].c_str());
          scale.x = (float)atof(strs[5].c_str());
          scale.y = (float)atof(strs[6].c_str());
          scale.z = (float)atof(strs[7].c_str());
          axis.x  = (float)atof(strs[8].c_str());
          axis.y  = (float)atof(strs[9].c_str());
          axis.z  = (float)atof(strs[10].c_str());
          angle   = (float)atof(strs[11].c_str());
          clr.x   = (float)atof(strs[12].c_str());
          clr.y   = (float)atof(strs[13].c_str());
          clr.z   = (float)atof(strs[14].c_str());
          clr.w   = (float)atof(strs[15].c_str());

          //Build a shape out of these variables.
          buildMesh(subdiv + 1,subdiv + 1,pos,scale,axis,angle,clr,(MeshType)type);
          strs.resize(0);
        }

      }
      myfile.close();
    }
  }


  MeshManager::~MeshManager()
  {
    for(unsigned i = 0; i < meshes.size(); ++i)
      delete meshes[i];
  }

  void MeshManager::buildMesh( unsigned slices, unsigned stacks, const D3DXVECTOR3& _pos, const D3DXVECTOR3& _sc, const D3DXVECTOR3& _ax, float _angle, const D3DXVECTOR4& _clr, MeshType _type )
  {
    meshes.push_back(new Mesh( lib, mtw, device, slices, stacks, _pos, _sc, _ax, _angle, _clr, _type));
  }

  unsigned MeshManager::numMeshes()
  {
    return meshes.size();
  }

  void MeshManager::setMtw( unsigned index )
  {
    meshes[index]->setMtw();
  }

  void Mesh::buildMtw()
  {
    D3DXMATRIX S, R, T;

    //Set up the MTW
    D3DXMatrixTranslation(&T, pos.x, pos.y, pos.z);
    D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);

    if(D3DXVec3Length(&axis) != 0)
      D3DXMatrixRotationAxis(&R, &axis, angle * toRad);
    else
      D3DXMatrixIdentity(&R);

    modToW = S * R * T ;
  }

  Mesh::Mesh( GeometryLibrary* lib, ID3D10EffectMatrixVariable* mtw, ID3D10Device* dev, unsigned slices, unsigned stacks, 
              const D3DXVECTOR3& _pos, const D3DXVECTOR3& _sc, const D3DXVECTOR3& _ax, float _angle, const D3DXVECTOR4& _clr, MeshType _type )
                : pos(_pos), scale(_sc), axis(_ax), angle(_angle), 
                  color(_clr), type_(_type), device(dev), modelToWorld(mtw),
                  slices_(slices), stacks_(stacks), staticSceneElement(true), libRef(lib)
  {
    switch(type_)
    {
      case MeshPlane:
        PlaneShape();
        break;
      case MeshCube:
        CubeShape();
        break;
      case MeshSphere:
        SphereShape();
        break;
    }

    vertBuffer = NULL;
    indBuffer  = NULL;

    buildMtw();
    getVertBuffer();
    getIndBuffer();
    generateGeometryInfo();
  }

  void Mesh::draw()
  {
    if(!staticSceneElement)
      buildMtw();

    UINT stride = sizeof(Vertex_Pos_Clr_Nrm_Tex);
    UINT offset = 0;

    device->IASetVertexBuffers(0, 1, &vertBuffer, &stride, &offset);
    device->IASetIndexBuffer(indBuffer, DXGI_FORMAT_R32_UINT, 0);
    
    device->DrawIndexed(numIndices, 0, 0);
  }

  void Mesh::update()
  {
    //Right now we do nothing.
  }

  void Mesh::getIndBuffer()
  {
    numIndices = inds.size();
    numFaces = numIndices / 3;

    D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * numIndices;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &inds[0];
    iinitData.SysMemPitch =  0;
    iinitData.SysMemSlicePitch = 0;
    HR(device->CreateBuffer(&ibd, &iinitData, &indBuffer));
  }

  void Mesh::getVertBuffer()
  {
    numVerts = verts.size();
    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex_Pos_Clr_Nrm_Tex) * numVerts;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &verts[0];
    vinitData.SysMemPitch =  0;
    vinitData.SysMemSlicePitch = 0;
    HR(device->CreateBuffer(&vbd, &vinitData, &vertBuffer));
  }

  void Mesh::buildIndexBuffer( )
  {
    for(unsigned i = 0; i < stacks_; ++i)
    {
      for(unsigned j = 0; j < slices_; ++j)
      {
        unsigned stride = (slices_ + 1) * 2;
        unsigned offset = j * 2;

        inds.push_back( i * stride + offset + 2 );
        inds.push_back( i * stride + offset + 0 );
        inds.push_back( i * stride + offset + 1 );

        inds.push_back( i * stride + offset + 2 );
        inds.push_back( i * stride + offset + 1 );
        inds.push_back( i * stride + offset + 3 );
      }
    }
  }

#define SCAST(x, y) static_cast<x>(y)

  void Mesh::PlaneShape()
  {
    for( unsigned row = 0; row < stacks_; ++row )
    {
      float yone = SCAST(float, row) / SCAST(float, stacks_);
      float ytwo = SCAST(float, row + 1) / SCAST(float, stacks_);

      for( unsigned col  = 0; col <= slices_; ++col )
      {
        float xone = SCAST(float, col) / SCAST(float, slices_);

        Vertex_Pos_Clr_Nrm_Tex v1, v2;

        v1.pos = vec3(xone - 0.5f, 0.5f - yone, 0.0f);
        v1.nrm = vec3(0.0f, 0.0f, 1.0f);
        v1.tex = vec2(xone, yone);
        v1.clr = color;

        v2.pos = vec3(xone - 0.5f, 0.5f - ytwo, 0.0f);
        v2.nrm = vec3(0.0f, 0.0f, 1.0f);
        v2.tex = vec2(xone, ytwo);
        v2.clr = color;

        verts.push_back(v1);
        verts.push_back(v2);
      }
    }
    
    buildIndexBuffer();
  }

  void Mesh::SphereShape()
  {
    float radi = 0.5f;

    for( unsigned row = 0; row < stacks_; ++row )
    {
      float yone = SCAST(float, row) / SCAST(float, stacks_);
      float ytwo = SCAST(float, row + 1) / SCAST(float, stacks_);

      float beta1 = halfPI - (yone * PI);
      float beta2 = halfPI - (ytwo * PI);

      float cosb1 = cosf(beta1);
      float sinb1 = sinf(beta1);
      float cosb2 = cosf(beta2);
      float sinb2 = sinf(beta2);

      for( unsigned col  = 0; col <= slices_; ++col )
      {
        float xone = SCAST(float, col) / SCAST(float, slices_);
        float alph1 = xone * PITWO;

        float cosa1 = cosf(alph1);
        float sina1 = sinf(alph1);

        Vertex_Pos_Clr_Nrm_Tex v1, v2;

        v1.pos = vec3( radi * sina1 * cosb1, radi * sinb1, radi * cosa1 * cosb1 );
        v1.nrm = v1.pos;
        v1.tex = vec2(xone, yone);
        v1.nrm.Normalize();
        v1.clr = color;

        v2.pos = vec3( radi * sina1 * cosb2, radi * sinb2, radi * cosa1 * cosb2 );
        v2.nrm = v2.pos;
        v2.tex = vec2(xone, ytwo);
        v2.nrm.Normalize();
        v2.clr = color;

        verts.push_back(v1);
        verts.push_back(v2);
      }
    }

    buildIndexBuffer();
  }

  void Mesh::CubeShape()
  {
    //Start with a plane.
    PlaneShape();

    unsigned numVertex = verts.size();
    unsigned numIndices = inds.size();

    mat4 tmpMat = mat4::Translation(0.5f, 0.0f, 0.0f) * mat4::Rotation(halfPI, vec3(0,1,0));

    for(unsigned i = 0; i < numVertex; ++i )
    {
      Vertex_Pos_Clr_Nrm_Tex tmp;

      tmp.pos = tmpMat.MultPt( verts[i].pos );
      tmp.nrm = tmpMat.MultVec( verts[i].nrm );
      tmp.tex = verts[i].tex;
      tmp.clr = color;

      verts.push_back( tmp );
    }

    for(unsigned i = 0; i < numIndices; ++i ) 
    {
      inds.push_back( inds[i] + numVertex);
    }

    tmpMat = mat4::Translation(0.0f, 0.0f, -0.5f) * mat4::Rotation(halfPI * 2.0f,  vec3(0,1,0));

    for(unsigned i = 0; i < numVertex; ++i )
    {
      Vertex_Pos_Clr_Nrm_Tex tmp;

      tmp.pos = tmpMat.MultPt( verts[i].pos );
      tmp.nrm = tmpMat.MultVec( verts[i].nrm );
      tmp.tex = verts[i].tex;
      tmp.clr = color;

      verts.push_back( tmp );
    }

    for(unsigned i = 0; i < numIndices; ++i ) 
    {
      inds.push_back( inds[i] + numVertex * 2);
    }

    tmpMat = mat4::Translation(-0.5f, 0.0f, 0.0f) * mat4::Rotation(halfPI * 3.0f,  vec3(0,1,0));

    for(unsigned i = 0; i < numVertex; ++i )
    {
      Vertex_Pos_Clr_Nrm_Tex tmp;

      tmp.pos = tmpMat.MultPt( verts[i].pos );
      tmp.nrm = tmpMat.MultVec( verts[i].nrm );
      tmp.tex = verts[i].tex;
      tmp.clr = color;

      verts.push_back( tmp );
    }

    for(unsigned i = 0; i < numIndices; ++i ) 
    {
      inds.push_back( inds[i] + numVertex * 3);
    }

    tmpMat = mat4::Translation(0.0f, 0.5f, 0.0f) * mat4::Rotation(halfPI * 3.0f,  vec3(1,0,0));

    for(unsigned i = 0; i < numVertex; ++i )
    {
      Vertex_Pos_Clr_Nrm_Tex tmp;

      tmp.pos = tmpMat.MultPt( verts[i].pos );
      tmp.nrm = tmpMat.MultVec( verts[i].nrm );
      tmp.tex = verts[i].tex;
      tmp.clr = color;

      verts.push_back( tmp );
    }

    for(unsigned i = 0; i < numIndices; ++i ) 
    {
      inds.push_back( inds[i] + numVertex * 4);
    }

    tmpMat = mat4::Translation(0.0f, -0.5f, 0.0f) * mat4::Rotation(halfPI,  vec3(1,0,0));

    for(unsigned i = 0; i < numVertex; ++i )
    {
      Vertex_Pos_Clr_Nrm_Tex tmp;

      tmp.pos = tmpMat.MultPt( verts[i].pos );
      tmp.nrm = tmpMat.MultVec( verts[i].nrm );
      tmp.tex = verts[i].tex;
      tmp.clr = color;

      verts.push_back( tmp );
    }

    for(unsigned i = 0; i < numIndices; ++i ) 
    {
      inds.push_back( inds[i] + numVertex * 5);
    }

    tmpMat = mat4::Translation(0.0f, 0.0f, 0.5f);

    for(unsigned i = 0; i < numVertex; ++i )
    {
      verts[i].pos = tmpMat.MultPt( verts[i].pos );
      verts[i].nrm = tmpMat.MultVec( verts[i].nrm );
    }
  }

  Mesh::~Mesh()
  {
    ReleaseCOM(vertBuffer);
    ReleaseCOM(indBuffer);
  }

  void Mesh::setActive()
  {
    staticSceneElement = false;
  }

  void Mesh::generateGeometryInfo()
  {
    TriangleList worldTris;

    mat4 mtw(modToW);

    mtw.Transpose();

    //Go through the index list and take it three at a time
    for(unsigned i = 0; i < numIndices; i += 3)
    {
      //Generate a worldspace triangle using the model to world matrix
      Vertex_Pos_Clr_Nrm_Tex v1, v2, v3;

      //Set up position
      v1.pos = mtw.MultPt( verts[inds[i + 0]].pos );
      v2.pos = mtw.MultPt( verts[inds[i + 1]].pos );
      v3.pos = mtw.MultPt( verts[inds[i + 2]].pos );

      //Set up normals in worldspace
      v1.nrm = mtw.MultVec( verts[inds[i + 0]].nrm );
      v2.nrm = mtw.MultVec( verts[inds[i + 1]].nrm );
      v3.nrm = mtw.MultVec( verts[inds[i + 2]].nrm );

      //Set up colors
      v1.clr =  verts[inds[i + 0]].clr;
      v2.clr =  verts[inds[i + 1]].clr;
      v3.clr =  verts[inds[i + 2]].clr;

      //Push it back to our list of scene triangles
      libRef->addTriToScene(Triangle(v1,v2,v3));
      worldTris.push_back(Triangle(v1,v2,v3));
    }

    std::vector<Mesh*> m;
    m.push_back(this);
    libRef->addObjectToScene(Object(m, GeometryLibrary::buildAABBFromTriList(worldTris)));
  }

  void Mesh::setMtw()
  {
    modelToWorld->SetMatrix((float*)&modToW);
  }

  void Mesh::setInactive()
  {
    staticSceneElement = true;
  }


  Object::Object( std::vector<Mesh*>& m, AABB& box ) : meshes(m), boundingBox(box)
  {

  }

  void Object::moveX( float xDist )
  {
    static vec3 dir(1,0,0);

    //move all mesh positions
    for(unsigned i = 0; i < meshes.size(); ++i)
      meshes[i]->pos.x += xDist * 0.016f;

    boundingBox.center += dir * xDist * 0.016f;
  }

  void Object::moveY( float yDist )
  {
    static vec3 dir(0,1,0);

    //move all mesh positions
    for(unsigned i = 0; i < meshes.size(); ++i)
      meshes[i]->pos.y += yDist * 0.016f;
    

    boundingBox.center += dir * yDist * 0.016f;
  }

  void Object::moveZ( float zDist )
  {
    static vec3 dir(0,0,1);

    //move all mesh positions
    for(unsigned i = 0; i < meshes.size(); ++i)
      meshes[i]->pos.z += zDist * 0.016f;

    boundingBox.center += dir * zDist * 0.016f;
  }

}
