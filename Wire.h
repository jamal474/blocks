#pragma once
#include<SFML/Graphics.hpp>

class Wire
{
public:

	//wire/line
	sf::RectangleShape line;

	Wire(sf::RenderWindow&, sf::Texture *, float);

	//getters and setters for private members
	float getAngRotation();
	float getAngVelocity();
	float getWireLength();

	//Handles the oscillating motion of wire
	void wireToAndFro(sf::RenderWindow&, float);



private:

	//wire length
	float wireLength;

	//current angle rotation from vertical
	float angRotation;

	//angle change on the wire
	float angVelocity;

	float angAcceleration;


};