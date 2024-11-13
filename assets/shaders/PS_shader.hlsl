Texture2D txDiffuse : register( t0 );
SamplerState samplerLinear : register( s0 );

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float4 COLOR: COLOR0;   
};

struct PS_OUTPUT
{
    float4 Color : SV_Target;
};

PS_OUTPUT PSMain( PS_INPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.Color = txDiffuse.Sample( samplerLinear, input.Tex);

    return output;
}
