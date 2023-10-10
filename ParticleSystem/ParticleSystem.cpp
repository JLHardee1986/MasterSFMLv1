#include "ParticleSystem.h"
#include "Updaters/Lifespan.h"
#include "Updaters/Spatial.h"
#include "Updaters/Forces.h"
#include "Updaters/Collisions.h"
#include "Updaters/Interpolator.h"
#include "Updaters/Drawables.h"
#include "Generators/PointPosition.h"
#include "Generators/LinePosition.h"
#include "Generators/AreaPosition.h"
#include "Generators/RandomColor.h"
#include "Generators/RandomVelocity.h"
#include "Generators/RandomLifespan.h"
#include "Generators/RandomSize.h"
#include "Generators/SizeRange.h"
#include "Generators/ColorRange.h"
#include "Generators/RotationRange.h"
#include "Generators/Texture.h"
#include "Generators/Properties.h"
#include "../MapSystem/Map.h"
#include "../StateSystem/StateManager.h"

ParticleSystem::ParticleSystem(StateManager* l_stateManager, TextureManager* l_textureManager, RandomGenerator* l_rand, Map* l_map)
	: m_stateManager(l_stateManager), m_textureMgr(l_textureManager), m_rand(l_rand), m_map(l_map)
{
	m_updaters.emplace("Lifespan", std::make_unique<LifespanUpdater>());
	m_updaters.emplace("Spatial", std::make_unique<SpatialUpdater>());
	m_updaters.emplace("Force", std::make_unique<ForceUpdater>());
	m_updaters.emplace("Collision", std::make_unique<CollisionUpdater>());
	m_updaters.emplace("Interpolator", std::make_unique<Interpolator>());
	m_updaters.emplace("Drawable", std::make_unique<DrawableUpdater>());

	RegisterGenerator<PointPosition>("PointPosition");
	RegisterGenerator<LinePosition>("LinePosition");
	RegisterGenerator<AreaPosition>("AreaPosition");
	RegisterGenerator<RandomColor>("RandomColor");
	RegisterGenerator<RandomVelocity>("RandomVelocity");
	RegisterGenerator<RandomLifespan>("RandomLifespan");
	RegisterGenerator<RandomSize>("RandomSize");
	RegisterGenerator<SizeRange>("SizeRange");
	RegisterGenerator<ColorRange>("ColorRange");
	RegisterGenerator<RotationRange>("RotationRange");
	RegisterGenerator<TextureGenerator>("Texture");
	RegisterGenerator<PropGenerator>("Properties");
}

Emitter* ParticleSystem::AddEmitter(std::unique_ptr<Emitter> l_emitter, const StateType& l_state) {
	auto ptr = l_emitter.get();
	l_emitter->SetParticleSystem(this);
	if (!GetGenerators(l_emitter->GetGenerators())) { 
		std::cout << "Failed to add emitter due to a missing resource: " << l_emitter->GetGenerators() << std::endl; 
		return nullptr;
	}
	if (l_state == StateType(0)) {
		if (m_emitterItr == m_emitters.end()) { std::cout << "Cannot add an emitter to a state without particles." << std::endl; return nullptr; }
		m_emitterItr->second.emplace_back(std::move(l_emitter));
		return ptr;
	}
	auto itr = m_emitters.find(l_state);
	if (itr == m_emitters.end()) { std::cout << "Cannot add an emitter to a state without particles." << std::endl; return nullptr; }
	itr->second.emplace_back(std::move(l_emitter));
	return ptr;
}

Emitter* ParticleSystem::AddEmitter(const sf::Vector3f& l_position,
	std::string& l_generators, unsigned int l_emitRate, const StateType& l_state)
{
	auto emitter = std::make_unique<Emitter>(l_position);
	emitter->SetGenerators(l_generators);
	emitter->SetEmitRate(l_emitRate);
	return AddEmitter(std::move(emitter), l_state);
}

void ParticleSystem::AddForce(ForceApplicator l_force, const StateType& l_state) {
	if (l_state == StateType(0)) {
		if (m_stateItr == m_container.end()) { std::cout << "Cannot add a force to a state without particles." << std::endl; return; }
		m_forces[m_currentState].emplace_back(l_force);
		return;
	}
	auto itr = m_forces.find(l_state);
	if(itr == m_forces.end()) { std::cout << "Cannot add a force to a state without particles." << std::endl; return; }
	itr->second.emplace_back(l_force);
}

void ParticleSystem::RemoveEmitter(Emitter* l_emitter) {
	m_removedEmitters.push_back(l_emitter);
}

Emitter* ParticleSystem::FindEmitter(const sf::Vector2f& l_point, const sf::Vector2f& l_area,
	unsigned int l_layerMin, unsigned int l_layerMax)
{
	if (m_emitterItr == m_emitters.end()) { return nullptr; }
	for (auto& emitter : m_emitterItr->second) {
		auto position = emitter->GetPosition();
		auto elevation = std::floor(static_cast<int>(position.z / (float)static_cast<int>(Sheet::Tile_Size)));
		if (l_layerMax != -1 && (elevation < l_layerMin || elevation > l_layerMax)) { continue; }
		sf::FloatRect box;
		box.left = position.x - (l_area.x / 2.f);
		box.top = position.y - (l_area.y / 2.f);
		box.width = l_area.x;
		box.height = l_area.y;
		if (box.contains(l_point)) { return emitter.get(); }
	}
	return nullptr;
}

GeneratorList* ParticleSystem::GetGenerators(const std::string& l_name) {
	auto itr = m_generators.find(l_name);
	if (itr == m_generators.end()) { return nullptr; }
	return &m_generators.find(l_name)->second;
}

TextureManager* ParticleSystem::GetTextureManager() const { return m_textureMgr; }
RandomGenerator* ParticleSystem::GetRand() const { return m_rand; }

void ParticleSystem::CreateState(const StateType& l_state) {
	if (m_container.find(l_state) != m_container.end()) { return; }
	m_container.emplace(l_state, std::make_unique<ParticleContainer>(m_textureMgr));
	m_emitters.emplace(l_state, EmitterList());
	m_forces.emplace(l_state, ForceApplicatorList());
	ChangeState(l_state);
}

void ParticleSystem::ChangeState(const StateType& l_state) {
	SetState(l_state);
	m_stateItr = m_container.find(m_currentState);
	m_emitterItr = m_emitters.find(m_currentState);


	auto c = static_cast<CollisionUpdater*>(m_updaters["Collision"].get());
	if (l_state == StateType::Game || l_state == StateType::MapEditor) { c->SetMap(m_map); } // REEST! FIX!!!
	else { c->SetMap(nullptr); }
	auto f = static_cast<ForceUpdater*>(m_updaters["Force"].get());
	auto forceItr = m_forces.find(m_currentState);
	if (forceItr == m_forces.end()) { f->SetApplicators(nullptr); return; }
	f->SetApplicators(&m_forces.find(m_currentState)->second);
}

void ParticleSystem::RemoveState(const StateType& l_state) {
	if (m_stateItr->first == l_state) {
		m_stateItr = m_container.end();
		m_emitterItr = m_emitters.end();
	}
	m_emitters.erase(l_state);
	m_forces.erase(l_state);
	m_container.erase(l_state);
}

void ParticleSystem::PurgeCurrentState() {
	m_emitterItr->second.clear();
}

bool ParticleSystem::ProcessLine(std::stringstream& l_stream) {
	std::string type;
	l_stream >> type;
	if (type == "Name") {
		if (!(l_stream >> m_loadingGenerator)) { std::cout << "Faulty particle file format." << std::endl; return false; }
		auto generators = GetGenerators(m_loadingGenerator);
		if (generators) { std::cout << "A generator set with the name '" << m_loadingGenerator << "' already exists." << std::endl; return false; }
	} else {
		if (m_loadingGenerator.empty()) { std::cout << "Faulty particle file format." << std::endl; return false; }
		auto itr = m_factory.find(type);
		if (itr == m_factory.end()) { std::cout << "Unknown generator type: " << type << std::endl; return true; }
		std::unique_ptr<BaseGenerator> generator(itr->second());
		l_stream >> *generator;
		m_generators[m_loadingGenerator].emplace_back(std::move(generator));
	}
	return true;
}

void ParticleSystem::ReadMapLine(const std::string& l_type, std::stringstream& l_stream) {
	if (l_type == "ParticleEmitter") {
		sf::Vector3f position;
		size_t emitRate;
		std::string generatorType;
		l_stream >> generatorType >> position.x >> position.y >> position.z >> emitRate;
		AddEmitter(position, generatorType, (unsigned int)emitRate, m_stateManager->GetNextToLast());
	} else if (l_type == "ForceApplicator") {
		sf::Vector3f position;
		sf::Vector3f force;
		float radius;
		l_stream >> position.x >> position.y >> position.z >> force.x >> force.y >> force.z >> radius;
		AddForce(ForceApplicator(position, force, radius), m_stateManager->GetNextToLast());
	}
}

void ParticleSystem::SaveMap(std::ofstream& l_fileStream) {
	for (auto& emitter : m_emitterItr->second) {
		l_fileStream << "ParticleEmitter " << emitter->GetGenerators() << " "
			<< emitter->GetPosition().x << " " << emitter->GetPosition().y << " "
			<< emitter->GetPosition().z << " " << emitter->GetEmitRate() << std::endl;
	}

	for (auto& force : m_forces[m_currentState]) {
		l_fileStream << "ForceApplicator " << force.m_center.x << " " << force.m_center.y
			<< " " << force.m_center.z << " " << force.m_force.x << " " << force.m_force.y
			<< " " << force.m_force.z << " " << force.m_radius << std::endl;
	}
}

void ParticleSystem::ResetForNextFile() {
	m_loadingGenerator.clear();
}

void ParticleSystem::Update(float l_dT) {
	if (m_stateItr == m_container.end()) { return; }
	for (auto& emitter : m_emitterItr->second) { emitter->Update(l_dT, m_stateItr->second.get()); }
	for (auto& updater : m_updaters) { updater.second->Update(l_dT, m_stateItr->second.get()); }
	if (!m_removedEmitters.size()) { return; }
	for (auto& removed : m_removedEmitters) {
		auto itr = std::remove_if(m_emitterItr->second.begin(), m_emitterItr->second.end(),
			[removed](std::unique_ptr<Emitter>& emitter) { return emitter.get() == removed; }
		);
		if (itr == m_emitterItr->second.end()) { continue; }
		m_emitterItr->second.erase(itr);
	}
	m_removedEmitters.clear();
}

void ParticleSystem::ApplyForce(const sf::Vector3f& l_center, const sf::Vector3f& l_force, float l_radius) {
	if (m_stateItr == m_container.end()) { return; }
	auto f = static_cast<ForceUpdater*>(m_updaters["Force"].get());
	auto container = m_stateItr->second.get();
	auto& positions = container->m_position;
	auto& velocities = container->m_velocity;
	for (size_t i = 0; i < container->m_countAlive; ++i) {
		f->Force(l_center, l_force, l_radius, positions[i], velocities[i]);
	}
}

void ParticleSystem::Draw(Window* l_window, int l_elevation) {
	if (m_stateItr == m_container.end()) { return; }
	auto container = m_stateItr->second.get();
	auto& drawables = container->m_drawable;
	auto& positions = container->m_position;
	auto& blendModes = container->m_addBlend;
	auto view = l_window->GetRenderWindow()->getView();
	auto renderer = l_window->GetRenderer();

	auto state = m_stateManager->GetCurrentStateType();
	if (state == StateType::Game || state == StateType::MapEditor) {
		renderer->UseShader("default");
	} else {
		renderer->DisableShader();
	}

	for (size_t i = 0; i < container->m_countAlive; ++i) {
		if (l_elevation >= 0) {
			if (positions[i].z < l_elevation * Sheet::Tile_Size) { continue; }
			if (positions[i].z >= (l_elevation + 1) * Sheet::Tile_Size) { continue; }
		} else if (positions[i].z < Sheet::Num_Layers * Sheet::Tile_Size) { continue; }
		renderer->AdditiveBlend(blendModes[i]);
		renderer->Draw(drawables[i]);
	}
	renderer->AdditiveBlend(false);
}