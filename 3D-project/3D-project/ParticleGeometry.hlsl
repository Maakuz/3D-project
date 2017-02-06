struct GS_OUT
{
	float4 pos : SV_POSITION;
    float2 uv : UV;
};

struct VS_OUT
{
    float3 position : POSITION;
};

cbuffer WVPMatrixBuffer
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};

static const float4 qCorners[4] =
{
    float4(-1.0f, 1.0f, 0.0f, 0.0f),
    float4(1.0f, 1.0f, 0.0f, 0.0f),
    float4(-1.0f, -1.0f, 0.0f, 0.0f),
    float4(1.0f, -1.0f, 0.0f, 0.0f)
};

static const float2 qTexCoords[4] =
{
    float2(0.0f, 1.0f),
    float2(1.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f)
};

[maxvertexcount(4)]
void main(point VS_OUT input[1], inout TriangleStream< GS_OUT> tStream)
{
    GS_OUT output;

    float4 pos = mul(float4(input[0].position, 1.0f), world);
    pos = mul(pos, view);

    for (int i = 0; i < 4; i++)
    {
        output.pos = mul(pos + qCorners[i], projection);
        output.uv = qTexCoords[i];
        tStream.Append(output);
    }
    tStream.RestartStrip();
}