#pragma once

class BaseUpdater {
public:
	virtual ~BaseUpdater() {}
	virtual void Update(float l_dT, ParticleContainer* l_particles) = 0;
};