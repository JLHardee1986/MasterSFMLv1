#include "Entity_Manager.h"
#include "System_Manager.h"
#include "../EntityEvents.h"
#include "../Components/C_SpriteSheet.h"
#include "../Components/C_Position.h"
#include "../Components/C_Collidable.h"
#include "../Components/C_SpriteSheet.h"

EntityManager::EntityManager(SystemManager* l_sysMgr, 
	TextureManager* l_textureMgr): m_idCounter(0), 
	m_systems(l_sysMgr), m_textureManager(l_textureMgr) {}

EntityManager::~EntityManager() { Purge(); }

int EntityManager::AddEntity(const Bitmask& l_mask) {
	unsigned int entity = m_idCounter;
	if (!m_entities.emplace(entity, EntityData(0, ComponentContainer())).second)
	{ return -1; }
	++m_idCounter;
	
	for(unsigned int i = 0; i < (unsigned int)Component::COUNT; ++i) {
		if(l_mask.GetBit(i)) { AddComponent(entity,(Component)i); }
	}
	// Notifying the system manager of a modified entity.
	m_systems->EntityModified(entity,l_mask);
	m_systems->AddEvent(entity,(EventID)EntityEvent::Spawned);
	return entity;
}

int EntityManager::AddEntity(const std::string& l_entityFile) {
	int e_id = -1;

	std::ifstream file;
	file.open(Utils::GetWorkingDirectory() + "media/Entities/" + l_entityFile + ".entity");
	if (!file.is_open()) {
		std::cout << "! Failed to load entity: " << l_entityFile << std::endl;
		return -1;
	}
	std::string line;
	std::string entityType;
	while(std::getline(file,line)) {
		if (line[0] == '|') { continue; }
		std::stringstream keystream(line);
		std::string type;
		keystream >> type;
		if(type == "Name") {
			keystream >> entityType;
		} else if(type == "Attributes") {
			if (e_id != -1) { continue; }
			Bitset set = 0;
			Bitmask mask;
			keystream >> set;
			mask.SetMask(set);
			e_id = AddEntity(mask);
			if(e_id == -1) { return -1; }
		} else if(type == "Component") {
			if (e_id == -1) { continue; }
			unsigned int c_id = 0;
			keystream >> c_id;
			auto component = GetComponent<C_Base>(e_id,(Component)c_id);
			if (!component) { continue; }
			keystream >> *component;
			if(component->GetType() == Component::SpriteSheet) {
				auto sheet = dynamic_cast<C_SpriteSheet*>(component);
				sheet->Create(m_textureManager);
			}
		}
	}
	file.close();
	m_entityTypes.emplace(std::make_pair(static_cast<EntityId>(e_id), entityType));
	return e_id;
}

bool EntityManager::RemoveEntity(const EntityId& l_id) {
	auto itr = m_entities.find(l_id);
	if (itr == m_entities.end()) { return false; }
	m_entities.erase(itr);
	m_systems->RemoveEntity(l_id);
	return true;
}

int EntityManager::FindEntityAtPoint(const sf::Vector2f& l_point, unsigned int l_elevationMin, unsigned int l_elevationMax) const {
	for (auto& entity : m_entities) {
		auto& bitmask = entity.second.first;
		auto& components = entity.second.second;
		if (!bitmask.GetBit(static_cast<unsigned int>(Component::Position))) { continue; }
		auto position = static_cast<C_Position*>(std::find_if(components.begin(), components.end(),
				[](const C_Type& l_component) { return l_component->GetType() == Component::Position; }
			)->get());

		if (position->GetElevation() < l_elevationMin || position->GetElevation() > l_elevationMax) { continue; }

		if (bitmask.GetBit(static_cast<unsigned int>(Component::Collidable))) {
			auto component = static_cast<C_Collidable*>(std::find_if(components.begin(), components.end(),
				[](const C_Type& l_component) { return l_component->GetType() == Component::Collidable; }
			)->get());
			if (component->GetCollidable().contains(l_point)) { return entity.first; }
		} else if (bitmask.GetBit(static_cast<unsigned int>(Component::SpriteSheet))) {
			auto component = static_cast<C_SpriteSheet*>(std::find_if(components.begin(), components.end(),
				[](const C_Type& l_component) { return l_component->GetType() == Component::SpriteSheet; }
			)->get());
			if (component->GetSpriteSheet()->GetSpriteBounds().contains(l_point)) { return entity.first; }
		} else {
			sf::FloatRect rect;
			rect.left = position->GetPosition().x - 16.f;
			rect.top = position->GetPosition().y - 16.f;
			rect.width = 32.f;
			rect.height = 32.f;
			if (rect.contains(l_point)) { return entity.first; }
		}
	}
	return -1;
}

void EntityManager::ReadMapLine(const std::string& l_type, std::stringstream& l_stream) {}
void EntityManager::SaveMap(std::ofstream& l_fileStream) {
	for (auto& entity : m_entities) {
		auto type = m_entityTypes.find(entity.first);
		if (type == m_entityTypes.end()) { continue; }
		Component comp = Component::Position;
		if (!entity.second.first.GetBit(static_cast<unsigned int>(comp))) { continue; }
		auto component = std::find_if(entity.second.second.begin(), entity.second.second.end(),
			[comp](C_Type& c) { return c->GetType() == comp; });
		if (component == entity.second.second.end()) { continue; }
		auto position = static_cast<C_Position*>(component->get());
		l_fileStream << "ENTITY " << type->second << " " << position->GetPosition().x << " "
			<< position->GetPosition().y << " " << position->GetElevation() << std::endl;
	}
}

bool EntityManager::AddComponent(const EntityId& l_entity, const Component& l_component)
{
	auto itr = m_entities.find(l_entity);
	if (itr == m_entities.end()) { return false; }
	if (itr->second.first.GetBit((unsigned int)l_component)) { return false; }
	// Component doesn't exist.
	auto itr2 = m_cFactory.find(l_component);
	if (itr2 == m_cFactory.end()) { return false; }
	// Component type does exist.
	itr->second.second.emplace_back(C_Type(itr2->second()));
	itr->second.first.TurnOnBit((unsigned int)l_component);
	// Notifying the system manager of a modified entity.
	m_systems->EntityModified(l_entity,itr->second.first);
	return true;
}

bool EntityManager::RemoveComponent(const EntityId& l_entity, const Component& l_component)
{
	auto itr = m_entities.find(l_entity);
	if (itr == m_entities.end()) { return false; }
	// Found the entity.
	if (!itr->second.first.GetBit((unsigned int)l_component)) { return false; }
	// Component exists.
	auto& container = itr->second.second;
	auto component = std::find_if(container.begin(), container.end(),
		[&l_component](C_Type& c) { return c->GetType() == l_component; });
	if (component == container.end()) { return false; }
	container.erase(component);
	itr->second.first.ClearBit((unsigned int)l_component);

	m_systems->EntityModified(l_entity, itr->second.first);
	return true;
}

bool EntityManager::HasComponent(const EntityId& l_entity, const Component& l_component) const
{
	auto itr = m_entities.find(l_entity);
	if (itr == m_entities.end()) { return false; }
	return itr->second.first.GetBit((unsigned int)l_component);
}

void EntityManager::Purge() {
	m_systems->PurgeEntities();
	m_entities.clear();
	m_idCounter = 0;
}