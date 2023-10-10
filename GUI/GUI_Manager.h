#pragma once
#include <unordered_map>
#include <functional>
#include <fstream>
#include <memory>
#include <SFML/Graphics.hpp>
#include "GUI_Interface.h"
#include "GUI_Event.h"
#include "Elements/GUI_DropDownMenu.h"
#include "../EventSystem/EventManager.h"
#include "../StateSystem/StateDependent.h"

using GUI_InterfaceType = std::pair<std::string, std::unique_ptr<GUI_Interface>>;
using GUI_Interfaces = std::vector<GUI_InterfaceType>;
using GUI_Container = std::unordered_map<StateType, GUI_Interfaces>;
using GUI_Events = std::unordered_map<StateType, std::vector<GUI_Event>>;
using GUI_Factory = std::unordered_map<GUI_ElementType, std::function<GUI_Element*(GUI_Interface*)>>;
using GUI_ElemTypes = std::unordered_map <std::string,GUI_ElementType>;

using GUI_BringToFront = std::vector<std::pair<StateType, GUI_Interface*>>;

struct SharedContext;
class GUI_Manager : public StateDependent{
	friend class GUI_Interface;
public:
	GUI_Manager(EventManager* l_evMgr, SharedContext* l_shared);
	~GUI_Manager();

	bool AddInterface(const StateType& l_state, const std::string& l_name);
	bool AddInterface(const std::string& l_name);
	GUI_Interface* GetInterface(const StateType& l_state, const std::string& l_name);
	GUI_Interface* GetInterface(const std::string& l_name);
	bool RemoveInterface(const StateType& l_state, const std::string& l_name);
	bool RemoveInterface(const std::string& l_name);

	bool BringToFront(const GUI_Interface* l_interface);
	bool BringToFront(const StateType& l_state, const std::string& l_name);

	bool LoadInterface(const StateType& l_state, const std::string& l_interface, const std::string& l_name);
	bool LoadInterface(const std::string& l_interface, const std::string& l_name);
	bool LoadInterface(GUI_Interface* l_interface, const std::string& l_fileName);

	void ChangeState(const StateType& l_state);
	void RemoveState(const StateType& l_state);

	SharedContext* GetContext() const;

	void DefocusAllInterfaces();

	void HandleClick(EventDetails* l_details);
	void HandleRelease(EventDetails* l_details);
	void HandleTextEntered(EventDetails* l_details);
	void HandleArrowKeys(EventDetails* l_details);

	void AddEvent(GUI_Event l_event);
	bool PollEvent(GUI_Event& l_event);

	void Update(float l_dT);
	void Render(sf::RenderWindow* l_wind);

	template<class T>
	void RegisterElement(const GUI_ElementType& l_id) {
		m_factory[l_id] = [](GUI_Interface* l_owner) -> GUI_Element*
		{ return new T("",l_owner); };
	}

	bool LoadStyle(const std::string& l_file, GUI_Element* l_element);
private:
	void HandleMovesToFront();
	GUI_Element* CreateElement(const GUI_ElementType& l_id, GUI_Interface* l_owner);
	GUI_ElementType StringToType(const std::string& l_string) const;

	GUI_Container m_interfaces;
	GUI_BringToFront m_toFront;
	GUI_Events m_events;
	SharedContext* m_context;
	EventManager* m_eventMgr;
	GUI_Factory m_factory;
	GUI_ElemTypes m_elemTypes;
};