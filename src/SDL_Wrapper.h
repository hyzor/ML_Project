#ifndef SDL_WRAPPER_H_
#define SDL_WRAPPER_H_

#include <SDL/SDL.h>
#include <SDL_image/SDL_image.h>
#include <SDL_ttf/SDL_ttf.h>
#include <iostream>

namespace SDL_Wrapper
{
	SDL_Surface* LoadSurface(std::string path, SDL_Surface* surface);

	class Texture
	{
	public:
		Texture();
		~Texture();

		bool CreateFromText(std::string text, SDL_Color color, TTF_Font* font, SDL_Renderer* renderer);
		bool LoadFromFile(std::string path, SDL_Renderer* renderer);
		void Free();
		void Render(int x, int y, SDL_Renderer* renderer, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	private:
		SDL_Texture* mTexture;
		int mWidth, mHeight;
	};
};

#endif