struct Particle
{
    float4 position;
    float3 velocity;
    float age;
};
AppendStructuredBuffer<Particle> nextSimState : register(u0);

cbuffer emitterLocation
{
    float4 emitterLocation;
    float4 randomVector;
}

cbuffer nrOfParticles
{
    uint nrOfParticles;
};

static const float3 reflectVectors[12] =
{
    float3(1.0f, 0.0f, 0.0f),
    float3(-1.0f,0.0f, 0.0f),
    float3(0.0f, 1.0f, 0.0f),
    float3(0.0f, -1.0f, 0.0f),
    float3(0.0f, 0.0f, 1.0f),
    float3(0.0f, 0.0f, -1.0f),
    float3(1.0f, 1.0f, 0.0f),
    float3(-1.0f, -1.0f, 0.0f),
    float3(0.0f, 1.0f, 1.0f),
    float3(0.0f, -1.0f, -1.0f),
    float3(1.0f, 0.0f, 1.0f),
    float3(-1.0f, 0.0f, -1.0f)
};


[numthreads(12, 1, 1)]
void main( uint3 GTID : SV_GroupThreadID )
{
    //checks that this insert doesnt overflow the buffer
    if(nrOfParticles <= 500)
    {
        Particle newParticle;

        newParticle.position = emitterLocation;

        //using reflect for a smoth distibution. 
        newParticle.velocity = reflect(randomVector.xyz, reflectVectors[GTID.x]);

        //so the particle has a random lifetime 
        newParticle.age = saturate(randomVector.x) * 200.0f;

        nextSimState.Append(newParticle);
    }
}