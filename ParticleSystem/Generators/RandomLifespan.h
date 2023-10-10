#pragma once
#include "../BaseGenerator.h"
#include "../ParticleSystem.h"

class RandomLifespan : public BaseGenerator {
public:
	RandomLifespan() : m_from(0.f), m_to(0.f) {}
	RandomLifespan(float l_from, float l_to) : m_from(l_from), m_to(l_to) {}
	void Generate(Emitter* l_emitter, ParticleContainer* l_particles, size_t l_from, size_t l_to) {
		auto& rand = *l_emitter->GetParticleSystem()->GetRand();
		auto& lifespans = l_particles->m_maxLifespan;
		for (auto i = l_from; i <= l_to; ++i) { lifespans[i] = rand(m_from, m_to); }
	}

	void ReadIn(std::stringstream& l_stream) {
		l_stream >> m_from >> m_to;
	}
private:
	float m_from;
	float m_to;
};