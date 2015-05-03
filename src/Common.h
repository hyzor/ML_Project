#ifndef COMMON_H_
#define COMMON_H_

struct vec2f
{
	vec2f(float _x, float _y)
	{
		this->x = _x;
		this->y = _y;
	}

	float x, y;
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
}

#endif