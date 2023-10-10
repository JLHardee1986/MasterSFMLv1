#pragma once
#include <unordered_map>
#include "TileInfo.h"
#include "TileSet.h"

using TileMapContainer = std::unordered_map<TileID, std::unique_ptr<Tile>>;
using TileType = int;

class TileMap {
public:
	TileMap(const sf::Vector2u& l_mapSize, TileSet& l_tileSet);

	Tile* SetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer, TileType l_tileType);
	Tile* SetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer, Tile* l_tileToCopy);
	Tile* GetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer);
	void PlotTileMap(TileMap& l_map, const sf::Vector2i& l_position, unsigned int l_startLayer);
	void RemoveTile(unsigned int l_x, unsigned int l_y, int l_layer = -1);
	void RemoveTiles(const sf::Vector2u& l_x, const sf::Vector2u& l_y, const sf::Vector2u& l_layer);
	sf::Vector2u GetMapSize() const;
	size_t GetTileCount() const;
	TileSet& GetTileSet() const;
	unsigned int GetLowestElevation() const;
	unsigned int GetHighestElevation() const;
	void SetMapSize(const sf::Vector2u& l_size);

	void SaveToFile(std::ofstream& l_stream);
	void ReadInTile(std::stringstream& l_stream);

	void Purge();
private:
	Tile* CreateTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer);
	TileMapContainer m_container;
	TileSet& m_tileSet;
	sf::Vector2u m_maxMapSize;
};