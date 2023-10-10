#pragma once
#include <SFML/System/Vector2.hpp>
#include "../Core/C_Base.h"

class C_Position : public C_Base{
public:
	C_Position(): C_Base(Component::Position), m_elevation(0) {}
	~C_Position() {}

	void ReadIn(std::stringstream& l_stream) {
		l_stream >> m_position.x >> m_position.y >> m_elevation;
	}

	sf::Vector2f GetPosition() const { return m_position; }
	sf::Vector2f GetOldPosition() const { return m_positionOld; }
	unsigned int GetElevation() const { return m_elevation; }

	void SetPosition(float l_x, float l_y) {
		m_positionOld = m_position;
		m_position = sf::Vector2f(l_x,l_y);
	}

	void SetPosition(const sf::Vector2f& l_vec) {
		m_positionOld = m_position;
		m_position = l_vec;
	}

	void SetElevation(unsigned int l_elevation) {
		m_elevation = l_elevation;
	}

	void MoveBy(float l_x, float l_y) {
		m_positionOld = m_position;
		m_position += sf::Vector2f(l_x,l_y);
	}

	void MoveBy(const sf::Vector2f& l_vec) {
		m_positionOld = m_position;
		m_position += l_vec;
	}

private:
	sf::Vector2f m_position;
	sf::Vector2f m_positionOld;
	unsigned int m_elevation;
};