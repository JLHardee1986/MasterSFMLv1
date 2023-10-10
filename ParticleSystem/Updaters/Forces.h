#pragma once
#include "../BaseUpdater.h"
#include "../ParticleContainer.h"

class ParticleSystem;

class ForceUpdater : public BaseUpdater {
	friend class ParticleSystem;
public:
	ForceUpdater() : m_applicators(nullptr) {}
	void Update(float l_dT, ParticleContainer* l_particles) {
		auto& velocities = l_particles->m_velocity;
		auto& gravity = l_particles->m_gravity;
		for (size_t i = 0; i < l_particles->m_countAlive; ++i) {
			if (!gravity[i]) { continue; }
			velocities[i].z -= Gravity * l_dT;
		}

		if (!m_applicators) { return; }
		auto& positions = l_particles->m_position;
		for (size_t i = 0; i < l_particles->m_countAlive; ++i) {
			for (auto& force : *m_applicators) {
				Force(force.m_center, force.m_force * l_dT, force.m_radius, positions[i], velocities[i]);
			}
		}
	}

	void SetApplicators(ForceApplicatorList* l_list) { m_applicators = l_list; }

	static const float Gravity;
private:
	void Force(const sf::Vector3f& l_center, const sf::Vector3f& l_force, float l_radius,
		sf::Vector3f& l_position, sf::Vector3f& l_velocity)
	{
		sf::Vector3f from(l_center.x - l_radius, l_center.y - l_radius, l_center.z - l_radius);
		sf::Vector3f to(l_center.x + l_radius, l_center.y + l_radius, l_center.z + l_radius);
		if (l_position.x < from.x) { return; }
		if (l_position.y < from.y) { return; }
		if (l_position.z < from.z) { return; }
		if (l_position.x > to.x) { return; }
		if (l_position.y > to.y) { return; }
		if (l_position.z > to.z) { return; }

		sf::Vector3f distance = l_center - l_position;
		sf::Vector3f a_distance = sf::Vector3f(std::abs(distance.x), std::abs(distance.y), std::abs(distance.z));
		float magnitude = std::sqrt(std::pow(a_distance.x, 2) + std::pow(a_distance.y, 2) + std::pow(a_distance.z, 2));
		sf::Vector3f normal = sf::Vector3f(
			a_distance.x / magnitude,
			a_distance.y / magnitude,
			a_distance.z / magnitude
		);
		sf::Vector3f loss = sf::Vector3f(
			std::abs(l_force.x) / (l_radius / a_distance.x),
			std::abs(l_force.y) / (l_radius / a_distance.y),
			std::abs(l_force.z) / (l_radius / a_distance.z)
		);
		sf::Vector3f applied = sf::Vector3f(
			(l_force.x > 0 ? l_force.x - loss.x : l_force.x + loss.x),
			(l_force.y > 0 ? l_force.y - loss.y : l_force.y + loss.y),
			(l_force.z > 0 ? l_force.z - loss.z : l_force.z + loss.z)
		);
		applied.x *= normal.x;
		applied.y *= normal.y;
		applied.z *= normal.z;
		if (distance.x < 0) { applied.x = -applied.x; }
		if (distance.y < 0) { applied.y = -applied.y; }
		if (distance.z < 0) { applied.z = -applied.z; }

		l_velocity += applied;
	}

	ForceApplicatorList* m_applicators;
};

const float ForceUpdater::Gravity = 128.f;