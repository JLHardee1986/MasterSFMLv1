#include <iostream>
#include <SFML/Graphics.hpp>

int main(int argc, char* argv[])
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 7;
	sf::RenderWindow m_rWnd({ 1024,768,32U }, "Master Video Game Template 2D",
		sf::Style::None, settings);
	m_rWnd.setFramerateLimit(30);

	while (m_rWnd.isOpen())
	{
		sf::Event e;
		while (m_rWnd.pollEvent(e))
		{
			switch (e.type)
			{
				case sf::Event::Closed:
					m_rWnd.close();
					break;
				case sf::Event::KeyReleased:
					switch (e.key.code)
					{
						case sf::Keyboard::Escape:
							m_rWnd.close();
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}
		m_rWnd.clear(sf::Color(47, 147, 247, 255));

		m_rWnd.display();
	}


	return 0;
}