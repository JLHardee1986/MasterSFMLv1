#pragma once
#include <string>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include "../../StateSystem/StateTypes.h"

class EventManager;
class GUI_Element;
class GUI_Interface;
class GUI_Manager;
struct EventDetails;

using DropDownEntries = std::vector<std::string>;

class GUI_VerticalDropDown {
public:
	GUI_VerticalDropDown(EventManager* l_eventManager, GUI_Manager* l_guiManager, GUI_Element* l_element = nullptr);
	~GUI_VerticalDropDown();

	void Setup(const StateType& l_state, const std::string& l_name, const std::string& l_fileName);

	void SetPosition(const sf::Vector2f& l_position);
	void Show();
	void BringToFront();
	void Hide();

	std::string GetSelected() const;
	void ResetSelected();

	void AddEntry(const std::string& l_entry);
	void RemoveEntry(const std::string& l_entry);
	void PurgeEntries();

	void OnClick(EventDetails* l_details);
	void Redraw();
private:
	GUI_Interface* m_interface;
	GUI_Element* m_element;
	std::string m_name;
	std::string m_entryStyle;
	StateType m_state;

	DropDownEntries m_entries;

	std::string m_callbackNameBase;

	std::string m_selection;

	GUI_Manager* m_guiManager;
	EventManager* m_eventManager;
};