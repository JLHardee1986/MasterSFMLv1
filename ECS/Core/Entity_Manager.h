#pragma once
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include "../Core/C_Base.h"
#include "../../Types/Bitmask.h"
#include "../../Resources/TextureManager.h"
#include "../../MapSystem/MapLoadee.h"
#include "../../MapSystem/MapDefinitions.h"

using EntityId = unsigned int;

using EntityIdList = std::vector<EntityId>;

using C_Type = std::unique_ptr<C_Base>;
using ComponentContainer = std::vector<C_Type>;
using EntityData = std::pair<Bitmask,ComponentContainer>;
using EntityContainer = std::unordered_map<EntityId,EntityData>;
using EntityTypes = std::unordered_map<EntityId, std::string>;
using ComponentFactory = std::unordered_map<Component,std::function<C_Base*(void)>>;

class SystemManager;
class EntityManager : public MapLoadee{
public:
	EntityManager(SystemManager* l_sysMgr, TextureManager* l_textureMgr);
	~EntityManager();

	int AddEntity(const Bitmask& l_mask);
	int AddEntity(const std::string& l_entityFile);
	bool RemoveEntity(const EntityId& l_id);

	int FindEntityAtPoint(const sf::Vector2f& l_point, unsigned int l_elevationMin = 0,
		unsigned int l_elevationMax = Sheet::Num_Layers - 1) const;

	void ReadMapLine(const std::string& l_type, std::stringstream& l_stream);
	void SaveMap(std::ofstream& l_fileStream);

	bool AddComponent(const EntityId& l_entity, const Component& l_component);

	template<class T>
	void AddComponentType(const Component& l_id) {
		m_cFactory[l_id] = []()->C_Base* { return new T(); };
	}

	template<class T>
	T* GetComponent(const EntityId& l_entity, const Component& l_component)
	{
		auto itr = m_entities.find(l_entity);
		if (itr == m_entities.end()) { return nullptr; }
		// Found the entity.
		if (!itr->second.first.GetBit((unsigned int)l_component)) { return nullptr; }
		// Component exists.
		auto& container = itr->second.second;
		auto component = std::find_if(container.begin(), container.end(),
			[&l_component](C_Type& c) { return c->GetType() == l_component; });
		return (component != container.end() ? dynamic_cast<T*>(&(*component->get())) : nullptr);
	}

	bool RemoveComponent(const EntityId& l_entity, const Component& l_component);
	bool HasComponent(const EntityId& l_entity, const Component& l_component) const;

	void Purge();
private:
	// Data members
	unsigned int m_idCounter;
	EntityContainer m_entities;
	EntityTypes m_entityTypes;
	ComponentFactory m_cFactory;

	SystemManager* m_systems;
	TextureManager* m_textureManager;
};