#include <SFML/Graphics.hpp>

#include "Engine.hpp"

Engine::Engine(sf::FloatRect bounds, float stepdt, int subSteps, float cellSize)
: bounds(bounds), stepdt(stepdt), subSteps(subSteps)
{
	grid.width = bounds.width;
	grid.height = bounds.height;
	grid.cellSize = cellSize;
}

void Engine::update()
{
	float subdt = getTimeSubstep();
	for(int i = 0; i < subSteps; i++)
	{
		applyGravity();
		checkCollisions();
		checkBoundaries();
		updatePositions(subdt);
	}
}

std::vector<VerletObject>& Engine::getObjects()
{
	return objects;
}

//void Engine::addObject(VerletObject o)
//{
//	objects.push_back(o);
//}

float Engine::getTimeStep()
{
	return stepdt;
}

float Engine::getTimeSubstep()
{
	return stepdt/static_cast<float>(subSteps);
}

void Engine::setObjectVelocity(VerletObject &object, sf::Vector2f v)
{
	object.setVelocity(v, getTimeSubstep());
}

