#pragma once
#include "EventTypes.h"
#include "../GUI/GUI_Event.h"

struct EventInfo {
	EventInfo() : m_type(EventInfoType::Normal), m_code(0) {}
	EventInfo(int l_eventCode) : m_type(EventInfoType::Normal), m_code(l_eventCode) {}
	EventInfo(const GUI_Event& l_guiEvent) : m_type(EventInfoType::GUI), m_gui(l_guiEvent) {}
	EventInfo(const EventInfoType& l_type) {
		if (m_type == EventInfoType::GUI) { DestroyGUIStrings(); }
		m_type = l_type;
		if (m_type == EventInfoType::GUI) { CreateGUIStrings("", ""); }
	}

	EventInfo(const EventInfo& l_rhs) { Move(l_rhs); }

	EventInfo& operator=(const EventInfo& l_rhs) {
		if (&l_rhs != this) { Move(l_rhs); }
		return *this;
	}

	~EventInfo() {
		if (m_type == EventInfoType::GUI) { DestroyGUIStrings(); }
	}
	union {
		int m_code;
		GUI_Event m_gui;
	};
	
	EventInfoType m_type;
private:
	void Move(const EventInfo& l_rhs) {
		if (m_type == EventInfoType::GUI) { DestroyGUIStrings(); }
		m_type = l_rhs.m_type;
		if (m_type == EventInfoType::Normal) { m_code = l_rhs.m_code; }
		else { CreateGUIStrings(l_rhs.m_gui.m_interface, l_rhs.m_gui.m_element); m_gui = l_rhs.m_gui; }
	}

	void DestroyGUIStrings() {
		m_gui.m_interface.~basic_string();
		m_gui.m_element.~basic_string();
	}

	void CreateGUIStrings(const std::string& l_interface, const std::string& l_element) {
		new (&m_gui.m_interface) std::string(l_interface);
		new (&m_gui.m_element) std::string(l_element);
	}
};