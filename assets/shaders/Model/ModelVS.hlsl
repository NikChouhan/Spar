cbuffer ConstantBuffer : register(b0)
{
    matrix mWorld;
    matrix mView;
    matrix mProjection;
};

struct VS_INPUT
{
    float3 Pos    : POSITION;
    float3 Normal : NORMAL;
    float2 Tex    : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos    : SV_POSITION;
    float3 Normal : NORMAL;
    float2 Tex    : TEXCOORD0;
};
    
VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    float4 worldPos = mul(float4(input.Pos, 1.0f), mWorld);
    output.Pos = mul(worldPos, mView);
    output.Pos = mul(output.Pos, mProjection);
    output.Normal = mul(float4(input.Normal, 1.0), mWorld).xyz;
    output.Tex = input.Tex;
    return output;
}