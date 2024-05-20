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
		circle.setOutlineThickness(0.1f);
		circle.setOutlineColor(sf::Color::Green);
		window.draw(circle);
	}
}
