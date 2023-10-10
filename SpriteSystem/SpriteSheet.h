#pragma once
#include <unordered_map>
#include <memory>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "Anim_Directional.h"
#include "../Resources/TextureManager.h"
#include "../WindowSystem/Window.h"
#include "../Utilities/Utilities.h"

using Animations = std::unordered_map<std::string, std::unique_ptr<Anim_Base>>;

class SpriteSheet{
public:
	SpriteSheet(TextureManager* l_textMgr);
	~SpriteSheet();

	void CropSprite(const sf::IntRect& l_rect);
	sf::Vector2u GetSpriteSize()const;
	sf::Vector2f GetSpritePosition()const;
	void SetSpriteSize(const sf::Vector2u& l_size);
	void SetSpritePosition(const sf::Vector2f& l_pos);

	void SetDirection(const Direction& l_dir);
	Direction GetDirection() const;
	void SetSheetPadding(const sf::Vector2f& l_padding);
	void SetSpriteSpacing(const sf::Vector2f& l_spacing);
	sf::Vector2f GetSheetPadding()const;
	sf::Vector2f GetSpriteSpacing()const;
	sf::FloatRect GetSpriteBounds()const;


	bool LoadSheet(const std::string& l_file);
	void ReleaseSheet();

	Anim_Base* GetCurrentAnim();
	bool SetAnimation(const std::string& l_name, bool l_play = false, bool l_loop = false);

	void Update(float l_dT);
	void Draw(Window* l_wnd);
private:
	std::string m_texture;
	sf::Sprite m_sprite;
	sf::Vector2u m_spriteSize;
	sf::Vector2f m_spriteScale;
	Direction m_direction;

	sf::Vector2f m_sheetPadding;
	sf::Vector2f m_spriteSpacing;

	std::string m_animType;
	Animations m_animations;
	Anim_Base* m_animationCurrent;

	TextureManager* m_textureManager;
};