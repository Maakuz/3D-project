sampler sSampler;
texture tex : register(t0);

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 norm : NORMAL;
    float2 tex : TEXCOORD;
};

float4 main(VS_OUT input) : SV_TARGET
{
    //Kanskse borde ändras lite
	return input.norm;

}