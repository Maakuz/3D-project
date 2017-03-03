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
	
    //creates the verticies with the vertexId as the index of current sim buffer
    output.position = currentSimState[vertexID].position.xyz;
    
    return output;
}