SamplerState sSampler; //kan sätta fancy flaggor här
texture2D tex : register(t0);

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 norm : NORMAL;
    float2 uv : TEXCOORD;
};

float4 main(VS_OUT input) : SV_TARGET
{
    //Kanskse borde ändras lite
    float4 color = tex.Sample(sSampler, input.uv);

    return color;
}