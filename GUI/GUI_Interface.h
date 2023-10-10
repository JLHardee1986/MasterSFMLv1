#pragma once
#include <unordered_map>
#include <memory>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "GUI_Element.h"

using Elements = std::unordered_map<std::string, std::unique_ptr<GUI_Element>>;

class GUI_Manager;

class GUI_Interface : public GUI_Element{
	friend class GUI_Element;
	friend class GUI_Manager;
public:
	GUI_Interface(const std::string& l_name, GUI_Manager* l_guiManager);
	~GUI_Interface();

	void SetPosition(const sf::Vector2f& l_pos);
	void PositionCenterScreen();

	bool AddElement(const GUI_ElementType& l_type, const std::string& l_name);
	GUI_Element* GetElement(const std::string& l_name) const;
	bool RemoveElement(const std::string& l_name);
	bool RemoveElementsContaining(const std::string& l_snippet);

	bool HasParent() const;
	GUI_Manager* GetManager() const;

	bool IsInside(const sf::Vector2f& l_point) const;

	void Focus();
	void Defocus();
	bool IsFocused() const;

	void ReadIn(std::stringstream& l_stream);
	void OnClick(const sf::Vector2f& l_mousePos);
	void OnRelease();
	void OnHover(const sf::Vector2f& l_mousePos);
	void OnLeave();
	void OnTextEntered(const char& l_char);
	void OnFocus();
	void OnDefocus();
	void OnArrowKey(const std::string& l_eventName);

	bool IsBeingMoved() const;
	bool IsMovable() const;
	void BeginMoving();
	void StopMoving();

	void ApplyStyle();

	sf::Vector2f GetPadding() const;
	void SetPadding(const sf::Vector2f& l_padding);
	sf::Vector2f GetGlobalPosition() const;
	sf::Vector2f GetContentSize() const;
	sf::Vector2i GetContentRectSize() const;
	sf::Vector2f GetContentOffset() const;
	void SetContentRectSize(const sf::Vector2i& l_size);
	void SetContentOffset(const sf::Vector2f& l_offset);

	void Redraw();
	bool NeedsContentRedraw() const;
	void RedrawContent();
	bool NeedsControlRedraw() const;
	void RedrawControls();

	void RequestContentRedraw();

	void ToggleTitleBar();

	void Update(float l_dT);
	void Draw(sf::RenderTarget* l_target);

	void UpdateScrollHorizontal(unsigned int l_percent);
	void UpdateScrollVertical(unsigned int l_percent);
private:
	bool ContentLayerContainsPoint(const sf::Vector2f& l_point);
	void DefocusTextfields();
	Elements m_elements;
	sf::Vector2f m_elementPadding;

	GUI_Interface* m_parent;
	GUI_Manager* m_guiManager;

	std::unique_ptr<sf::RenderTexture> m_backdropTexture;
	sf::Sprite m_backdrop;

	// Movement.
	sf::RectangleShape m_titleBar;
	sf::Vector2f m_moveMouseLast;
	bool m_showTitleBar;
	bool m_movable;
	bool m_beingMoved;
	bool m_focused;

	// Variable size.
	void AdjustContentSize(const GUI_Element* l_reference = nullptr);
	void SetContentSize(const sf::Vector2f& l_vec);
	std::unique_ptr<sf::RenderTexture> m_contentTexture;
	sf::Sprite m_content;
	sf::Vector2f m_contentSize;
	sf::Vector2i m_contentRectSize;
	sf::Vector2f m_contentPositionOffset;
	int m_scrollHorizontal;
	int m_scrollVertical;
	bool m_contentRedraw;

	// Control layer.
	std::unique_ptr<sf::RenderTexture> m_controlTexture;
	sf::Sprite m_control;
	bool m_controlRedraw;
};