#pragma once
#include <SFML/Audio/SoundBuffer.hpp>
#include "ResourceManager.h"

class AudioManager : public ResourceManager<AudioManager, sf::SoundBuffer>{
public:
	AudioManager() : ResourceManager("audio.cfg") {}

	bool Load(sf::SoundBuffer* l_resource, const std::string& l_path) {
		return l_resource->loadFromFile(Utils::GetWorkingDirectory() + l_path);
	}
};