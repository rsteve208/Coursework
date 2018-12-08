/*
=================
cAsteroid.h
- Header file for class definition - SPECIFICATION
- Header file for the Rocket class which is a child of cSprite class
=================
*/
#ifndef _CANGRYCAT_H
#define _CANGRYCAT_H
#include "cSprite.h"

class cAngryCat : public cSprite
{
private:
	int asteroidVelocity;

public:
	cAngryCat();
	void update(double deltaTime);		// Rocket update method
	void setAsteroidVelocity(int asteroidVel);   // Sets the velocity for the asteroid
	int getAsteroidVelocity();				 // Gets the asteroid velocity
};
#endif