#pragma once
#include <memory>
#include "C_Drawable.h"
#include "../../SpriteSystem/SpriteSheet.h"

class C_SpriteSheet : public C_Drawable{
public:
	C_SpriteSheet(): C_Drawable(Component::SpriteSheet),m_spriteSheet(nullptr) {}
	~C_SpriteSheet() {}

	void ReadIn(std::stringstream& l_stream) { l_stream >> m_sheetName; }

	void Create(TextureManager* l_textureMgr, const std::string& l_name = "")
	{
		if (m_spriteSheet) { m_spriteSheet.release(); }
		m_spriteSheet = std::make_unique<SpriteSheet>(l_textureMgr);
		m_spriteSheet->LoadSheet("media/Spritesheets/" + (!l_name.empty() ? l_name : m_sheetName) + ".sheet");
	}

	SpriteSheet* GetSpriteSheet() { return m_spriteSheet.get(); }

	void UpdatePosition(const sf::Vector2f& l_vec) {
		m_spriteSheet->SetSpritePosition(l_vec);
	}

	sf::Vector2u GetSize() const {
		return m_spriteSheet->GetSpriteSize();
	}

	void Draw(Window* l_wind) {
		if (!m_spriteSheet) { return; }
		m_spriteSheet->Draw(l_wind);
	}
private:
	std::unique_ptr<SpriteSheet> m_spriteSheet;
	std::string m_sheetName;
};