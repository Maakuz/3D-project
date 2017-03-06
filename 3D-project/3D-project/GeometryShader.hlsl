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
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
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

        //maybe that this only  works because our world matrix is a identity matrix.
        //calculats tangent and bitangent for normal mapping
        float3 e1 = input[1].wPos - input[0].wPos;
        float3 e2 = input[2].wPos - input[0].wPos;
        float2 dUV1 = input[1].uv - input[0].uv;
        float2 dUV2 = input[2].uv - input[0].uv;

        // 1 diveded by determinant constant
        float c = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

        //tangent
        float3 t;
        t.x = c * (dUV2.y * e1.x - dUV1.y * e2.x);
        t.y = c * (dUV2.y * e1.y - dUV1.y * e2.y);
        t.z = c * (dUV2.y * e1.z - dUV1.y * e2.z);
        t = normalize(t);

        //bitangent
        float3 bt;
        bt.x = c * (-dUV2.x * e1.x + dUV1.x * e2.x);
        bt.y = c * (-dUV2.x * e1.y + dUV1.x * e2.y);
        bt.z = c * (-dUV2.x * e1.z + dUV1.x * e2.z);
        //float3 bt = cross(t, input[0].norm.xyz);
        bt = normalize(bt);


        for (uint i = 0; i < 3; i++)
        {
            GS_OUT element;
            element.pos = input[i].pos;
            element.wPos = input[i].wPos;
            element.norm = input[i].norm;
            element.uv = input[i].uv;
            element.tangent = t;
            element.bitangent = bt;
            element.mtl = input[i].mtl;
            output.Append(element);
        }
        output.RestartStrip();
    }
}