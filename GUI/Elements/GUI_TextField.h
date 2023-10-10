#pragma once
#include "../GUI_Element.h"

class GUI_Textfield : public GUI_Element{
public:
	GUI_Textfield(const std::string& l_name, GUI_Interface* l_owner);
	~GUI_Textfield();

	void HandleChar(const char& l_char);
	void HandleArrowKey(const std::string& l_eventName);

	void ReadIn(std::stringstream& l_stream);
	void OnClick(const sf::Vector2f& l_mousePos);
	void OnRelease();
	void OnHover(const sf::Vector2f& l_mousePos);
	void OnLeave();
	void OnDefocus();
	void Update(float l_dT);
	void Draw(sf::RenderTarget* l_target);
private:
	void ChangeCursorPosition(int l_pos);
	float GetXForCursor();
	sf::Vertex m_cursor[2];
	int m_cursorPosition;
	size_t m_cursorSize;

	float m_time;
	bool m_showCursor;
};