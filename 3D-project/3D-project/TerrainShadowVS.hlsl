struct VS_IN
{
	float3 pos : POSITION;
	float3 norm : NORMAL;
	float2 uv : TEXCOORD;
	int mtl : MTLNR;
};


struct VS_OUT
{
	float4 pos : SV_POSITION;
};

cbuffer lightWVP
{
	matrix lightWorld;
	matrix lightView;
	matrix lightProjection;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = float4(input.pos, 1);

	output.pos = mul(output.pos, lightWorld);
	output.pos = mul(output.pos, lightView);
	output.pos = mul(output.pos, lightProjection);

	return output;
}