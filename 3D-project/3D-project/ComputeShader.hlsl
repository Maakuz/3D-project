struct Particle
{
    float3 position;
    float3 velocity;
    float time;
};
AppendStructuredBuffer<Particle> nextSimState : register(u0);
ConsumeStructuredBuffer<Particle> CurrentSimState : register(u1);

cbuffer currentTime
{
    float time;
};

cbuffer nrOfParticles
{
    uint nrOfParticles;
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

        //calculate next particle
        currentParticle.position = currentParticle.velocity * time;

        currentParticle.time += time;

        if(currentParticle.time < 1000000.0f)
        {
            nextSimState.Append(currentParticle);
        }
    }
}