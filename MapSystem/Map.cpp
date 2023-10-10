#include "Map.h"
#include "../StateSystem/StateManager.h"
#include "../ECS/Core/Entity_Manager.h"
#include "../ECS/Components/C_Position.h"
#include "../WindowSystem/Window.h"
#include "../StateSystem/States/State_Loading.h"

Map::Map(Window* l_window, EntityManager* l_entityManager, TextureManager* l_textureManager)
	: m_window(l_window), m_textureManager(l_textureManager), m_defaultTile(nullptr),
	m_entityManager(l_entityManager), m_tileMap({ 0, 0 }, m_tileSet), m_playerId(-1), m_tileSet(l_textureManager),
	m_gameTime(0.f), m_dayLength(30.f)
{
	m_layerSprite.setPosition({ 0,0 });
}

Map::~Map() {
	PurgeMap();
	m_tileSet.Purge();
}

void Map::SetStateManager(StateManager* l_stateManager) { m_stateManager = l_stateManager; }

Tile* Map::GetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer)
{
	return m_tileMap.GetTile(l_x, l_y, l_layer);
}

TileInfo* Map::GetDefaultTile() { return &m_defaultTile; }
TileSet* Map::GetTileSet() { return &m_tileSet; }

TileMap* Map::GetTileMap() { return &m_tileMap; }

unsigned int Map::GetTileSize()const{ return Sheet::Tile_Size; }
sf::Vector2u Map::GetMapSize()const{ return m_tileMap.GetMapSize(); }
sf::Vector2f Map::GetPlayerStart()const{ return m_playerStart; }
int Map::GetPlayerId()const{ return m_playerId; }

void Map::SaveToFile(const std::string& l_file) {
	std::ofstream file(l_file, std::ios::out);
	if (!file.is_open()) { std::cout << "Failed to save map to location: " << l_file << std::endl; return; }
	auto mapSize = m_tileMap.GetMapSize();
	file << "SIZE " << mapSize.x << " " << mapSize.y << std::endl;
	file << "DEFAULT_FRICTION " << m_defaultTile.m_friction.x << " " << m_defaultTile.m_friction.y << std::endl;
	file << "SHEET " << m_tileSet.GetName() << std::endl;
	for (auto& loadee : m_loadees) { loadee->SaveMap(file); }
	m_tileMap.SaveToFile(file);
	file.close();
}

void Map::Redraw(sf::Vector3i l_from, sf::Vector3i l_to) {
	auto mapSize = m_tileMap.GetMapSize();
	if (mapSize.x == 0 || mapSize.y == 0) { return; }
	if (l_from.x < 0 || l_from.y < 0 || l_from.z < 0) { return; }
	if (l_from.x >= (static_cast<int>(mapSize.x)) || l_from.y >= (static_cast<int>(mapSize.y))) { return; }

	sf::Vector3i originalTo = l_to;
	if (l_to.x < 0 || l_to.x >= (static_cast<int>(mapSize.x))) { l_to.x = (static_cast<int>(mapSize.x - 1)); }
	if (l_to.y < 0 || l_to.y >= (static_cast<int>(mapSize.y))) { l_to.y = (static_cast<int>(mapSize.y - 1)); }
	if (l_to.z < 0 || l_to.z >= Sheet::Num_Layers) { l_to.z = Sheet::Num_Layers - 1; }

	auto realMapSize = mapSize * static_cast<unsigned int>(Sheet::Tile_Size);
	for (auto layer = l_from.z; layer <= l_to.z; ++layer) {
		if (m_textures[layer].getSize() == realMapSize) { continue; }
		if (!m_textures[layer].create(realMapSize.x, realMapSize.y)) { std::cout << "Failed creating tile layer texture!" << std::endl; }
	}

	ClearMapTexture(l_from, originalTo);

	for (auto x = l_from.x; x <= l_to.x; ++x) {
		for (auto y = l_from.y; y <= l_to.y; ++y) {
			for (auto layer = l_from.z; layer <= l_to.z; ++layer) {
				auto tile = m_tileMap.GetTile(x, y, layer);
				if (!tile) { continue; }
				auto& sprite = tile->m_properties->m_sprite;
				sprite.setPosition(static_cast<float>(x * Sheet::Tile_Size), static_cast<float>(y * Sheet::Tile_Size));
				m_textures[layer].draw(sprite);
			}
		}
	}

	for (auto layer = l_from.z; layer <= l_to.z; ++layer) { m_textures[layer].display(); }
}

void Map::ClearMapTexture(sf::Vector3i l_from, sf::Vector3i l_to) {
	auto mapSize = m_tileMap.GetMapSize();
	if (l_from.x < 0 || l_from.y < 0 || l_from.z < 0) { return; }
	if ((l_from.x >= static_cast<int>(mapSize.x)) || (l_from.y >= (static_cast<int>(mapSize.y)))) { return; }
	auto toLayer = (l_to.z < 0 || l_to.z >= Sheet::Num_Layers ? Sheet::Num_Layers - 1 : l_to.z);
	if (l_to.x == -1 && l_to.y == -1) {
		// The entire map needs to be cleared, so just invoke .clear() on desired layers.
		for (auto layer = l_from.z; layer <= toLayer; ++layer) { m_textures[layer].clear({ 0,0,0,0 }); }
		return;
	}
	// Portion of the map needs clearing.
	auto position = sf::Vector2i(l_from.x, l_from.y) * static_cast<int>(Sheet::Tile_Size);
	auto size = sf::Vector2i(
			((l_to.x < 0 ? mapSize.x - 1 : l_to.x) - l_from.x) + 1,
			((l_to.y < 0 ? mapSize.y - 1 : l_to.y) - l_from.y) + 1)
		* static_cast<int>(Sheet::Tile_Size);

	sf::RectangleShape shape;
	shape.setPosition(sf::Vector2f(position));
	shape.setSize(sf::Vector2f(size));
	shape.setFillColor(sf::Color(0,0,0,-255));
	for (auto layer = l_from.z; layer <= toLayer; ++layer) {
		m_textures[layer].draw(shape, sf::BlendMultiply);
		m_textures[layer].display();
	}
}

bool Map::ProcessLine(std::stringstream& l_stream) {
	std::string type;
	if (!(l_stream >> type)) { return false; }
	if(type == "TILE") {
		m_tileMap.ReadInTile(l_stream);
	} else if(type == "SIZE") {
		sf::Vector2u mapSize;
		l_stream >> mapSize.x >> mapSize.y;
		m_tileMap.SetMapSize(mapSize);
	} else if(type == "DEFAULT_FRICTION") {
		l_stream >> m_defaultTile.m_friction.x >> m_defaultTile.m_friction.y;
	} else if(type == "ENTITY") {
		// Set up entity here.
		std::string name;
		l_stream >> name;
		if (name == "Player" && m_playerId != -1) { return true; }
		int entityId = m_entityManager->AddEntity(name);
		if (entityId < 0) { return true; }
		if(name == "Player") { m_playerId = entityId; }
		auto position = m_entityManager->GetComponent<C_Position>(entityId,Component::Position);
		if(position) { l_stream >> *position; }
	} else if (type == "SHEET") {
		std::string sheetName;
		l_stream >> sheetName;
		m_tileSet.ResetWorker();
		m_tileSet.AddFile(Utils::GetWorkingDirectory() + "media/Tilesheets/" + sheetName);
		m_tileSet.SetName(sheetName);
		auto loading = m_stateManager->GetState<State_Loading>(StateType::Loading);
		loading->AddLoader(&m_tileSet);

		while (!m_tileSet.IsDone()) { std::cout << "Waiting for tile set to load..." << std::endl; sf::sleep(sf::seconds(0.5f)); }
	} else {
		// Something else.
		std::cout << "! Passing type \"" << type << "\" to map loadees." << std::endl;
		for (auto& loadee : m_loadees) { loadee->ReadMapLine(type, l_stream); }
	}
	return true;
}

void Map::AddLoadee(MapLoadee* l_loadee) { m_loadees.emplace_back(l_loadee); }

void Map::RemoveLoadee(MapLoadee* l_loadee) {
	m_loadees.erase(std::find_if(m_loadees.begin(), m_loadees.end(),
		[l_loadee](MapLoadee* l_arg) { return l_arg == l_loadee; }
	));
}

void Map::Update(float l_dT) {
	m_gameTime += l_dT;
	if (m_gameTime > m_dayLength * 2) { m_gameTime = 0.f; }
	float timeNormal = m_gameTime / m_dayLength;
	if(timeNormal > 1.f){ timeNormal = 2.f - timeNormal; }
	auto shader = m_window->GetRenderer()->GetShader("default");
	if (!shader) { return; }
	shader->setUniform("timeNormal", timeNormal);
}

void Map::Draw(unsigned int l_layer) {
	if (l_layer >= Sheet::Num_Layers) { return; }
	m_layerSprite.setTexture(m_textures[l_layer].getTexture());
	m_layerSprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(m_textures[l_layer].getSize())));
	m_window->GetRenderer()->UseShader("default");
	m_window->GetRenderer()->Draw(m_layerSprite);
}

void Map::PurgeMap() {
	m_tileMap.Purge();
	m_entityManager->Purge();
	m_playerId = -1;
	m_tileSet.Purge();
	Redraw();
}