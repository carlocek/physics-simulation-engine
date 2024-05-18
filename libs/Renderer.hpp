#pragma once

#include <SFML/Graphics.hpp>

#include "VerletObject.hpp"
#include "Engine.hpp"

class Renderer
{
private:
	sf::RenderTarget& window;
//	std::vector<VerletObject>& objects;

public:
	Renderer(sf::RenderTarget& window);
	void render(Engine& engine);
};
