#include "SDL_Wrapper.h"

SDL_Surface* SDL_Wrapper::LoadSurface(std::string path, SDL_Surface* screenSurface)
{
	SDL_Surface* optSurface = NULL;

	SDL_Surface* rawSurface = SDL_LoadBMP(path.c_str());

	if (!rawSurface)
	{
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}
	else
	{
		// Convert surface to screen format
		optSurface = SDL_ConvertSurface(rawSurface, screenSurface->format, NULL);

		if (!optSurface)
		{
			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		SDL_FreeSurface(rawSurface);
	}

	return optSurface;
}

SDL_Wrapper::Texture::Texture()
{
	mTexture = NULL;
	mWidth = mHeight = 0;
}

SDL_Wrapper::Texture::~Texture()
{
	Free();
}

bool SDL_Wrapper::Texture::LoadFromFile(std::string path, SDL_Renderer* renderer)
{
	Free();

	SDL_Texture* texture = NULL;

	SDL_Surface* surface = IMG_Load(path.c_str());

	if (surface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0xFF, 0xFF));

		texture = SDL_CreateTextureFromSurface(renderer, surface);

		if (texture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			mWidth = surface->w;
			mHeight = surface->h;
		}

		SDL_FreeSurface(surface);
	}

	mTexture = texture;

	return mTexture != NULL;
}

bool SDL_Wrapper::Texture::CreateFromText(std::string text, SDL_Color color, TTF_Font* font, SDL_Renderer* renderer)
{
	Free();

	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);

	if (!surface)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		mTexture = SDL_CreateTextureFromSurface(renderer, surface);

		if (!mTexture)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			mWidth = surface->w;
			mHeight = surface->h;
		}

		SDL_FreeSurface(surface);
	}

	return mTexture != NULL;
}

void SDL_Wrapper::Texture::Render(int x, int y, SDL_Renderer* renderer, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip);
}

void SDL_Wrapper::Texture::Free()
{
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}