#pragma once
#include <unordered_map>
#include <memory>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>

class Window;

using ShaderList = std::unordered_map<std::string, std::unique_ptr<sf::Shader>>;

class Renderer {
	friend Window;
public:
	Renderer(Window* l_window, bool l_useShaders = true);

	void AdditiveBlend(bool l_flag);
	bool UseShader(const std::string& l_name);
	void DisableShader();
	sf::Shader* GetShader(const std::string& l_name);

	void BeginDrawing();
	bool IsDrawing()const;
	void Draw(const sf::Shape& l_shape, sf::RenderTarget* l_target = nullptr);
	void Draw(const sf::Sprite& l_sprite, sf::RenderTarget* l_target = nullptr);
	void Draw(const sf::Drawable& l_drawable, sf::RenderTarget* l_target = nullptr);
	void EndDrawing();
private:
	void LoadShaders();

	Window* m_window;
	ShaderList m_shaders;
	sf::Shader* m_currentShader;
	bool m_addBlend;
	bool m_drawing;
	bool m_useShaders;
	unsigned int m_drawCalls;
};