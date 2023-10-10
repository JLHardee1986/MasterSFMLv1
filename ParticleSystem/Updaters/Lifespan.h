#pragma once
#include "../BaseUpdater.h"
#include "../ParticleContainer.h"

class LifespanUpdater : public BaseUpdater {
public:
	void Update(float l_dT, ParticleContainer* l_particles) {
		auto& lifespans = l_particles->m_lifespan;
		auto& maxLifespans = l_particles->m_maxLifespan;
		for (size_t i = 0; i < l_particles->m_countAlive;) {
			lifespans[i] += l_dT;
			if (lifespans[i] < maxLifespans[i]) { ++i; continue; }
			l_particles->Disable(i);
		}
	}
};