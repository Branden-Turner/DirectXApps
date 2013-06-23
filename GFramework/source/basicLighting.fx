cbuffer cbPerObject
{ 
  float4x4 ModelToWorld;
};

cbuffer cbPerFrame
{
  float4x4 Camera;
  float4x4 CameraProjection;
};

struct ShapeVSInput
{
  float3 m_pos : POSITION;
  float4 m_clr : COLOR;
  float3 m_nrm : NORMAL;
  float2 m_tex : TEXCOORD; 
};

struct ShapeVSOutput
{
 float4 pos : SV_POSITION;
 float4 clr : COLOR;
 float3 psc : POSITION;
 float3 nrm : NORMAL;
 float2 tex : TEXCOORD;
};

ShapeVSOutput VS( ShapeVSInput IN )
{
  ShapeVSOutput OUT;

  OUT.pos = mul(  mul( float4( IN.m_pos, 1.0f ), ModelToWorld ), CameraProjection );
  OUT.psc = mul(  mul( float4( IN.m_pos, 1.0f ), ModelToWorld ), Camera).xyz;

  OUT.nrm = mul(  mul( float4( IN.m_nrm, 0.0f ), ModelToWorld ), Camera).xyz;
	
  // Just pass vertex color into the pixel shader.
  OUT.clr = IN.m_clr;

  OUT.tex = IN.m_tex;

  return OUT;
}

struct ShapePSInput
{
 float4 pos : SV_POSITION;
 float4 clr : COLOR;
 float3 psc : POSITION;
 float3 nrm : NORMAL;
 float2 tex : TEXCOORD;
};



float4 PS( ShapePSInput IN ) : SV_Target
{
  float4 lightDif = float4(0.8f, 0.8f, 0.8f, 1.0f);
  float4 specClr = float4(1.0f,1.0f,1.0f,1.0f);
  float4 ambClr = float4(0.2f, 0.2f, 0.2f, 1.0f);
  float shinyCoeff = 255.0f;

  //Turns out when you put a point light at the start, the view is the same as the light vector
  float3 lVec = normalize(-IN.psc);
  float3 norm = normalize(IN.nrm);
  float NL = max(dot(norm, lVec), 0);

  float3 rVec = normalize( (2 * NL * norm) - lVec);
  float4 specFinal  = pow( max( dot(rVec, lVec), 0) , shinyCoeff) * specClr;
  return ambClr + lightDif * IN.clr * NL + specFinal;
}

technique10 LightTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}