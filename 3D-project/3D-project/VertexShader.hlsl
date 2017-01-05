struct VS_IN
{
    float3 pos : POSITION;
    float3 color : COLOR;
};


float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}