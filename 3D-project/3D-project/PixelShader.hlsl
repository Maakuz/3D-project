SamplerState sSampler; //kan s�tta fancy flaggor h�r
texture2D positions : register(t0);
texture2D normals : register(t1);
texture2D colors : register(t2);

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

float4 main(VS_OUT input) : SV_TARGET
{
    //Kanskse borde �ndras lite
    //float4 color = tex.Sample(sSampler, input.uv);

    return float4(1, 0, 1, 1);
}