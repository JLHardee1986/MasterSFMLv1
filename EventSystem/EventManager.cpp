#include "EventManager.h"
#include "../StateSystem/StateManager.h"

EventManager::EventManager() : m_hasFocus(true) { LoadBindings(); }
EventManager::~EventManager() {}

bool EventManager::AddBinding(std::unique_ptr<Binding> l_binding) {
	if(m_bindings.find(l_binding->m_name) != m_bindings.end())
		return false;

	return m_bindings.emplace(l_binding->m_name, std::move(l_binding)).second;
}

bool EventManager::RemoveBinding(std::string l_name) {
	auto itr = m_bindings.find(l_name);
	if (itr == m_bindings.end()) { return false; }
	m_bindings.erase(itr);
	return true;
}

void EventManager::ChangeState(const StateType& l_state) { SetState(l_state); }
void EventManager::RemoveState(const StateType& l_state) { m_callbacks.erase(l_state); }
void EventManager::SetFocus(bool l_focus) { m_hasFocus = l_focus; }

void EventManager::HandleEvent(sf::Event& l_event) {
	// Handling SFML events.
	for(auto &b_itr : m_bindings) {
		auto& bind = b_itr.second;
		for(auto &e_itr : bind->m_events) {
			EventType sfmlEvent = static_cast<EventType>(l_event.type);
			if (e_itr.first == EventType::GUI_Click || e_itr.first == EventType::GUI_Release ||
				e_itr.first == EventType::GUI_Hover || e_itr.first == EventType::GUI_Leave)
			{
				continue;
			}
			if (e_itr.first != sfmlEvent) { continue; }
			if (sfmlEvent == EventType::KeyDown || sfmlEvent == EventType::KeyUp) {
				if (e_itr.second.m_code == l_event.key.code) {
					// Matching event/keystroke.
					// Increase count.
					if (bind->m_details.m_keyCode == -1) {
						bind->m_details.m_keyCode = e_itr.second.m_code;
					}
					if (sfmlEvent == EventType::KeyDown) {
						bind->m_details.m_controlPressed = l_event.key.control;
						bind->m_details.m_altPressed = l_event.key.alt;
						bind->m_details.m_shiftPressed = l_event.key.shift;
						bind->m_details.m_systemPressed = l_event.key.system;
					}
					++(bind->c);
					break;
				}
			} else if (sfmlEvent == EventType::MButtonDown || sfmlEvent == EventType::MButtonUp) {
				if (e_itr.second.m_code == l_event.mouseButton.button) {
					// Matching event/keystroke.
					// Increase count.
					bind->m_details.m_mouse.x = l_event.mouseButton.x;
					bind->m_details.m_mouse.y = l_event.mouseButton.y;
					if (bind->m_details.m_keyCode == -1) {
						bind->m_details.m_keyCode = e_itr.second.m_code;
					}
					++(bind->c);
					break;
				}
			} else {
				// No need for additional checking.
				if (sfmlEvent == EventType::MouseWheel) {
					bind->m_details.m_mouseWheelDelta = l_event.mouseWheel.delta;
				} else if (sfmlEvent == EventType::WindowResized) {
					bind->m_details.m_size.x = l_event.size.width;
					bind->m_details.m_size.y = l_event.size.height;
				} else if (sfmlEvent == EventType::TextEntered) {
					bind->m_details.m_textEntered = l_event.text.unicode;
				}
				++(bind->c);
			}
		}
	}
}

void EventManager::HandleEvent(GUI_Event& l_event) {
	for (auto &b_itr : m_bindings) {
		auto& bind = b_itr.second;
		for (auto &e_itr : bind->m_events)
		{
			if (e_itr.first != EventType::GUI_Click && e_itr.first != EventType::GUI_Release &&
				e_itr.first != EventType::GUI_Hover && e_itr.first != EventType::GUI_Leave &&
				e_itr.first != EventType::GUI_Focus && e_itr.first != EventType::GUI_Defocus)
			{ continue; }
			if ((e_itr.first == EventType::GUI_Click && l_event.m_type != GUI_EventType::Click) ||
				(e_itr.first == EventType::GUI_Release && l_event.m_type != GUI_EventType::Release) ||
				(e_itr.first == EventType::GUI_Hover && l_event.m_type != GUI_EventType::Hover) ||
				(e_itr.first == EventType::GUI_Leave && l_event.m_type != GUI_EventType::Leave) ||
				(e_itr.first == EventType::GUI_Focus && l_event.m_type != GUI_EventType::Focus) ||
				(e_itr.first == EventType::GUI_Defocus && l_event.m_type != GUI_EventType::Defocus))
			{ continue; }

			if (e_itr.second.m_gui.m_interface != l_event.m_interface) { continue; }
			if (e_itr.second.m_gui.m_element == "*") {
				if (l_event.m_element.empty()) { continue; }
			} else if (e_itr.second.m_gui.m_element != l_event.m_element) { continue; }
			bind->m_details.m_guiInterface = l_event.m_interface;
			bind->m_details.m_guiElement = l_event.m_element;
			bind->m_details.m_mouse = sf::Vector2i((int)l_event.m_clickCoords.x, (int)l_event.m_clickCoords.y);
			++(bind->c);
		}
	}
}

void EventManager::Update() {
	if (!m_hasFocus) { return; }
	for (auto &b_itr : m_bindings) {
		auto& bind = b_itr.second;
		for(auto &e_itr : bind->m_events) {
			switch(e_itr.first) {
			case(EventType::Keyboard):
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key(e_itr.second.m_code))) {
					if (bind->m_details.m_keyCode != -1) {
						bind->m_details.m_keyCode = e_itr.second.m_code;
					}
					++(bind->c);
				}
				break;
			case(EventType::Mouse):
				if(sf::Mouse::isButtonPressed(sf::Mouse::Button(e_itr.second.m_code))) {
					if (bind->m_details.m_keyCode != -1) {
						bind->m_details.m_keyCode = e_itr.second.m_code;
					}
					++(bind->c);
				}
				break;
			case(EventType::Joystick):
				// Up for expansion.
				break;
			}
		}

		if(bind->m_events.size() == bind->c) {
			auto otherCallbacks = m_callbacks.find(StateType(0));
			auto stateCallbacks = m_callbacks.find(m_currentState);

			if (otherCallbacks != m_callbacks.end()) {
				auto callItr = otherCallbacks->second.find(bind->m_name);
				if (callItr != otherCallbacks->second.end()) {
					// Pass in information about events.
					callItr->second(&bind->m_details);
				}
			}

			if(stateCallbacks != m_callbacks.end()) {
				auto callItr = stateCallbacks->second.find(bind->m_name);
				if(callItr != stateCallbacks->second.end()) {
					// Pass in information about events.
					callItr->second(&bind->m_details);
				}
			}
		}
		bind->c = 0;
		bind->m_details.Clear();
	}
}

void EventManager::LoadBindings() {
	std::string delimiter = ":";

	std::ifstream bindings;
	bindings.open(Utils::GetWorkingDirectory() + "keys.cfg");
	if (!bindings.is_open()) { std::cout << "! Failed loading keys.cfg." << std::endl; return; }
	std::string line;
	while(std::getline(bindings,line)) {
		std::stringstream keystream(line);
		std::string callbackName;
		keystream >> callbackName;
		auto bind = std::make_unique<Binding>(callbackName);
		while(!keystream.eof()) {
			std::string keyval;
			keystream >> keyval;
			int start = 0;
			int end = static_cast<int>(keyval.find(delimiter));
			if (end == std::string::npos) { bind.release(); break; }
			EventType type = EventType(stoi(keyval.substr(start, end-start)));

			if (type == EventType::GUI_Click || type == EventType::GUI_Release ||
				type == EventType::GUI_Hover || type == EventType::GUI_Leave ||
				type == EventType::GUI_Focus || type == EventType::GUI_Defocus)
			{
				start = (static_cast<int>(end + delimiter.length()));
				end = (static_cast<int>(keyval.find(delimiter, start)));
				std::string window = keyval.substr(start, end - start);
				std::string element;
				if (end != std::string::npos) {
					start = end + (int)delimiter.length();
					end = (int)keyval.length();
					element = keyval.substr(start, end);
				}
				GUI_Event guiEvent;
				guiEvent.m_interface = window;
				guiEvent.m_element = element;
				EventInfo eventInfo(guiEvent);
				bind->BindEvent(type, std::move(eventInfo));
			} else {
				int code = std::stoi(keyval.substr(end + delimiter.length(),
					keyval.find(delimiter,end + delimiter.length())));
				EventInfo eventInfo(code);
				bind->BindEvent(type, std::move(eventInfo));
			}
		}

		if (!AddBinding(std::move(bind))) { std::cout << "Failed adding binding..." << std::endl; }
	}
	bindings.close();
}