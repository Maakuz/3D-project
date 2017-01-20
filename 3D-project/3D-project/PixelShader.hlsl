SamplerState sSampler; //kan sätta fancy flaggor här
texture2D positions : register(t0);
texture2D normals : register(t1);
texture2D colors : register(t2);

cbuffer lightBuffer : register(b0)
{
    float3 lightColor;
    float4 lightPos;
    float3 lightDir;
    float4 lightRange;
    float2 spotlightAngle;
};

cbuffer cameraPos : register(b1)
{
    float4 camPos;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 posVS : VSPOS;
    float2 uv : TEXCOORD;
};

float4 main(VS_OUT input) : SV_TARGET
{
    float4 difLightDir = normalize(positions.Sample(sSampler, input.uv) - lightPos);
    float diffuse = saturate(dot(difLightDir, normals.Sample(sSampler, input.uv)));

    return colors.Sample(sSampler, input.uv) * diffuse;
}