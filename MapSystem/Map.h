#pragma once
#include <unordered_map>
#include <map>
#include <fstream>
#include <sstream>
#include <memory>
#include <array>
#include <SFML/Graphics.hpp>
#include "MapDefinitions.h"
#include "TileInfo.h"
#include "TileMap.h"
#include "TileSet.h"
#include "MapLoadee.h"
#include "../Resources/TextureManager.h"
#include "../ThreadWorkers/FileLoader.h"
#include "../Utilities/Utilities.h"
#include "../WindowSystem/Window.h"
#include "../ECS/Core/Entity_Manager.h"

using MapLoadees = std::vector< MapLoadee* >;

class Map : public FileLoader{
public:
	Map(Window* l_window, EntityManager* l_entityManager, TextureManager* l_textureManager);
	~Map();

	void SetStateManager(StateManager* l_stateManager);

	Tile* GetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer);
	TileInfo* GetDefaultTile();

	TileSet* GetTileSet();
	TileMap* GetTileMap();

	unsigned int GetTileSize()const;
	sf::Vector2u GetMapSize()const;
	sf::Vector2f GetPlayerStart()const;
	int GetPlayerId()const;

	void PurgeMap();

	void AddLoadee(MapLoadee* l_loadee);
	void RemoveLoadee(MapLoadee* l_loadee);

	void SaveToFile(const std::string& l_file);

	void Update(float l_dT);
	void ClearMapTexture(sf::Vector3i l_from = sf::Vector3i(0, 0, 0), sf::Vector3i l_to = sf::Vector3i(-1, -1, -1));
	void Redraw(sf::Vector3i l_from = sf::Vector3i(0, 0, 0), sf::Vector3i l_to = sf::Vector3i(-1, -1, -1));
	void Draw(unsigned int l_layer);
protected:
	bool ProcessLine(std::stringstream& l_stream);

	Window* m_window;
	EntityManager* m_entityManager;
	TextureManager* m_textureManager;
	StateManager* m_stateManager;

	TileSet m_tileSet;
	TileMap m_tileMap;
	MapLoadees m_loadees;

	std::array<sf::RenderTexture, Sheet::Num_Layers> m_textures;
	sf::Sprite m_layerSprite;

	sf::Vector2u m_sheetSize;

	TileInfo m_defaultTile;

	sf::Vector2f m_playerStart;
	int m_playerId;

	float m_gameTime;
	float m_dayLength;
};