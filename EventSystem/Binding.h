#pragma once
#include <vector>
#include "EventTypes.h"
#include "EventInfo.h"
#include "EventDetails.h"

using Events = std::vector<std::pair<EventType, EventInfo>>;

struct Binding {
	Binding(const std::string& l_name) : m_name(l_name), m_details(l_name), c(0) {}
	void BindEvent(EventType l_type, EventInfo l_info = EventInfo()) { m_events.emplace_back(l_type, std::move(l_info)); }

	Events m_events;
	std::string m_name;
	int c; // Count of events that are "happening".

	EventDetails m_details;
};