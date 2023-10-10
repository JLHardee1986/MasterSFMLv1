#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "../Resources/TextureManager.h"

class ParticleContainer {
public:
	ParticleContainer(TextureManager* l_textureManager)
		: m_textureManager(l_textureManager), m_countAlive(0) { Reset(); }

	~ParticleContainer() {
		for (size_t i = 0; i < Max_Particles; ++i) {
			if (m_texture[i].empty()) { continue; }
			m_textureManager->ReleaseResource(m_texture[i]);
		}
	}

	void Reset() {
		for (size_t i = 0; i < Max_Particles; ++i) { ResetParticle(i); }
		m_countAlive = 0;
	}

	void Enable(size_t l_id) {
		if (m_countAlive >= Max_Particles) { return; }
		m_alive[l_id] = true;
		Swap(l_id, m_countAlive);
		++m_countAlive;
	}

	void Disable(size_t l_id) {
		if (!m_countAlive) { return; }
		ResetParticle(l_id);
		Swap(l_id, m_countAlive - 1);
		--m_countAlive;
	}

	const static size_t Max_Particles = 3000;

	sf::Vector3f m_position[Max_Particles];
	sf::Vector3f m_velocity[Max_Particles];
	sf::Vector3f m_acceleration[Max_Particles];
	sf::Vector2f m_startSize[Max_Particles];
	sf::Vector2f m_currentSize[Max_Particles];
	sf::Vector2f m_finalSize[Max_Particles];
	sf::Color m_startColor[Max_Particles];
	sf::Color m_currentColor[Max_Particles];
	sf::Color m_finalColor[Max_Particles];
	std::string m_texture[Max_Particles];
	sf::RectangleShape m_drawable[Max_Particles];
	float m_startRotation[Max_Particles];
	float m_currentRotation[Max_Particles];
	float m_finalRotation[Max_Particles];
	float m_lifespan[Max_Particles];
	float m_maxLifespan[Max_Particles];
	bool m_gravity[Max_Particles];
	bool m_addBlend[Max_Particles];
	bool m_alive[Max_Particles];

	size_t m_countAlive;
	TextureManager* m_textureManager;
protected:
	void Swap(size_t l_first, size_t l_second) {
		std::swap(m_position[l_first], m_position[l_second]);
		std::swap(m_velocity[l_first], m_velocity[l_second]);
		std::swap(m_acceleration[l_first], m_acceleration[l_second]);
		std::swap(m_startSize[l_first], m_startSize[l_second]);
		std::swap(m_currentSize[l_first], m_currentSize[l_second]);
		std::swap(m_finalSize[l_first], m_finalSize[l_second]);
		std::swap(m_startColor[l_first], m_startColor[l_second]);
		std::swap(m_currentColor[l_first], m_currentColor[l_second]);
		std::swap(m_finalColor[l_first], m_finalColor[l_second]);
		std::swap(m_texture[l_first], m_texture[l_second]);
		std::swap(m_drawable[l_first], m_drawable[l_second]);
		std::swap(m_startRotation[l_first], m_startRotation[l_second]);
		std::swap(m_currentRotation[l_first], m_currentRotation[l_second]);
		std::swap(m_finalRotation[l_first], m_finalRotation[l_second]);
		std::swap(m_lifespan[l_first], m_lifespan[l_second]);
		std::swap(m_maxLifespan[l_first], m_maxLifespan[l_second]);
		std::swap(m_gravity[l_first], m_gravity[l_second]);
		std::swap(m_addBlend[l_first], m_addBlend[l_second]);
		std::swap(m_alive[l_first], m_alive[l_second]);
	}

	void ResetParticle(size_t l_id) {
		m_alive[l_id] = false;
		m_gravity[l_id] = false;
		m_addBlend[l_id] = false;
		m_lifespan[l_id] = 0.f;
		m_maxLifespan[l_id] = 0.f;
		m_position[l_id] = { 0.f, 0.f, 0.f };
		m_velocity[l_id] = { 0.f, 0.f, 0.f };
		m_acceleration[l_id] = { 0.f, 0.f, 0.f };
		m_startRotation[l_id] = 0.f;
		m_currentRotation[l_id] = 0.f;
		m_finalRotation[l_id] = 0.f;
		m_startSize[l_id] = { 0.f, 0.f };
		m_currentSize[l_id] = { 0.f, 0.f };
		m_finalSize[l_id] = { 0.f, 0.f };
		m_startColor[l_id] = { 0, 0, 0, 0 };
		m_currentColor[l_id] = { 0, 0, 0, 0 };
		m_finalColor[l_id] = { 0, 0, 0, 0 };
		if (!m_texture[l_id].empty()) {
			m_textureManager->ReleaseResource(m_texture[l_id]);
			m_texture[l_id].clear();
			m_drawable[l_id].setTexture(nullptr);
		}
	}
};