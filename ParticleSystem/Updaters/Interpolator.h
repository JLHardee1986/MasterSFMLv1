#pragma once
#include "../BaseUpdater.h"
#include "../ParticleContainer.h"
#include "../../Utilities/Utilities.h"

class Interpolator : public BaseUpdater {
public:
	void Update(float l_dT, ParticleContainer* l_particles) {
		auto& startColors = l_particles->m_startColor;
		auto& currentColors = l_particles->m_currentColor;
		auto& finalColors = l_particles->m_finalColor;
		auto& lifespans = l_particles->m_lifespan;
		auto& maxLifespans = l_particles->m_maxLifespan;
		for (size_t i = 0; i < l_particles->m_countAlive; ++i) {
			if (startColors[i] == finalColors[i]) { continue; }
			currentColors[i].r = Utils::Interpolate<sf::Uint8>(0.f, maxLifespans[i], startColors[i].r, finalColors[i].r, lifespans[i]);
			currentColors[i].g = Utils::Interpolate<sf::Uint8>(0.f, maxLifespans[i], startColors[i].g, finalColors[i].g, lifespans[i]);
			currentColors[i].b = Utils::Interpolate<sf::Uint8>(0.f, maxLifespans[i], startColors[i].b, finalColors[i].b, lifespans[i]);
			currentColors[i].a = Utils::Interpolate<sf::Uint8>(0.f, maxLifespans[i], startColors[i].a, finalColors[i].a, lifespans[i]);
		}

		auto& startRotations = l_particles->m_startRotation;
		auto& currentRotations = l_particles->m_currentRotation;
		auto& finalRotations = l_particles->m_finalRotation;

		for (size_t i = 0; i < l_particles->m_countAlive; ++i) {
			if (startRotations[i] == finalRotations[i]) { continue; }
			currentRotations[i] = Utils::Interpolate<float>(0.f, maxLifespans[i], startRotations[i], finalRotations[i], lifespans[i]);
		}

		auto& startSizes = l_particles->m_startSize;
		auto& currentSizes = l_particles->m_currentSize;
		auto& finalSizes = l_particles->m_finalSize;

		for (size_t i = 0; i < l_particles->m_countAlive; ++i) {
			if (startSizes[i] == finalSizes[i]) { continue; }
			currentSizes[i] = sf::Vector2f(
				Utils::Interpolate<float>(0.f, maxLifespans[i], startSizes[i].x, finalSizes[i].x, lifespans[i]),
				Utils::Interpolate<float>(0.f, maxLifespans[i], startSizes[i].y, finalSizes[i].y, lifespans[i]));
		}
	}
};