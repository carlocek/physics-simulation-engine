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
	if(spawnClock.getElapsedTime().asSeconds() >= objectSpawnDelay)
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
	const int WIN_WIDTH = 1500;
	const int WIN_HEIGHT = 1000;
	const float frameRate = 60.0f;
	const float timeStep = 1.0f / frameRate;
	const int subSteps = 4;
	float objRadius = 5.0f;
	const float objRigidness = 1.0f;
	const float linkStiffnessLow = 0.001f;
	const float linkStiffnessMedium = 0.01f;
	const float linkStiffnessHigh = 0.1f;
	float linkStiffness;

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

	auto titleText = tgui::Label::create("Physics Simulation Engine");
	titleText->setSize({"80%", "5%"});
	titleText->setPosition({"10%", "5%"});
	titleText->setTextSize(26);
//	titleText->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	panel->add(titleText);

	auto instructionsText = tgui::Label::create("To create objects click on the button and then click on the black area to spawn them \n"
			"To link objects click on the button and select two existing objects by clicking on them");
	instructionsText->setSize({"80%", "10%"});
	instructionsText->setPosition({"10%", "10%"});
	instructionsText->setTextSize(16);
//	instructionsText->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
	panel->add(instructionsText);

//	auto radiusSlider = tgui::Slider::create(5, 15);
//	radiusSlider->setSize({"80%", "3%"});
//	radiusSlider->setStep(5);
//	radiusSlider->setPosition({"10%", "25%"});
//	radiusSlider->setValue(5); // Default value
//	panel->add(radiusSlider);
//
//	auto radiusText = tgui::Label::create("Object radius: 5");
//	radiusText->setSize({"80%", "4%"});
//	radiusText->setPosition({"10%", "30%"});
//	radiusText->setTextSize(16);
//	panel->add(radiusText);

	auto createObjectButton = tgui::Button::create("Create Object");
	createObjectButton->setSize({"25%", "4%"});
	createObjectButton->setPosition({"10%", "35%"});
	panel->add(createObjectButton);

	auto createFixedObjectCheckbox = tgui::CheckBox::create("Fixed");
	createFixedObjectCheckbox->setSize({"10%", "4%"});
	createFixedObjectCheckbox->setPosition({"40%", "35%"});
	panel->add(createFixedObjectCheckbox);

	auto createLinkButton = tgui::Button::create("Create Link");
	createLinkButton->setSize({"25%", "4%"});
	createLinkButton->setPosition({"10%", "45%"});
	panel->add(createLinkButton);

	auto createSpringLinkCheckbox = tgui::CheckBox::create("Spring");
	createSpringLinkCheckbox->setSize({"10%", "4%"});
	createSpringLinkCheckbox->setPosition({"40%", "45%"});
	panel->add(createSpringLinkCheckbox);

	auto stiffnessComboBox = tgui::ComboBox::create();
	stiffnessComboBox->setSize({"25%", "4%"});
	stiffnessComboBox->setPosition({"65%", "45%"});
	stiffnessComboBox->addItem("Low");
	stiffnessComboBox->addItem("Medium");
	stiffnessComboBox->addItem("High");
	stiffnessComboBox->setDefaultText("Stiffness");
	panel->add(stiffnessComboBox);

	auto simulationComboBox = tgui::ComboBox::create();
	simulationComboBox->setSize({"80%", "4%"});
	simulationComboBox->setPosition({"10%", "55%"});
	simulationComboBox->addItem("Free Mode");
	simulationComboBox->addItem("Collision Simulation");
//	simulationComboBox->addItem("Cloth Simulation (not implemented)");
	simulationComboBox->setDefaultText("Select a mode");
	panel->add(simulationComboBox);

	auto runSimulationButton = tgui::Button::create("Start/Stop Simulation");
	runSimulationButton->setSize({"80%", "4%"});
	runSimulationButton->setPosition({"10%", "65%"});
	panel->add(runSimulationButton);

	auto objectCountText = tgui::Label::create("Objects: 0");
	objectCountText->setSize({"80%", "5%"});
	objectCountText->setPosition({"10%", "75%"});
	objectCountText->setTextSize(16);
	panel->add(objectCountText);

	auto frameRateText = tgui::Label::create("FPS: 0");
	frameRateText->setSize({"80%", "5%"});
	frameRateText->setPosition({"10%", "80%"});
	frameRateText->setTextSize(16);
	panel->add(frameRateText);

	createObjectButton->onClick([&](){addObj = !addObj; addLink = false;});
	createLinkButton->onClick([&](){addObj = false; addLink = !addLink; firstObj = -1; secondObj = -1;});
	stiffnessComboBox->onItemSelect([&](const tgui::String& item)
	{
		if(item == "Low")
		{
			linkStiffness = linkStiffnessLow;
		}
		else if(item == "Medium")
		{
			linkStiffness = linkStiffnessMedium;
		}
		else if(item == "High")
		{
			linkStiffness = linkStiffnessHigh;
		}
		else
		{
			linkStiffness = 1.0f;
		}
	});
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
//		else if(item == "Cloth Simulation (not implemented)")
//		{
//		}
	});
	runSimulationButton->onClick([&](){simulationRunning = !simulationRunning;});
	//END GUI CREATION

    sf::Vector2u windowSize = window.getSize();
    sf::FloatRect windowBounds(0, 0, windowSize.x - panel->getFullSize().x, windowSize.y);
    Engine engine(windowBounds, timeStep, subSteps, 2.0*objRadius);
    Renderer renderer(window);

//    radiusSlider->onValueChange([&](float value)
//	{
//		radiusText->setText("Object radius: " + std::to_string(static_cast<int>(value)));
//		if(value != objRadius)
//		{
//			engine.setGridCellSize(2.0*value);
//		}
//		objRadius = value;
//	});

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
							objCount++;
						}
						else if(addLink)
						{
							bool isSpring = createSpringLinkCheckbox->isChecked();
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
									//setting rest length to half the initial distance between linked objects to see sprng effect
									if(isSpring)
										restLength *= 0.5f;
									Link link(firstObj, secondObj, restLength, linkStiffness, isSpring);
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
			simulationRunning = false;
			collisionSimSelected = false;
		}

		if(simulationRunning)
			engine.update();
		window.clear(sf::Color::Black);
		renderer.render(engine);

		objectCountText->setText("Objects: " + std::to_string(objCount));
		frameRateText->setText("FPS: " + std::to_string(static_cast<int>(currentFrameRate)));

		gui.draw();

		window.display();
	}
}




