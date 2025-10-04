#pragma once
#include<SFML/Graphics.hpp>
#include<vector>
#include"Wire.h"

class Box
{
public:

	//array for boxes
	std::vector<sf::RectangleShape> array;

	Box(sf::RenderWindow& , sf::Texture *, float, float);

	//getters and setters
	void setAngRotation(float);
	void setAngVelocity(float);
	float getBoxSize();

	//Create new Box after Previous Box is Dropped
	void createBox(sf::RenderWindow&, Wire, int, float, sf::Texture *);
	//Handles the oscillating motion of Box
	void boxToAndFro(sf::RenderWindow&, bool&, int, float, float);


private:

	//box width/height
	float boxSize;

	//current anglular rotation of the box
	float angRotation;

	//angle change on the box
	float angVelocity;

	float angAcceleration;


};

