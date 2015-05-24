#ifndef COMMON_H_
#define COMMON_H_

/*
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG
*/

struct vec2f
{
	vec2f(float _x, float _y)
	{
		this->x = _x;
		this->y = _y;
	}

	float x, y;
};

struct vec2
{
	vec2()
	{
		x = 0;
		y = 0;
	}

	vec2(int _x, int _y)
	{
		this->x = _x;
		this->y = _y;
	}

	int x, y;
};

namespace MathHelper
{
	static const float PI = 3.14159265359f;

	static float RadiansToDegrees(float radians)
	{
		return radians * 57.2957795f;
	}

	static float DegreesToRadians(float degrees)
	{
		return degrees * 0.0174532925f;
	}

	static double RadiansToDegrees_Double(double radians)
	{
		return radians * 57.2957795;
	}

	static double DegreesToRadians_Double(double degrees)
	{
		return degrees * 0.0174532925;
	}
}

namespace Box2dHelper
{
	static const float PixelsPerMeter = 30.0f;
	static const float Units = 1.0f / PixelsPerMeter;

	static const int b2VelIterations = 6;
	static const int b2PosIterations = 2;
}

#endif