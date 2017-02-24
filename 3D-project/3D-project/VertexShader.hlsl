struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 posVS : VSPOS;
    float2 uv : TEXCOORD;
};

cbuffer matrixBuffer : register(b0)
{
    matrix WVMatrix;
    matrix invProjMatrix;
};

float4 getWVPos()
{
    // Calculate the view space vertex position
   // float4 pos = mul(input.PositionOS, WorldViewMatrix);

    // For a directional light we can clamp in the vertex shader, since we only interpolate in the XY direction
    //float3 positionVS = mul(input.PositionOS, InvProjMatrix);
    //output.ViewRay = float3(positionVS.xy / positionVS.z, 1.0f);

}

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

    output.pos = float4(input.pos, 1);
    //output.posVS = getWVPos();
    output.uv = input.uv;
	
    return output;
}