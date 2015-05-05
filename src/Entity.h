#ifndef ENTITY_H_
#define ENTITY_H_

#include <SDL/SDL.h>
#include <Box2D/Box2D.h>

#include "SDL_Wrapper.h"
#include "Common.h"

class Entity
{
public:
	Entity();
	Entity(float x, float y, int width, int height, int health, int damage, float angle, SDL_Wrapper::Texture* texture, b2World* world);
	virtual ~Entity();

	bool Init();

	b2Vec2 GetPosition() const;
	float GetAngle(bool radians) const;
	b2Vec2 GetLinearVelocity() const;
	float GetAngularVelocity() const;

	void Draw(SDL_Renderer* renderer);
	virtual void Update(float dt);
	void Reset();

protected:
	SDL_Rect* mSdlRect;
	SDL_Wrapper::Texture* mTexture;
	b2Body* mb2Body;

	b2Vec2 mLocalVec;

	float mSpawnPosX, mSpawnPosY;
	int mWidth, mHeight;
	int mHealth;
	int mDamage;

	bool mIsAlive;
};

#endif