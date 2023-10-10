#pragma once
#include <unordered_map>
#include <memory>
#include <SFML/Graphics.hpp>
#include "S_Base.h"
#include "../../WindowSystem/Window.h"
#include "../../Types/Event_Queue.h"
#include "../../Observer/MessageHandler.h"
//#include "../../Debug/DebugOverlay.h"

using SystemContainer = std::unordered_map<System, std::unique_ptr<S_Base>>;
using EntityEventContainer = std::unordered_map<EntityId,EventQueue>;

class EntityManager;
class SystemManager{
public:
	SystemManager();
	~SystemManager();

	void SetEntityManager(EntityManager* l_entityMgr);
	EntityManager* GetEntityManager();
	MessageHandler* GetMessageHandler();

	template<class T>
	void AddSystem(const System& l_system) {
		m_systems[l_system] = std::make_unique<T>(this);
	}

	template<class T>
	T* GetSystem(const System& l_system) {
		auto itr = m_systems.find(l_system);
		return(itr != m_systems.end() ? dynamic_cast<T*>(itr->second.get()) : nullptr);
	}
	
	void AddEvent(const EntityId& l_entity, const EventID& l_event);

	void Update(float l_dT);
	void HandleEvents();
	void Draw(Window* l_wind, unsigned int l_elevation);

	void EntityModified(const EntityId& l_entity, const Bitmask& l_bits);
	void RemoveEntity(const EntityId& l_entity);
	
	void PurgeEntities();
	void PurgeSystems();
private:
	SystemContainer m_systems;
	EntityManager* m_entityManager;
	EntityEventContainer m_events;
	MessageHandler m_messages;
};