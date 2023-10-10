#pragma once
#include <string>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Renderer.h"
#include "../EventSystem/EventManager.h"

class Window{
public:
	Window(const std::string& l_title = "Window", const sf::Vector2u& l_size = {640,480}, bool l_useShaders = true);
	~Window();

	void BeginDraw();
	void EndDraw();

	void Update();

	bool IsDone() const;
	bool IsFullscreen() const;
	bool IsFocused() const;

	void ToggleFullscreen(EventDetails* l_details);
	void Close(EventDetails* l_details = nullptr);

	sf::RenderWindow* GetRenderWindow();
	Renderer* GetRenderer();
	EventManager* GetEventManager();
	sf::Vector2u GetWindowSize()const;
	sf::FloatRect GetViewSpace()const;
private:
	void Setup(const std::string& l_title, const sf::Vector2u& l_size);
	void Create();

	sf::RenderWindow m_window;
	Renderer m_renderer;
	EventManager m_eventManager;
	sf::Vector2u m_windowSize;
	std::string m_windowTitle;
	bool m_isDone;
	bool m_isFullscreen;
	bool m_isFocused;

	bool m_shadersLoaded;
};