#pragma once
#include <SFML/Graphics/Color.hpp>
#include "../BaseGenerator.h"
#include "../ParticleSystem.h"

class RandomColor : public BaseGenerator {
public:
	RandomColor() = default;
	RandomColor(const sf::Vector3i& l_from, const sf::Vector3i& l_to) : m_from(l_from), m_to(l_to) {}
	void Generate(Emitter* l_emitter, ParticleContainer* l_particles, size_t l_from, size_t l_to) {
		auto& rand = *l_emitter->GetParticleSystem()->GetRand();
		auto& colors = l_particles->m_currentColor;
		for (auto i = l_from; i <= l_to; ++i) {
			sf::Color target{
				static_cast<sf::Uint8>(rand(m_from.x, m_to.x)),
				static_cast<sf::Uint8>(rand(m_from.y, m_to.y)),
				static_cast<sf::Uint8>(rand(m_from.z, m_to.z)),
				255
			};
			colors[i] = target;
		}
	}

	void ReadIn(std::stringstream& l_stream) {
		l_stream >> m_from.x >> m_to.x >> m_from.y >> m_to.y >> m_from.z >> m_to.z;
	}
private:
	sf::Vector3i m_from;
	sf::Vector3i m_to;
};