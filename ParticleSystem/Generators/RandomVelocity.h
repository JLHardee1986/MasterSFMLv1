#pragma once
#include "../BaseGenerator.h"
#include "../ParticleSystem.h"

class RandomVelocity : public BaseGenerator {
public:
	RandomVelocity() = default;
	RandomVelocity(const sf::Vector3f& l_from, const sf::Vector3f& l_to)
		: m_from(l_from), m_to(l_to) {}

	void Generate(Emitter* l_emitter, ParticleContainer* l_particles, size_t l_from, size_t l_to) {
		auto& rand = *l_emitter->GetParticleSystem()->GetRand();
		auto& velocities = l_particles->m_velocity;
		for (auto i = l_from; i <= l_to; ++i) {
			sf::Vector3f target{
				rand(m_from.x, m_to.x),
				rand(m_from.y, m_to.y),
				rand(m_from.z, m_to.z)
			};
			velocities[i] = target;
		}
	}

	void ReadIn(std::stringstream& l_stream) {
		l_stream >> m_from.x >> m_to.x >> m_from.y >> m_to.y >> m_from.z >> m_to.z;
	}
private:
	sf::Vector3f m_from;
	sf::Vector3f m_to;
};