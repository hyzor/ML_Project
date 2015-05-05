#include "TextureManager.h"

TextureManager* TextureManager::mInstance = nullptr;


TextureManager::TextureManager()
{
}

void TextureManager::Init(SDL_Renderer* renderer, std::string texturesPath)
{
	mSdlRenderer = renderer;
	mTexturesPath = texturesPath;
}

TextureManager* TextureManager::GetInstance()
{
	if (!mInstance)
	{
		mInstance = new TextureManager();
	}

	return mInstance;
}

void TextureManager::Destroy()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

SDL_Wrapper::Texture* TextureManager::LoadTexture(std::string fileName)
{
	if (mTexturesMap[fileName] == nullptr)
	{
		SDL_Wrapper::Texture* texture = new SDL_Wrapper::Texture();
		texture->LoadFromFile(mTexturesPath + fileName, mSdlRenderer);
		mTexturesMap[fileName] = texture;
	}

	return mTexturesMap[fileName];
}

TextureManager::~TextureManager()
{
	for (auto it : mTexturesMap)
	{
		if (it.second)
			delete it.second;
	}

	mTexturesMap.clear();
}
