#pragma once
#include <vector>
#include <unordered_map>
#include <assert.h>
#include <functional>
#include <fstream>
#include <sstream>
#include <memory>
#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Binding.h"
#include "../Utilities/Utilities.h"
#include "../StateSystem/StateDependent.h"

using Bindings = std::unordered_map<std::string, std::unique_ptr<Binding>>;
// Callback container.
using CallbackContainer = std::unordered_map<std::string, std::function<void(EventDetails*)>>;
// State callback container.
enum class StateType;
using Callbacks = std::unordered_map<StateType, CallbackContainer>;

class EventManager : public StateDependent{
public:
	EventManager();
	~EventManager();

	bool AddBinding(std::unique_ptr<Binding> l_binding);
	bool RemoveBinding(std::string l_name);

	void ChangeState(const StateType& l_state);
	void RemoveState(const StateType& l_state);
	void SetFocus(bool l_focus);

	// Needs to be defined in the header!
	template<class T>
	bool AddCallback(const StateType& l_state, const std::string& l_name, 
		void(T::*l_func)(EventDetails*), T* l_instance)
	{
		auto itr = m_callbacks.emplace(l_state, CallbackContainer()).first;
		auto temp = std::bind(l_func,l_instance, std::placeholders::_1);
		return itr->second.emplace(l_name, temp).second;
	}
	
	template<class T>
	bool AddCallback(const std::string& l_name, void(T::*l_func)(EventDetails*), T* l_instance) {
		return AddCallback<T>(m_currentState, l_name, l_func, l_instance);
	}

	bool RemoveCallback(const StateType& l_state, const std::string& l_name) {
		auto itr = m_callbacks.find(l_state);
		if (itr == m_callbacks.end()) { return false; }
		auto itr2 = itr->second.find(l_name);
		if (itr2 == itr->second.end()) { return false; }
		itr->second.erase(l_name);
		return true;
	}

	void HandleEvent(sf::Event& l_event);
	void HandleEvent(GUI_Event& l_event);
	void Update();

	sf::Vector2i GetMousePos(sf::RenderWindow* l_wind = nullptr) const {
		return (l_wind ? sf::Mouse::getPosition(*l_wind) : sf::Mouse::getPosition());
	}
private:
	void LoadBindings();

	Bindings m_bindings;
	Callbacks m_callbacks;

	bool m_hasFocus;
};