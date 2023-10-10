#include "GUI_Interface.h"
#include "GUI_Manager.h"
#include "Elements/GUI_TextField.h"
#include "../SharedContext.h"
#include "../WindowSystem/Window.h"

GUI_Interface::GUI_Interface(const std::string& l_name, GUI_Manager* l_guiManager)
	: GUI_Element(l_name, GUI_ElementType::Window, this), m_parent(nullptr), m_guiManager(l_guiManager),
	m_movable(false), m_beingMoved(false), m_showTitleBar(false), m_focused(false), m_scrollHorizontal(0),
	m_scrollVertical(0), m_contentRedraw(true), m_controlRedraw(true)
{
	m_backdropTexture = std::make_unique<sf::RenderTexture>();
	m_contentTexture = std::make_unique<sf::RenderTexture>();
	m_controlTexture = std::make_unique<sf::RenderTexture>();
}

GUI_Interface::~GUI_Interface() {}

void GUI_Interface::SetPosition(const sf::Vector2f& l_pos) {
	GUI_Element::SetPosition(l_pos);
	m_backdrop.setPosition(l_pos);
	m_content.setPosition(l_pos + m_contentPositionOffset);
	m_control.setPosition(l_pos);
	m_titleBar.setPosition(m_position.x, m_position.y - m_titleBar.getSize().y);
	m_visual.m_text.setPosition(m_titleBar.getPosition() + m_style[m_state].m_textPadding);
}

void GUI_Interface::PositionCenterScreen() {
	auto size = m_style[m_state].m_size;
	auto w_size = m_guiManager->GetContext()->m_wind->GetWindowSize();
	SetPosition({(w_size.x / 2) - size.x / 2, (w_size.y / 2) - size.y / 2 });
}

bool GUI_Interface::AddElement(const GUI_ElementType& l_type, 
	const std::string& l_name)
{
	if (m_elements.find(l_name) != m_elements.end()) { return false; }
	std::unique_ptr<GUI_Element> element (m_guiManager->CreateElement(l_type, this));
	if (!element) { return false; }
	element->SetName(l_name);
	element->SetOwner(this);
	m_elements.emplace(l_name, std::move(element));
	m_contentRedraw = true;
	m_controlRedraw = true;
	return true;
}

GUI_Element* GUI_Interface::GetElement(const std::string& l_name) const{
	auto itr = m_elements.find(l_name);
	return(itr != m_elements.end() ? itr->second.get() : nullptr);
}

bool GUI_Interface::RemoveElement(const std::string& l_name) {
	auto itr = m_elements.find(l_name);
	if (itr == m_elements.end()) { return false; }
	m_elements.erase(itr);
	m_contentRedraw = true;
	m_controlRedraw = true;
	AdjustContentSize();
	return true;
}

bool GUI_Interface::RemoveElementsContaining(const std::string& l_snippet) {
	bool changed = false;
	for (auto itr = m_elements.begin(); itr != m_elements.end();) {
		if (itr->first.find(l_snippet) == std::string::npos) { ++itr; continue; }
		itr = m_elements.erase(itr);
		changed = true;
	}
	if (!changed) { return false; }
	m_contentRedraw = true;
	m_contentRedraw = true;
	AdjustContentSize();
	return true;
}

bool GUI_Interface::HasParent() const{ return m_parent != nullptr; }
GUI_Manager* GUI_Interface::GetManager() const{ return m_guiManager; }

bool GUI_Interface::IsInside(const sf::Vector2f& l_point) const{
	if (GUI_Element::IsInside(l_point)) { return true; }
	return (m_titleBar.getGlobalBounds().contains(l_point) && m_showTitleBar);
}

void GUI_Interface::Focus() { OnFocus(); }
void GUI_Interface::Defocus() { OnDefocus(); }
bool GUI_Interface::IsFocused() const { return m_focused; }

void GUI_Interface::ReadIn(std::stringstream& l_stream) {
	std::string movableState;
	std::string titleShow;
	std::string title;
	l_stream >> m_elementPadding.x >> m_elementPadding.y 
		>> movableState >> titleShow;
	Utils::ReadQuotedString(l_stream, title);
	m_visual.m_text.setString(title);
	if (movableState == "Movable") { m_movable = true; }
	if (titleShow == "Title") { m_showTitleBar = true; }
}

bool GUI_Interface::ContentLayerContainsPoint(const sf::Vector2f& l_point) {
	auto pos = GetGlobalPosition();
	auto size = (m_contentRectSize == sf::Vector2i(0, 0) ?
		sf::Vector2i(m_style.at(m_state).m_size)
		: m_contentRectSize);
	pos += m_contentPositionOffset;
	sf::FloatRect rect;
	rect.left = pos.x;
	rect.top = pos.y;
	rect.width = (static_cast<float>(size.x));
	rect.height = (static_cast<float>(size.y));
	return rect.contains(l_point);
}

void GUI_Interface::OnClick(const sf::Vector2f& l_mousePos) {
	DefocusTextfields();
	if (m_titleBar.getGlobalBounds().contains(l_mousePos) && m_showTitleBar)
	{
		if (m_movable) { m_beingMoved = true; }
		for (auto& itr : m_elements) {
			if (!itr.second->IsActive()) { continue; }
			itr.second->OnInterfaceClick(l_mousePos - GetPosition());
		}
		return;
	}
	GUI_Event event;
	event.m_type = GUI_EventType::Click;
	event.m_interface = m_name;
	event.m_element.clear();
	auto interfacePos = GetPosition();
	event.m_clickCoords.x = l_mousePos.x - interfacePos.x;
	event.m_clickCoords.y = l_mousePos.y - interfacePos.y;
	m_guiManager->AddEvent(event);

	bool contentHasMouse = ContentLayerContainsPoint(l_mousePos);
	for (auto &itr : m_elements) {
		if (!itr.second->IsActive()) { continue; }
		auto position = itr.second->GetPosition();
		auto scrollOffset = sf::Vector2f(0.f, 0.f);
		if (!itr.second->IsInside(l_mousePos)) {
			itr.second->OnInterfaceClick(l_mousePos - interfacePos - position + scrollOffset);
			continue;
		}
		if (!contentHasMouse && !itr.second->IsControl()) { continue; }

		event.m_element = itr.second->m_name;
		if (!itr.second->IsControl()) {
			position += m_contentPositionOffset;
			scrollOffset = sf::Vector2f((float)(static_cast<int>(m_scrollHorizontal)), (float)(static_cast<float>(m_scrollVertical)));
		}
		event.m_clickCoords.x = l_mousePos.x - interfacePos.x - position.x + scrollOffset.x;
		event.m_clickCoords.y = l_mousePos.y - interfacePos.y - position.y + scrollOffset.y;

		itr.second->OnClick({ event.m_clickCoords.x, event.m_clickCoords.y });
		m_guiManager->AddEvent(event);
	}
	SetState(GUI_ElementState::Clicked);
}

void GUI_Interface::OnRelease() {
	GUI_Event event;
	event.m_type = GUI_EventType::Release;
	event.m_interface = m_name;
	event.m_element.clear();
	auto interfacePos = GetPosition();
	auto mousePos = m_guiManager->GetContext()->m_eventManager->GetMousePos(m_guiManager->GetContext()->m_wind->GetRenderWindow());
	event.m_clickCoords.x = mousePos.x - interfacePos.x;
	event.m_clickCoords.y = mousePos.y - interfacePos.y;
	m_guiManager->AddEvent(event);
	for (auto &itr : m_elements) {
		if (itr.second->GetState() != GUI_ElementState::Clicked)
		{
			continue;
		}
		event.m_element = itr.second->m_name;

		auto position = itr.second->GetPosition();
		auto scrollOffset = sf::Vector2f(0.f, 0.f);
		if (!itr.second->IsControl()) {
			position += m_contentPositionOffset;
			scrollOffset = sf::Vector2f((float)(static_cast<int>(m_scrollHorizontal)), (float)(static_cast<float>(m_scrollVertical)));
		}
		event.m_clickCoords.x = mousePos.x - interfacePos.x - position.x + scrollOffset.x;
		event.m_clickCoords.y = mousePos.y - interfacePos.y - position.y + scrollOffset.y;

		itr.second->OnRelease();
		m_guiManager->AddEvent(event);
	}
	SetState(GUI_ElementState::Focused);
}

void GUI_Interface::OnHover(const sf::Vector2f& l_mousePos) {
	GUI_Event event;
	event.m_type = GUI_EventType::Hover;
	event.m_interface = m_name;
	event.m_element.clear();
	event.m_clickCoords.x = l_mousePos.x;
	event.m_clickCoords.y = l_mousePos.y;
	m_guiManager->AddEvent(event);

	SetState(GUI_ElementState::Focused);
}

void GUI_Interface::OnLeave() {
	GUI_Event event;
	event.m_type = GUI_EventType::Leave;
	event.m_interface = m_name;
	event.m_element.clear();
	m_guiManager->AddEvent(event);

	SetState(GUI_ElementState::Neutral);
}

void GUI_Interface::OnTextEntered(const char& l_char) {
	for (auto &itr : m_elements) {
		if (!itr.second->IsActive()) { continue; }
		if (itr.second->GetType() != GUI_ElementType::Textfield) { continue; }
		if (itr.second->GetState() != GUI_ElementState::Clicked) { continue; }
		auto field = static_cast<GUI_Textfield*>(itr.second.get());
		field->HandleChar(l_char);
		return;
	}
}

void GUI_Interface::OnFocus() { 
	m_guiManager->BringToFront(this);
	if (m_focused) { return; }
	m_focused = true;
	GUI_Event event;
	event.m_type = GUI_EventType::Focus;
	event.m_interface = m_name;
	event.m_element.clear();
	m_guiManager->AddEvent(event);
	for (auto& itr : m_elements) {
		if (!itr.second->IsActive()) { continue; }
		itr.second->OnFocus();
		event.m_element = itr.second->m_name;
		m_guiManager->AddEvent(event);
	}
}

void GUI_Interface::OnDefocus() {
	if (!m_focused) { return; }
	m_focused = false;
	GUI_Event event;
	event.m_type = GUI_EventType::Defocus;
	event.m_interface = m_name;
	event.m_element.clear();

	for (auto& itr : m_elements) {
		if (!itr.second->IsActive()) { continue; }
		itr.second->OnDefocus();
		event.m_element = itr.second->m_name;
		m_guiManager->AddEvent(event);
	}
}

void GUI_Interface::OnArrowKey(const std::string& l_eventName) {
	for (auto &itr : m_elements) {
		if (!itr.second->IsActive()) { continue; }
		if (itr.second->GetType() != GUI_ElementType::Textfield) { continue; }
		auto field = static_cast<GUI_Textfield*>(itr.second.get());
		field->HandleArrowKey(l_eventName);
		return;
	}
}

sf::Vector2f GUI_Interface::GetPadding() const{ return m_elementPadding; }
void GUI_Interface::SetPadding(const sf::Vector2f& l_padding) { m_elementPadding = l_padding;  }

void GUI_Interface::Update(float l_dT) {
	auto mousePos = sf::Vector2f(
		m_guiManager->GetContext()->m_eventManager->GetMousePos(
		m_guiManager->GetContext()->m_wind->GetRenderWindow()));

	if (m_beingMoved && m_moveMouseLast != mousePos) {
		sf::Vector2f difference = mousePos - m_moveMouseLast;
		m_moveMouseLast = mousePos;
		sf::Vector2f newPosition = m_position + difference;
		SetPosition(newPosition);
	}

	bool contentHasMouse = ContentLayerContainsPoint(mousePos);
	for (auto &itr : m_elements) {
		if (itr.second->NeedsRedraw()) {
			if (itr.second->IsControl()) { m_controlRedraw = true; } 
			else { m_contentRedraw = true; }
		}
		if (!itr.second->IsActive()) { continue; }
		itr.second->Update(l_dT);
		if (m_beingMoved) { continue; }
		GUI_Event event;
		event.m_interface = m_name;
		event.m_element = itr.second->m_name;
		event.m_clickCoords.x = mousePos.x;
		event.m_clickCoords.y = mousePos.y;
		if (IsInside(mousePos) && itr.second->IsInside(mousePos) 
			&& !m_titleBar.getGlobalBounds().contains(mousePos)
			&& !(!contentHasMouse && !itr.second->IsControl())
			&& m_state != GUI_ElementState::Neutral)
		{
			if (itr.second->GetState() != GUI_ElementState::Neutral) { continue; }
			itr.second->OnHover(mousePos);
			event.m_type = GUI_EventType::Hover;
			m_guiManager->AddEvent(event);
		} else if (itr.second->GetState() == GUI_ElementState::Focused) {
			itr.second->OnLeave();
			event.m_type = GUI_EventType::Leave;
			m_guiManager->AddEvent(event);
		}
	}
}

void GUI_Interface::Draw(sf::RenderTarget* l_target) {
	l_target->draw(m_backdrop);
	l_target->draw(m_content);
	l_target->draw(m_control);

	if (!m_showTitleBar) { return; }
	l_target->draw(m_titleBar);
	l_target->draw(m_visual.m_text);
}

bool GUI_Interface::IsBeingMoved() const{ return m_beingMoved; }
bool GUI_Interface::IsMovable() const{ return m_movable;  }

void GUI_Interface::BeginMoving() {
	if (!m_showTitleBar || !m_movable) { return; }
	m_beingMoved = true;
	SharedContext* context = m_guiManager->GetContext();
	m_moveMouseLast = sf::Vector2f(context->m_eventManager->
		GetMousePos(context->m_wind->GetRenderWindow()));
}

void GUI_Interface::StopMoving() { m_beingMoved = false; }

sf::Vector2f GUI_Interface::GetGlobalPosition() const{
	sf::Vector2f pos = m_position;
	GUI_Interface* i = m_parent;
	while (i) {
		pos += i->GetPosition();
		i = i->m_parent;
	}
	return pos;
}

void GUI_Interface::ApplyStyle() {
	GUI_Element::ApplyStyle(); // Call base method.
	m_visual.m_backgroundSolid.setPosition(0.f,0.f);
	m_visual.m_backgroundImage.setPosition(0.f,0.f);
	m_titleBar.setSize(sf::Vector2f(m_style[m_state].m_size.x, 16.f));
	m_titleBar.setPosition(m_position.x,m_position.y - m_titleBar.getSize().y);
	m_titleBar.setFillColor(m_style[m_state].m_elementColor);
	m_visual.m_text.setPosition(m_titleBar.getPosition() + m_style[m_state].m_textPadding);
	m_visual.m_glyph.setPosition(m_titleBar.getPosition() + m_style[m_state].m_glyphPadding);
}

void GUI_Interface::Redraw() {
	if (m_backdropTexture->getSize().x != m_style[m_state].m_size.x ||
		m_backdropTexture->getSize().y != m_style[m_state].m_size.y)
	{
		m_backdropTexture->create(
			static_cast<unsigned int>(m_style[m_state].m_size.x),
			static_cast<unsigned int>(m_style[m_state].m_size.y));
	}
	m_backdropTexture->clear(sf::Color(0, 0, 0, 0));
	ApplyStyle();
	m_backdropTexture->draw(m_visual.m_backgroundSolid);

	if (!m_style[m_state].m_backgroundImage.empty()) {
		m_backdropTexture->draw(m_visual.m_backgroundImage);
	}

	m_backdropTexture->display();
	m_backdrop.setTexture(m_backdropTexture->getTexture());
	m_backdrop.setTextureRect(sf::IntRect(0, 0, 
		static_cast<int>(m_style[m_state].m_size.x),
		static_cast<int>(m_style[m_state].m_size.y)));
	SetRedraw(false);
}
bool GUI_Interface::NeedsContentRedraw() const{ return m_contentRedraw; }
void GUI_Interface::RedrawContent() {
	if (m_contentTexture->getSize().x != m_contentSize.x ||
		m_contentTexture->getSize().y != m_contentSize.y)
	{
		m_contentTexture->create(
			static_cast<unsigned int>(m_contentSize.x),
			static_cast<unsigned int>(m_contentSize.y));
	}

	m_contentTexture->clear(sf::Color(0, 0, 0, 0));

	for (auto &itr : m_elements) {
		auto& element = itr.second;
		if (!element->IsActive() || element->IsControl()) { continue; }
		element->ApplyStyle();
		element->Draw(m_contentTexture.get());
		element->SetRedraw(false);
	}

	m_contentTexture->display();
	m_content.setTexture(m_contentTexture->getTexture());

	sf::Vector2i size = (m_contentRectSize == sf::Vector2i(0, 0) ?
		sf::Vector2i((static_cast<int>(m_style[m_state].m_size.x)), (static_cast<int>(m_style[m_state].m_size.y))) :
		m_contentRectSize);

	m_content.setTextureRect(sf::IntRect(
		m_scrollHorizontal, m_scrollVertical,
		size.x, size.y
	));
	m_contentRedraw = false;
}
bool GUI_Interface::NeedsControlRedraw() const{ return m_controlRedraw; }
void GUI_Interface::RedrawControls() {
	if (m_controlTexture->getSize().x != m_style[m_state].m_size.x ||
		m_controlTexture->getSize().y != m_style[m_state].m_size.y)
	{
		m_controlTexture->create(
			static_cast<unsigned int>(m_style[m_state].m_size.x),
			static_cast<unsigned int>(m_style[m_state].m_size.y));
	}
	m_controlTexture->clear(sf::Color(0, 0, 0, 0));

	for (auto &itr : m_elements) {
		auto& element = itr.second;
		if (!element->IsActive() || !element->IsControl()) { continue; }
		element->ApplyStyle();
		element->Draw(m_controlTexture.get());
		element->SetRedraw(false);
	}

	m_controlTexture->display();
	m_control.setTexture(m_controlTexture->getTexture());
	m_control.setTextureRect(sf::IntRect(0, 0,
		static_cast<int>(m_style[m_state].m_size.x),
		static_cast<int>(m_style[m_state].m_size.y)));
	m_controlRedraw = false;
}

void GUI_Interface::RequestContentRedraw() { m_contentRedraw = true; }

void GUI_Interface::ToggleTitleBar() { m_showTitleBar = !m_showTitleBar; }

void GUI_Interface::AdjustContentSize(const GUI_Element* l_reference) {
	if (l_reference) {
		sf::Vector2f bottomRight = l_reference->GetPosition() + l_reference->GetSize();
		if (bottomRight.x > m_contentSize.x) { m_contentSize.x = bottomRight.x; m_controlRedraw = true; }
		if (bottomRight.y > m_contentSize.y) { m_contentSize.y = bottomRight.y; m_controlRedraw = true; }
		return;
	}

	sf::Vector2f farthest = GetSize();

	for (auto &itr : m_elements) {
		auto& element = itr.second;
		if (!element->IsActive() || element->IsControl()) { continue; }
		sf::Vector2f bottomRight = element->GetPosition() + element->GetSize();
		if (bottomRight.x > farthest.x) { farthest.x = bottomRight.x; m_controlRedraw = true; }
		if (bottomRight.y > farthest.y) { farthest.y = bottomRight.y; m_controlRedraw = true; }
	}
	SetContentSize(farthest);
}

void GUI_Interface::SetContentSize(const sf::Vector2f& l_vec) { m_contentSize = l_vec; }

void GUI_Interface::UpdateScrollHorizontal(unsigned int l_percent) {
	if (l_percent > 100) { return; }
	float size = (m_contentRectSize.x == 0 ? GetSize().x : m_contentRectSize.x);
	m_scrollHorizontal = static_cast<int>(((m_contentSize.x - size) / 100) * l_percent);
	sf::IntRect rect = m_content.getTextureRect();
	m_content.setTextureRect(sf::IntRect(m_scrollHorizontal, m_scrollVertical, rect.width, rect.height));
}

void GUI_Interface::UpdateScrollVertical(unsigned int l_percent) {
	if (l_percent > 100) { return; }
	float size = (m_contentRectSize.y == 0 ? GetSize().y : m_contentRectSize.y);
	m_scrollVertical = static_cast<int>(((m_contentSize.y - size) / 100) * l_percent);
	sf::IntRect rect = m_content.getTextureRect();
	m_content.setTextureRect(sf::IntRect(m_scrollHorizontal, m_scrollVertical, rect.width, rect.height));
}

sf::Vector2f GUI_Interface::GetContentSize() const{ return m_contentSize; }

sf::Vector2i GUI_Interface::GetContentRectSize() const { return m_contentRectSize; }
sf::Vector2f GUI_Interface::GetContentOffset() const { return m_contentPositionOffset; }
void GUI_Interface::SetContentRectSize(const sf::Vector2i& l_size) { m_contentRectSize = l_size; }
void GUI_Interface::SetContentOffset(const sf::Vector2f& l_offset) { m_contentPositionOffset = l_offset; m_content.setPosition(GetPosition() + l_offset); }

void GUI_Interface::DefocusTextfields() {
	GUI_Event event;
	event.m_type = GUI_EventType::Defocus;
	event.m_interface = m_name;
	event.m_element.clear();
	for (auto &itr : m_elements) {
		if (itr.second->GetType() != GUI_ElementType::Textfield) { continue; }
		itr.second->OnDefocus();
		if (!itr.second->IsActive() || !IsActive()) { continue; }
		event.m_element = itr.second->m_name;
		m_guiManager->AddEvent(event);
	}
}