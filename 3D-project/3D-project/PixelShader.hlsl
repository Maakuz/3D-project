SamplerState sSampler; //kan s�tta fancy flaggor h�r
texture2D tex : register(t0);

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 norm : NORMAL;
    float2 uv : TEXCOORD;
};

float4 main(VS_OUT input) : SV_TARGET
{
    //Kanskse borde �ndras lite
    float4 color = tex.Sample(sSampler, input.uv);

    return color;
}