#ifndef DEBUGDRAW_H_
#define DEBUGDRAW_H_

#include <SDL/SDL.h>
#include <Box2D/Box2D.h>
#include <SDL_gfx/SDL2_gfxPrimitives.h>

#include "Common.h"

class DebugDraw : public b2Draw
{
public:
	DebugDraw(SDL_Renderer* renderer, Uint8 alpha = 150);
	~DebugDraw();

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	void DrawTransform(const b2Transform& xf);

private:
	void Polygon(int(*f)(SDL_Renderer*, const Sint16*, const Sint16*, int, Uint8, Uint8, Uint8, Uint8), const b2Vec2 *vertices, int32 vertexCount, const b2Color &color, const Uint8 a);
	void Circle(int(*f)(SDL_Renderer*, Sint16, Sint16, Sint16, Uint8, Uint8, Uint8, Uint8), const b2Vec2 &center, float32 radius, const b2Color &color, const Uint8 a);

	SDL_Renderer* mSdlRenderer;
	Uint8 mAlpha;
};

#endif