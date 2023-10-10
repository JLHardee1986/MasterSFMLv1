#pragma once
#include "../GUI_Element.h"
#include "../Specialized/VerticalDropDown.h"

class GUI_DropDownMenu : public GUI_Element {
public:
	GUI_DropDownMenu(const std::string& l_name, GUI_Interface* l_owner);
	~GUI_DropDownMenu();

	GUI_VerticalDropDown* GetMenu();

	void ReadIn(std::stringstream& l_stream);
	void OnClick(const sf::Vector2f& l_mousePos);
	void OnInterfaceClick(const sf::Vector2f& l_mousePos);
	void OnRelease();
	void OnHover(const sf::Vector2f& l_mousePos);
	void OnLeave();
	void OnFocus();
	void OnDefocus();
	void Setup();
	void Update(float l_dT);
	void Draw(sf::RenderTarget* l_target);
private:
	GUI_VerticalDropDown m_dropDown;
	std::string m_fileName;
};