#include "TrackParticle.h"
#include "Random.h"

void TrackParticle::Particle::Update()
{
	velocity += accel;
	Vector3 parentSpd = parent->GetWorldPosition() - parentPrePos;
	position += parentSpd;
	parentPrePos = parent->GetWorldPosition();
	position += velocity;
	float f = 1.0f / frame.GetRemainTimeRate();
	scale = s_scale + (e_scale - s_scale) / f;
}


void TrackParticle::Add(const AddProp& particleProp)
{
	Random_Float randPos(-particleProp.posRange, particleProp.posRange);
	Random_Float randVel(-particleProp.velRange, particleProp.velRange);
	Random_Float randAcc(-particleProp.accRange, particleProp.accRange);

	for (unsigned short i = 0; i < particleProp.addNum; i++)
	{
		particles.emplace_front();
		Particle& p = particles.front();
		p.parent = particleProp.parent;
		p.parentPrePos = p.parent->GetWorldPosition();
		p.position = Vector3(randPos(), randPos(), randPos()) + particleProp.posOffset;
		p.velocity = Vector3(randVel(), randVel(), randVel()) + particleProp.velOffset;
		p.accel = Vector3(randAcc(), randAcc(), 0) + particleProp.accOffset;
		p.frame = particleProp.lifeTime;
		p.scale = p.s_scale = particleProp.start_scale;
		p.e_scale = particleProp.end_scale;
	}
}

void TrackParticle::Update()
{
	particles.remove_if([](Particle& particle) { return particle.frame.CountDown(); });
	for (auto& particle : particles) { particle.Update(); }
}