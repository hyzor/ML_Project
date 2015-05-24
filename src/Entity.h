#ifndef ENTITY_H_
#define ENTITY_H_

#include <SDL/SDL.h>
#include <Box2D/Box2D.h>

#include "SDL_Wrapper.h"
#include "Common.h"
#include "TextureManager.h"

class Entity
{
public:
	static enum Type
	{
		PROJECTILE = 0x0001,
		SHIP = 0x0002,
		SENSOR = 0x0004,
		STATIC = 0x0008
	};

public:
	Entity();
	Entity(float x, float y, int width, int height, int health, int damage, float angle, bool isImmovable, SDL_Wrapper::Texture* texture);
	Entity(float x, float y, int width, int height, int health, int damage, float angle, SDL_Wrapper::Texture* texture);
	virtual ~Entity();

	b2Body* Getb2Body() const;

	b2Vec2 GetPosition(bool inMeters) const;
	float GetAngle(bool inRadians) const;
	float GetAngle_NonRetarded(bool inRadians) const;
	b2Vec2 GetLinearVelocity() const;
	float GetAngularVelocity() const;

	int GetHealth() const;
	int GetHealth_Init() const;

	void SetPosition(b2Vec2 pos);

	bool IsAlive() const;

	b2Vec2 GetDimensions(bool inMeters) const;

	virtual void Draw(SDL_Renderer* renderer, double alpha);
	virtual void Update(float dt);
	void Reset();

	void SetPrevPos(b2Vec2 prevPos);
	void SetPrevAngle(float angle);

	void DoCollide(int collisionDamage);
	int GetCollisionDamage() const;

	virtual bool Init_b2(b2World* world, bool isBullet, unsigned int type);
	virtual bool Init_SDL();
	void SetType(unsigned int Type);
	unsigned int GetType() const;

	void SetCollisionEnabled(bool enabled);
	void Setb2BodyType(b2BodyType bodyType);

protected:
	SDL_Rect* mSdlClipRect;
	SDL_Point* mSdlCenterPoint;
	SDL_Wrapper::Texture* mTexture;
	b2Body* mb2Body;

	b2Vec2 mb2LocalInitVec;

	float mPrevPosX, mPrevPosY;
	float mSpawnPosX, mSpawnPosY;
	float mWidth, mHeight;
	int mHealth;
	int mDamage;

	double mPrevAngle;

	int mInitialHealth;

	bool mIsAlive;
	bool mIsImmovable;

	float mInitAngle;

	b2Fixture* mb2Fixture;

	unsigned int mType;
};

#endif