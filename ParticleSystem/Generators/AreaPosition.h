#pragma once
#include "../BaseGenerator.h"
#include "../Emitter.h"
#include "../ParticleSystem.h"

class AreaPosition : public BaseGenerator {
public:
	AreaPosition() = default;
	AreaPosition(const sf::Vector3f& l_deviation): m_deviation(l_deviation) {}
	void Generate(Emitter* l_emitter, ParticleContainer* l_particles, size_t l_from, size_t l_to) {
		auto& positions = l_particles->m_position;
		auto center = l_emitter->GetPosition();
		auto rangeFrom = sf::Vector3f(center.x - m_deviation.x, center.y - m_deviation.y, center.z - m_deviation.z);
		auto rangeTo = sf::Vector3f(center.x + m_deviation.x, center.y + m_deviation.y, center.z + m_deviation.z);
		auto& rand = *l_emitter->GetParticleSystem()->GetRand();
		for (auto i = l_from; i <= l_to; ++i) {
			positions[i] = sf::Vector3f(
				rand(rangeFrom.x, rangeTo.x),
				rand(rangeFrom.y, rangeTo.y),
				rand(rangeFrom.z, rangeTo.z)
			);
		}
	}

	void ReadIn(std::stringstream& l_stream) {
		l_stream >> m_deviation.x >> m_deviation.y >> m_deviation.z;
	}
private:
	sf::Vector3f m_deviation;
};