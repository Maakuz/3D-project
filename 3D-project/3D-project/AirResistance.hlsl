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
void main(uint3 DTID : SV_DispatchThreadID)
{
    //flatten id
    uint threadID = DTID.x + DTID.y * 512 + DTID.z * 512 * 512;

    //check if this thread has a particle to update
    if (threadID < nrOfParticles)
    {
        Particle currentParticle = CurrentSimState.Consume();

        //calculate next postion
        currentParticle.position.xyz += float3(currentParticle.velocity * time * 0.001);


        currentParticle.velocity.y = currentParticle.velocity.y  - 9.82f * time * 0.001;
        //f = ((p * Cd * A)/2) * v^2
        float arx = ((0.00129f * 0.8f * (0.016f * 0.016f)) / 2.0f) * currentParticle.velocity.x * currentParticle.velocity.x;
        float ary = ((0.00129f * 0.8f * (0.016f * 0.016f)) / 2.0f) * currentParticle.velocity.y * currentParticle.velocity.y;
        float arz = ((0.00129f * 0.8f * (0.016f * 0.016f)) / 2.0f) * currentParticle.velocity.z * currentParticle.velocity.z;

        // f = ma, a = f/m
        float ax = arx / 0.01f;
        float ay = ary / 0.01f;
        float az = arz / 0.01f;

        //v = v0 + at
        currentParticle.velocity.x = currentParticle.velocity.x + -1 * sign(currentParticle.velocity.x) * ax * time * 0.001;
        currentParticle.velocity.y = currentParticle.velocity.y + -1 * sign(currentParticle.velocity.y) * ay * time * 0.001;
        currentParticle.velocity.z = currentParticle.velocity.z + -1 * sign(currentParticle.velocity.z) * az * time * 0.001;


        

        currentParticle.age += time;

        if (currentParticle.age < 15000.0f)
        {
            nextSimState.Append(currentParticle);
        }
    }
}