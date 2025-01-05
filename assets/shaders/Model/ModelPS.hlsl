Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D metallicRoughnessTexture : register(t2);

SamplerState samplerState : register(s0);

cbuffer MaterialBuffer : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
    float4 specularColor;
    float specularPower;
};

struct PS_INPUT
{
    float4 Pos    : SV_POSITION;
    float3 Normal : NORMAL;
    float2 Tex    : TEXCOORD0;
};

float4 PSMain(PS_INPUT input) : SV_Target
{
    // Normalize the input normal
    float3 normal = normalize(input.Normal);
    
    // Sample the texture
    float4 texColor = albedoTexture.Sample(samplerState, input.Tex);
    
    // Basic lighting direction (can be moved to constant buffer)
    float3 lightDir = normalize(input.Pos);
    
    // Calculate diffuse lighting
    float diffuseIntensity = max(dot(normal, lightDir), 0.0f);
    
    // Combine ambient and diffuse lighting with texture color
    float4 finalColor = texColor * (ambientColor + diffuseColor * diffuseIntensity);
    finalColor.a = texColor.a;
    
    return finalColor;

    //return float4(1.f, 0.f, 0.f, 1.0f);
}