#pragma once
#include <SFML/Graphics/Color.hpp>
#include "../BaseGenerator.h"

class ColorRange : public BaseGenerator {
public:
	ColorRange() = default;
	ColorRange(const sf::Color& l_start, const sf::Color& l_finish): m_start(l_start), m_finish(l_finish) {}
	void Generate(Emitter* l_emitter, ParticleContainer* l_particles, size_t l_from, size_t l_to) {
		auto& beginning = l_particles->m_startColor;
		auto& current = l_particles->m_currentColor;
		auto& ending = l_particles->m_finalColor;
		for (auto i = l_from; i <= l_to; ++i) {
			beginning[i] = m_start;
			current[i] = m_start;
			ending[i] = m_finish;
		}
	}

	void ReadIn(std::stringstream& l_stream) {
		int s_r = 0, s_g = 0, s_b = 0, s_a = 0;
		int f_r = 0, f_g = 0, f_b = 0, f_a = 0;
		l_stream >> s_r >> s_g >> s_b >> s_a;
		l_stream >> f_r >> f_g >> f_b >> f_a;

		m_start = { static_cast<sf::Uint8>(s_r), static_cast<sf::Uint8>(s_g), static_cast<sf::Uint8>(s_b), static_cast<sf::Uint8>(s_a) };
		m_finish = { static_cast<sf::Uint8>(f_r), static_cast<sf::Uint8>(f_g), static_cast<sf::Uint8>(f_b), static_cast<sf::Uint8>(f_a) };
	}
private:
	sf::Color m_start;
	sf::Color m_finish;
};