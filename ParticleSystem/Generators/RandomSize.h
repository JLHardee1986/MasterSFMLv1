#pragma once
#include "../BaseGenerator.h"
#include "../ParticleSystem.h"

class RandomSize : public BaseGenerator {
public:
	RandomSize() : m_from(0), m_to(0) {}
	RandomSize(int l_from, int l_to): m_from(l_from), m_to(l_to) {}

	void Generate(Emitter* l_emitter, ParticleContainer* l_particles, size_t l_from, size_t l_to) {
		auto& rand = *l_emitter->GetParticleSystem()->GetRand();
		auto& sizes = l_particles->m_currentSize;
		for (auto i = l_from; i <= l_to; ++i) {
			float size = static_cast<float>(rand(m_from, m_to));
			sizes[i] = sf::Vector2f(size, size);
		}
	}

	void ReadIn(std::stringstream& l_stream) {
		l_stream >> m_from >> m_to;
	}
private:
	int m_from;
	int m_to;
};