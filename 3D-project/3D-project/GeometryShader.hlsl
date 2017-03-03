struct VS_OUT
{
    float4 pos : Position;
    float4 wPos : WPOS;
    float4 norm : NORMAL;
    float2 uv : TEXCOORD;
    int mtl : MTLNR;
};

struct GS_OUT
{
    float4 pos : SV_Position;
    float4 wPos : WPOS;
    float4 norm : NORMAL;
    float2 uv : TEXCOORD;
    int mtl : MTLNR;
};

cbuffer cameraBuffer
{
    float3 cameraPos;
};

[maxvertexcount(3)]
void main(triangle VS_OUT input[3] : SV_POSITION, inout TriangleStream<GS_OUT> output)
{
    //creates vector from camera to triangle
    float3 cToT = cameraPos - input[0].wPos.xyz;

    float check = dot(cToT, input[0].norm.xyz);

    //checks if backfacing
    if (sign(check) >= 0)
    {
        for (uint i = 0; i < 3; i++)
        {
            GS_OUT element;
            element.pos = input[i].pos;
            element.wPos = input[i].wPos;
            element.norm = input[i].norm;
            element.uv = input[i].uv;
            element.mtl = input[i].mtl;
            output.Append(element);
        }
        output.RestartStrip();
    }
}