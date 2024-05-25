#pragma once

#include <SFML/Graphics.hpp>

class Link
{
private:
	int first;
	int second;
	float restLength;
	float stiffness; // Only for spring links
	bool spring;
public:
	Link(int first, int second, float restLength, float stiffness, bool spring);
	int getFirst() const;
	int getSecond() const;
	bool isSpring() const;
	void setSpring(bool isSpring);
	float getRestLength() const;
	float getStiffness() const;
};

