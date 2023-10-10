#pragma once
#include <SFML/System/Vector2.hpp>

inline unsigned int ConvertCoords(unsigned int l_x, unsigned int l_y, unsigned int l_layer, const sf::Vector2u& l_mapSize)
{
	return ((l_layer*l_mapSize.y + l_y) * l_mapSize.x + l_x);
}

inline void ConvertCoords(unsigned int l_id, unsigned int& l_x, unsigned int& l_y, unsigned int& l_layer, const sf::Vector2u& l_mapSize) {
	l_layer = (l_id / (l_mapSize.x * l_mapSize.y));
	l_y = (l_id - (l_layer * l_mapSize.x * l_mapSize.y)) / l_mapSize.x;
	l_x = l_id - (l_mapSize.x * (l_y + (l_mapSize.y * l_layer)));
}

inline bool ValidCoords(unsigned int l_x, unsigned int l_y, unsigned int l_layer, const sf::Vector2u& l_mapSize) {
	return !(l_x < 0 || l_y < 0 || l_x >= l_mapSize.x || l_y >= l_mapSize.y || l_layer < 0 || l_layer >= Sheet::Num_Layers);
}