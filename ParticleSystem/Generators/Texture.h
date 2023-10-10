#pragma once
#include "../BaseGenerator.h"
#include "../ParticleSystem.h"

class TextureGenerator : public BaseGenerator {
public:
	TextureGenerator() = default;
	TextureGenerator(const std::string& l_texture) : m_texture(l_texture) {}
	void Generate(Emitter* l_emitter, ParticleContainer* l_particles, size_t l_from, size_t l_to) {
		if (m_texture.empty()) { return; }
		TextureManager* manager = l_emitter->GetParticleSystem()->GetTextureManager();
		if (!manager->RequireResource(m_texture)) { return; } // +1 for checking and a quick pointer.
		auto& textures = l_particles->m_texture;
		auto& drawables = l_particles->m_drawable;
		auto resource = manager->GetResource(m_texture);
		auto size = resource->getSize();
		for (auto i = l_from; i <= l_to; ++i) {
			textures[i] = m_texture;
			manager->RequireResource(m_texture);
			drawables[i].setTexture(resource);
			drawables[i].setTextureRect(sf::IntRect(0, 0, size.x, size.y));
		}
		manager->ReleaseResource(m_texture); // Release the initial +1.
	}

	void ReadIn(std::stringstream& l_stream) {
		l_stream >> m_texture;
	}
private:
	std::string m_texture;
};