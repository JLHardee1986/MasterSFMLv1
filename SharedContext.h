#pragma once
class Window;
class EventManager;
class TextureManager;
class FontManager;
class AudioManager;
class SoundManager;
class SystemManager;
class EntityManager;
class Map;
class GUI_Manager;
class RandomGenerator;
class ParticleSystem;

struct SharedContext{
	SharedContext():
		m_wind(nullptr),
		m_eventManager(nullptr),
		m_textureManager(nullptr),
		m_fontManager(nullptr),
		m_audioManager(nullptr),
		m_soundManager(nullptr),
		m_systemManager(nullptr),
		m_entityManager(nullptr),
		m_gameMap(nullptr),
		m_guiManager(nullptr),
		m_rand(nullptr),
		m_particles(nullptr)
	{}

	Window* m_wind;
	EventManager* m_eventManager;
	TextureManager* m_textureManager;
	FontManager* m_fontManager;
	AudioManager* m_audioManager;
	SoundManager* m_soundManager;
	SystemManager* m_systemManager;
	EntityManager* m_entityManager;
	Map* m_gameMap;
	GUI_Manager* m_guiManager;
	RandomGenerator* m_rand;
	ParticleSystem* m_particles;
};