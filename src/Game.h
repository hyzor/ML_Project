#ifndef GAME_H_
#define GAME_H_

#include <string>

#include <ga/ga.h>
#include <SDL/SDL.h>
#include <SDL_ttf/SDL_ttf.h>
#include <Box2D/Box2D.h>

#include "Ship.h"
#include "SDL_Wrapper.h"
#include "World.h"
#include "TextureManager.h"
#include "MyB2DebugDraw.h"
#include "MyB2ContactListener.h"
#include "MyGenome.h"
#include "Common.h"

class MyGA;
class MyGA2;
class Game
{
public:
	Game();
	Game(SDL_Window* sdlWindow, SDL_Renderer* sdlRenderer, unsigned int screenW, unsigned int screenH,
		b2World* _b2World, TextureManager* textureManager, TTF_Font* mainFont);
	~Game();

	bool Init(std::string assetsDir, std::string fontsDir, std::string texturesDir, double dt_fixed);

	void DoPhysicsStep(float dt, int b2VelIterations, int b2PosIterations);
	void UpdateWorld(float dt);
	void Draw(double alpha);

	void DoPostProcessing();

	Ship* GetPlayerShip();
	World* GetWorld() const;

	void Reset();

	void RunGA(float dt);

private:
	SDL_Window* mSDL_Window;
	SDL_Event mSDL_Event;
	SDL_Renderer* mSDL_Renderer;

	TextureManager* mTextureManager;

	// Box2d
	b2World* mB2World;
	int mB2VelIterations;
	int mB2PosIterations;

	unsigned int mScreenWidth, mScreenHeight;

	World* mWorld;

	// GA
	GAPopulation* mGaPop;
	MyGA2* mGA;

	// Graphics
	SDL_Wrapper::Texture* mBackground;
	TTF_Font* mMainFont;

	Ship* mPlayerShip;
	std::string mAssetsDir;
	std::string mFontsDir;
	std::string mTexturesDir;
};

#endif