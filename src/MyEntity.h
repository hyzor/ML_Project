#ifndef MYENTITY_H_
#define MYENTITY_H_

#include <SDL/SDL.h>
#include <Box2D/Box2D.h>

#include "SDL_Wrapper.h"
#include "Common.h"

class MyEntity
{
public:
	static enum Behaviours
	{
		SHOOTFREQ
	};

	static enum MovementPatterns
	{
		ZIGZAG,
		CIRCLE,
		LINE
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

	void ActivateEventTrigger(Events movement, bool activate);

	void Update(float dt);
	void Draw(SDL_Renderer* renderer);

	b2Body* Getb2Body() const;
	float GetAngle() const;
	float GetAngle_Degrees() const;

protected:
	float mVelocity;
	int mHealth;
	float mPosX, mPosY;
	float mAngle;
	int mDmg;
	int mWidth, mHeight;

private:
	SDL_Rect* mSdlRect;
	SDL_Wrapper::Texture* mTexture;
	b2Body* mb2Body;

	bool mEventTriggers[Events::NUM_EVENTS];
};

#endif