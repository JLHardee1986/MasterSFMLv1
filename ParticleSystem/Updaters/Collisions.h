#pragma once
#include "../BaseUpdater.h"
#include "../ParticleContainer.h"
#include "../../Utilities/Utilities.h"
#include "../../MapSystem/Map.h"

class CollisionUpdater : public BaseUpdater {
public:
	void Update(float l_dT, ParticleContainer* l_particles) {
		auto& positions = l_particles->m_position;
		auto& velocities = l_particles->m_velocity;
		for (size_t i = 0; i < l_particles->m_countAlive; ++i) {
			if (positions[i].z > 0.f) { continue; }
			positions[i].z = 0.f; velocities[i].z = 0.f;
		}

		if (!m_map) { return; }
		for (size_t i = 0; i < l_particles->m_countAlive; ++i) {
			if (positions[i].z > 0.f) { continue; }
			ApplyFriction(l_dT, positions[i], velocities[i]);
		}
	}
	void SetMap(Map* l_map) { m_map = l_map; }
private:
	void ApplyFriction(float l_dT, sf::Vector3f& l_position, sf::Vector3f& l_velocity) {
		sf::Vector2i tileCoords = sf::Vector2i(
			static_cast<int>(floor(l_position.x / Sheet::Tile_Size)),
			static_cast<int>(floor(l_position.y / Sheet::Tile_Size)));
		auto tile = m_map->GetTile(tileCoords.x, tileCoords.y, 0);
		sf::Vector2f friction;
		if (!tile) { friction = m_map->GetDefaultTile()->m_friction; } else { friction = tile->m_properties->m_friction; }
		friction.x *= std::abs(l_velocity.x);
		friction.y *= std::abs(l_velocity.y);
		friction *= l_dT;
		if (l_velocity.x != 0.f && friction.x != 0.f) {
			if (std::abs(l_velocity.x) - std::abs(friction.x) < 0.f) {
				l_velocity.x = 0.f;
			} else {
				l_velocity.x += (l_velocity.x > 0.f ? friction.x * -1.f : friction.x);
			}
		}

		if (l_velocity.y != 0.f && friction.y != 0.f) {
			if (std::abs(l_velocity.y) - std::abs(friction.y) < 0.f) {
				l_velocity.y = 0.f;
			} else {
				l_velocity.y += (l_velocity.y > 0.f ? friction.y * -1.f : friction.y);
			}
		}
	}
	Map* m_map;
};