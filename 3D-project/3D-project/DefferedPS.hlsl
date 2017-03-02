SamplerState sSampler; //can set flags if needed
texture2D tex : register(t0);

struct GS_OUT
{
    float4 pos : SV_Position;
    float4 wPos : WPOS;
    float4 norm : NORMAL;
    float2 uv : TEXCOORD;
    int mtl : MTLNR;
};


struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
    int mtl : SV_Target3;
};


PS_OUT main(GS_OUT input)
{
    PS_OUT outPut;

    outPut.color = tex.Sample(sSampler, input.uv);
    outPut.position = input.wPos;

    outPut.normal = normalize(input.norm);
    outPut.mtl = input.mtl;

    return outPut;
}