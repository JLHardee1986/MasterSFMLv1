#include "VerticalDropDown.h"
#include "../GUI_Manager.h"

GUI_VerticalDropDown::GUI_VerticalDropDown(EventManager* l_eventManager, GUI_Manager* l_guiManager, GUI_Element* l_element)
	: m_eventManager(l_eventManager), m_guiManager(l_guiManager), m_interface(nullptr), m_element(l_element)
{}

GUI_VerticalDropDown::~GUI_VerticalDropDown() {
	m_eventManager->RemoveCallback(m_state, m_callbackNameBase + "Click");
	m_eventManager->RemoveCallback(m_state, m_callbackNameBase + "Leave");
	if (m_interface) { m_guiManager->RemoveInterface(m_state, m_name); }
}

void GUI_VerticalDropDown::Setup(const StateType& l_state, const std::string& l_name, const std::string& l_fileName) {
	if (m_interface) { return; }
	m_state = l_state; m_name = l_name;
	if (!m_guiManager->LoadInterface(m_state, l_fileName, m_name)) { return; }
	m_interface = m_guiManager->GetInterface(m_state, m_name);
	m_interface->SetActive(false);
	m_entryStyle = m_interface->GetElement("Entry_")->GetStyleName();

	m_callbackNameBase = "_callback" + m_name;

	std::unique_ptr<Binding> bind;
	GUI_Event event;
	event.m_interface = m_name;
	event.m_element = "*";
	bind = std::make_unique<Binding>(m_callbackNameBase + "Click");
	event.m_type = GUI_EventType::Click;
	EventInfo info(event);
	bind->BindEvent(EventType::GUI_Click, info);
	m_eventManager->AddBinding(std::move(bind));
	m_eventManager->AddCallback(m_state, m_callbackNameBase + "Click", &GUI_VerticalDropDown::OnClick, this);
}

void GUI_VerticalDropDown::SetPosition(const sf::Vector2f& l_position) {
	if (!m_interface) { return; }
	m_interface->SetPosition(l_position);
}

void GUI_VerticalDropDown::Show() {
	if (!m_interface) { return; }
	m_interface->SetActive(true);
	m_interface->Focus();
}

void GUI_VerticalDropDown::BringToFront() {
	if (!m_interface) { return; }
	m_guiManager->BringToFront(m_interface);
}

void GUI_VerticalDropDown::Hide() {
	if (!m_interface) { return; }
	m_interface->SetActive(false);
	if (!m_element) { return; }
	m_element->SetState(GUI_ElementState::Neutral);
}

std::string GUI_VerticalDropDown::GetSelected() const { return m_selection; }

void GUI_VerticalDropDown::ResetSelected() {
	m_selection = "";
	if (!m_element) { return; }
	m_element->SetText("Select...");
}

void GUI_VerticalDropDown::AddEntry(const std::string& l_entry) {
	m_entries.emplace_back(l_entry);
}

void GUI_VerticalDropDown::RemoveEntry(const std::string& l_entry) {
	m_entries.erase(std::find_if(m_entries.begin(), m_entries.end(),
		[&l_entry](const std::string& l_str) { return l_entry == l_str; }
	));
}

void GUI_VerticalDropDown::PurgeEntries() { m_entries.clear(); }

void GUI_VerticalDropDown::OnClick(EventDetails* l_details) {
	if (l_details->m_guiElement == "") { return; }
	m_selection = m_interface->GetElement(l_details->m_guiElement)->GetText();
	Hide();
	if (!m_element) { return; }
	m_element->SetText(m_selection);
	m_element->SetRedraw(true);
}

void GUI_VerticalDropDown::Redraw() {
	if (!m_interface) { return; }
	m_interface->RemoveElementsContaining("Entry_");
	size_t id = 0;
	auto position = sf::Vector2f(0, 0);
	for (auto& entry : m_entries) {
		m_interface->AddElement(GUI_ElementType::Label, "Entry_" + std::to_string(id));
		auto element = m_interface->GetElement("Entry_" + std::to_string(id));
		element->SetText(entry);
		element->SetPosition(position);
		m_guiManager->LoadStyle(m_entryStyle, element);
		position.y += element->GetSize().y + 1.f;
		++id;
	}
}