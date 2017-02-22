struct Particle
{
    float4 position;
    float3 velocity;
    float age;
};

ConsumeStructuredBuffer<Particle> CurrentSimState : register(u0);
AppendStructuredBuffer<Particle> nextSimState : register(u1);


cbuffer nrOfParticles
{
    uint nrOfParticles;
};

cbuffer currentTime
{
    //time in ms
    float time;
};

[numthreads(512, 1, 1)]
void main( uint3 DTID : SV_DispatchThreadID )
{
    //flatten id
    uint threadID = DTID.x + DTID.y * 512 + DTID.z * 512 * 512;

    //check if this thread has a particle to update
    if(threadID < nrOfParticles)
    {
        Particle currentParticle = CurrentSimState.Consume();

        //calculate next postion
        currentParticle.position.xyz += float3(currentParticle.velocity * time * 0.001);

        currentParticle.age += time;

        if(currentParticle.age < 15000.0f)
        {
            nextSimState.Append(currentParticle);
        }
    }
}