#pragma once
#include "../BaseGenerator.h"

class SizeRange : public BaseGenerator {
public:
	SizeRange() : m_start(0), m_finish(0) {}
	SizeRange(float l_start, float l_finish) : m_start(l_start), m_finish(l_finish) {}
	void Generate(Emitter* l_emitter, ParticleContainer* l_particles, size_t l_from, size_t l_to) {
		if (m_start == m_finish) {
			auto& sizes = l_particles->m_currentSize;
			for (auto i = l_from; i <= l_to; ++i) {
				sizes[i] = sf::Vector2f(m_start, m_start);
			}
		} else {
			auto& beginning = l_particles->m_startSize;
			auto& ending = l_particles->m_finalSize;
			for (auto i = l_from; i <= l_to; ++i) {
				beginning[i] = sf::Vector2f(m_start, m_start);
				ending[i] = sf::Vector2f(m_finish, m_finish);
			}
		}
	}

	void ReadIn(std::stringstream& l_stream) {
		l_stream >> m_start >> m_finish;
	}
private:
	float m_start;
	float m_finish;
};