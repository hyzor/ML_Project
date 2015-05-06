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
	Entity();
	Entity(float x, float y, int width, int height, int health, int damage, float angle, SDL_Wrapper::Texture* texture, b2World* world);
	virtual ~Entity();

	bool Init();

	b2Body* Getb2Body() const;

	b2Vec2 GetPosition(bool inMeters) const;
	float GetAngle(bool inRadians) const;
	b2Vec2 GetLinearVelocity() const;
	float GetAngularVelocity() const;

	bool IsAlive() const;

	b2Vec2 GetDimensions(bool inMeters) const;

	void Draw(SDL_Renderer* renderer);
	virtual void Update(float dt);
	void Reset();

	void DoCollide(int collisionDamage);
	int GetCollisionDamage() const;

protected:
	SDL_Rect* mSdlClipRect;
	SDL_Point* mSdlCenterPoint;
	SDL_Wrapper::Texture* mTexture;
	b2Body* mb2Body;

	b2Vec2 mb2LocalInitVec;

	float mSpawnPosX, mSpawnPosY;
	float mWidth, mHeight;
	int mHealth;
	int mDamage;

	bool mIsAlive;
};

#endif