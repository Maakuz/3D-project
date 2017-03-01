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

static const float3 reflectVectors[8] =
{
    float3(1.0f, 0.0f, 0.0f),
    float3(-1.0f,0.0f, 0.0f),
    float3(0.0f, 1.0f, 0.0f),
    float3(0.0f, -1.0f, 0.0f),
    float3(0.0f, 0.0f, 1.0f),
    float3(0.0f, 0.0f, -1.0f),
    float3(-1.0f, 1.0f, 0.0f),
    float3(-1.0f, -1.0f, 0.0f)
};


[numthreads(8, 1, 1)]
void main( uint3 GTID : SV_GroupThreadID )
{
    if(nrOfParticles <506)
    {
        Particle newParticle;

        newParticle.position = emitterLocation;

        newParticle.velocity = reflect(randomVector.xyz, reflectVectors[GTID.x]);

        newParticle.age = normalize(randomVector.x) * 200.0f;

        nextSimState.Append(newParticle);
    }
}