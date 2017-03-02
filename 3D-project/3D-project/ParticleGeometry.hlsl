struct GS_OUT
{
	float4 pos : SV_POSITION;
    float4 norm : NORMAL;
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

static const float4 quadCorners[4] =
{
    float4(-0.016f, 0.016f, 0.0f, 0.0f),
    float4(0.016f, 0.016f, 0.0f, 0.0f),
    float4(-0.016f, -0.016f, 0.0f, 0.0f),
    float4(0.016f, -0.016f, 0.0f, 0.0f)
};

static const float2 quadTexCoords[4] =
{
    float2(1.0f, 0.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(0.0f, 1.0f)
};

[maxvertexcount(4)]
void main(point VS_OUT input[1], inout TriangleStream< GS_OUT> tStream)
{
    //this shader takes points and blows them up to billboarded quads.
    GS_OUT output;

    float4 pos = mul(float4(input[0].position, 1.0f), world);
    pos = mul(pos, view);
    pos = mul(pos, projection);
    float4 normal = float4(cross((pos + quadCorners[0]).xyz, (pos + quadCorners[1]).xyz), 1.0f);

    for (int i = 0; i < 4; i++)
    {
        //this results in a billboarded particle since the quad is created facing the camera in projection space
        output.pos = pos + quadCorners[i];
        output.norm = normal;
        output.uv = quadTexCoords[i];
        tStream.Append(output);
    }
    tStream.RestartStrip();
}