SamplerState sSampler; //can set flags if needed
texture2D tex : register(t0);

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 wPos : WPOS;
    float4 norm : NORMAL;
    float2 uv : TEXCOORD;
};


struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};

cbuffer mtlLightBuffer
{
    float3 ambient;
    float3 diffuse;
};

PS_OUT main(VS_OUT input)
{
    PS_OUT outPut;

    outPut.position = input.pos;
    outPut.color = tex.Sample(sSampler, input.uv);
    outPut.color = float4(saturate((outPut.color.xyz  * diffuse) + (outPut.color.xyz * ambient)), 1.0f);
    outPut.position = input.wPos;
    outPut.normal = input.norm;


    return outPut;
}