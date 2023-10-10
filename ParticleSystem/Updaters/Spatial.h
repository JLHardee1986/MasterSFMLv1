#pragma once
#include "../BaseUpdater.h"
#include "../ParticleContainer.h"

class SpatialUpdater : public BaseUpdater {
public:
	void Update(float l_dT, ParticleContainer* l_particles) {
		auto& velocities = l_particles->m_velocity;
		auto& accelerations = l_particles->m_acceleration;
		for (size_t i = 0; i < l_particles->m_countAlive; ++i) { velocities[i] += accelerations[i] * l_dT; }
		auto& positions = l_particles->m_position;
		for (size_t i = 0; i < l_particles->m_countAlive; ++i) { positions[i] += velocities[i] * l_dT; }
	}
};