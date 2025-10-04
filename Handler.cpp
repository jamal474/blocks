#include<SFML/Graphics.hpp>
#include<iostream>
#include"Box.h"
#include"Wire.h"

//check collision between two rectangular bodies
bool collision(sf::RectangleShape& entity1, sf::RectangleShape& entity2, float velocity)
{
	if ((entity1.getPosition().y + entity1.getSize().y / 2.f + velocity >= entity2.getPosition().y - entity2.getSize().y / 2.f) && (entity1.getPosition().x + entity1.getSize().x / 2.f >= entity2.getPosition().x - entity2.getSize().x / 2.f && entity1.getPosition().x - entity1.getSize().x / 2.f <= entity2.getPosition().x + entity2.getSize().x / 2.f))
	{
		entity1.setPosition({ entity1.getPosition().x, entity2.getPosition().y - (entity1.getSize().y / 2.f + entity2.getSize().y / 2.f) });
		return true;
	}
	return false;
}

//handle drop of box from wire and subsequent action
void dropHandler(sf::RectangleShape platform, Box& boxes, Wire wire, int& currentBox, float& velocity, float& viewAdjustment, bool& isDropped, sf::RenderWindow& window, sf::View& view,sf::Texture *blockTexture)
{
	//stop Box rotation when falling
	boxes.array[currentBox].setRotation(0.f);

	//if collison with platform or previous block
	if (!collision(boxes.array[currentBox], platform, velocity) && ((currentBox != 0 && !collision(boxes.array[currentBox], boxes.array[currentBox - 1], velocity)) || currentBox == 0))
	{
		velocity += 0.3f;
		boxes.array[currentBox].setPosition({ boxes.array[currentBox].getPosition().x, boxes.array[currentBox].getPosition().y + velocity });
	}
	else
	{
		//After Collision with box or platfrom
		isDropped = false;

		//Create new box
		currentBox++;
		boxes.createBox(window, wire, currentBox, viewAdjustment, blockTexture);

		//set its Rotation and position according to wire's current behaviour
		boxes.array[currentBox].setRotation(wire.getAngRotation());
		boxes.setAngRotation(wire.getAngRotation());
		boxes.setAngVelocity(wire.getAngVelocity());

	}
}

//Draw on the screen
void Draw(sf::RenderWindow& window, Box boxes, Wire wire, sf::RectangleShape platform)
{
	//Draw Blocks
	for (int i = 0; i < boxes.array.size(); i++)
		window.draw(boxes.array[i]);

	//Draw Crane Wire
	window.draw(wire.line);
	//Draw Block Platfrom
	window.draw(platform);

}

//move the view up when the previous block's height - 100.f reaches view Center
void updateViewCenter(sf::View& view, Box boxes, float& viewAdjustment, float windowWidth, int currentBox, sf::Sprite &sp2) {
	//UpdateView
	if (currentBox != 0)
	{
		//Y coordinate for the height of the tower
		float targetCenterY = boxes.array[currentBox - 1].getPosition().y;

		//move view if its center is lower than targetCenterY - offset
		float offset = 2 * boxes.getBoxSize();
		if (view.getCenter().y > targetCenterY - offset )
		{
			//0.5f is the speed of center's movement
			float nextCenterY = view.getCenter().y - 1.f;

			//adjustment for the pendulum components
			viewAdjustment -= 1.f;

			// Make sure we don't overshoot our target
			if (nextCenterY < targetCenterY - offset)
			{
				nextCenterY = targetCenterY - offset;
			}

			//setting the new center
			view.setCenter({ windowWidth / 2.f, nextCenterY });
			sp2.setPosition({ sp2.getPosition().x, sp2.getPosition().y - 1.f});
		}
	}
}