#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <unordered_map>
#include <iostream>

#include "VerletObject.hpp"

struct CollisionCell
{
	static constexpr int capacity = 4;
	int objects[capacity] = {};
	int objCount = 0;
	void add(int id)
	{
		if(objCount < capacity)
			objects[objCount++] = id;
	}
};

struct CollisionGrid
{
	int width;
	int height;
	int cellSize;
	std::vector<CollisionCell> cells;
	void clear()
	{
		for(auto& cell : cells)
			cell.objCount = 0;
	}
};

class Engine
{
private:
	std::vector<VerletObject> objects;
	sf::Vector2f gravity = {0.0f, 980.f};
	sf::FloatRect bounds; // window boundaries
	const float stepdt;
	const int subSteps;
	CollisionGrid grid;

	void applyGravity()
	{
		for (auto& obj : objects)
			obj.accelerate(gravity);
	}

	void updatePositions(float dt)
	{
		for (auto& obj : objects)
			obj.updatePosition(dt);
	}

	void checkBoundaries()
	{
		for (auto& obj : objects)
			obj.checkBoundaries(bounds);
	}

	void checkCollisionsNaive()
	{
		const float responseCoef = 1.0f; // to adjust collision elasticity
		// iterate on all objects
		for(int i = 0; i < objects.size(); i++)
		{
			VerletObject& obj1 = objects[i];
			// iterate on object involved in new collision pairs
			for(int j = i+1; j < objects.size(); j++)
			{
				VerletObject& obj2 = objects[j];
				const sf::Vector2f distVec = obj1.getPosition() - obj2.getPosition();
				const float distSqr = distVec.x * distVec.x + distVec.y * distVec.y;
				const float min_dist = obj1.getRadius() + obj2.getRadius();
				// check overlap
				if(distSqr < min_dist * min_dist)
				{
					const float dist = sqrt(distSqr);
					const sf::Vector2f distVecNor = distVec / dist;
					const float massRatio1 = obj1.getRadius() / (obj1.getRadius() + obj2.getRadius());
					const float massRatio2 = obj2.getRadius() / (obj1.getRadius() + obj2.getRadius());
					const float delta = 0.5f * responseCoef * (dist - min_dist);
					// update positions, moving each obj by half of the overlapping segment in opposite directions
					obj1.setPosition(obj1.getPosition() - distVecNor * (massRatio2 * delta));
					obj2.setPosition(obj2.getPosition() + distVecNor * (massRatio1 * delta));
				}
			}
		}
	}


	void checkCollisions()
	{
		populateGrid();
		for(int cellIndex = 0; cellIndex < grid.width*grid.height; cellIndex++)
		{
			if(grid.cells[cellIndex].objCount > 0)
			{
				CollisionCell& cell = grid.cells[cellIndex];
				// check collisions with neighboring cells and current cell itself
				std::vector<int> neighbors = getNeighboringCells(cellIndex);
				for(int neighborIndex : neighbors)
				{
					if(grid.cells[neighborIndex].objCount > 0)
					{
						CollisionCell& neighborCell = grid.cells[neighborIndex];
						for(int i = 0; i < cell.objCount; i++)
						{
							VerletObject& obj1 = objects[cell.objects[i]];
							for (int j = 0; j < neighborCell.objCount; j++)
							{
								VerletObject& obj2 = objects[neighborCell.objects[j]];
								solveCollision(obj1, obj2);
							}
						}
					}
				}
			}
		}
	}

	void solveCollision(VerletObject& obj1, VerletObject& obj2)
	{
		const float responseCoef = 1.0f;
		const float eps = 0.0001f;
	    const sf::Vector2f distVec = obj1.getPosition() - obj2.getPosition();
	    const float distSqr = distVec.x * distVec.x + distVec.y * distVec.y;
	    const float min_dist = obj1.getRadius() + obj2.getRadius();
	    if(distSqr < min_dist * min_dist && distSqr > eps)
	    {
	        const float dist = sqrt(distSqr);
	        const sf::Vector2f distVecNor = distVec / dist;
	        const float massRatio1 = obj1.getRadius() / (obj1.getRadius() + obj2.getRadius());
	        const float massRatio2 = obj2.getRadius() / (obj1.getRadius() + obj2.getRadius());
	        const float delta = 0.5f * responseCoef * (dist - min_dist);
	        obj1.setPosition(obj1.getPosition() - distVecNor * (massRatio2 * delta));
	        obj2.setPosition(obj2.getPosition() + distVecNor * (massRatio1 * delta));
	    }
	}

	void populateGrid()
	{
		grid.clear();
		int i = 0;
		for(VerletObject& obj : objects)
		{
			int cellIndex = getCellIndex(obj.getPosition());
			grid.cells[cellIndex].add(i);
			i++;
		}
	}

	int getCellIndex(const sf::Vector2f& position)
	{
		int x = static_cast<int>(position.x) / grid.cellSize;
		int y = static_cast<int>(position.y) / grid.cellSize;
		return x + y * grid.width;
	}

	std::vector<int> getNeighboringCells(int cellIndex)
	{
	    std::vector<int> neighbors;
	    int x = cellIndex % grid.width;
	    int y = cellIndex / grid.width;
	    for(int dx = -1; dx <= 1; ++dx)
	    {
	        for(int dy = -1; dy <= 1; ++dy)
	        {
				if(x + dx >= 0 && x + dx < grid.width && y + dy >= 0 && y + dy < grid.height)
					neighbors.push_back((x + dx) + (y + dy) * grid.width);
	        }
	    }
	    return neighbors;
	}

public:
	Engine(sf::FloatRect bounds, float stepdt, int subSteps, float objRadius);
	void update();
//	void addObject(VerletObject o);
	std::vector<VerletObject>& getObjects();
	float getTimeStep();
	float getTimeSubstep();
	void setObjectVelocity(VerletObject& object, sf::Vector2f v);
};
