Texture2D texture0 : register(t0);
SamplerState samplerState : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
};
float4 PSMain(PS_INPUT input) : SV_Target
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}