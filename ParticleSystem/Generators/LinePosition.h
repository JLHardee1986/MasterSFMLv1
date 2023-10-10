#pragma once
#include "../BaseGenerator.h"
#include "../Emitter.h"
#include "../ParticleSystem.h"

enum class LineAxis{ x, y, z };

class LinePosition : public BaseGenerator {
public:
	LinePosition() : m_axis(LineAxis::x), m_deviation(0.f) {}
	LinePosition(LineAxis l_axis, float l_deviation): m_axis(l_axis), m_deviation(l_deviation) {}
	void Generate(Emitter* l_emitter, ParticleContainer* l_particles, size_t l_from, size_t l_to) {
		auto& positions = l_particles->m_position;
		auto center = l_emitter->GetPosition();
		auto& rand = *l_emitter->GetParticleSystem()->GetRand();
		for (auto i = l_from; i <= l_to; ++i) {
			if (m_axis == LineAxis::x) { center.x = rand(center.x - m_deviation, center.x + m_deviation); }
			else if (m_axis == LineAxis::y) { center.y = rand(center.y - m_deviation, center.y + m_deviation); }
			else { center.z = rand(center.z - m_deviation, center.z + m_deviation); }
			positions[i] = center;
		}
	}

	void ReadIn(std::stringstream& l_stream) {
		std::string axis;
		l_stream >> axis >> m_deviation;
		if (axis == "x") { m_axis = LineAxis::x; }
		else if (axis == "y") { m_axis = LineAxis::y; }
		else if (axis == "z") { m_axis = LineAxis::z; }
		else { std::cout << "Faulty axis: " << axis << std::endl; }
	}
private:
	LineAxis m_axis;
	float m_deviation;
};