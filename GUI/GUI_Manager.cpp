#include "GUI_Manager.h"
#include "Elements/GUI_Label.h"
#include "Elements/GUI_Scrollbar.h"
#include "Elements/GUI_Textfield.h"
#include "Elements/GUI_CheckBox.h"
#include "Elements/GUI_Sprite.h"
#include "../SharedContext.h"
#include "../WindowSystem/Window.h"

GUI_Manager::GUI_Manager(EventManager* l_evMgr, SharedContext* l_shared)
	: m_eventMgr(l_evMgr), m_context(l_shared)
{
	RegisterElement<GUI_Label>(GUI_ElementType::Label);
	RegisterElement<GUI_Scrollbar>(GUI_ElementType::Scrollbar);
	RegisterElement<GUI_Textfield>(GUI_ElementType::Textfield);
	RegisterElement<GUI_CheckBox>(GUI_ElementType::CheckBox);
	RegisterElement<GUI_Sprite>(GUI_ElementType::Sprite);
	RegisterElement<GUI_DropDownMenu>(GUI_ElementType::DropDownMenu);
	m_elemTypes.emplace("Label", GUI_ElementType::Label);
	m_elemTypes.emplace("Button", GUI_ElementType::Button);
	m_elemTypes.emplace("Scrollbar", GUI_ElementType::Scrollbar);
	m_elemTypes.emplace("TextField", GUI_ElementType::Textfield);
	m_elemTypes.emplace("Interface", GUI_ElementType::Window);
	m_elemTypes.emplace("CheckBox", GUI_ElementType::CheckBox);
	m_elemTypes.emplace("Sprite", GUI_ElementType::Sprite);
	m_elemTypes.emplace("DropDownMenu", GUI_ElementType::DropDownMenu);

	m_eventMgr->AddCallback(StateType(0), "Mouse_Left", &GUI_Manager::HandleClick, this);
	m_eventMgr->AddCallback(StateType(0), "Mouse_Left_Release", &GUI_Manager::HandleRelease, this);
	m_eventMgr->AddCallback(StateType(0), "Mouse_Right", &GUI_Manager::HandleClick, this);
	m_eventMgr->AddCallback(StateType(0), "Mouse_Right_Release", &GUI_Manager::HandleRelease, this);
	m_eventMgr->AddCallback(StateType(0), "Text_Entered", &GUI_Manager::HandleTextEntered, this);
	m_eventMgr->AddCallback(StateType(0), "Key_LeftArrow", &GUI_Manager::HandleArrowKeys, this);
	m_eventMgr->AddCallback(StateType(0), "Key_RightArrow", &GUI_Manager::HandleArrowKeys, this);
}

GUI_Manager::~GUI_Manager() {
	m_eventMgr->RemoveCallback(StateType(0), "Mouse_Left");
	m_eventMgr->RemoveCallback(StateType(0), "Mouse_Left_Release");
	m_eventMgr->RemoveCallback(StateType(0), "Text_Entered");
	m_eventMgr->RemoveCallback(StateType(0), "Key_LeftArrow");
	m_eventMgr->RemoveCallback(StateType(0), "Key_RightArrow");
}
GUI_Interface* GUI_Manager::GetInterface(const StateType& l_state, const std::string& l_name)
{
	auto s = m_interfaces.find(l_state);
	if (s == m_interfaces.end()) { return nullptr; }
	auto i = std::find_if(s->second.begin(), s->second.end(),
		[&l_name](GUI_InterfaceType& l_pair) { return l_pair.first == l_name; }
	);
	return (i != s->second.end() ? i->second.get() : nullptr);
}

GUI_Interface* GUI_Manager::GetInterface(const std::string& l_name) { return GetInterface(m_currentState, l_name); }

bool GUI_Manager::RemoveInterface(const StateType& l_state, const std::string& l_name)
{
	auto s = m_interfaces.find(l_state);
	if (s == m_interfaces.end()) { return false; }
	auto i = std::remove_if(s->second.begin(), s->second.end(),
		[&l_name](GUI_InterfaceType& l_pair)
		{
			return l_pair.first == l_name;
		}
	);

	if (i == s->second.end()) { return false; }
	s->second.erase(i);
	return true;
}

bool GUI_Manager::RemoveInterface(const std::string& l_name) { return RemoveInterface(m_currentState, l_name); }

bool GUI_Manager::BringToFront(const GUI_Interface* l_interface) {
	return BringToFront(m_currentState, l_interface->GetName());
}

bool GUI_Manager::BringToFront(const StateType& l_state, const std::string& l_name) {
	auto state = m_interfaces.find(m_currentState);
	if (state == m_interfaces.end()) { return false; }
	auto i = std::find_if(state->second.begin(), state->second.end(),
		[&l_name](GUI_InterfaceType& l_pair) { return l_pair.first == l_name; }
	);
	if (i == state->second.end()) { return false; }
	m_toFront.emplace_back(l_state, i->second.get());
	return true;
}

void GUI_Manager::ChangeState(const StateType& l_state) {
	if (m_currentState == l_state) { return; }
	HandleRelease(nullptr);
	SetState(l_state);
}

void GUI_Manager::RemoveState(const StateType& l_state) {
	m_events.erase(l_state);
	m_interfaces.erase(l_state);
}

SharedContext* GUI_Manager::GetContext() const { return m_context; }

void GUI_Manager::DefocusAllInterfaces() {
	auto state = m_interfaces.find(m_currentState);
	if (state == m_interfaces.end()) { return; }
	for (auto &itr : state->second) { itr.second->Defocus(); }
}

void GUI_Manager::HandleClick(EventDetails* l_details) {
	auto state = m_interfaces.find(m_currentState);
	if (state == m_interfaces.end()) { return; }
	sf::Vector2i mousePos = m_eventMgr->GetMousePos(m_context->m_wind->GetRenderWindow());
	for (auto itr = state->second.rbegin(); itr != state->second.rend(); ++itr) {
		auto i = itr->second.get();
		if (!i->IsInside(sf::Vector2f(mousePos))) { continue; }
		if (!i->IsActive()) { continue; }
		if (l_details->m_keyCode == static_cast<int>(MouseButtonType::Left)) {
			DefocusAllInterfaces();
			i->Focus();
			i->OnClick(sf::Vector2f(mousePos));
			if (i->IsBeingMoved()) { i->BeginMoving(); }
		}
		l_details->m_hasBeenProcessed = true;
		return;
	}
	state->second.back().second->Defocus();
}

void GUI_Manager::HandleRelease(EventDetails* l_details) {
	if (l_details && l_details->m_keyCode != static_cast<int>(MouseButtonType::Left)) { return; }
	auto state = m_interfaces.find(m_currentState);
	if (state == m_interfaces.end()) { return; }
	for (auto &itr : state->second) {
		auto& i = itr.second;
		if (!i->IsActive()) { continue; }
		if (i->GetState() == GUI_ElementState::Clicked)
		{
			i->OnRelease();
		}
		if (i->IsBeingMoved()) { i->StopMoving(); }
	}
}

void GUI_Manager::HandleTextEntered(EventDetails* l_details) {
	auto state = m_interfaces.find(m_currentState);
	if (state == m_interfaces.end()) { return; }
	for (auto &itr : state->second) {
		if (!itr.second->IsActive()) { continue; }
		if (!itr.second->IsFocused()) { continue; }
		itr.second->OnTextEntered(l_details->m_textEntered);
		return;
	}
}

void GUI_Manager::HandleArrowKeys(EventDetails* l_details) {
	auto state = m_interfaces.find(m_currentState);
	if (state == m_interfaces.end()) { return; }
	for (auto &itr : state->second) {
		if (!itr.second->IsActive()) { continue; }
		if (!itr.second->IsFocused()) { continue; }
		itr.second->OnArrowKey(l_details->m_name);
		return;
	}
}

void GUI_Manager::AddEvent(GUI_Event l_event) {
	m_events[m_currentState].push_back(l_event);
}

bool GUI_Manager::PollEvent(GUI_Event& l_event) {
	if (m_events[m_currentState].empty()) { return false; }
	l_event = m_events[m_currentState].back();
	m_events[m_currentState].pop_back();
	return true;
}
void GUI_Manager::Update(float l_dT) {
	sf::Vector2i mousePos = m_eventMgr->GetMousePos(m_context->m_wind->GetRenderWindow());
	HandleMovesToFront();
	auto state = m_interfaces.find(m_currentState);
	if (state == m_interfaces.end()) { return; }
	bool HoverCaptured = false;
	for (auto itr = state->second.rbegin(); itr != state->second.rend(); ++itr) {
		auto& i = itr->second;
		if (!i->IsActive()) { continue; }
		i->Update(l_dT);
		if (i->IsBeingMoved()) { continue; }
		if (i->IsInside(sf::Vector2f(mousePos)) && !HoverCaptured) {
			if (i->GetState() == GUI_ElementState::Neutral) { i->OnHover(sf::Vector2f(mousePos)); }
			HoverCaptured = true;
			continue;
		} else if (i->GetState() == GUI_ElementState::Focused) {
			i->OnLeave();
		}
	}
}
void GUI_Manager::Render(sf::RenderWindow* l_wind) {
	auto state = m_interfaces.find(m_currentState);
	if (state == m_interfaces.end()) { return; }
	sf::View CurrentView = l_wind->getView();
	l_wind->setView(l_wind->getDefaultView());
	for (auto &itr : state->second) {
		auto& i = itr.second;
		if (!i->IsActive()) { continue; }
		if (i->NeedsRedraw()) { i->Redraw(); }
		if (i->NeedsContentRedraw()) { i->RedrawContent(); }
		if (i->NeedsControlRedraw()) { i->RedrawControls(); }
		i->Draw(l_wind);
	}
	l_wind->setView(CurrentView);
}
void GUI_Manager::HandleMovesToFront() {
	for (auto& itr : m_toFront) {
		auto state = m_interfaces.find(m_currentState);
		if (state == m_interfaces.end()) { continue; }
		auto& list = state->second;
		auto i = std::find_if(list.begin(), list.end(),
			[&itr](const GUI_InterfaceType& l_pair) { return itr.second == l_pair.second.get(); }
		);
		if (i == list.end()) { continue; }
		auto name = i->first;
		auto ptr = std::move(i->second);
		list.erase(i);
		list.emplace_back(name, std::move(ptr));
	}
}

GUI_Element* GUI_Manager::CreateElement(const GUI_ElementType& l_id, GUI_Interface* l_owner) {
	if (l_id == GUI_ElementType::Window) { return new GUI_Interface("", this); }
	auto f = m_factory.find(l_id);
	return (f != m_factory.end() ? f->second(l_owner) : nullptr);
}

bool GUI_Manager::AddInterface(const StateType& l_state,
	const std::string& l_name)
{
	auto s = m_interfaces.emplace(l_state, GUI_Interfaces()).first;
	auto i = std::find_if(s->second.begin(), s->second.end(),
		[&l_name](GUI_InterfaceType& l_pair) { return l_pair.first == l_name; }
	);
	if (i != s->second.end()) { std::cout << "Interface with the name " << l_name << " already exists!" << std::endl; return false; }
	std::unique_ptr<GUI_Interface> temp = std::make_unique<GUI_Interface>(l_name, this);
	s->second.emplace_back(l_name, std::move(temp));
	return true;
}

bool GUI_Manager::AddInterface(const std::string& l_name) { return AddInterface(m_currentState, l_name); }

bool GUI_Manager::LoadInterface(const StateType& l_state,
	const std::string& l_interface, const std::string& l_name)
{
	std::ifstream file;
	file.open(Utils::GetWorkingDirectory() + "media/GUI_Interfaces/" + l_interface);
	std::string InterfaceName;

	if (!file.is_open()) {
		std::cout << "! Failed to load: " << l_interface << std::endl;
		return false;
	}
	std::string line;
	while (std::getline(file, line)) {
		if (line[0] == '|') { continue; }
		std::stringstream keystream(line);
		std::string key;
		keystream >> key;
		if (key == "Interface") {
			std::string style;
			keystream >> InterfaceName >> style;
			if (!AddInterface(l_state, l_name)) {
				std::cout << "Failed adding interface: " << l_name << std::endl;
				return false;
			}
			auto i = GetInterface(l_state, l_name);
			i->SetFileName(InterfaceName);
			keystream >> *i;
			if (!LoadStyle(style, i)) {
				std::cout << "Failed loading style file: " << style << " for interface " << l_name << std::endl;
			}
			i->SetContentSize(i->GetSize());
			i->Setup();
		} else if (key == "Element") {
			if (InterfaceName.empty()) {
				std::cout << "Error: 'Element' outside or before declaration of 'Interface'!" << std::endl;
				continue;
			}
			std::string type;
			std::string name;
			sf::Vector2f position;
			std::string style;
			std::string layer;
			keystream >> type >> name >> position.x >> position.y >> style >> layer;
			GUI_ElementType eType = StringToType(type);
			if (eType == GUI_ElementType::None) {
				std::cout << "Unknown element('" << name << "') type: '" << type << "'" << std::endl;
				continue;
			}

			auto i = GetInterface(l_state, l_name);
			if (!i) { continue; }
			if (!i->AddElement(eType, name)) { continue; }
			auto e = i->GetElement(name);
			keystream >> *e;
			e->SetPosition(position);
			e->SetControl((layer == "Control"));
			if (!LoadStyle(style, e)) { std::cout << "Failed loading style file: " << style << " for element " << name << std::endl; }
			e->Setup();
		}
	}
	file.close();
	return true;
}

bool GUI_Manager::LoadInterface(const std::string& l_interface, const std::string& l_name) {
	return LoadInterface(m_currentState, l_interface, l_name);
}

bool GUI_Manager::LoadInterface(GUI_Interface* l_interface, const std::string& l_fileName) {
	std::ifstream file;
	file.open(Utils::GetWorkingDirectory() + "media/GUI_Interfaces/" + l_fileName);
	std::string InterfaceName;

	if (!file.is_open()) {
		std::cout << "! Failed to load: " << l_interface << std::endl;
		return false;
	}
	std::string line;
	while (std::getline(file, line)) {
		if (line[0] == '|') { continue; }
		std::stringstream keystream(line);
		std::string key;
		keystream >> key;
		if (key == "Interface") {
			std::string style;
			keystream >> InterfaceName >> style;
			l_interface->SetFileName(InterfaceName);
			keystream >> *l_interface;
			if (!LoadStyle(style, l_interface)) {
				std::cout << "Failed loading style file: " << style << " for interface " << l_fileName << std::endl;
			}
			l_interface->SetContentSize(l_interface->GetSize());
		} else if (key == "Element") {
			if (InterfaceName.empty()) {
				std::cout << "Error: 'Element' outside or before declaration of 'Interface'!" << std::endl;
				continue;
			}
			std::string type;
			std::string name;
			sf::Vector2f position;
			std::string style;
			std::string layer;
			keystream >> type >> name >> position.x >> position.y >> style >> layer;
			GUI_ElementType eType = StringToType(type);
			if (eType == GUI_ElementType::None) {
				std::cout << "Unknown element('" << name << "') type: '" << type << "'" << std::endl;
				continue;
			}

			if (!l_interface->AddElement(eType, name)) { continue; }
			auto e = l_interface->GetElement(name);
			keystream >> *e;
			e->SetPosition(position);
			e->SetControl((layer == "Control"));
			if (!LoadStyle(style, e)) {
				std::cout << "Failed loading style file: " << style << " for element " << name << std::endl;
				continue;
			}
		}
	}
	file.close();
	return true;
}

bool GUI_Manager::LoadStyle(const std::string& l_file, GUI_Element* l_element) {
	std::ifstream file;
	file.open(Utils::GetWorkingDirectory() + "media/GUI_Styles/" + l_file);

	std::string currentState;
	GUI_Style ParentStyle;
	GUI_Style TemporaryStyle;
	if (!file.is_open()) {
		std::cout << "! Failed to load: " << l_file << std::endl;
		return false;
	}
	ParentStyle.m_styleName = l_file;
	TemporaryStyle.m_styleName = l_file;
	std::string line;
	while (std::getline(file, line)) {
		if (line[0] == '|') { continue; }
		std::stringstream keystream(line);
		std::string type;
		keystream >> type;
		if (type.empty()) { continue; }
		if (type == "State") {
			if (!currentState.empty()) {
				std::cout << "Error: 'State' keyword found inside another state!" << std::endl;
				continue;
			}
			keystream >> currentState;
		} else if (type == "/State") {
			if (currentState.empty()) {
				std::cout << "Error: '/State' keyword found prior to 'State'!" << std::endl;
				continue;
			}
			GUI_ElementState state = GUI_ElementState::Neutral;
			if (currentState == "Hover") { state = GUI_ElementState::Focused; }
			else if (currentState == "Clicked") { state = GUI_ElementState::Clicked; }

			if (state == GUI_ElementState::Neutral) {
				ParentStyle = TemporaryStyle;
				l_element->UpdateStyle(GUI_ElementState::Neutral, TemporaryStyle);
				l_element->UpdateStyle(GUI_ElementState::Focused, TemporaryStyle);
				l_element->UpdateStyle(GUI_ElementState::Clicked, TemporaryStyle);
			} else {
				l_element->UpdateStyle(state, TemporaryStyle);
			}
			TemporaryStyle = ParentStyle;
			currentState.clear();
		} else {
			// Handling style information.
			if (currentState.empty()) {
				std::cout << "Error: '" << type << "' keyword found outside of a state!" << std::endl;
				continue;
			}
			if (type == "Size") {
				keystream >> TemporaryStyle.m_size.x >> TemporaryStyle.m_size.y;
			} else if (type == "BgColor") {
				int r, g, b, a = 0;
				keystream >> r >> g >> b >> a;
				TemporaryStyle.m_backgroundColor = sf::Color(r,g,b,a);
			} else if (type == "BgImage") {
				keystream >> TemporaryStyle.m_backgroundImage;
			} else if (type == "BgImageColor") {
				int r, g, b, a = 0;
				keystream >> r >> g >> b >> a;
				TemporaryStyle.m_backgroundImageColor = sf::Color(r, g, b, a);
			} else if (type == "TextColor") {
				int r, g, b, a = 0;
				keystream >> r >> g >> b >> a;
				TemporaryStyle.m_textColor = sf::Color(r, g, b, a);
			} else if (type == "TextSize") {
				keystream >> TemporaryStyle.m_textSize;
			} else if (type == "TextOriginCenter") {
				TemporaryStyle.m_textCenterOrigin = true;
			} else if (type == "StretchToFit") {
				TemporaryStyle.m_stretchToFit = true;
			} else if (type == "Font") {
				keystream >> TemporaryStyle.m_textFont;
			} else if (type == "TextPadding") {
				keystream >> TemporaryStyle.m_textPadding.x >> TemporaryStyle.m_textPadding.y;
			} else if (type == "ElementColor") {
				int r, g, b, a = 0;
				keystream >> r >> g >> b >> a;
				TemporaryStyle.m_elementColor = sf::Color(r, g, b, a);
			} else if (type == "Glyph") {
				keystream >> TemporaryStyle.m_glyph;
			} else if (type == "GlyphPadding") {
				keystream >> TemporaryStyle.m_glyphPadding.x >> TemporaryStyle.m_glyphPadding.y;
			} else {
				std::cout << "Error: style tag '" << type << "' is unknown!" << std::endl;
			}
		}
	}
	file.close();
	return true;
}

GUI_ElementType GUI_Manager::StringToType(const std::string& l_string) const {
	auto t = m_elemTypes.find(l_string);
	return (t != m_elemTypes.end() ? t->second : GUI_ElementType::None);
}