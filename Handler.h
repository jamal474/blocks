#pragma once

//handle drop of box from wire and subsequent action
void dropHandler(sf::RectangleShape, Box&, Wire, int&, float&, float&, bool&, sf::RenderWindow&, sf::View&, sf::Texture *);

//check collision between two rectangular bodies
bool collision(sf::RectangleShape&, sf::RectangleShape&, float);

//Draw on the screen
void Draw(sf::RenderWindow&, Box, Wire, sf::RectangleShape);

//update view center
void updateViewCenter(sf::View&, Box, float&, float, int, sf::Sprite &);