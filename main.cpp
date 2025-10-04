#include<SFML/Graphics.hpp> 
#include<SFML/Window.hpp> 
#include<iostream>
#include<algorithm>
#include<vector>
#include<cmath>

#include"Box.h"
#include"Wire.h"
#include"Handler.h"

#define pi 3.14159265f

int main() {

	float windowWidth = 400;
	float windowHeight = 500;
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Game Demo", sf::Style::Close | sf::Style::Default);
	window.setFramerateLimit(60);

	//adjustment offset for the boxes according to view
	float viewAdjustment = 0.f;



	//check if current box has been dropped
	bool isDropped = false;
	//Dropping velocity
	float velocity = 0.f;


	//Box varibles
	float boxSize = 50.f;
	float wireLength = 100.f;
	int currentBox = 0;

	//Building background
	sf::Texture bg1;
	sf::Sprite sp1;
	if (!bg1.loadFromFile("textures/background.png"))
	{
		std::cout << "Error in loading building image" << std::endl;
	}
	sp1.setTexture(bg1);
	sp1.setScale({ 0.6, 0.6});
	sp1.setPosition({ -20.f, 200.f });

	//Stars background
	sf::Texture bg2;
	sf::Sprite sp2;
	if (!bg2.loadFromFile("textures/sky.jpg"))
	{
		std::cout << "Error in loading star background" << std::endl;
	}
	sp2.setTexture(bg2);
	sp2.setScale({ 0.4,0.4 });

	//Block Texture for boxes
	sf::Texture blockTexture;
	if (!blockTexture.loadFromFile("textures/block.png"))
	{
		std::cout << "Error loading block image" << std::endl;
	}

	//wire texture
	sf::Texture wireTexture;
	if (!wireTexture.loadFromFile("textures/hook.png"))
	{
		std::cout << "Error loading hook image for crane wire" << std::endl;
	}
	wireTexture.setSmooth(true);


	//Define Boxes
	Box boxes(window, &blockTexture, boxSize, wireLength);
	//crane wire
	Wire wire(window, &wireTexture,  wireLength);

	//View for upward movement
	sf::View view;
	view.setSize({ windowWidth, windowHeight });
	view.setCenter({ windowWidth / 2.f,windowHeight / 2.f });


	//Define Platform
	sf::RectangleShape platform(sf::Vector2f(3.f * boxSize, 10.f));
	platform.setFillColor(sf::Color::Transparent);
	platform.setOrigin({ 2.f * boxSize, 5.f });
	platform.setPosition({ windowWidth / 2.f,windowHeight - 70.f });

	/*float 
	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			{
				window.close();
			}
		}

		/* <------------- update -------------->  */

		//Perform oscillation on the wire and box
		wire.wireToAndFro(window, viewAdjustment);
		boxes.boxToAndFro(window, isDropped, currentBox, viewAdjustment, wireLength);

		//box drop Event
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !isDropped)
		{
			isDropped = true;
			boxes.setAngRotation(0.0f); //box
		}

		//when Box is Dropped
		if (isDropped)
		{
			dropHandler(platform, boxes, wire, currentBox, velocity, viewAdjustment, isDropped, window, view, &blockTexture);
			


			if (boxes.array[currentBox].getPosition().y - boxSize / 2.f > window.getSize().y)
			{
				window.close();
			}
		}

		//update View as the tower height increases
		updateViewCenter(view, boxes, viewAdjustment, windowWidth, currentBox, sp2);
		


		/* <--------------- Draw ----------------------> */
		window.clear();
		
		//set View
		window.setView(view);

		window.draw(sp2);
		window.draw(sp1);
		Draw(window, boxes, wire, platform);
		window.display();
	}

	return 0;
}