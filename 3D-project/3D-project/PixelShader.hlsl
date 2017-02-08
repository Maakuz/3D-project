SamplerState sSampler; //kan sätta fancy flaggor här
texture2D positions : register(t0);
texture2D normals : register(t1);
texture2D colors : register(t2);
texture2D mtl : register(t3);

cbuffer lightBuffer : register(b0)
{
    float4 lightPos;
    float4 lightColor;
    float4 lightDir;
    float4 spotlightAngle;
    float4 lightRange;
};

cbuffer cameraPos : register(b1)
{
    float4 camPos;
};

struct mtlStruct
{
    float4 ambient;
    float3 diff;
    float4 specular;
};

cbuffer mtlLightBuffer : register(b1)
{
    mtlStruct mtls[10];
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 posVS : VSPOS;
    float2 uv : TEXCOORD;
};

float4 main(VS_OUT input) : SV_TARGET
{
   /* int mtl = positions.Sample(sSampler, input.uv);
    float4 difLightDir = normalize(positions.Sample(sSampler, input.uv) - lightPos);
    float diffuse = saturate(dot(difLightDir, normals.Sample(sSampler, input.uv)));

    return colors.Sample(sSampler, input.uv);*/

    float4 lVec;
    float4 lighting;
    float3 diffuse;
    float distance;
    float attenuation;

    //Light vector
    lVec = lightPos - positions.Sample(sSampler, input.uv);

    lVec = -lightDir;

    //  //attenuation here somewhere
    //distance = length(lVec);
    //attenuation = max(0, 1.f - (distance / lightRange.x));

    //lVec /= distance;

    diffuse = saturate(dot(normalize(lVec.xyz), normals.Sample(sSampler, input.uv)));
    diffuse *= lightColor.xyz * colors.Sample(sSampler, input.uv);
    
   //TODO: Specularity

    lighting = float4(diffuse, 1);

    return float4(positions.Sample(sSampler, input.uv).xyz, 1);
    return  lighting;
}