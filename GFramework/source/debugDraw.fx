cbuffer cbPerFrame
{
  float4x4 CameraProjection;
};

struct LineVSInput
{
  float3 m_pos : POSITION;
  float4 m_clr : COLOR;
  float3 m_nrm : NORMAL;
  float2 m_tex : TEXCOORD; 
};

struct LineVSOutput
{
 float4 pos : SV_POSITION;
 float4 clr : COLOR;
};

LineVSOutput VS( LineVSInput IN )
{
  LineVSOutput OUT;

  OUT.pos = mul( float4( IN.m_pos, 1.0f ), CameraProjection );
  OUT.clr = IN.m_clr;

  return OUT;
}

struct LinePSInput
{
 float4 pos : SV_POSITION;
 float4 clr : COLOR;
};



float4 PS( LinePSInput IN ) : SV_Target
{
  return IN.clr;
}

technique10 LineTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}