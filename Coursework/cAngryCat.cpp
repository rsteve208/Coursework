/*
=================
cAngryCat.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cAngryCat.h"

/*
=================================================================
Defualt Constructor
=================================================================
*/
cAngryCat::cAngryCat() : cSprite()
{
	this->asteroidVelocity = 0;
}
/*
=================================================================
Update the sprite position
=================================================================
*/

void cAngryCat::update(double deltaTime)
{

	this->setSpriteRotAngle((float)(this->getSpriteRotAngle() +(5.0f * deltaTime))); 
	if (this->getSpriteRotAngle() > 360)
	{
		this->setSpriteRotAngle(this->getSpriteRotAngle() - 360.0f);
	}

	SDL_Rect currentSpritePos = this->getSpritePos();
	currentSpritePos.x += (int)(this->getSpriteTranslation().x * deltaTime);
	currentSpritePos.y -= (int)(this->getSpriteTranslation().y * deltaTime);

	this->setSpritePos({ currentSpritePos.x, currentSpritePos.y });
	this->setBoundingRect();
}
/*
=================================================================
  Sets the velocity for the Asteroid
=================================================================
*/
void cAngryCat::setAsteroidVelocity(int AsteroidVel)
{
	asteroidVelocity = AsteroidVel;
}
/*
=================================================================
  Gets the Asteroid velocity
=================================================================
*/
int cAngryCat::getAsteroidVelocity()
{
	return asteroidVelocity;
}
