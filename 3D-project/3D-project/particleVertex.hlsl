struct Particle
{
    float4 position;
    float3 velocity;
    float age;
};
StructuredBuffer<Particle> currentSimState;

struct VS_OUT
{
    float3 position : POSITION;
};

VS_OUT main( uint vertexID : SV_VertexID)
{
    VS_OUT output;
	
    output.position = currentSimState[vertexID].position.xyz;
    
    return output;
}