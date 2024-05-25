#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <unordered_map>
#include <iostream>

#include "VerletObject.hpp"
#include "Link.hpp"

struct CollisionCell
{
	static constexpr int capacity = 4;
	uint32_t objects[capacity] = {};
	int objCount = 0;
	void add(uint32_t id)
	{
		if(objCount < capacity)
			objects[objCount++] = id;
	}
};

struct CollisionGrid
{
	uint32_t width;
	uint32_t height;
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
	std::vector<Link> links;
	sf::Vector2f gravity = {0.0f, 980.f};
	sf::FloatRect bounds; // window boundaries
	const float stepdt;
	const int subSteps;
	CollisionGrid grid;

	void applyGravity()
	{
		for (auto& obj : objects)
			if(!obj.isFixed())
				obj.accelerate(gravity);
	}

	void updatePositions(float dt)
	{
		for (auto& obj : objects)
			obj.updatePosition(dt);
	}

	void solveBoundaryConstraints()
	{
		for (auto& obj : objects)
			obj.checkBoundaries(bounds);
	}

	void solveCollisionsNaive()
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


	void solveCollisions()
	{
		populateGrid();
		for(uint32_t cellIndex = 0; cellIndex < grid.width*grid.height; cellIndex++)
		{
			if(grid.cells[cellIndex].objCount > 0)
			{
				CollisionCell& cell = grid.cells[cellIndex];
				// check collisions with neighboring cells and current cell itself
				std::vector<uint32_t> neighbors = getNeighboringCells(cellIndex);
				for(uint32_t neighborIndex : neighbors)
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
		const float responseCoef = (obj1.getRigidness()+obj2.getRigidness()) / 2;
		const float eps = 0.0001f;
	    const sf::Vector2f distVec = obj1.getPosition() - obj2.getPosition();
	    const float distSqr = distVec.x * distVec.x + distVec.y * distVec.y;
	    const float minDist = obj1.getRadius() + obj2.getRadius();
	    if(distSqr < minDist * minDist && distSqr > eps)
	    {
	        const float dist = sqrt(distSqr);
	        const sf::Vector2f distVecNor = distVec / dist;
	        const float massRatio1 = obj1.getRadius() / (obj1.getRadius() + obj2.getRadius());
	        const float massRatio2 = obj2.getRadius() / (obj1.getRadius() + obj2.getRadius());
	        const float delta = 0.5f * responseCoef * (dist - minDist);
	        if (obj1.isFixed() && !obj2.isFixed())
			{
				obj2.setPosition(obj2.getPosition() + distVecNor * delta);
			}
			else if (!obj1.isFixed() && obj2.isFixed())
			{
				obj1.setPosition(obj1.getPosition() - distVecNor * delta);
			}
			else if (!obj1.isFixed() && !obj2.isFixed())
			{
				obj1.setPosition(obj1.getPosition() - distVecNor * (massRatio2 * delta));
				obj2.setPosition(obj2.getPosition() + distVecNor * (massRatio1 * delta));
			}
	    }
	}

	void populateGrid()
	{
		grid.clear();
		uint32_t i = 0;
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

	std::vector<uint32_t> getNeighboringCells(uint32_t cellIndex)
	{
	    std::vector<uint32_t> neighbors;
	    uint32_t x = cellIndex % grid.width;
	    uint32_t y = cellIndex / grid.width;
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

	void solveLinkConstraints()
	{
	    for(Link& link : links)
	    {
	        VerletObject& obj1 = objects[link.getFirst()];
	        VerletObject& obj2 = objects[link.getSecond()];

	        sf::Vector2f distVec = obj2.getPosition() - obj1.getPosition();
	        float dist = sqrt(distVec.x * distVec.x + distVec.y * distVec.y);
	        const sf::Vector2f distVecNor = distVec / dist;
	        const float delta = 0.5f * (dist - link.getRestLength());

	        sf::Vector2f correction = distVecNor * delta;

	        if(link.isSpring())
	        {
	            correction *= link.getStiffness(); // The spring force is scaled by stiffness
	        }
	        if(!obj1.isFixed())
	        	obj1.setPosition(obj1.getPosition() + correction);
	        if(!obj2.isFixed())
	        	obj2.setPosition(obj2.getPosition() - correction);
	    }
	}

	void solveObjectLinkCollisions()
	{
	    for(VerletObject& obj : objects)
	    {
	        for(Link& link : links)
	        {
	        	if(&obj != &objects[link.getFirst()] && &obj != &objects[link.getSecond()])
	        		solveObjectLinkCollision(obj, link, objects);
	        }
	    }
	}

	void solveObjectLinkCollision(VerletObject& obj, const Link& link, std::vector<VerletObject>& objects)
	{
	    const sf::Vector2f& pos1 = objects[link.getFirst()].getPosition();
	    const sf::Vector2f& pos2 = objects[link.getSecond()].getPosition();
	    // find the closest point on the segment pos1-pos2 to objPos
	    sf::Vector2f segment = pos2 - pos1;
	    float segmentLengthSquared = segment.x * segment.x + segment.y * segment.y;

	    float t = std::max(0.f, std::min(1.f, ((obj.getPosition() - pos1).x * segment.x + (obj.getPosition() - pos1).y * segment.y) / segmentLengthSquared));
	    sf::Vector2f closestPoint = pos1 + t * segment;
	    // move the object away from the closest point on the segment
	    sf::Vector2f distVec = obj.getPosition() - closestPoint;
	    float dist = sqrt(distVec.x * distVec.x + distVec.y * distVec.y);
	    if(dist < obj.getRadius())
	    {
	        sf::Vector2f distVecNor = distVec / dist;
	        float overlap = obj.getRadius() - dist;
	        obj.setPosition(obj.getPosition() + distVecNor * overlap);
	        // adjust the link's end objects
	        if(!objects[link.getFirst()].isFixed())
	        {
	            objects[link.getFirst()].setPosition(objects[link.getFirst()].getPosition() - distVecNor * overlap * 0.5f);
	        }
	        if(!objects[link.getSecond()].isFixed())
	        {
	            objects[link.getSecond()].setPosition(objects[link.getSecond()].getPosition() - distVecNor * overlap * 0.5f);
	        }
	    }
	}

public:
	Engine(sf::FloatRect bounds, float stepdt, int subSteps, float cellSize);
	void update();
//	void addObject(VerletObject o);
	std::vector<VerletObject>& getObjects();
	std::vector<Link>& getLinks();
	float getTimeStep();
	float getTimeSubstep();
	void setObjectVelocity(VerletObject& object, sf::Vector2f v);
	const CollisionGrid& getGrid() const;
};
