#pragma once
#include "../BaseGenerator.h"

class RotationRange : public BaseGenerator {
public:
	RotationRange() : m_start(0.f), m_finish(0.f) {}
	RotationRange(float l_start, float l_finish) : m_start(l_start), m_finish(l_finish) {}
	void Generate(Emitter* l_emitter, ParticleContainer* l_particles, size_t l_from, size_t l_to) {
		auto& beginning = l_particles->m_startRotation;
		auto& ending = l_particles->m_finalRotation;
		for (auto i = l_from; i <= l_to; ++i) {
			beginning[i] = m_start;
			ending[i] = m_finish;
		}
	}

	void ReadIn(std::stringstream& l_stream) {
		l_stream >> m_start >> m_finish;
	}
private:
	float m_start;
	float m_finish;
};