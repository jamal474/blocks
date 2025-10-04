#include<iostream>
#include "Wire.h"

#define pi 3.14159265f

//Wire class constructor
Wire::Wire(sf::RenderWindow& window, sf::Texture *wireTexture, float wireLength)
{

	//set Data members
	this->angVelocity = 0.8f;
	this->angRotation = 0.f;
	this->wireLength = wireLength;
	this->angAcceleration = 0.01f;

	//set wire's position and appearance
	this->line.setSize(sf::Vector2f(1.0f, wireLength));
	this->line.setScale({ 12.f,1.1f });
	this->line.setTexture(wireTexture);
	this->line.setOrigin({ 0.5f, wireLength / 2.f });
	this->line.setPosition({ window.getSize().x / 2.f, wireLength / 2.f });

}

//Handles the oscillating motion of wire
void Wire::wireToAndFro(sf::RenderWindow& window, float viewAdjustment)
{

	//wire rotation around top mid axis
	this->line.setPosition({ window.getSize().x / 2.f - (this->wireLength / 2.f) * std::sin(pi * this->angRotation / 180.f) , (this->wireLength / 2.f) * std::cos(pi * this->angRotation / 180) + viewAdjustment });
	this->line.rotate(this->angVelocity);

	//change angular velocity for to and fro of wire
	this->angRotation = this->angRotation + this->angVelocity;
	if (this->angRotation >= 0.f)
	{
		this->angVelocity -= this->angAcceleration;
	}
	if (this->angRotation < 0.f)
	{
		this->angVelocity += this->angAcceleration;
	}

}

//get private member angRotation
float Wire::getAngRotation()
{
	return this->angRotation;
}

//get private member angular velocity
float Wire::getAngVelocity()
{
	return this->angVelocity;
}

float Wire::getWireLength() {
	return this->wireLength;
}
