#include <algorithm>
#include <iostream>
#include "GUI_Textfield.h"
#include "../../Utilities/Utilities.h"

GUI_Textfield::GUI_Textfield(const std::string& l_name, GUI_Interface* l_owner)
	: GUI_Element(l_name, GUI_ElementType::Textfield , l_owner), m_cursorPosition(0), m_cursorSize(0), m_time(0.f), m_showCursor(false)
{
	
}

GUI_Textfield::~GUI_Textfield() {}

void GUI_Textfield::HandleChar(const char& l_char) {
	if (GetState() != GUI_ElementState::Clicked) { return; }
	if (l_char == 8) {
		// Backspace.
		if (!m_cursorPosition) { return; }
		auto text = GetText();
		text.erase(m_cursorPosition - 1, 1);
		SetText(text);
		ChangeCursorPosition(-1);
		return;
	}
	if (l_char < 32 || l_char > 126) { return; }
	auto text = GetText();
	text.insert(m_cursorPosition, 1, l_char);
	SetText(text);
	ChangeCursorPosition(+1);
	return;
}

void GUI_Textfield::HandleArrowKey(const std::string& l_eventName) {
	if (m_state != GUI_ElementState::Clicked) { return; }
	if (l_eventName == "Key_LeftArrow") {
		ChangeCursorPosition(-1);
	} else if (l_eventName == "Key_RightArrow") {
		ChangeCursorPosition(1);
	}
	m_needsRedraw = true;
}

void GUI_Textfield::ReadIn(std::stringstream& l_stream) {
	std::string content;
	Utils::ReadQuotedString(l_stream, content);
	m_visual.m_text.setString(content);
}
void GUI_Textfield::OnClick(const sf::Vector2f& l_mousePos) {
	// check the local coordinates and move cursor to click pos!
	SetState(GUI_ElementState::Clicked);
	m_cursorPosition = (static_cast<int>(GetText().size()));
	if (m_cursorPosition < 0) { m_cursorPosition = 0; }
}
void GUI_Textfield::OnRelease() {}
void GUI_Textfield::OnHover(const sf::Vector2f& l_mousePos) {
	if (m_state == GUI_ElementState::Clicked) { return; }
	SetState(GUI_ElementState::Focused);
}
void GUI_Textfield::OnLeave() {
	if (m_state == GUI_ElementState::Clicked) { return; }
	SetState(GUI_ElementState::Neutral);
}

void GUI_Textfield::OnDefocus() { SetState(GUI_ElementState::Neutral); }

void GUI_Textfield::Update(float l_dT) {
	if (m_state != GUI_ElementState::Clicked) { return; }
	m_time += l_dT;
	if (m_time >= 0.5f) { m_showCursor = !m_showCursor; m_time = 0.f; m_needsRedraw = true; }
	m_cursorSize = m_visual.m_text.getCharacterSize();
	auto pos = GetPosition();
	pos.x += GetXForCursor();
	pos.x += 1;
	pos.y += 1;
	m_cursor[0].position = pos;
	m_cursor[1].position = pos;
	m_cursor[1].position.y += m_cursorSize;
}

void GUI_Textfield::Draw(sf::RenderTarget* l_target) {
	l_target->draw(m_visual.m_backgroundSolid);
	if (!m_style[m_state].m_glyph.empty()) {
		l_target->draw(m_visual.m_glyph);
	}
	l_target->draw(m_visual.m_text);
	if (m_state != GUI_ElementState::Clicked) { return; }
	if (!m_showCursor) { return; }
	l_target->draw(m_cursor, 2, sf::Lines);
}

void GUI_Textfield::ChangeCursorPosition(int l_pos) {
	m_cursorPosition += l_pos;
	if (m_cursorPosition < 0) { m_cursorPosition = 0; }
	else if (m_cursorPosition > m_visual.m_text.getString().getSize()) {
		m_cursorPosition = (static_cast<int>(m_visual.m_text.getString().getSize()));
	}
}

float GUI_Textfield::GetXForCursor() {
	auto str = m_visual.m_text.getString().toAnsiString();
	auto font = m_visual.m_text.getFont();
	float result = 0.f;
	for (size_t i = 0; i < m_cursorPosition; ++i) {
		auto glyph = font->getGlyph(str[i], m_visual.m_text.getCharacterSize(), false);
		result += glyph.advance;
	}
	return result;
}