#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#include "VerletObject.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"

int main()
{
	const int WIN_WIDTH = 1000;
	const int WIN_HEIGHT = 1000;
	const float frameRate = 60.f;
	bool mousePressed;

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Verlet Engine");
    window.setFramerateLimit(frameRate);

    const float timeStep = 1.0f / frameRate;
    const int subSteps = 8;
    sf::Vector2u windowSize = window.getSize();
    sf::FloatRect windowBounds(0, 0, windowSize.x, windowSize.y);
    Engine engine(windowBounds, timeStep, subSteps);

    const sf::Vector2f objectSpawnPosition = {500.0f, 200.0f};
    const float objectSpawnSpeed = 1000.f;
    const float objectSpawnDelay = 0.001f;
    const int maxObjCount = 1000;
    int objCount = 0;

    Renderer renderer(window);

    sf::Clock clock;
    while (window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				window.close();
			}
		}
		if(objCount < maxObjCount && clock.getElapsedTime().asSeconds() >= objectSpawnDelay)
		{
			clock.restart();
			objCount++;
			VerletObject obj(objectSpawnPosition, 8.f);
			const float angle  = M_PI/6.f;
			engine.setObjectVelocity(obj, objectSpawnSpeed * sf::Vector2f{cos(angle), sin(angle)});
			engine.getObjects().push_back(obj);
		}
		engine.update();
		window.clear(sf::Color::Black);
		renderer.render(engine);
		window.display();
	}
}




