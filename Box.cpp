#include<iostream>
#include "Box.h"
#include "Wire.h"

#define pi 3.14159265f

Box::Box(sf::RenderWindow& window, sf::Texture *blockTexture, float boxSize, float wireLength) {

	//set Data Members
	this->angRotation = 0.f;
	this->angVelocity = 0.8f;
	this->angAcceleration = 0.01f;
	this->boxSize = boxSize;
	array = std::vector<sf::RectangleShape>(1);

	//set Initial Box's appearance and position
	this->array[0] = sf::RectangleShape(sf::Vector2f(boxSize, boxSize));
	this->array[0].setTexture(blockTexture);
	this->array[0].setOutlineThickness(0);
	this->array[0].setOrigin({ boxSize / 2.f, boxSize / 2.f });
	this->array[0].setPosition({ window.getSize().x / 2.f, wireLength + boxSize / 2.f });
}

//Create new Box after Previous Box is Dropped
void Box::createBox(sf::RenderWindow& window, Wire wire, int currentBox, float viewAdjustment, sf:: Texture *blockTexture) {
	

	this->array.push_back(sf::RectangleShape(sf::Vector2f(this->boxSize, this->boxSize)));
	//this->array[currentBox].setOutlineColor(sf::Color::White);
	//this->array[currentBox].setFillColor(sf::Color(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1));
	this->array[currentBox].setTexture(blockTexture);
	this->array[currentBox].setOutlineThickness(0);
	this->array[currentBox].setOrigin({ this->boxSize / 2.f, this->boxSize / 2.f });
	this->array[currentBox].setPosition({ window.getSize().x / 2.f - (wire.getWireLength() + this->boxSize / 2.f) * std::sin(pi * wire.getAngRotation() / 180.f) , (wire.getWireLength() + this->boxSize / 2.f) * std::cos(pi * wire.getAngRotation() / 180) + viewAdjustment });
}

//Handles the oscillating motion of Box
void Box::boxToAndFro(sf::RenderWindow& window, bool& isDropped, int currentBox, float viewAdjustment, float wireLength) {
	if (!isDropped)
	{
		this->array[currentBox].setPosition({ window.getSize().x / 2.f - (wireLength + boxSize / 2.f) * std::sin(pi * this->angRotation / 180.f) , (wireLength + boxSize / 2.f) * std::cos(pi * this->angRotation / 180) + viewAdjustment });
		this->array[currentBox].rotate(this->angVelocity);

		//update the current rotation of the box 
		this->angRotation = this->angRotation + this->angVelocity;
	}

	//to and fro of box
	//std::cout<< this->angRotation <<" "<<this->angVelocity<<std::endl;
	if (this->angRotation >= 0.f)
	{
		this->angVelocity -= this->angAcceleration;
	}
	if (this->angRotation < 0.f)
	{
		this->angVelocity += this->angAcceleration;
	}
}

//set private member angRotation
void Box::setAngRotation(float angRotation) {
	this->angRotation = angRotation;
}

//set private member angVelocity
void Box::setAngVelocity(float angVelocity) {
	this->angVelocity = angVelocity;
}


float Box::getBoxSize() {
	return this->boxSize;
}