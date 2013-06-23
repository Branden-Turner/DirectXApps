/*! ====================================================================================================================
  @file shapes.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.26.2011

  @brief
    List of shape types, precompiled so everyone has the enums...

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_SHAPES_H
#define VIRTUS_SHAPES_H

namespace Grimelins {

  enum ShapeType
  {
    ShapePlane,
    ShapeCube,
    ShapeSphere,
    ShapeCylinder,
    ShapeCone,
    ShapeTorus,
    ShapeQuad2D, // Quads do not have normals. 
    ShapeQuad3D, // Quads do not have normals. 
    ShapeSkybox, // Skybox is facing inwards and does not have normals. 
    Shapes_MAX
  };

}

#endif // Include Guard