#include "GUI_DropDownMenu.h"
#include "../GUI_Manager.h"
#include "../../SharedContext.h"

GUI_DropDownMenu::GUI_DropDownMenu(const std::string& l_name, GUI_Interface* l_owner)
	: GUI_Element(l_name, GUI_ElementType::DropDownMenu, l_owner),
	m_dropDown(m_owner->GetManager()->GetContext()->m_eventManager, m_owner->GetManager(), this)
{
	m_dropDown.Hide();
	SetText("Select...");
}

GUI_DropDownMenu::~GUI_DropDownMenu() {}

GUI_VerticalDropDown* GUI_DropDownMenu::GetMenu() { return &m_dropDown; }

void GUI_DropDownMenu::ReadIn(std::stringstream& l_stream) {
	if (!(l_stream >> m_fileName)) { return; }
}

void GUI_DropDownMenu::OnClick(const sf::Vector2f& l_mousePos) {
	if(m_state == GUI_ElementState::Clicked) {
		SetState(GUI_ElementState::Neutral);
		m_dropDown.Hide();
		return;
	}
	SetState(GUI_ElementState::Clicked);
	m_dropDown.Show();
	m_dropDown.SetPosition(GetGlobalPosition() + sf::Vector2f(0.f, GetSize().y));
}
void GUI_DropDownMenu::OnInterfaceClick(const sf::Vector2f& l_mousePos) {
	SetState(GUI_ElementState::Neutral);
	m_dropDown.Hide();
}

void GUI_DropDownMenu::OnRelease() {}

void GUI_DropDownMenu::OnHover(const sf::Vector2f& l_mousePos) {
	if(m_state == GUI_ElementState::Clicked) { return; }
	SetState(GUI_ElementState::Focused);
}

void GUI_DropDownMenu::OnLeave() {
	if (m_state == GUI_ElementState::Clicked) { return; }
	SetState(GUI_ElementState::Neutral);
}

void GUI_DropDownMenu::OnFocus() {}

void GUI_DropDownMenu::OnDefocus() {
	if (m_state != GUI_ElementState::Clicked) { return; }
	m_dropDown.BringToFront();
}

void GUI_DropDownMenu::Setup() { m_dropDown.Setup(m_owner->GetManager()->GetCurrentState(), "_dropdown_" + m_name, m_fileName); }

void GUI_DropDownMenu::Update(float l_dT) {}
void GUI_DropDownMenu::Draw(sf::RenderTarget* l_target) {
	l_target->draw(m_visual.m_backgroundSolid);
	if (!m_style[m_state].m_glyph.empty()) { l_target->draw(m_visual.m_glyph); }
	l_target->draw(m_visual.m_text);
}