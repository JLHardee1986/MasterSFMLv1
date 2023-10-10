#include "Renderer.h"
#include "../WindowSystem/Window.h"
#include "../Utilities/Utilities.h"

Renderer::Renderer(Window* l_window, bool l_useShaders) : m_window(l_window), m_useShaders(l_useShaders),
	m_drawing(false), m_addBlend(false), m_drawCalls(0), m_currentShader(nullptr) {}

void Renderer::AdditiveBlend(bool l_flag) { m_addBlend = l_flag; }

bool Renderer::UseShader(const std::string& l_name) {
	if (!m_useShaders) { return false; }
	m_currentShader = GetShader(l_name);
	return (m_currentShader != nullptr);
}

void Renderer::DisableShader() { m_currentShader = nullptr; }

sf::Shader* Renderer::GetShader(const std::string& l_name) {
	if (!m_useShaders) { return nullptr; }
	auto shader = m_shaders.find(l_name);
	if (shader == m_shaders.end()) { return nullptr; }
	return shader->second.get();
}

void Renderer::BeginDrawing() { m_drawing = true; m_drawCalls = 0; }
bool Renderer::IsDrawing() const { return m_drawing; }
void Renderer::EndDrawing() { m_drawing = false; }

void Renderer::Draw(const sf::Shape& l_shape, sf::RenderTarget* l_target) {
	if (!l_target) {
		if (!m_window->GetViewSpace().intersects(l_shape.getGlobalBounds())) { return; }
	}
	Draw((const sf::Drawable&)l_shape, l_target);
}

void Renderer::Draw(const sf::Sprite& l_sprite, sf::RenderTarget* l_target) {
	if (!l_target) {
		if (!m_window->GetViewSpace().intersects(l_sprite.getGlobalBounds())) { return; }
	}
	Draw((const sf::Drawable&)l_sprite, l_target);
}

void Renderer::Draw(const sf::Drawable& l_drawable, sf::RenderTarget* l_target) {
	if (!l_target) { l_target = m_window->GetRenderWindow(); }
	l_target->draw(l_drawable, (m_addBlend ? sf::BlendAdd :
		m_currentShader && m_useShaders ? m_currentShader : sf::RenderStates::Default));
	++m_drawCalls;
}

void Renderer::LoadShaders() {
	if (!m_useShaders) { return; }
	auto directory = Utils::GetWorkingDirectory() + "media/Shaders/";
	auto v_shaders = Utils::GetFileList(directory, "*.vert", false);
	auto f_shaders = Utils::GetFileList(directory, "*.frag", false);

	for (auto& shader : v_shaders) {
		auto& file = shader.first;
		auto name = file.substr(0, file.find(".vert"));
		auto fragShader = std::find_if(f_shaders.begin(), f_shaders.end(),
			[&name](std::pair<std::string, bool>& l_pair) { return l_pair.first == name + ".frag"; }
		);

		auto shaderItr = m_shaders.emplace(name, std::move(std::make_unique<sf::Shader>()));
		auto& shader = shaderItr.first->second;
		if (fragShader != f_shaders.end()) {
			shader->loadFromFile(directory + name + ".vert", directory + name + ".frag");
			f_shaders.erase(fragShader);
		} else {
			shader->loadFromFile(directory + name + ".vert", sf::Shader::Vertex);
		}
	}

	for (auto& shader : f_shaders) {
		auto& file = shader.first;
		auto name = file.substr(0, file.find(".frag"));
		auto shaderItr = m_shaders.emplace(name, std::move(std::make_unique<sf::Shader>()));
		auto& shader = shaderItr.first->second;
		shader->loadFromFile(directory + name + ".frag", sf::Shader::Fragment);
	}
}