struct VS_OUT
{
    float4 pos : SV_Position;
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

[maxvertexcount(3)]
void main(triangle VS_OUT input[3] : SV_POSITION, inout TriangleStream<GS_OUT> output)
{
    float3 check = cross(input[1].pos.xyz - input[0].pos.xyz, input[2].pos.xyz - input[0].pos.xyz);

    if (sign(check.z) == -1)
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