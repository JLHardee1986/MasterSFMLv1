#include "GUI_Sprite.h"
#include "../GUI_Manager.h"
#include "../../SharedContext.h"
#include "../../Resources/TextureManager.h"

GUI_Sprite::GUI_Sprite(const std::string& l_name, GUI_Interface* l_owner)
	: GUI_Element(l_name, GUI_ElementType::Sprite, l_owner), m_textureMgr(m_owner->GetManager()->GetContext()->m_textureManager) {}
GUI_Sprite::~GUI_Sprite() {
	if (!m_texture.empty()) { m_textureMgr->ReleaseResource(m_texture); }
}

void GUI_Sprite::SetTexture(const std::string& l_texture) {
	if (!m_texture.empty()) { m_textureMgr->ReleaseResource(m_texture); m_texture = ""; }
	if (!m_textureMgr->RequireResource(l_texture)) { return; }
	m_sprite.setTexture(*m_textureMgr->GetResource(l_texture));
	m_texture = l_texture;
	m_needsRedraw = true;
	m_owner->SetRedraw(true);
}

void GUI_Sprite::SetTexture(sf::RenderTexture& l_renderTexture) {
	if (!m_texture.empty()) { m_textureMgr->ReleaseResource(m_texture); m_texture = ""; }
	m_sprite.setTexture(l_renderTexture.getTexture());
	m_needsRedraw = true;
	m_owner->SetRedraw(true);
}

void GUI_Sprite::ReadIn(std::stringstream& l_stream) {
	std::string TextureName;
	if (!(l_stream >> TextureName)) { return; }
	SetTexture(TextureName);
}
void GUI_Sprite::OnClick(const sf::Vector2f& l_mousePos) { SetState(GUI_ElementState::Clicked); }
void GUI_Sprite::OnRelease() { SetState(GUI_ElementState::Neutral); }
void GUI_Sprite::OnHover(const sf::Vector2f& l_mousePos) { SetState(GUI_ElementState::Focused); }
void GUI_Sprite::OnLeave() { SetState(GUI_ElementState::Neutral); }

void GUI_Sprite::ApplyStyle() {
	GUI_Element::ApplyStyle();
	
	auto texture = m_sprite.getTexture();
	if (texture) {
		m_style[m_state].m_size = sf::Vector2f(m_sprite.getTexture()->getSize());
	}

	m_sprite.setPosition(GetPosition());
}

void GUI_Sprite::Update(float l_dT) {}

void GUI_Sprite::Draw(sf::RenderTarget* l_target) {
	l_target->draw(m_sprite);
	l_target->draw(m_visual.m_text);
}
