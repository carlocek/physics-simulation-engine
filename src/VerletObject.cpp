#include <SFML/Graphics.hpp>

#include "VerletObject.hpp"
#include <iostream>

VerletObject::VerletObject(sf::Vector2f position, float radius, float rigidness, bool fixed)
: position(position), prevPosition(position), acceleration(0.0f, 0.0f), radius(radius), rigidness(rigidness), fixed(fixed)
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
		const float dist = position.x;
		const float delta = 0.5f * rigidness * (radius - dist);
		const sf::Vector2f distVecNor = {1.0f, 0.0f};
		setPosition(getPosition() + distVecNor * delta);
	}
	if(position.x + radius > bounds.left + bounds.width)
	{
		const float dist = bounds.left + bounds.width - position.x;
		const float delta = 0.5f * rigidness * (radius - dist);
		const sf::Vector2f distVecNor = {1.0f, 0.0f};
		setPosition(getPosition() - distVecNor * delta);
	}
	if(position.y - radius < bounds.top)
	{
		const float dist = position.y;
		const float delta = 0.5f * rigidness * (radius - dist);
		const sf::Vector2f distVecNor = {0.0f, 1.0f};
		setPosition(getPosition() + distVecNor * delta);
	}
	if(position.y + radius > bounds.top + bounds.height)
	{
		const float dist = bounds.top + bounds.height - position.y;
		const float delta = 0.5f * rigidness * (radius - dist);
		const sf::Vector2f distVecNor = {0.0f, 1.0f};
		setPosition(getPosition() - distVecNor * delta);
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

float VerletObject::getRigidness() const
{
	return rigidness;
}

void VerletObject::setFixed()
{
	this->fixed = true;
}

sf::Vector2f VerletObject::getPrevPosition() const
{
	return prevPosition;
}

void VerletObject::setPrevPosition(sf::Vector2f prevPosition)
{
	this->prevPosition = prevPosition;
}
