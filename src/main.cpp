#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#include "VerletObject.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"

int main()
{
	const int WIN_WIDTH = 600;
	const int WIN_HEIGHT = 600;
	const float frameRate = 60.f;
	const float timeStep = 1.0f / frameRate;
	const int subSteps = 4;
	const sf::Vector2f objectSpawnPosition = {100.0f, 100.0f};
	const float objectSpawnSpeed = 1000.f;
	const float objectSpawnDelay = 0.05f;
	const float angle  = -M_PI/6.f;
	const int maxObjCount = 8000;
	const float objRadius = 3.f;

	sf::Font font;
	if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
	{
		std::cerr << "Failed to load font\n";
		return -1;
	}

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Verlet Engine");
    window.setFramerateLimit(frameRate);

    sf::Vector2u windowSize = window.getSize();
    sf::FloatRect windowBounds(0, 0, windowSize.x, windowSize.y);
    Engine engine(windowBounds, timeStep, subSteps, 2.0*objRadius);
    Renderer renderer(window);

    int objCount = 0;
    sf::Clock frameClock;
    sf::Clock spawnClock;
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

		sf::Time frameTime = frameClock.restart();
		float frameTimeSeconds = frameTime.asSeconds();
		float currentFrameRate = 1.0f / frameTimeSeconds;
//		std::cout << currentFrameRate << std::endl;

		if(currentFrameRate < frameRate-40)
		{
			std::cout << "framerate dropped below 60" << std::endl;
			std::cout << "max objects: " + std::to_string(objCount) << std::endl;
//			break;
		}

		if(objCount < maxObjCount && spawnClock.getElapsedTime().asSeconds() >= objectSpawnDelay)
		{
			spawnClock.restart();
			objCount++;
			VerletObject obj(objectSpawnPosition, objRadius);
			engine.setObjectVelocity(obj, objectSpawnSpeed * sf::Vector2f{cos(angle), sin(angle)});
			engine.getObjects().push_back(obj);
		}
		engine.update();
		window.clear(sf::Color::Black);
		renderer.render(engine);

		sf::Text info("objects: " + std::to_string(objCount), font, 14);
		info.setFillColor(sf::Color::White);
		info.setPosition(10, 10);
		window.draw(info);

		window.display();
	}
}




