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
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();


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

	theAreaClicked = { 0, 0 };
	// Store the textures
	textureName = { "theWall", "thePlayer", "playerAlt", "theBackground", "catRocket", "wallTwo", "wallThree", "gameOver"  };
	texturesToUse = { "Images\\Sprites\\wall.png", "Images\\Sprites\\cat.png", "Images\\Sprites\\cat2.png", "Images\\Bkg\\background.png", "Images\\Sprites\\catRocket.png", "Images\\Sprites\\wall.png", "Images\\Sprites\\wall.png", "Images\\Bkg\\gameover.png" };
	for (int tCount = 0; tCount < (int)textureName.size(); tCount++)
	{
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}


	//Store and assign the textures for buttons
	buttonNameList = { "exit", "play", "replay", "menu"};
	btnTexturesToUse = { "Images\\Buttons\\exit.png", "Images\\Buttons\\play.png", "Images\\Buttons\\replay.png", "Images\\Buttons\\menu.png" };
	btnPos = { { 400, 375 }, { 400, 300 }, { 400, 300 }, {400, 700 } };
	for (unsigned int bCount = 0; bCount < buttonNameList.size(); bCount++)
	{
		theTextureMgr->addTexture(buttonNameList[bCount], btnTexturesToUse[bCount]);
	}
	for (unsigned int bCount = 0; bCount < buttonNameList.size(); bCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(buttonNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(buttonNameList[bCount])->getTWidth(), theTextureMgr->getTexture(buttonNameList[bCount])->getTHeight());
		theButtonMgr->add(buttonNameList[bCount], newBtn);
	}
	theGameState = gameState::menu;
	theBtnType = btnTypes::exit;

	// Create textures for Game Dialogue (text)
	fontList = { "amatic", "amaticBold" };
	fontsToUse = { "Fonts/AmaticSC-Regular.ttf", "Fonts/Amatic-Bold.ttf" };
	for (int fonts = 0; fonts < (int)fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}
	gameTextList = { "Angry Cats!", "Score : ", "Instructions: Use the spacebar to blast through the walls and earn points!", "Lives : " };
	strScore = gameTextList[1];
	strScore += to_string(theScore).c_str();
	
	strLives = gameTextList[3];
	strLives += to_string(theLives).c_str();

	theTextureMgr->addTexture("Title", theFontMgr->getFont("amaticBold")->createTextTexture(theRenderer, gameTextList[0], textType::solid, { 200, 10, 0, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("theScore", theFontMgr->getFont("amaticBold")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 200, 10, 0, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("theLives", theFontMgr->getFont("amaticBold")->createTextTexture(theRenderer, strLives.c_str(), textType::solid, { 200, 10, 0, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("instructions", theFontMgr->getFont("amaticBold")->createTextTexture(theRenderer, gameTextList[2], textType::solid, { 200, 10, 0, 0 }, { 0, 0, 0, 0 }));

	// Load game sounds
	soundList = { "theme", "rocket", "explosion", "meow" , "mouseClick" };
	soundTypes = { soundType::music, soundType::sfx, soundType::sfx, soundType::sfx, soundType::sfx };
	soundsToUse = { "Audio\\theme.mp3", "Audio\\spacebar.wav", "Audio\\wallDestroyed.wav", "Audio\\catMeow.mp3", "Audio\\mouseClick.mp3" };
	for (int sounds = 0; sounds < (int)soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	//Play the background music on a loop
	theSoundMgr->getSnd("theme")->play(-1);

	//Assign all textures
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
	//Walls can collide on initial load
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

	switch (theGameState)
	{
	case gameState::menu:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("Title");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("instructions");
		pos = { 50, 175, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		// Render Button
		theButtonMgr->getBtn("play")->render(theRenderer, &theButtonMgr->getBtn("play")->getSpriteDimensions(), &theButtonMgr->getBtn("play")->getSpritePos(), theButtonMgr->getBtn("play")->getSpriteScale());
		theButtonMgr->getBtn("exit")->setSpritePos({ 400, 375 });
		theButtonMgr->getBtn("exit")->render(theRenderer, &theButtonMgr->getBtn("exit")->getSpriteDimensions(), &theButtonMgr->getBtn("exit")->getSpritePos(), theButtonMgr->getBtn("exit")->getSpriteScale());
	}
	break;
	case gameState::end:
	{

		//Replace the background
		spriteBkgd.setTexture(theTextureMgr->getTexture("gameOver"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("gameOver")->getTWidth(), theTextureMgr->getTexture("gameOver")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Display the final score
		tempTextTexture = theTextureMgr->getTexture("Title");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("theScore");
		pos = { 900, 5, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		
		//Render the buttons
		theButtonMgr->getBtn("replay")->setSpritePos({ 500, 500 });
		theButtonMgr->getBtn("replay")->render(theRenderer, &theButtonMgr->getBtn("replay")->getSpriteDimensions(), &theButtonMgr->getBtn("replay")->getSpritePos(), theButtonMgr->getBtn("replay")->getSpriteScale());
		theButtonMgr->getBtn("exit")->setSpritePos({ 500, 575 });
		theButtonMgr->getBtn("exit")->render(theRenderer, &theButtonMgr->getBtn("exit")->getSpriteDimensions(), &theButtonMgr->getBtn("exit")->getSpritePos(), theButtonMgr->getBtn("exit")->getSpriteScale());
	}
	break;
	case gameState::quit:
	{
		//Close the game
		loop = false;
	}
	break;
	case gameState::playing:
	{

		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("Title");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		// Render updated score value
		if (updateScore == true)
		{
			theTextureMgr->deleteTexture("theScore");
			theTextureMgr->addTexture("theScore", theFontMgr->getFont("amaticBold")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 200, 10, 0, 0 }, { 0, 0, 0, 0 }));
			updateScore = false;
		}

		tempTextTexture = theTextureMgr->getTexture("theScore");
		pos = { 900, 5, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		// Render the lives
		if (updateLives == true)
		{
			theTextureMgr->deleteTexture("theLives");
			theTextureMgr->addTexture("theLives", theFontMgr->getFont("amaticBold")->createTextTexture(theRenderer, strLives.c_str(), textType::solid, { 200, 10, 0, 0 }, { 0, 0, 0, 0 }));
			updateLives = false;
		}

		tempTextTexture = theTextureMgr->getTexture("theLives");
		pos = { 10, 700, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		// Render the instructions
		tempTextTexture = theTextureMgr->getTexture("instructions");
		pos = { 300, 700, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 0.5, 0.5 };
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
	}

	break;
	default:
		break;
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
	// CHeck Button clicked and change state
	if (theGameState == gameState::menu || theGameState == gameState::end)
	{
		theGameState = theButtonMgr->getBtn("exit")->update(theGameState, gameState::quit, theAreaClicked);
	}
	else
	{
		theGameState = theButtonMgr->getBtn("exit")->update(theGameState, gameState::end, theAreaClicked);
	}
	theGameState = theButtonMgr->getBtn("play")->update(theGameState, gameState::playing, theAreaClicked);
	theGameState = theButtonMgr->getBtn("menu")->update(theGameState, gameState::menu, theAreaClicked);
	theGameState = theButtonMgr->getBtn("replay")->update(theGameState, gameState::playing, theAreaClicked);

	
		
	// If the player has lost all of their lives then the game will quit
	if (theLives <= 0)
	{
		theGameState = gameState::end;
		
	}
	

	//All sounds and movement will be suspended while the gamestate is not playing
	if (theGameState == gameState::playing)
	{
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

		//See above for theWall
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

		//See above for theWall
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
			//Get the position where the mouse has been clicked
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
			{
				theAreaClicked = { event.motion.x, event.motion.y };
			}
			break;
			}
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{


			case SDLK_SPACE:
				//If spacebar is pressed and the gamestate is playing then
				if (theGameState == gameState::playing)
				{
					//Play the rocket sound
					cout << "space down" << endl;
					spaceHeldDown = true;
					if (playSound == true)
					{
						cout << "sound played" << endl;
						theSoundMgr->getSnd("rocket")->play(0);
						playSound = false;
					}
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

