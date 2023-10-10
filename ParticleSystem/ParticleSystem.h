#pragma once
#include <memory>
#include <functional>
#include "ParticleContainer.h"
#include "BaseUpdater.h"
#include "BaseGenerator.h"
#include "Emitter.h"
#include "ForceApplicator.h"
#include "../WindowSystem/Window.h"
#include "../Utilities/RandomGenerator.h"
#include "../StateSystem/StateTypes.h"
#include "../StateSystem/StateDependent.h"
#include "../ThreadWorkers/FileLoader.h"
#include "../MapSystem/MapLoadee.h"

using Updaters = std::unordered_map<std::string, std::unique_ptr<BaseUpdater>>;
using EmitterList = std::vector<std::unique_ptr<Emitter>>;
using Emitters = std::unordered_map<StateType, EmitterList>;
using GeneratorList = std::vector<std::unique_ptr<BaseGenerator>>;
using Generators = std::unordered_map < std::string, GeneratorList >;
using RemovedEmitters = std::vector<Emitter*>;
using Particles = std::unordered_map<StateType, std::unique_ptr<ParticleContainer>>;
using ForceApplicatorList = std::vector<ForceApplicator>;
using ForceApplicators = std::unordered_map<StateType, ForceApplicatorList>;

using GeneratorFactory = std::unordered_map<std::string, std::function<BaseGenerator*(void)>>;

class Map;

class ParticleSystem : public FileLoader, public StateDependent, public MapLoadee{
public:
	ParticleSystem(StateManager* l_stateManager, TextureManager* l_textureMgr, RandomGenerator* l_rand, Map* l_map);
	Emitter* AddEmitter(std::unique_ptr<Emitter> l_emitter, const StateType& l_state = StateType(0));
	Emitter* AddEmitter(const sf::Vector3f& l_position, std::string& l_generators, unsigned int l_emitRate, const StateType& l_state = StateType(0));
	void AddForce(ForceApplicator l_force, const StateType& l_state = StateType(0));
	void RemoveEmitter(Emitter* l_emitter);

	Emitter* FindEmitter(const sf::Vector2f& l_point, const sf::Vector2f& l_area,
		unsigned int l_layerMin = 0, unsigned int l_layerMax = -1);

	GeneratorList* GetGenerators(const std::string& l_name);

	TextureManager* GetTextureManager() const;
	RandomGenerator* GetRand() const;

	void CreateState(const StateType& l_state);
	void ChangeState(const StateType& l_state);
	void RemoveState(const StateType& l_state);

	void PurgeCurrentState();

	void ReadMapLine(const std::string& l_type, std::stringstream& l_stream);
	void SaveMap(std::ofstream& l_fileStream);

	void Update(float l_dT);
	void ApplyForce(const sf::Vector3f& l_center, const sf::Vector3f& l_force, float l_radius);
	void Draw(Window* l_window, int l_elevation);
private:
	bool ProcessLine(std::stringstream& l_stream);
	void ResetForNextFile();

	template<class T>
	void RegisterGenerator(const std::string& l_name) { m_factory[l_name] = []() -> BaseGenerator* { return new T(); }; }

	std::string m_loadingGenerator;
	Particles m_container;
	Particles::iterator m_stateItr;
	Emitters::iterator m_emitterItr;
	Updaters m_updaters;
	Emitters m_emitters;
	Generators m_generators;
	GeneratorFactory m_factory;
	ForceApplicators m_forces;
	RemovedEmitters m_removedEmitters;
	TextureManager* m_textureMgr;
	StateManager* m_stateManager;
	RandomGenerator* m_rand;
	Map* m_map;
};