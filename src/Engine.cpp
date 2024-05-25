#include <SFML/Graphics.hpp>

#include "Engine.hpp"

Engine::Engine(sf::FloatRect bounds, float stepdt, int subSteps, float cellSize)
: bounds(bounds), stepdt(stepdt), subSteps(subSteps)
{
	grid.width = bounds.width / cellSize;
	grid.height = bounds.height / cellSize;
	grid.cellSize = cellSize;
	grid.cells.resize(grid.width * grid.height);
}

void Engine::update()
{
	float subdt = getTimeSubstep();
	for(uint8_t i = 0; i < subSteps; i++)
	{
		applyGravity();
		solveCollisions();
		solveObjectLinkCollisions();
		solveLinkConstraints();
		solveBoundaryConstraints();
		updatePositions(subdt);
	}
}

std::vector<VerletObject>& Engine::getObjects()
{
	return objects;
}

std::vector<Link>& Engine::getLinks()
{
	return links;
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

const CollisionGrid& Engine::getGrid() const
{
	return grid;
}
