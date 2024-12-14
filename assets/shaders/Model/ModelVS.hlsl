cbuffer ConstantBuffer : register(b0)
{
    matrix mWorld;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float4 worldPos = mul(float4(input.Pos, 1.0f), mWorld);
    output.Pos = worldPos;
    output.Normal = mul((float3x3)mWorld, input.Normal);
    output.Tex = input.Tex;
    return output;
}