#pragma once
#include "../../StateSystem/BaseState.h"
#include "../../EventSystem/EventManager.h"
#include "../../ThreadWorkers/FileLoader.h"

using LoaderContainer = std::vector<FileLoader*>;

class State_Loading : public BaseState {
public:
	State_Loading(StateManager* l_stateManager);
	~State_Loading();

	void OnCreate();
	void OnDestroy();

	void Activate();
	void Deactivate();

	void AddLoader(FileLoader* l_loader);
	bool HasWork() const;

	void Update(const sf::Time& l_time);
	void Draw();

	void SetManualContinue(bool l_continue);
	void Proceed(EventDetails* l_details);
private:
	void UpdateText(const std::string& l_text, float l_percentage);
	float CalculatePercentage();
	LoaderContainer m_loaders;
	sf::Text m_text;
	sf::RectangleShape m_rect;
	unsigned short m_percentage;
	size_t m_originalWork;
	bool m_manualContinue;
};