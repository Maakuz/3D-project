struct GS_OUT
{
    float4 pos : SV_POSITION;
    float4 norm : NORMAL;
    float2 uv : UV;
};

texture2D paricleTex : register(t0);
SamplerState sSampler : register(s0);

struct PS_OUT
{
	float4 position : SV_Target0;
	float4 normal : SV_Target1;
	float4 color : SV_Target2;
	int mtl : SV_Target3;
};


PS_OUT main(in GS_OUT input)
{
	PS_OUT outPut;

    outPut.color = paricleTex.Sample(sSampler, input.uv);
	outPut.position = input.pos;
    outPut.normal = input.norm;
    outPut.mtl = 0;
	
	return outPut;
}