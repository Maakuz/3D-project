SamplerState sSampler; //can set flags if needed
texture2D tex : register(t0);
texture2D normalMap : register(t1);

struct GS_OUT
{
    float4 pos : SV_Position;
    float4 wPos : WPOS;
    float4 norm : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    int mtl : MTLNR;
};


struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
    float mtl : SV_Target3;
};

float3 normalMapping(in float3 tangent, in float3 bitangent, in float3 normal, in float3 mapNormal)
{
    //from tangent to worldspace
    float3x3 TBN = float3x3(tangent, bitangent, normal);

    float3 newNormal = mul(mapNormal, TBN);

    return newNormal;
}

PS_OUT main(GS_OUT input)
{
    PS_OUT outPut;

    //converts from rgb to xyz
    float3 mapNormal = 2 * normalMap.Sample(sSampler, input.uv) - 1;

    outPut.normal = float4(normalize(normalMapping(input.tangent, input.bitangent, input.norm.xyz, mapNormal)), 1.0f);

    outPut.color = tex.Sample(sSampler, input.uv);
    outPut.position = input.wPos;

   
    outPut.mtl = input.mtl;

    return outPut;
}