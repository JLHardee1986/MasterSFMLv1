#include "State_Loading.h"
#include "../StateManager.h"
#include "../../WindowSystem/Window.h"
#include "../../Resources/FontManager.h"
#include "../../Utilities/Helpers.h"

State_Loading::State_Loading(StateManager* l_stateManager)
	: BaseState(l_stateManager), m_originalWork(0), m_percentage(0), m_manualContinue(true) {}

State_Loading::~State_Loading() {}

void State_Loading::OnCreate() {
	auto context = m_stateMgr->GetContext();
	context->m_fontManager->RequireResource("Main");
	m_text.setFont(*context->m_fontManager->GetResource("Main"));
	m_text.setCharacterSize(14);
	m_text.setStyle(sf::Text::Bold);

	sf::Vector2u windowSize = m_stateMgr->GetContext()->m_wind->GetRenderWindow()->getSize();

	m_rect.setFillColor(sf::Color(0, 150, 0, 255));
	m_rect.setSize(sf::Vector2f(0.f, 16.f));
	m_rect.setOrigin(0.f, 8.f);
	m_rect.setPosition(0.f, windowSize.y / 2.f);

	EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
	evMgr->AddCallback(StateType::Loading, "Key_Space", &State_Loading::Proceed, this);
}

void State_Loading::OnDestroy() {
	auto context = m_stateMgr->GetContext();
	EventManager* evMgr = context->m_eventManager;
	evMgr->RemoveCallback(StateType::Loading, "Key_Space");
	context->m_fontManager->ReleaseResource("Main");
}

void State_Loading::Update(const sf::Time& l_time) {
	if (m_loaders.empty()) { if (!m_manualContinue) { Proceed(nullptr); } return; }
	auto windowSize = m_stateMgr->GetContext()->m_wind->GetRenderWindow()->getSize();
	if (m_loaders.back()->IsDone()) {
		m_loaders.back()->OnRemove();
		m_loaders.pop_back();
		if (m_loaders.empty()) {
			m_rect.setSize(sf::Vector2f(static_cast<float>(windowSize.x), 16.f));
			UpdateText(".Press space to continue.", 100.f);
			return;
		}
	}
	if (!m_loaders.back()->HasStarted()) { m_loaders.back()->Begin(); }

	auto percentage = CalculatePercentage();
	UpdateText("", percentage);
	m_rect.setSize(sf::Vector2f((windowSize.x / 100) * percentage, 16.f));
}

void State_Loading::Draw() {
	sf::RenderWindow* wind = m_stateMgr->GetContext()->m_wind->GetRenderWindow();
	wind->draw(m_rect);
	wind->draw(m_text);
}

void State_Loading::UpdateText(const std::string& l_text, float l_percentage) {
	m_text.setString(std::to_string(static_cast<int>(l_percentage)) + "%" + l_text);
	auto windowSize = m_stateMgr->GetContext()->m_wind->GetRenderWindow()->getSize();
	m_text.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
	Utils::CenterSFMLText(m_text);
}

float State_Loading::CalculatePercentage() {
	float absolute = 100.f;
	if (m_loaders.empty()) { return absolute; }
	if (m_loaders.back()->GetTotalLines()) {
		float d = (100.f * (m_originalWork - m_loaders.size())) / static_cast<float>(m_originalWork);
		float current = (100.f * m_loaders.back()->GetCurrentLine()) / static_cast<float>(m_loaders.back()->GetTotalLines());
		float totalCurrent = current / static_cast<float>(m_originalWork);
		absolute = d + totalCurrent;
	}
	return absolute;
}

void State_Loading::SetManualContinue(bool l_continue) { m_manualContinue = l_continue; }

void State_Loading::Proceed(EventDetails* l_details) {
	if (!m_loaders.empty()) { return; }
	m_stateMgr->SwitchTo(m_stateMgr->GetNextToLast());
}

void State_Loading::AddLoader(FileLoader* l_loader) { m_loaders.emplace_back(l_loader); l_loader->OnAdd(); }
bool State_Loading::HasWork() const { return !m_loaders.empty(); }
void State_Loading::Activate() { m_originalWork = m_loaders.size(); }
void State_Loading::Deactivate() {}