#pragma once
#include "../BaseUpdater.h"
#include "../ParticleContainer.h"
#include "../../MapSystem/MapDefinitions.h"

class DrawableUpdater : public BaseUpdater {
public:
	void Update(float l_dT, ParticleContainer* l_particles) {
		auto& positions = l_particles->m_position;
		auto& drawables = l_particles->m_drawable;
		for (size_t i = 0; i < l_particles->m_countAlive; ++i) { drawables[i].setPosition(positions[i].x, positions[i].y); }
		auto& sizes = l_particles->m_currentSize;
		for (size_t i = 0; i < l_particles->m_countAlive; ++i) { drawables[i].setSize(sizes[i]); }
		for (size_t i = 0; i < l_particles->m_countAlive; ++i) {
			float ScaleFactor = std::max((positions[i].z / Sheet::Tile_Size) * ScaleToElevationRatio, 1.f);
			drawables[i].setScale(ScaleFactor, ScaleFactor);
		}
		auto& colors = l_particles->m_currentColor;
		for (size_t i = 0; i < l_particles->m_countAlive; ++i) { drawables[i].setFillColor(colors[i]); }
		auto& rotations = l_particles->m_currentRotation;
		for (size_t i = 0; i < l_particles->m_countAlive; ++i) { drawables[i].setRotation(rotations[i]); }

		for (size_t i = 0; i < l_particles->m_countAlive; ++i) {
			drawables[i].setOrigin(drawables[i].getLocalBounds().width / 2, drawables[i].getLocalBounds().height / 2);
		}
	}
	static const float ScaleToElevationRatio;
};

const float DrawableUpdater::ScaleToElevationRatio = 1.5f;