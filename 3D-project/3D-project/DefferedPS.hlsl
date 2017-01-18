SamplerState sSampler; //kan sätta fancy flaggor här
texture2D tex : register(t0);

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 norm : NORMAL;
    float2 uv : TEXCOORD;
};

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 color : SV_Target1;
    float4 normal : SV_Target2;
};

PS_OUT main(VS_OUT input)
{
    PS_OUT outPut;

    outPut.position = input.pos;
    outPut.color = tex.Sample(sSampler, input.uv);
    outPut.normal = input.norm;

    return outPut;
}