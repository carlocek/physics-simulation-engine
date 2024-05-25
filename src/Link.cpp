#include <SFML/Graphics.hpp>

#include "Link.hpp"

Link::Link(int first, int second, float restLength, float stiffness, bool spring)
: first(first), second(second), restLength(restLength), stiffness(stiffness), spring(spring)
{}

int Link::getFirst() const
{
	return first;
}

int Link::getSecond() const
{
	return second;
}

bool Link::isSpring() const
{
	return spring;
}

void Link::setSpring(bool isSpring)
{
	this->spring = isSpring;
}

float Link::getRestLength() const
{
	return restLength;
}

float Link::getStiffness() const
{
	return stiffness;
}

