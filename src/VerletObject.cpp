#include <SFML/Graphics.hpp>

#include "VerletObject.hpp"

VerletObject::VerletObject(sf::Vector2f position, float radius, bool fixed)
: position(position), prevPosition(position), acceleration(0.0f, 0.0f), radius(radius), fixed(fixed)
{}

void VerletObject::updatePosition(float dt)
{
	// compute how much we moved
	const sf::Vector2f displacement = position - prevPosition;
	// update position
	prevPosition = position;
	position = position + displacement + acceleration * (dt*dt);
	// reset acceleration
	acceleration = {0.0f, 0.0f};
}

void VerletObject::accelerate(sf::Vector2f a)
{
	acceleration += a;
}

void VerletObject::checkBoundaries(sf::FloatRect bounds)
{
	if(position.x - radius < bounds.left)
	{
		position.x = bounds.left + radius;
		prevPosition.x = 2 * position.x - prevPosition.x;
	}
	if(position.x + radius > bounds.left + bounds.width)
	{
		position.x = bounds.left + bounds.width - radius;
		prevPosition.x = 2 * position.x - prevPosition.x;
	}
	if(position.y - radius < bounds.top)
	{
		position.y = bounds.top + radius;
		prevPosition.y = 2 * position.y - prevPosition.y;
	}
	if(position.y + radius > bounds.top + bounds.height)
	{
		position.y = bounds.top + bounds.height - radius;
		prevPosition.y = 2 * position.y - prevPosition.y;
	}
}

sf::Color VerletObject::getColor() const
{
	return color;
}

sf::Vector2f VerletObject::getPosition() const
{
	return position;
}

void VerletObject::setPosition(sf::Vector2f position)
{
	this->position = position;
}

float VerletObject::getRadius() const
{
	return radius;
}

void VerletObject::setVelocity(sf::Vector2f v, float dt)
{
	prevPosition = position - (v * dt);
}

bool VerletObject::isFixed() const
{
	return fixed;
}

void VerletObject::setFixed()
{
	this->fixed = true;
}
