#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
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

void collisionSimulation(Engine& engine, sf::Clock& spawnClock, int& objCount, const float& objRadius, const float& objRigidness)
{
	const int maxObjCount = 10;
	const float objectSpawnDelay = 0.05f;
	const float objectSpawnSpeed = 1000.f;
	const float angle = -M_PI/6.0f;
	const sf::Vector2f objectSpawnPosition = {100.0f, 100.0f};
	if(objCount < maxObjCount && spawnClock.getElapsedTime().asSeconds() >= objectSpawnDelay)
	{
		spawnClock.restart();
		objCount++;
		VerletObject obj(objectSpawnPosition, objRadius, objRigidness, false);
		engine.setObjectVelocity(obj, objectSpawnSpeed * sf::Vector2f{cos(angle), sin(angle)});
		engine.getObjects().push_back(obj);
	}

}

int main()
{
	// TODO: adjust main with functions to easily switch between simulations (collisions, cloth, free mode)
	const int WIN_WIDTH = 1500;
	const int WIN_HEIGHT = 1000;
	const float frameRate = 60.0f;
	const float timeStep = 1.0f / frameRate;
	const int subSteps = 4;
	const float objRadius = 5.0f;
	const float objRigidness = 1.0f;
	const float linkStiffness = 1.0f;

	int objCount = 0;
	int selectedObj = -1;
	int firstObj = -1;
	int secondObj = -1;
	bool simulationRunning = false, collisionSimSelected = false, addObj = false, addObjFixed = false, addLink = false;

	sf::Font font;
	if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
	{
		std::cerr << "Failed to load font\n";
		return -1;
	}

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Physics Simulation Engine");
    window.setFramerateLimit(frameRate);

    //START GUI CREATION
    tgui::Gui gui{window};
    tgui::Theme::setDefault("C:/TGUI-1.2/themes/Black.txt");
    auto panel = tgui::Panel::create();
	panel->setSize({"30%", "100%"});
	panel->setPosition({"70%", "0%"});
	gui.add(panel);

	auto titleLabel = tgui::Label::create("Physics Simulation Engine");
	titleLabel->setSize({"80%", "5%"});
	titleLabel->setPosition({"10%", "5%"});
	titleLabel->setTextSize(26);
	titleLabel->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	panel->add(titleLabel);

	auto createObjectButton = tgui::Button::create("Create Object");
	createObjectButton->setSize({"80%", "5%"});
	createObjectButton->setPosition({"10%", "15%"});
	panel->add(createObjectButton);

	auto createFixedObjectCheckbox = tgui::CheckBox::create("Fixed");
	createFixedObjectCheckbox->setSize({"10%", "3%"});
	createFixedObjectCheckbox->setPosition({"10%", "25%"});
	panel->add(createFixedObjectCheckbox);

	auto createLinkButton = tgui::Button::create("Create Link");
	createLinkButton->setSize({"80%", "5%"});
	createLinkButton->setPosition({"10%", "35%"});
	panel->add(createLinkButton);

	auto simulationComboBox = tgui::ComboBox::create();
	simulationComboBox->setSize({"80%", "5%"});
	simulationComboBox->setPosition({"10%", "45%"});
	simulationComboBox->addItem("Free Mode");
	simulationComboBox->addItem("Collision Simulation");
	simulationComboBox->addItem("Cloth Simulation");
	simulationComboBox->setDefaultText("Select a mode");
	panel->add(simulationComboBox);

	auto runSimulationButton = tgui::Button::create("Run Simulation");
	runSimulationButton->setSize({"80%", "5%"});
	runSimulationButton->setPosition({"10%", "55%"});
	panel->add(runSimulationButton);

	createObjectButton->onClick([&](){addObj = !addObj; addLink = false;});
	createLinkButton->onClick([&](){addObj = false; addLink = !addLink; firstObj = -1; secondObj = -1;});
	simulationComboBox->onItemSelect([&](const tgui::String& item)
	{
		if(item == "Free Mode")
		{
			collisionSimSelected = false;
		}
		else if(item == "Collision Simulation")
		{
			collisionSimSelected = !collisionSimSelected;
		}
		else if(item == "Simulation 3")
		{
		}
	});
	runSimulationButton->onClick([&](){simulationRunning = !simulationRunning;});
	//END GUI CREATION

    sf::Vector2u windowSize = window.getSize();
    sf::FloatRect windowBounds(0, 0, windowSize.x - panel->getFullSize().x, windowSize.y);
    Engine engine(windowBounds, timeStep, subSteps, 2.0*objRadius);
    Renderer renderer(window);

    sf::Clock frameClock;
    sf::Clock spawnClock;
    while (window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			gui.handleEvent(event);
			if(event.type == sf::Event::Closed)
			{
				window.close();
			}

			if(event.type == sf::Event::MouseButtonPressed)
			{
				if(event.mouseButton.button == sf::Mouse::Left)
				{
					sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
					if(!windowBounds.contains(mousePos))
						continue;
					if(!simulationRunning)
					{
						if(addObj)
						{
							bool fixed = createFixedObjectCheckbox->isChecked();
							VerletObject obj(mousePos, objRadius, objRigidness, fixed);
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

		}

		if(simulationRunning && collisionSimSelected)
		{
			collisionSimulation(engine, spawnClock, objCount, objRadius, objRigidness);
		}



		sf::Time frameTime = frameClock.restart();
		float frameTimeSeconds = frameTime.asSeconds();
		float currentFrameRate = 1.0f / frameTimeSeconds;
		if(currentFrameRate < frameRate-40)
		{
			std::cout << "framerate dropped below 20" << std::endl;
			std::cout << "max objects: " + std::to_string(objCount) << std::endl;
		}

		if(simulationRunning)
			engine.update();
		window.clear(sf::Color::Black);
		renderer.render(engine);

		sf::Text info("objects: " + std::to_string(objCount), font, 14);
		info.setFillColor(sf::Color::White);
		info.setPosition(10, 10);
		window.draw(info);

		gui.draw();

		window.display();
	}
}




