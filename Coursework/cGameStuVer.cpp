/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{
	
}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);

	/* Let the computer pick a random number */
	random_device rd;    // non-deterministic engine 
	mt19937 gen{ rd() }; // deterministic engine. For most common uses, std::mersenne_twister_engine, fast and high-quality.

	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();
	theScore = 0;
	theLives = 9;

	// Store the textures
	textureName = { "theWall", "thePlayer", "playerAlt", "theBackground", "catRocket", "wallTwo", "wallThree" };
	texturesToUse = { "Images\\Sprites\\wall.png", "Images\\Sprites\\cat.png", "Images\\Sprites\\cat2.png", "Images\\Bkg\\background.png", "Images\\Sprites\\catRocket.png", "Images\\Sprites\\wall.png", "Images\\Sprites\\wall.png" };
	for (int tCount = 0; tCount < (int)textureName.size(); tCount++)
	{
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	// Create textures for Game Dialogue (text)
	fontList = { "amatic", "amaticBold" };
	fontsToUse = { "Fonts/AmaticSC-Regular.ttf", "Fonts/Amatic-Bold.ttf" };
	for (int fonts = 0; fonts < (int)fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 48);
	}
	gameTextList = { "Angry Cats!", "Score : ", "Instructions: Use the spacebar to blast through the walls and earn points!", "Lives : " };
	strScore = gameTextList[1];
	strScore += to_string(theScore).c_str();
	
	strLives = gameTextList[3];
	strLives += to_string(theLives).c_str();

	theTextureMgr->addTexture("Title", theFontMgr->getFont("amaticBold")->createTextTexture(theRenderer, gameTextList[0], textType::solid, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("theScore", theFontMgr->getFont("amaticBold")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("instructions", theFontMgr->getFont("amaticBold")->createTextTexture(theRenderer, gameTextList[2], textType::solid, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("theLives", theFontMgr->getFont("amaticBold")->createTextTexture(theRenderer, strLives.c_str(), textType::solid, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));

	// Load game sounds
	soundList = { "theme", "rocket", "explosion", "meow" };
	soundTypes = { soundType::music, soundType::sfx, soundType::sfx, soundType::sfx };
	soundsToUse = { "Audio\\theme.mp3", "Audio\\spacebar.wav", "Audio\\wallDestroyed.wav", "Audio\\catMeow.mp3" };
	for (int sounds = 0; sounds < (int)soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("theme")->play(-1);

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

	thePlayer.setSpritePos({ 105, 530 });
	thePlayer.setTexture(theTextureMgr->getTexture("thePlayer"));
	thePlayer.setSpriteDimensions(theTextureMgr->getTexture("thePlayer")->getTWidth(), theTextureMgr->getTexture("thePlayer")->getTHeight());
	thePlayer.setSpriteTranslation({ 50,50 });



	theWall.setSpritePos({ 600, 497 });
	theWall.setTexture(theTextureMgr->getTexture("theWall"));
	theWall.setSpriteDimensions(theTextureMgr->getTexture("theWall")->getTWidth(), theTextureMgr->getTexture("theWall")->getTHeight());
	theWall.canCollide = true;


	theWallTwo.setSpritePos({ 700, 497 });
	theWallTwo.setTexture(theTextureMgr->getTexture("wallTwo"));
	theWallTwo.setSpriteDimensions(theTextureMgr->getTexture("wallTwo")->getTWidth(), theTextureMgr->getTexture("wallTwo")->getTHeight());
	theWallTwo.canCollide = true;

	theWallThree.setSpritePos({ 600, 497 });
	theWallThree.setTexture(theTextureMgr->getTexture("wallThree"));
	theWallThree.setSpriteDimensions(theTextureMgr->getTexture("wallThree")->getTWidth(), theTextureMgr->getTexture("wallThree")->getTHeight());
	theWallThree.canCollide = true;

	// Create vector array of textures

}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);
	spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
	
	// Render the Title
	cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
	SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	FPoint scale = { 1, 1 };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

	// Render updated score value
	if (updateScore == true)
	{
		theTextureMgr->deleteTexture("theScore");
		theTextureMgr->addTexture("theScore", theFontMgr->getFont("amaticBold")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
		updateScore = false;
	}

	tempTextTexture = theTextureMgr->getTexture("theScore");
	pos = { 850, 5, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	scale = { 1, 1 };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

	// Render the lives
	if (updateLives == true)
	{
		theTextureMgr->deleteTexture("theLives");
		theTextureMgr->addTexture("theLives", theFontMgr->getFont("amaticBold")->createTextTexture(theRenderer, strLives.c_str(), textType::solid, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
		updateLives = false;
	}

	tempTextTexture = theTextureMgr->getTexture("theLives");
	pos = { 10, 700, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	scale = { 1, 1 };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

	// Render the instructions
	tempTextTexture = theTextureMgr->getTexture("instructions");
	pos = { 500, 700, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	scale = { 1, 1 };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

	// render the player and walls
	thePlayer.render(theRenderer, &thePlayer.getSpriteDimensions(), &thePlayer.getSpritePos(), thePlayer.getSpriteRotAngle(), &thePlayer.getSpriteCentre(), thePlayer.getSpriteScale());
	thePlayer.setBoundingRect();

	// will only render the walls if they player has not destroyed them
	if (theWall.visible == true)
	{
		theWall.render(theRenderer, &theWall.getSpriteDimensions(), &theWall.getSpritePos(), theWall.getSpriteRotAngle(), &theWall.getSpriteCentre(), theWall.getSpriteScale());
		theWall.setBoundingRect();
	}

	if (theWallTwo.visible == true)
	{
		theWallTwo.render(theRenderer, &theWallTwo.getSpriteDimensions(), &theWallTwo.getSpritePos(), theWallTwo.getSpriteRotAngle(), &theWallTwo.getSpriteCentre(), theWallTwo.getSpriteScale());
		theWallTwo.setBoundingRect();
	}

	if (theWallThree.visible == true)
	{
		theWallThree.render(theRenderer, &theWallThree.getSpriteDimensions(), &theWallThree.getSpritePos(), theWallThree.getSpriteRotAngle(), &theWallThree.getSpriteCentre(), theWallThree.getSpriteScale());
		theWallThree.setBoundingRect();
	}

	SDL_RenderPresent(theRenderer);
}



void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{	
	// If the player has lost all of their lives then the game will quit
	if (theLives <= 0)
	{
		loop = false;
	}
	

	// Put the wall positions into their own variable
	SDL_Point wallPos = { theWall.getSpritePos().x, theWall.getSpritePos().y };
	// Set the wall move speed to 3 moving from right to left
	wallPos.x -= 3;
	// update the sprite position
	theWall.setSpritePos(wallPos);
	// If the wall has gone off thr screen to the left
	if (theWall.getSpritePos().x < -31)
	{
		// Get the sprite position and update it to the right side of the window
		wallPos = { theWall.getSpritePos().x, theWall.getSpritePos().y };
		wallPos.x = WINDOW_WIDTH;
		theWall.setSpritePos(wallPos);
		// Allow collisions and start rendering it again
		theWall.canCollide = true;
		theWall.visible = true;
	}

	SDL_Point wallPosTwo = { theWallTwo.getSpritePos().x, theWallTwo.getSpritePos().y };
	wallPosTwo.x -= 1;
	theWallTwo.setSpritePos(wallPosTwo);
	if (theWallTwo.getSpritePos().x < -31)
	{
		wallPosTwo = { theWallTwo.getSpritePos().x, theWallTwo.getSpritePos().y };
		wallPosTwo.x = WINDOW_WIDTH;
		theWallTwo.setSpritePos(wallPosTwo);
		theWallTwo.canCollide = true;
		theWallTwo.visible = true;
	}

	SDL_Point wallPosThree = { theWallThree.getSpritePos().x, theWallThree.getSpritePos().y };
	wallPosThree.x -= 2;
	theWallThree.setSpritePos(wallPosThree);
	if (theWallThree.getSpritePos().x < -31)
	{
		wallPosThree = { theWallThree.getSpritePos().x, theWallThree.getSpritePos().y };
		wallPosThree.x = WINDOW_WIDTH;
		theWallThree.setSpritePos(wallPosThree);
		theWallThree.canCollide = true;
		theWallThree.visible = true;
	}

	//cout << spaceHeldDown << endl;
	// Get the player's position
	SDL_Point playerPos = { thePlayer.getSpritePos().x, thePlayer.getSpritePos().y };

	// If the spacebar is held down
	if (spaceHeldDown)
	{
		// Swap the player texture
		thePlayer.setTexture(theTextureMgr->getTexture("catRocket"));
		// As long as the player is not near the edge of the window
		if (playerPos.x < (WINDOW_WIDTH - 100))
		{
			// Move the player at a rate of 4 to the right
			playerPos.x += 4;
			thePlayer.setSpritePos(playerPos);
		}

	}
	else {

		// Swap the player texture
		thePlayer.setTexture(theTextureMgr->getTexture("thePlayer"));

		// Move the player at a rate of 5 to the left as long as their x coordinate is greater than 100
		if (playerPos.x > 100)
		{
			playerPos.x -= 5;
			thePlayer.setSpritePos(playerPos);
		}



	}

	// If the player collides with a wall
	if (thePlayer.collidedWith(&thePlayer.getBoundingRect(), &theWall.getBoundingRect()) == true)
	{
		// If the wall is able to be collided with 
		if (theWall.canCollide)
		{
			// Turn the wall's collision off
			theWall.canCollide = false;
			// Print collision to the console
			cout << "Collision" << endl;
			// If the spacebar is being held down
			if (spaceHeldDown == true)
			{
				// Add to the score
				theScore += 10;
				strScore = gameTextList[1];
				strScore += to_string(theScore).c_str();
				// Update the score
				updateScore = true;
				//Play the explosion sound
				theSoundMgr->getSnd("explosion")->play(0);
				// Stop rendering the wall
				theWall.visible = false;
			}
			// If the player is not holding space and collides with the wall
			else
			{
				//Print damage to the console
				cout << "Damage" << endl;
				//Play the meow sound
				theSoundMgr->getSnd("meow")->play(0);
				// Reduce the lives by 1
				theLives--;
				strLives = gameTextList[3];
				strLives += to_string(theLives).c_str();
				// Update the lives string
				updateLives = true;
			}
		}
	}

	// See comments for theWall
	if (thePlayer.collidedWith(&thePlayer.getBoundingRect(), &theWallTwo.getBoundingRect()))
	{
		if (theWallTwo.canCollide)
		{
			theWallTwo.canCollide = false;
			cout << "Collision" << endl;
			if (spaceHeldDown == true)
			{
				theScore += 10;
				strScore = gameTextList[1];
				strScore += to_string(theScore).c_str();
				updateScore = true;
				theSoundMgr->getSnd("explosion")->play(0);
				theWallTwo.visible = false;
			}
			else
			{
				cout << "Damage" << endl;
				theSoundMgr->getSnd("meow")->play(0);
				theLives--;
				strLives = gameTextList[3];
				strLives += to_string(theLives).c_str();
				updateLives = true;
			}
		}
	}

	//See comments for theWall
	if (thePlayer.collidedWith(&thePlayer.getBoundingRect(), &theWallThree.getBoundingRect()))
	{
		if (theWallThree.canCollide)
		{
			theWallThree.canCollide = false;
			cout << "Collision" << endl;
			if (spaceHeldDown == true)
			{
				cout << "1 point" << endl;
				theScore += 10;
				strScore = gameTextList[1];
				strScore += to_string(theScore).c_str();
				updateScore = true;
				theSoundMgr->getSnd("explosion")->play(0);
				theWallThree.visible = false;
			}
			else
			{
				cout << "Damage" << endl;
				theSoundMgr->getSnd("meow")->play(0);
				theLives--;
				strLives = gameTextList[3];
				strLives += to_string(theLives).c_str();
				updateLives = true;
			}
		}
	}

	
	
	// Update the player's position
	thePlayer.update(deltaTime);
}

bool cGame::getInput(bool theLoop)
{
	
	SDL_Event event;
	bool playSound = true;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{


			case SDLK_SPACE:
				cout << "space down" << endl;
				spaceHeldDown = true;
				if (playSound == true)
				{
					cout << "sound played" << endl;
					theSoundMgr->getSnd("rocket")->play(0);
					playSound = false;
				}
				break;


			default:
				break;
			}

			break;

		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_SPACE:
				spaceHeldDown = false;
				cout << "space up" << endl;
				//xcout << "up" << endl;

				break;

				default:
				break;

				break;

			}


		}
	}
		return theLoop;
	
}
		
	



double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	//Quit FONT system
	TTF_Quit();

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

