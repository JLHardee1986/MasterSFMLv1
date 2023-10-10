#pragma once
#include "../BaseGenerator.h"
#include "../Emitter.h"

class PointPosition : public BaseGenerator {
public:
	void Generate(Emitter* l_emitter, ParticleContainer* l_particles, size_t l_from, size_t l_to) {
		auto& positions = l_particles->m_position;
		auto center = l_emitter->GetPosition();
		for (auto i = l_from; i <= l_to; ++i) { positions[i] = center; }
	}
};