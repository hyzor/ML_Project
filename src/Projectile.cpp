#include "Projectile.h"


Projectile::Projectile()
	: Entity()
{
}


Projectile::Projectile(float x, float y, int width, int height, int health, int damage, float initAngle, SDL_Wrapper::Texture* texture, b2World* world)
	: Entity(x, y, width, height, health, damage, initAngle, texture, world)
{
	mInitAngle = initAngle;

	//mb2Body->SetLinearVelocity(initVel);
}

Projectile::~Projectile()
{
}

void Projectile::Init(b2Vec2 velocity)
{
	mInitVelocity = velocity;
	mb2Body->SetLinearVelocity(velocity);
}

void Projectile::Update(float dt)
{
	//b2Vec2 stepVelocity = dt * mInitVelocity;

	b2Vec2 forceDirection = mb2Body->GetWorldVector(b2Vec2(0, 1));
	forceDirection *= 40000.0f;
	mb2Body->ApplyForce(-forceDirection, mb2Body->GetWorldCenter(), true);
}
