#pragma once
#include "cSprite.h"
class cWall :
	public cSprite
{
public:
	cWall();
	~cWall();

	bool visible = true;
};

