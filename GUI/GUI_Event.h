#pragma once
#include <string>

enum class GUI_EventType{ None, Click, Release, Hover, Leave, Focus, Defocus };

struct ClickCoordinates{
	ClickCoordinates& operator=(const ClickCoordinates& l_rhs) {
		if (&l_rhs != this) {
			x = l_rhs.x;
			y = l_rhs.y;
		}
		return *this;
	}
	float x, y;
};

struct GUI_Event{
	GUI_Event& operator=(const GUI_Event& l_rhs) {
		if (&l_rhs != this) {
			m_type = l_rhs.m_type;
			m_element = l_rhs.m_element;
			m_interface = l_rhs.m_interface;
			m_clickCoords = l_rhs.m_clickCoords;
		}
		return *this;
	}

	GUI_EventType m_type;
	std::string m_element;
	std::string m_interface;
	union{
		ClickCoordinates m_clickCoords;
	};
};