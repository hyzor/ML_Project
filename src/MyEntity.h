#ifndef MYENTITY_H_
#define MYENTITY_H_

#include <cmath>

#include <SDL/SDL.h>
#include <Box2D/Box2D.h>

#include "SDL_Wrapper.h"
#include "Common.h"

class MyEntity
{
public:
	static enum Attributes
	{
		SHOOTFREQ,
		MOVEMENT_ZIGZAG,
		MOVEMENT_CIRCLE,
		MOVEMENT_LINE
	};

	static enum Events
	{
		THRUST_FORWARD,
		THRUST_BACKWARD,
		TORQUE_LEFT,
		TORQUE_RIGHT,
		SHOOT,
		NUM_EVENTS
	};

public:
	MyEntity();
	MyEntity(float xPos, float yPos, int width, int height, int hp, int dmg, SDL_Wrapper::Texture& texture, b2World* world);
	~MyEntity();

	void ActivateEventTrigger(Events event, bool activate);

	void Update(float dt);
	void Draw(SDL_Renderer* renderer);

	b2Body* Getb2Body() const;
	float GetAngle() const;
	float GetAngle_Degrees() const;

	void RotateTo(b2Vec2 point, float degreesPerStep);
	void RotateTo_Torque(b2Vec2 point, float dt);

	void MoveTo(b2Vec2 point, float radius, float dt);

protected:
	float mVelocity;
	int mHealth;
	float mPosX, mPosY;
	float mAngle;
	int mDmg;
	int mWidth, mHeight;

	float mTorque;
	float mMagnitude;

private:
	SDL_Rect* mSdlRect;
	SDL_Wrapper::Texture* mTexture;
	b2Body* mb2Body;

	bool mEventTriggers[Events::NUM_EVENTS];
};

#endif