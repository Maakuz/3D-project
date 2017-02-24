SamplerState sSampler; //kan s�tta fancy flaggor h�r
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
    float4 camPos;
};

struct mtlStruct
{
    float4 ambient;
    float3 diff;
    float4 specular;
};

cbuffer mtlLightBuffer : register(b2)
{
    mtlStruct mtls[10];
};

struct VS_OUT
{
	float4 pos : SV_Position;
	float4 posVS : VSPOS;
	float4 lightPos : LIGHTPOSITION;
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

	//For directional lights only
    lVec = -lightDir;
    //  //attenuation here somewhere
    //distance = length(lVec);
    //attenuation = max(0, 1.f - (distance / lightRange.x));

    //lVec /= distance;

    diffuse = saturate(dot(normalize(lVec.xyz), normals.Sample(sSampler, input.uv)));
    diffuse *= lightColor.xyz * colors.Sample(sSampler, input.uv);
    
   //TODO: Specularity

    lighting = float4(diffuse, 1);

	//Shadow mapping
	input.lightPos.xyz /= input.lightPos.w;

	//Light frustum culling maybe
	/*if (input.lightPos.x < -1.f || input.lightPos.y < -1.f	|| input.lightPos.x > 1.f || 
		input.lightPos.y > 1.f	|| input.lightPos.z < 0.f	|| input.lightPos.z > 1.f)
		lighting = float4(0.1, 0.1, 0.1, 1);
*/

	//Convert to texture coords
	input.lightPos.x = (input.lightPos.x * 0.5) + 0.5;
	input.lightPos.y = (input.lightPos.y * -0.5) + 0.5;

	float depth = shadowMap.Sample(sSampler, input.lightPos.xy).x;

	if (depth < input.lightPos.z)
		lighting *= float4(0.5, 0.5, 0.5, 1);



	//return lightColor;
    //return float4(shadowMap.Sample(sSampler, input.uv).xyz, 1);
    return  lighting;
}