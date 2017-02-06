struct Particle
{
    float3 position;
    float3 velocity;
    float age;
};
StructuredBuffer<Particle> currentSimState;

struct VS_IN
{
    uint vertexID : SV_VertexID;
};

struct VS_OUT
{
    float3 position : POSITION;
};

VS_OUT main( in VS_IN input)
{
    VS_OUT output;
	
    output.position = currentSimState[input.vertexID].position;
}