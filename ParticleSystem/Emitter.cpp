#include "Emitter.h"
#include "ParticleSystem.h"

Emitter::Emitter(const sf::Vector3f& l_position, int l_maxParticles) : m_position(l_position), 
	m_maxParticles(l_maxParticles), m_emitRate(0), m_accumulator(0.f), m_system(nullptr) {}

void Emitter::Update(float l_dT, ParticleContainer* l_particles) {
	if (m_generators.empty()) { return; }
	auto generatorList = m_system->GetGenerators(m_generators);
	if (!generatorList) { return; }
	m_accumulator += l_dT * m_emitRate;
	if (m_accumulator < 1.f) { return; }
	auto num_particles = static_cast<int>(m_accumulator);
	m_accumulator -= num_particles;
	if (m_maxParticles != -1) {
		if (num_particles > m_maxParticles) { num_particles = m_maxParticles; m_maxParticles = 0; }
		else { m_maxParticles -= num_particles; }
	}
	size_t from = l_particles->m_countAlive;
	size_t to = (l_particles->m_countAlive + num_particles > l_particles->Max_Particles
		? l_particles->Max_Particles - 1
		: l_particles->m_countAlive + num_particles - 1);

	for (auto& generator : *generatorList) { generator->Generate(this, l_particles, from, to); }
	for (auto i = from; i <= to; ++i) { l_particles->Enable(i); }
	if (!m_maxParticles) { m_system->RemoveEmitter(this); }
}

void Emitter::SetPosition(const sf::Vector3f& l_position) { m_position = l_position; }
sf::Vector3f Emitter::GetPosition() const { return m_position; }
void Emitter::SetEmitRate(size_t l_nPerSecond) { m_emitRate = l_nPerSecond; }
size_t Emitter::GetEmitRate() const { return m_emitRate; }
void Emitter::SetGenerators(const std::string& l_generators) {
	m_generators = l_generators;
}
void Emitter::SetParticleSystem(ParticleSystem* l_system) { m_system = l_system; }
std::string Emitter::GetGenerators()const { return m_generators; }
ParticleSystem* Emitter::GetParticleSystem() const { return m_system; }