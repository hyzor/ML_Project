#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include <vector>
#include <map>

#include <SDL/SDL.h>

#include "SDL_Wrapper.h"

class TextureManager
{
public:
	static TextureManager* GetInstance();

	void Init(SDL_Renderer* renderer, std::string texturesPath);
	static void Destroy();

	SDL_Wrapper::Texture* LoadTexture(std::string fileName);

private:
	TextureManager();
	~TextureManager();

	static TextureManager* mInstance;

	//std::vector<SDL_Wrapper::Texture*> mTextures;
	std::map<std::string, SDL_Wrapper::Texture*> mTexturesMap;
	SDL_Renderer* mSdlRenderer;
	std::string mTexturesPath;
};

#endif