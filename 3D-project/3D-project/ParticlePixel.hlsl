struct GS_out
{
    float4 pos : SV_Position;
    float2 uv : UV;
};

texture2D paricleTex : register(t0);
SamplerState sSampler : register(s0);


float4 main(in GS_out input) : SV_TARGET
{
    float4 color = paricleTex.sample(sSampler, input.uv);

    return color;
}