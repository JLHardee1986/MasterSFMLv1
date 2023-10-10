#pragma once
#include "../BaseGenerator.h"

class PropGenerator : public BaseGenerator {
public:
	PropGenerator(bool l_gravity = true, bool l_additive = false): m_gravity(l_gravity), m_additive(l_additive) {}
	void Generate(Emitter* l_emitter, ParticleContainer* l_particles, size_t l_from, size_t l_to) {
		auto& gravity = l_particles->m_gravity;
		for (auto i = l_from; i <= l_to; ++i) { gravity[i] = m_gravity; }
		auto& additive = l_particles->m_addBlend;
		for (auto i = l_from; i <= l_to; ++i) { additive[i] = m_additive; }
	}

	void ReadIn(std::stringstream& l_stream) {
		int gravity = 1;
		int additive = 0;
		l_stream >> gravity >> additive;
		m_gravity = (gravity != 0);
		m_additive = (additive != 0);
	}
private:
	bool m_gravity;
	bool m_additive;
};