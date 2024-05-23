#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#include "VerletObject.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"

int selectObjectAtPosition(Engine& engine, const sf::Vector2f& position)
{
    const float selectionRadius = engine.getGrid().cellSize / 2.0f;
    float minDistSqr = selectionRadius * selectionRadius;
    int selectedObj = -1;
    for(int i = 0; i < engine.getObjects().size(); ++i)
    {
        const auto& obj = engine.getObjects()[i];
        sf::Vector2f distVec = obj.getPosition() - position;
        float distSqr = distVec.x * distVec.x + distVec.y * distVec.y;
        if(distSqr < minDistSqr)
        {
            minDistSqr = distSqr;
            selectedObj = i;
        }
    }

    return selectedObj;
}

int main()
{
	const int WIN_WIDTH = 1000;
	const int WIN_HEIGHT = 1000;
	const float frameRate = 60.f;
	const float timeStep = 1.0f / frameRate;
	const int subSteps = 4;
	const sf::Vector2f objectSpawnPosition = {100.0f, 100.0f};
	const float objectSpawnSpeed = 1000.f;
	const float objectSpawnDelay = 0.05f;
	const float angle  = -M_PI/6.f;
	const int maxObjCount = 8000;
	const float objRadius = 10.f;
	const float linkStiffness = 1.f;

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
    int selectedObj = -1;
    int firstObj = -1;
    int secondObj = -1;
    bool simulationRunning = false;
    bool addObj = false;
    bool addObjFixed = false;
    bool addLink = false;
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

			if(event.type == sf::Event::MouseButtonPressed)
			{
				if(event.mouseButton.button == sf::Mouse::Left)
				{
					sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
					if (!simulationRunning) {
						if(addObj)
						{
							VerletObject obj(mousePos, objRadius, false);
							engine.getObjects().push_back(obj);
						}
						else if(addObjFixed)
						{
							VerletObject obj(mousePos, objRadius, true);
							engine.getObjects().push_back(obj);
						}
						else if(addLink)
						{
							int selectedObj = selectObjectAtPosition(engine, mousePos);
							if(selectedObj != -1)
							{
								if(firstObj == -1)
								{
									firstObj = selectedObj;
								}
								else if(secondObj == -1)
								{
									secondObj = selectedObj;
									sf::Vector2f pos1 = engine.getObjects()[firstObj].getPosition();
									sf::Vector2f pos2 = engine.getObjects()[secondObj].getPosition();
									float restLength = sqrt((pos2.x - pos1.x) * (pos2.x - pos1.x) + (pos2.y - pos1.y) * (pos2.y - pos1.y));
									Link link(firstObj, secondObj, restLength, linkStiffness, false);
									engine.getLinks().push_back(link);
									firstObj = -1;
									secondObj = -1;
								}
							}
						}
					}
				}
			}

			if(event.type == sf::Event::KeyPressed)
			{
				if(event.key.code == sf::Keyboard::Enter)
				{
					simulationRunning = !simulationRunning;
				}
				else if(event.key.code == sf::Keyboard::O)
				{
					addObj = !addObj;
					addObjFixed = false;
				}
				else if(event.key.code == sf::Keyboard::F)
				{
					addObjFixed = !addObjFixed;
					addObj = false;
				}
				else if(event.key.code == sf::Keyboard::L)
				{
					addLink = !addLink;
					firstObj = -1;
					secondObj = -1;
				}
			}
		}

//		// COLLISION SIMULATION
//		if(objCount < maxObjCount && spawnClock.getElapsedTime().asSeconds() >= objectSpawnDelay)
//		{
//			spawnClock.restart();
//			objCount++;
//			VerletObject obj(objectSpawnPosition, objRadius, false);
//			engine.setObjectVelocity(obj, objectSpawnSpeed * sf::Vector2f{cos(angle), sin(angle)});
//			engine.getObjects().push_back(obj);
//		}
		sf::Time frameTime = frameClock.restart();
		float frameTimeSeconds = frameTime.asSeconds();
		float currentFrameRate = 1.0f / frameTimeSeconds;
		if(currentFrameRate < frameRate-40)
		{
			std::cout << "framerate dropped below 20" << std::endl;
			std::cout << "max objects: " + std::to_string(objCount) << std::endl;
		}

		if (simulationRunning)
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




