#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include "../GUI_Element.h"

class TextureManager;

class GUI_Sprite : public GUI_Element {
public:
	GUI_Sprite(const std::string& l_name, GUI_Interface* l_owner);
	~GUI_Sprite();

	void SetTexture(const std::string& l_texture);
	void SetTexture(sf::RenderTexture& l_renderTexture);

	void ReadIn(std::stringstream& l_stream);
	void OnClick(const sf::Vector2f& l_mousePos);
	void OnRelease();
	void OnHover(const sf::Vector2f& l_mousePos);
	void OnLeave();
	void ApplyStyle();
	void Update(float l_dT);
	void Draw(sf::RenderTarget* l_target);
private:
	TextureManager* m_textureMgr;
	std::string m_texture;
	sf::Sprite m_sprite;
};