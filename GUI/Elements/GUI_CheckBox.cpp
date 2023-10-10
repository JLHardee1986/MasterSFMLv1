#include "GUI_CheckBox.h"

GUI_CheckBox::GUI_CheckBox(const std::string & l_name, GUI_Interface * l_owner)
	: GUI_Element(l_name, GUI_ElementType::CheckBox, l_owner) {}

GUI_CheckBox::~GUI_CheckBox() {}
void GUI_CheckBox::ReadIn(std::stringstream & l_stream) {}

void GUI_CheckBox::OnClick(const sf::Vector2f & l_mousePos) {
	if(m_state == GUI_ElementState::Clicked) { SetState(GUI_ElementState::Neutral); }
	else { SetState(GUI_ElementState::Clicked); }
}
void GUI_CheckBox::OnRelease() {}

void GUI_CheckBox::OnHover(const sf::Vector2f & l_mousePos) {
	if(m_state == GUI_ElementState::Clicked) { return; }
	SetState(GUI_ElementState::Focused);
}

void GUI_CheckBox::OnLeave() {
	if (m_state == GUI_ElementState::Clicked) { return; }
	SetState(GUI_ElementState::Neutral);
}

void GUI_CheckBox::Update(float l_dT) {}
void GUI_CheckBox::Draw(sf::RenderTarget* l_target) {}
