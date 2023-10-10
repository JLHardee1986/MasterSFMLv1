#pragma once
#include <memory>
#include <string>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

class ParticleSystem;
class ParticleContainer;

class Emitter {
public:
	Emitter(const sf::Vector3f& l_position, int l_maxParticles = -1);
	void Update(float l_dT, ParticleContainer* l_particles);
	void SetPosition(const sf::Vector3f& l_position);
	sf::Vector3f GetPosition() const;
	size_t GetEmitRate() const;

	void SetEmitRate(size_t l_nPerSecond);
	void SetParticleSystem(ParticleSystem* l_system);
	void SetGenerators(const std::string& l_generators);
	std::string GetGenerators() const;
	ParticleSystem* GetParticleSystem() const;
private:
	std::string m_generators;
	size_t m_emitRate;
	int m_maxParticles;
	sf::Vector3f m_position;
	float m_accumulator;
	ParticleSystem* m_system;
};