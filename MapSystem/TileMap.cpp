#include "TileMap.h"
#include "MapHelpers.h"

TileMap::TileMap(const sf::Vector2u& l_mapSize, TileSet& l_tileSet) : m_tileSet(l_tileSet) {
	SetMapSize(l_mapSize);
}

Tile* TileMap::SetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer, TileType l_tileType) {
	auto tileInfo = m_tileSet.GetContainer().find(l_tileType);
	if (tileInfo == m_tileSet.GetContainer().end()) { std::cout << "Tile info for " << l_tileType << " wasn't found." << std::endl; return nullptr; }
	auto tile = GetTile(l_x, l_y, l_layer);
	if (!tile) {
		tile = CreateTile(l_x, l_y, l_layer);
		if(!tile){ std::cout << "Couldn't create tile " << l_tileType << std::endl; return nullptr; }
	}
	tile->m_properties = tileInfo->second.get();
	return tile;
}

Tile* TileMap::SetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer, Tile* l_tileToCopy) {
	auto tile = GetTile(l_x, l_y, l_layer);
	if (!tile) {
		tile = CreateTile(l_x, l_y, l_layer);
		if (!tile) { std::cout << "Couldn't create tile..." << std::endl; return nullptr; }
	}
	*tile = *l_tileToCopy;
	return tile;
}

Tile* TileMap::GetTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer)
{
	if (!ValidCoords(l_x, l_y, l_layer, m_maxMapSize)) { return nullptr; }

	auto itr = m_container.find(ConvertCoords(l_x, l_y, l_layer, m_maxMapSize));
	if (itr == m_container.end()) { return nullptr; }
	return itr->second.get();
}

void TileMap::PlotTileMap(TileMap& l_map, const sf::Vector2i& l_position, unsigned int l_startLayer) {
	for (unsigned int x = 0; x < l_map.GetMapSize().x; ++x) {
		for (unsigned int y = 0; y < l_map.GetMapSize().y; ++y) {
			for (int layer = 0; layer < (int)Sheet::Num_Layers; ++layer) {
				auto position = l_position + sf::Vector2i(x, y);
				if (position.x >= static_cast<int>(m_maxMapSize.x) || position.y >= static_cast<int>(m_maxMapSize.y)) { continue; }
				auto tile = l_map.GetTile(x, y, layer);
				if (!tile) { continue; } // Empty tile. Skip it, don't overwrite the main map!
				auto newTile = SetTile(l_position.x + x, l_position.y + y, layer + l_startLayer, tile->m_properties->m_id); // use lower-level code to avoid having to re-fetch TileInfo struct.
				if (!newTile) { std::cout << "Failed adding tile..." << std::endl; continue; }
				*newTile = *tile;
			}
		}
	}
}
 
void TileMap::RemoveTile(unsigned int l_x, unsigned int l_y, int l_layer) {
	if (!ValidCoords(l_x, l_y, l_layer, m_maxMapSize)) { return; }

	for (auto layer = (l_layer == -1 ? 0 : l_layer);
		layer <= (l_layer == -1 ? Sheet::Num_Layers : l_layer); ++layer)
	{
		auto itr = m_container.find(ConvertCoords(l_x, l_y, layer, m_maxMapSize));
		if (itr == m_container.end()) { continue; }
		m_container.erase(itr);
	}
}

void TileMap::RemoveTiles(const sf::Vector2u& l_x, const sf::Vector2u& l_y, const sf::Vector2u& l_layer)
{
	if (!ValidCoords(l_x.x, l_y.x, l_layer.x, m_maxMapSize)) { return; }
	if (!ValidCoords(l_x.y, l_y.y, l_layer.y, m_maxMapSize)) { return; }

	for (auto x = l_x.x; x <= l_x.y; ++x) {
		for (auto y = l_y.x; y <= l_y.y; ++y) {
			for (auto layer = l_layer.x; layer <= l_layer.y; ++layer) {
				auto itr = m_container.find(ConvertCoords(x, y, layer, m_maxMapSize));
				if (itr == m_container.end()) { continue; }
				m_container.erase(itr);
			}
		}
	}
}

sf::Vector2u TileMap::GetMapSize() const {
	return m_maxMapSize;
}

size_t TileMap::GetTileCount() const { return m_container.size(); }

TileSet& TileMap::GetTileSet() const { return m_tileSet; }

unsigned int TileMap::GetLowestElevation() const {
	unsigned int x = 0, y = 0, layer = 0;
	unsigned int ret = 0;
	for (auto& tile : m_container) {
		ConvertCoords(tile.first, x, y, layer, m_maxMapSize);
		if (layer == 0) { return 0; }
		if (layer < ret) { ret = layer; }
	}
	return ret;
}

unsigned int TileMap::GetHighestElevation() const {
	unsigned int x = 0, y = 0, layer = 0;
	unsigned int ret = 0;
	for (auto& tile : m_container) {
		ConvertCoords(tile.first, x, y, layer, m_maxMapSize);
		if (layer == Sheet::Num_Layers - 1) { return Sheet::Num_Layers - 1; }
		if (layer > ret) { ret = layer; }
	}
	return ret;
}

void TileMap::SetMapSize(const sf::Vector2u& l_size) {
	// Perform re-allocations and re-calculations of tiles that are already inside the map, as the dimensions are changing.
	m_maxMapSize = l_size;
}

void TileMap::SaveToFile(std::ofstream& l_stream) {
	bool firstLine = true;
	for (auto& tile : m_container) {
		auto coordinateID = tile.first;
		unsigned int x = 0, y = 0, layer = 0;
		ConvertCoords(coordinateID, x, y, layer, m_maxMapSize);
		auto tileID = tile.second->m_properties->m_id;
		if (!firstLine) { l_stream << std::endl; }
		l_stream << "TILE " << tileID << " " << x << " " << y << " " << layer
			<< " " << (tile.second->m_solid ? 1 : 0);
		if (firstLine) { firstLine = false; }
	}
}

void TileMap::ReadInTile(std::stringstream& l_stream) {
	int tileId = 0;
	l_stream >> tileId;
	if (tileId < 0) {
		std::cout << "! Bad tile id: " << tileId << std::endl;
		return;
	}
	auto itr = m_tileSet.GetContainer().find(tileId);
	if (itr == m_tileSet.GetContainer().end()) {
		std::cout << "! Tile id(" << tileId << ") was not found in tileset." << std::endl;
		return;
	}
	sf::Vector2u tileCoords;
	unsigned int tileLayer = 0;
	unsigned int tileSolidity = 0;
	l_stream >> tileCoords.x >> tileCoords.y >> tileLayer >> tileSolidity;
	auto mapSize = GetMapSize();
	if (tileCoords.x > mapSize.x || tileCoords.y > mapSize.y || tileLayer >= Sheet::Num_Layers) {
		std::cout << "! Tile is out of range: " << tileCoords.x << " " << tileCoords.y << std::endl;
		return;
	}
	auto tile = std::make_unique<Tile>();
	// Bind properties of a tile from a set.
	tile->m_properties = itr->second.get();
	tile->m_solid = (tileSolidity != 0);
	std::string warp;
	l_stream >> warp;
	tile->m_warp = false;
	if (warp == "WARP") { tile->m_warp = true; }

	if (!m_container.emplace(ConvertCoords(tileCoords.x, tileCoords.y, tileLayer, m_maxMapSize), std::move(tile)).second) {
		// Duplicate tile detected!
		std::cout << "! Duplicate tile! : " << tileCoords.x << " " << tileCoords.y << std::endl;
		return;
	}
}

void TileMap::Purge() { m_container.clear(); }

Tile* TileMap::CreateTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer) {
	auto tile = std::make_unique<Tile>();
	auto tileAddr = tile.get();
	if (!m_container.emplace(ConvertCoords(l_x, l_y, l_layer, m_maxMapSize), std::move(tile)).second) {
		return nullptr;
	}
	return tileAddr;
}