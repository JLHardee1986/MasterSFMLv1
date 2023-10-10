#pragma once
#include <SFML/System/Vector3.hpp>

struct ForceApplicator {
	ForceApplicator(const sf::Vector3f& l_center, const sf::Vector3f& l_force, float l_radius)
		: m_center(l_center), m_force(l_force), m_radius(l_radius) {}
	sf::Vector3f m_center;
	sf::Vector3f m_force;
	float m_radius;
};