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

static const float3 direction[8] =
{
    normalize(float3(1.0f, 1.0f, 1.0f)),
    normalize(float3(-1.0f, 1.0f, 1.0f)),
    normalize(float3(-1.0f, -1.0f, 1.0f)),
    normalize(float3(1.0f, -1.0f, 1.0f)),
    normalize(float3(1.0f, 1.0f, -1.0f)),
    normalize(float3(-1.0f, 1.0f, -1.0f)),
    normalize(float3(-1.0f, -1.0f, -1.0f)),
    normalize(float3(1.0f, -1.0f, -1.0f))
};


[numthreads(8, 1, 1)]
void main( uint3 GTID : SV_GroupThreadID )
{
    Particle newParticle;

    newParticle.position = emitterLocation;

    newParticle.velocity = normalize(cross(direction[GTID.x], randomVector.xyz));

    newParticle.age = randomVector.x * 4.0f;

    nextSimState.Append(newParticle);
}