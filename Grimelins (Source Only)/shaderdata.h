/*! ====================================================================================================================
  @file shaderdata.h

  Codename Virtus

  @author Joseph Busch <joseph.busch@digipen.edu>
  @date Sep.18.2011

  @brief
    Variety of possible structs to send to the shader

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_SHADERDATA_H
#define VIRTUS_SHADERDATA_H

namespace Grimelins {

      // All of these should be multiples of float4's or
      // shit will explode and what not.

      // Also DefaultMatrix is most probably set every frame
    struct DefaultVSData
    {
      float4x4 Projection;
      float4x4 Camera;
      float4x4 CameraProjection;
    };

    struct DebugLinePSData
    {
      float4 Color;
      float4 Vars;
    };

} // namespace Virtus

#endif // Include Guard