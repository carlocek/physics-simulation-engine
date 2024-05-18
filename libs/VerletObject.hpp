#pragma once

#include <SFML/Graphics.hpp>

class VerletObject
{
private:
	sf::Vector2f position;
	sf::Vector2f prevPosition;
	sf::Vector2f acceleration;
	float radius = 10.0f;
	sf::Color color = sf::Color::White;

public:
	VerletObject(sf::Vector2f position, float radius);
	void updatePosition(float dt);
	void accelerate(sf::Vector2f a);
	void checkBoundaries(sf::FloatRect bounds);
	sf::Color getColor() const;
	void setColor(sf::Color color = sf::Color::White);
	sf::Vector2f getPosition() const;
	void setPosition(sf::Vector2f position);
	float getRadius() const;
	void setRadius(float radius = 10.0f);
	void setVelocity(sf::Vector2f v, float dt);
};
