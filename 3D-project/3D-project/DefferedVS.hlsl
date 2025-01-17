struct VS_IN
{
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float2 uv : TEXCOORD;
    int mtl : MTLNR;
    float3 offset : OFFSET;
};

struct VS_OUT
{
    float4 pos : Position;
    float4 wPos : WPOS;
    float4 norm : NORMAL;
    float2 uv : TEXCOORD;
    int mtl : MTLNR;
};

cbuffer WVPMatrixBuffer
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    input.pos.x += input.offset.x;
    input.pos.y += input.offset.y;
    input.pos.z += input.offset.z;

    output.pos = float4(input.pos, 1);

    output.wPos = float4(input.pos, 1);
    output.wPos = mul(output.wPos, world);

    
    output.norm = float4(input.norm, 0);
    output.norm = normalize(mul(output.norm, world));

    output.uv = input.uv;

    output.pos = mul(output.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);

    return output;
}