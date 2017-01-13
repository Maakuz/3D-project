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
<<<<<<< HEAD
	return input.norm;
=======
	return float4(input.norm);
>>>>>>> 49f8c98c12481bba2a7ee5f3110f52bb466eb41e
}