//===========================================================================
// Most of the code is directly taken from:
// https://github.com/Deraen/ohj2710
// ( https://github.com/Deraen/ohj2710/blob/master/jni/src/DebugDraw.cpp )
// The original author of this code is therefore Juho Teperi.
// Some slight modifications have been made.
//===========================================================================

#include "MyB2DebugDraw.h"


MyB2DebugDraw::MyB2DebugDraw(SDL_Renderer* renderer, Uint8 alpha)
{
	mSdlRenderer = renderer;
	mAlpha = alpha;
}


MyB2DebugDraw::~MyB2DebugDraw()
{
}

void MyB2DebugDraw::Polygon(int(*f)(SDL_Renderer*, const Sint16*, const Sint16*, int, Uint8, Uint8, Uint8, Uint8), const b2Vec2 *vertices, int32 vertexCount, const b2Color &color, const Uint8 a)
{
	Uint8 r = Uint8(color.r * 255);
	Uint8 g = Uint8(color.g * 255);
	Uint8 b = Uint8(color.b * 255);

	Sint16* vx = new Sint16[vertexCount];
	Sint16* vy = new Sint16[vertexCount];
	for (unsigned int i = 0; i < (unsigned int)vertexCount; ++i)
	{
		b2Vec2 pixels = vertices[i];
		pixels *= Box2dHelper::PixelsPerMeter;
		Sint16 x = static_cast<Sint16>(pixels.x);
		Sint16 y = static_cast<Sint16>(pixels.y);
		vx[i] = x;
		vy[i] = y;
	}

	f(mSdlRenderer, vx, vy, vertexCount, r, g, b, a);
	delete[] vx;
	delete[] vy;
}

void MyB2DebugDraw::Circle(int(*f)(SDL_Renderer*, Sint16, Sint16, Sint16, Uint8, Uint8, Uint8, Uint8), const b2Vec2 &center, float32 radius, const b2Color &color, const Uint8 a)
{
	Uint8 r = Uint8(color.r * 255);
	Uint8 g = Uint8(color.g * 255);
	Uint8 b = Uint8(color.b * 255);

	b2Vec2 pixels = center;
	pixels *= Box2dHelper::PixelsPerMeter;

	Sint16 radiusPx = static_cast<Sint16>(radius * Box2dHelper::PixelsPerMeter);
	Sint16 x = static_cast<Sint16>(pixels.x);
	Sint16 y = static_cast<Sint16>(pixels.y);

	f(mSdlRenderer, x, y, radiusPx, r, g, b, a);
}

void MyB2DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	Polygon(polygonRGBA, vertices, vertexCount, color, mAlpha);
}

void MyB2DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	Polygon(filledPolygonRGBA, vertices, vertexCount, color, mAlpha);
}

void MyB2DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	Circle(circleRGBA, center, radius, color, mAlpha);
}

void MyB2DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	Circle(filledCircleRGBA, center, radius, color, mAlpha);
}

void MyB2DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	Uint8 r = Uint8(color.r * 255);
	Uint8 g = Uint8(color.g * 255);
	Uint8 b = Uint8(color.b * 255);

	b2Vec2 pixels1 = p1;
	pixels1 *= Box2dHelper::PixelsPerMeter;

	b2Vec2 pixels2 = p2;
	pixels2 *= Box2dHelper::PixelsPerMeter;

	lineRGBA(mSdlRenderer,
		static_cast<Sint16>(pixels1.x),
		static_cast<Sint16>(pixels1.y),
		static_cast<Sint16>(pixels2.x),
		static_cast<Sint16>(pixels2.y), r, g, b, mAlpha);
}

void MyB2DebugDraw::DrawTransform(const b2Transform& xf)
{
	//throw std::logic_error("The method or operation is not implemented.");
}
