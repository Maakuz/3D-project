SamplerState sSampler; //kan sätta fancy flaggor här
texture2D positions : register(t0);
texture2D normals : register(t1);
texture2D colors : register(t2);
texture2D mtl : register(t3);
texture2D shadowMap : register(t4);


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
    float3 camPos;
};

struct mtlStruct
{
    float4 ambient;
    float4 diff;
    float4 specular;
};

cbuffer mtlLightBuffer : register(b2)
{
    mtlStruct mtls[10];
};

cbuffer lightMatrixes : register(b3)
{
    matrix lightWorld;
    matrix lightView;
    matrix lightProjection;
};

struct VS_OUT
{
	float4 pos : SV_Position;
	float4 posVS : VSPOS;
	float2 uv : TEXCOORD;
};

float4 main(VS_OUT input) : SV_TARGET
{

   float4 color = colors.Sample(sSampler, input.uv);
   float4 pPos = positions.Sample(sSampler, input.uv);
   float4 norm = normals.Sample(sSampler, input.uv);

   int m = (int)mtl.Sample(sSampler, input.uv);
    if(m == 10)
    {
        return color;
    }
  
    float4 lVec;
    float4 lighting;
    float3 diffuse;
    float3 ambient;
    float3 specular;
    float weight;
    float attenuation;


    //Light vector
    lVec = lightPos - positions.Sample(sSampler, input.uv);

	//For directional lights only
    lVec = -lightDir;
    
    float nVSL = saturate(dot(normalize(lVec.xyz), norm));

    diffuse = nVSL;
    diffuse *= lightColor.xyz * color.xyz * mtls[m].diff.xyz;

    ambient = color.xyz * mtls[m].ambient.xyz;


   
    float3 camera2Pixel = camPos - pPos.xyz;
    //used to reflect
    float3 H = normalize(lVec.xyz + camera2Pixel);
    specular = pow(saturate(dot(norm.xyz, H)), mtls[m].specular.w) * lightColor.xyz * mtls[m].specular.xyz * nVSL;


    lighting = float4(diffuse + ambient + specular, 1);

//*************************Shadow mapping*********************************
    float4 posFromLight = pPos;

    posFromLight = mul(posFromLight, lightView);
    posFromLight = mul(posFromLight, lightProjection);

	//Convert to texture coords
    posFromLight.x = (posFromLight.x * 0.5) + 0.5;
    posFromLight.y = (posFromLight.y * -0.5) + 0.5;


    float depth = shadowMap.Sample(sSampler, posFromLight.xy).x;

    if (depth < posFromLight.z - 0.01)
		lighting *= float4(0.5, 0.5, 0.5, 1);

//********************Shadow mapping end*********************************

    return  lighting;
}