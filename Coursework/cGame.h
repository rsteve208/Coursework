#ifndef _GAME_H
#define _GAME_H

/*
==================================================================================
cGame.h
==================================================================================
*/

#include <SDL.h>
#include "cWall.h"
#include <random>
// Game specific includes
#include "angryCatGame.h"


using namespace std;

class cGame
{
public:
	cGame();

	void initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void cleanUp(SDL_Window* theSDLWND);
	void render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre);
	void update();
	void update(double deltaTime);
	bool getInput(bool theLoop);
	double getElapsedSeconds();
	bool loop;

	static cGame* getInstance();

private:

	static cGame* pInstance;
	// for framerates
	time_point< high_resolution_clock > m_lastTime;
	time_point< high_resolution_clock > m_CurrentTime;
	duration< double > deltaTime;

	// Sprites for displaying background and rocket textures
	cSprite spriteBkgd;
	cPlayer thePlayer;
	cAngryCat theAsteroid;
	cWall theWall;
	cWall theWallTwo;
	cWall theWallThree;
	// game related variables
	vector<LPCSTR> textureName;
	vector<LPCSTR> textName;
	vector<LPCSTR> texturesToUse;
	vector<cAngryCat*> theAsteroids;
	vector<cSprite*> theExplosions;
	// Fonts to use
	vector<LPCSTR> fontList;
	vector<LPCSTR> fontsToUse;
	// Text for Game
	vector<LPCSTR> gameTextList;
	// Game Sounds
	vector<LPCSTR> soundList;
	vector<soundType> soundTypes;
	vector<LPCSTR> soundsToUse;
	// Create vector array of button textures
	vector<LPCSTR> buttonNameList;
	vector<LPCSTR> btnTexturesToUse;
	vector<SDL_Point> btnPos;
	vector <cButton> theButtons;
	// Game objects
	// Define the elements and there position in/on the array/map
	int renderWidth, renderHeight;
	gameState theGameState;
	SDL_Point theAreaClicked;
	btnTypes theBtnType;
	SDL_Rect pos;
	FPoint scale;
	SDL_Rect aRect;
	SDL_Color aColour;
	cTexture* tempTextTexture;
	// Define the score and lives
	int theScore;
	string strScore;

	int theLives;
	string strLives;
	// Set the booleans for updating text
	bool updateScore = false;
	bool spaceHeldDown = false;
	bool updateLives = false;
};

#endif
