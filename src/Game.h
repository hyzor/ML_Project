#ifndef GAME_H_
#define GAME_H_

#include <string>

#include <ga/ga.h>
#include <SDL/SDL.h>
#include <SDL_ttf/SDL_ttf.h>
#include <Box2D/Box2D.h>

#include "MyGA.h"
#include "Ship.h"
#include "SDL_Wrapper.h"
#include "World.h"
#include "TextureManager.h"
#include "MyB2DebugDraw.h"
#include "MyB2ContactListener.h"
#include "MyGenome.h"

class Game
{
public:
	Game();
	Game(SDL_Window* sdlWindow, SDL_Renderer* sdlRenderer, unsigned int screenW, unsigned int screenH,
		b2World* _b2World, TextureManager* textureManager, TTF_Font* mainFont);
	~Game();

	bool Init(std::string assetsDir, std::string fontsDir, std::string texturesDir);

	void Update(float dt);
	void Draw();

	Ship* GetPlayerShip();
	World* GetWorld() const;

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
	MyGA* mGA;

	// Graphics
	SDL_Wrapper::Texture* mBackground;
	SDL_Wrapper::Texture* mDebugText;
	TTF_Font* mMainFont;

	Ship* mPlayerShip;
	std::string mAssetsDir;
	std::string mFontsDir;
	std::string mTexturesDir;
};

#endif