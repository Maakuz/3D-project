struct VS_IN
{
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 norm : NORMAL;
    float2 tex : TEXCOORD;
};

cbuffer WVPMatrixBuffer
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

    output.pos = float4(input.pos, 1);
    output.norm = float4(input.norm, 0);
    output.tex = input.tex;

    output.pos = mul(output.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);

    return output;
}