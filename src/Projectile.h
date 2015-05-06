#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include <Box2D/Box2D.h>

#include "Entity.h"

class Projectile : public Entity
{
public:
	Projectile();
	Projectile(float x, float y, int width, int height, int health, int damage, float initAngle, SDL_Wrapper::Texture* texture, b2World* world);
	~Projectile();

	void Init(float lifetime);
	void Update(float dt);

private:
	b2Vec2 mInitVelocity;
	float mInitAngle;

	float mLifetime;
	float mAlivetimer;
};

#endif