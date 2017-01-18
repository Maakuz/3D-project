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
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
};

PS_OUT DeferredPixelShader(VS_OUT input) : SV_TARGET
{
    PS_OUT outPut;

    outPut.color = tex.Sample(sSampler, input.uv);
    outPut.normal = input.norm;

    return outPut;
}