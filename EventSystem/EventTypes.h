#pragma once
#include <SFML/Window/Event.hpp>

enum class EventType {
	KeyDown = sf::Event::KeyPressed,
	KeyUp = sf::Event::KeyReleased,
	MButtonDown = sf::Event::MouseButtonPressed,
	MButtonUp = sf::Event::MouseButtonReleased,
	MouseWheel = sf::Event::MouseWheelMoved,
	WindowResized = sf::Event::Resized,
	GainedFocus = sf::Event::GainedFocus,
	LostFocus = sf::Event::LostFocus,
	MouseEntered = sf::Event::MouseEntered,
	MouseLeft = sf::Event::MouseLeft,
	Closed = sf::Event::Closed,
	TextEntered = sf::Event::TextEntered,
	Keyboard = sf::Event::Count + 1, Mouse, Joystick,
	GUI_Click, GUI_Release, GUI_Hover, GUI_Leave, GUI_Focus, GUI_Defocus
};

enum class EventInfoType { Normal, GUI = -1 };
enum class MouseButtonType { Left = sf::Mouse::Left, Right = sf::Mouse::Right,
	Middle = sf::Mouse::Middle, Button1 = sf::Mouse::XButton1, Button2 = sf::Mouse::XButton2
};