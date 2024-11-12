cbuffer ConstantBuffer : register(b0)
{
    matrix mWorld;
    matrix mView;
    matrix mProjection;
    float4 vMeshColor;
}; 


struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float4 COLOR: COLOR0;
};

VS_OUTPUT VSMain( VS_INPUT input )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( input.Pos, mWorld );
    output.Pos = mul( output.Pos, mView );
    output.Pos = mul( output.Pos, mProjection );
    output.Tex = input.Tex;
    output.COLOR = vMeshColor;
    return output;
}

