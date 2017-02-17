struct GS_out
{
    float4 pos : SV_Position;
    float2 uv : UV;
};

texture2D paricleTex : register(t0);
SamplerState sSampler : register(s0);


float4 main(in GS_out input) : SV_TARGET
{
    float4 color = paricleTex.Sample(sSampler, input.uv);
    //float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);

    return color;
}