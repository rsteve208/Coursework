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

	// Store the textures
	textureName = { "theWall", "thePlayer", "playerAlt", "theBackground", "catRocket", "wallTwo", "wallThree" };
	texturesToUse = { "Images\\Sprites\\wall.png", "Images\\Sprites\\cat.png", "Images\\Sprites\\cat2.png", "Images\\Bkg\\background.png", "Images\\Sprites\\catRocket.png", "Images\\Sprites\\wall.png", "Images\\Sprites\\wall.png" };
	for (int tCount = 0; tCount < (int)textureName.size(); tCount++)
	{
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	// Create textures for Game Dialogue (text)
	fontList = { "digital", "spaceAge" };
	fontsToUse = { "Fonts/digital-7.ttf", "Fonts/space age.ttf" };
	for (int fonts = 0; fonts < (int)fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}
	gameTextList = { "Cat", "Score : ", "Instructions: Use the spacebar to blast through the walls and earn points!" };
	strScore = gameTextList[1];
	strScore += to_string(theScore).c_str();

	theTextureMgr->addTexture("Title", theFontMgr->getFont("spaceAge")->createTextTexture(theRenderer, gameTextList[0], textType::solid, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("theScore", theFontMgr->getFont("spaceAge")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("instructions", theFontMgr->getFont("spaceAge")->createTextTexture(theRenderer, gameTextList[2], textType::solid, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));

	// Load game sounds
	soundList = { "theme", "shot", "explosion" };
	soundTypes = { soundType::music, soundType::sfx, soundType::sfx };
	soundsToUse = { "Audio/who10Edit.wav", "Audio/shot007.wav", "Audio/explosion2.wav" };
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

	theWallTwo.setSpritePos({ 700, 497 });
	theWallTwo.setTexture(theTextureMgr->getTexture("wallTwo"));
	theWallTwo.setSpriteDimensions(theTextureMgr->getTexture("wallTwo")->getTWidth(), theTextureMgr->getTexture("wallTwo")->getTHeight());

	theWallThree.setSpritePos({ 600, 497 });
	theWallThree.setTexture(theTextureMgr->getTexture("wallThree"));
	theWallThree.setSpriteDimensions(theTextureMgr->getTexture("wallThree")->getTWidth(), theTextureMgr->getTexture("wallThree")->getTHeight());

	// Create vector array of textures

}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	bool loop = true;

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
	tempTextTexture = theTextureMgr->getTexture("theScore");
	pos = { 750, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	scale = { 1, 1 };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

	// Render the instructions
	tempTextTexture = theTextureMgr->getTexture("instructions");
	pos = { 500, 700, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	scale = { 1, 1 };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

	// render the player and walls
	thePlayer.render(theRenderer, &thePlayer.getSpriteDimensions(), &thePlayer.getSpritePos(), thePlayer.getSpriteRotAngle(), &thePlayer.getSpriteCentre(), thePlayer.getSpriteScale());
	theWall.render(theRenderer, &theWall.getSpriteDimensions(), &theWall.getSpritePos(), theWall.getSpriteRotAngle(), &theWall.getSpriteCentre(), theWall.getSpriteScale());
	theWallTwo.render(theRenderer, &theWallTwo.getSpriteDimensions(), &theWallTwo.getSpritePos(), theWallTwo.getSpriteRotAngle(), &theWallTwo.getSpriteCentre(), theWallTwo.getSpriteScale());
	theWallThree.render(theRenderer, &theWallThree.getSpriteDimensions(), &theWallThree.getSpritePos(), theWallThree.getSpriteRotAngle(), &theWallThree.getSpriteCentre(), theWallThree.getSpriteScale());

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
	

	SDL_Point wallPos = { theWall.getSpritePos().x, theWall.getSpritePos().y };
	wallPos.x -= 3;
	theWall.setSpritePos(wallPos);
	if (theWall.getSpritePos().x < -31)
	{
		 wallPos = { theWall.getSpritePos().x, theWall.getSpritePos().y };
		 wallPos.x = WINDOW_WIDTH;
		 theWall.setSpritePos(wallPos);
	}

	SDL_Point wallPosTwo = { theWallTwo.getSpritePos().x, theWallTwo.getSpritePos().y };
	wallPosTwo.x -= 1;
	theWallTwo.setSpritePos(wallPosTwo);
	if (theWallTwo.getSpritePos().x < -31)
	{
		wallPosTwo = { theWallTwo.getSpritePos().x, theWallTwo.getSpritePos().y };
		wallPosTwo.x = WINDOW_WIDTH;
		theWallTwo.setSpritePos(wallPosTwo);
	}

	SDL_Point wallPosThree = { theWallThree.getSpritePos().x, theWallThree.getSpritePos().y };
	wallPosThree.x -= 2;
	theWallThree.setSpritePos(wallPosThree);
	if (theWallThree.getSpritePos().x < -31)
	{
		wallPosThree = { theWallThree.getSpritePos().x, theWallThree.getSpritePos().y };
		wallPosThree.x = WINDOW_WIDTH;
		theWallThree.setSpritePos(wallPosThree);
	}

	//cout << spaceHeldDown << endl;
	SDL_Point playerPos = { thePlayer.getSpritePos().x, thePlayer.getSpritePos().y };

	if (spaceHeldDown) {
		thePlayer.setTexture(theTextureMgr->getTexture("catRocket"));
		if (playerPos.x < (WINDOW_WIDTH - 100))
		{
			playerPos.x += 4;
			thePlayer.setSpritePos(playerPos);
		}
	
	}
	else {

		thePlayer.setTexture(theTextureMgr->getTexture("thePlayer"));
		
		if (playerPos.x > 100)
		{
			playerPos.x -= 5;
			thePlayer.setSpritePos(playerPos);
		}
		
		
		
	}




	// Update the player's position
	thePlayer.update(deltaTime);
}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			/*
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			*/
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					/*case SDLK_ESCAPE:
						theLoop = false;
						break;*/

				case SDLK_SPACE:
					cout << "space down" << endl;
					spaceHeldDown = true;
					/*
					thePlayer.setTexture(theTextureMgr->getTexture("catRocket"));
					cout << "down" << endl;
					theSoundMgr->getSnd("shot")->play(0);*/
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
				}

				break;
			
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

