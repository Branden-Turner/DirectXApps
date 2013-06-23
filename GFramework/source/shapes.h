#ifndef SHAPE_H
#define SHAPE_H

namespace cs350
{
  enum MeshType
  {
    MeshPlane,
    MeshCube,
    MeshSphere,
    MeshModel,
    MaxMeshTypes
  };

  class Mesh
  {
    public:
      Mesh( GeometryLibrary* lib, ID3D10EffectMatrixVariable* mtw, ID3D10Device* dev, unsigned slices, unsigned stacks, 
             const D3DXVECTOR3& _pos, const D3DXVECTOR3& _sc, const D3DXVECTOR3& _ax, float _angle, const D3DXVECTOR4& _clr, MeshType _type);
      ~Mesh();
      void draw();
      void update();
      MeshType type() const;
      void setActive();
      void setInactive();
      void setMtw();

      //World space qualities
      D3DXVECTOR3 pos;
      D3DXVECTOR3 scale;
      D3DXVECTOR3 axis;
      float angle;
      D3DXVECTOR4 color;

      D3DXMATRIX modToW;

    friend class MeshManager;
    friend class GeometryLibrary;
    private:
      std::vector<DWORD> inds;
      std::vector<Vertex_Pos_Clr_Nrm_Tex> verts;
      void getIndBuffer();
      void getVertBuffer();
      void buildMtw();
      void buildIndexBuffer();
      void generateGeometryInfo();
      unsigned numVerts;
      unsigned numFaces;
      unsigned numIndices;
      MeshType type_;
      ID3D10Buffer* vertBuffer;
      ID3D10Buffer* indBuffer;
      ID3D10Device* device;
      ID3D10EffectMatrixVariable* modelToWorld;
      unsigned slices_, stacks_;
      bool staticSceneElement;
      void PlaneShape();
      void SphereShape();
      void CubeShape();
      
      
      GeometryLibrary* libRef;
  };

  class MeshManager
  {
    public:
      MeshManager(ID3D10Device* dev, ID3D10EffectMatrixVariable* mtow, GeometryLibrary* gLib);
      ~MeshManager();
      void drawMeshes();
      void drawMesh(unsigned index);
      void updateMeshes();
      unsigned numMeshes();
      void buildMesh( unsigned slices, unsigned stacks, const D3DXVECTOR3& _pos, const D3DXVECTOR3& _sc, const D3DXVECTOR3& _ax, float _angle, const D3DXVECTOR4& _clr, MeshType _type);
      void setMtw(unsigned index);

    private:
      std::vector<Mesh*> meshes;
      ID3D10Device* device;
      ID3D10EffectMatrixVariable* mtw;
      GeometryLibrary* lib;
  };

  //A graphics object is simply a collection of meshes, with a bounding volume
  struct Object
  {
    Object( std::vector<Mesh*>& m, AABB& box );
    std::vector<Mesh*> meshes;
    AABB boundingBox;
    void moveX( float xDist );
    void moveY( float yDist );
    void moveZ( float zDist );
  };

  

}

#endif