#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

#include "VerletObject.hpp"

class Engine
{
private:
	std::vector<VerletObject> objects;
	sf::Vector2f gravity = {0.0f, 980.f};
	sf::FloatRect bounds; // window boundaries
	const float stepdt;
	const int subSteps;

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

	void checkCollisions()
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

	void checkBoundaries()
	{
		for (auto& obj : objects)
			obj.checkBoundaries(bounds);
	}

public:
	Engine(sf::FloatRect bounds, float stepdt, int subSteps);
	void update();
//	void addObject(VerletObject o);
	std::vector<VerletObject>& getObjects();
	float getTimeStep();
	float getTimeSubstep();
	void setObjectVelocity(VerletObject& object, sf::Vector2f v);
};
