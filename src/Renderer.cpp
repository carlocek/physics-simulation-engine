#include <SFML/Graphics.hpp>
#include <iostream>

#include "Renderer.hpp"
#include "Engine.hpp"


Renderer::Renderer(sf::RenderTarget& window)
: window(window)
{}

void Renderer::render(Engine& engine)
{
	sf::CircleShape circle(1.0f);
	circle.setPointCount(32);
	circle.setOrigin(1.0f, 1.0f);
	for(const auto& obj : engine.getObjects())
	{
		circle.setPosition(obj.getPosition());
		circle.setScale(obj.getRadius(), obj.getRadius());
		circle.setFillColor(obj.getColor());
		if(obj.isFixed())
		{
			circle.setOutlineThickness(0.5f);
			circle.setOutlineColor(sf::Color::Green);
		}
		else
		{
			circle.setOutlineThickness(0.0f); // Reset outline for non-fixed objects
			circle.setOutlineColor(sf::Color::Transparent); // This can be any color as thickness is 0
		}
		window.draw(circle);
	}

	for(const auto& link : engine.getLinks())
	{
		sf::Vertex line[] = {
			sf::Vertex(engine.getObjects()[link.getFirst()].getPosition(), sf::Color::White),
			sf::Vertex(engine.getObjects()[link.getSecond()].getPosition(), sf::Color::White)
		};
		window.draw(line, 2, sf::Lines);
	}
}
