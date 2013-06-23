#ifndef VERT_H
#define VERT_H

namespace cs350
{

  #pragma pack(1)
  struct Vertex_Pos_Clr_Nrm
  {
    vec3 pos;
    vec4 clr;
    vec3 nrm;
  };
  #pragma pack()

  #pragma pack(1)
  struct Vertex_Pos_Clr_Tex
  {
    vec3 pos;
    vec4 clr;
    vec2 tex;
  };
  #pragma pack()

  #pragma pack(1)
  struct Vertex_Pos_Clr_Nrm_Tex
  {
    vec3 pos;
    vec4 clr;
    vec3 nrm;
    vec2 tex;
  };
  #pragma pack()


}

#endif