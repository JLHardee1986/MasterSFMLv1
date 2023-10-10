#pragma once
#include <SFML/Graphics.hpp>
#include "../../WindowSystem/Window.h"
#include "../Core/C_Base.h"

class C_Drawable : public C_Base{
public:
	C_Drawable(const Component& l_type) : C_Base(l_type) {}
	virtual ~C_Drawable() {}

	virtual void UpdatePosition(const sf::Vector2f& l_vec) = 0;
	virtual sf::Vector2u GetSize() const = 0;
	virtual void Draw(Window* l_wind) = 0;
private:

};