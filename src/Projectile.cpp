#include "Projectile.h"


Projectile::Projectile()
	: Entity()
{
}


Projectile::Projectile(float x, float y, int width, int height, int health, int damage, float initAngle, SDL_Wrapper::Texture* texture, b2World* world)
	: Entity(x, y, width, height, health, damage, initAngle, texture, world)
{
	mInitAngle = initAngle;
	mb2Body->SetBullet(true);

	//mb2Body->SetLinearVelocity(initVel);
}

Projectile::~Projectile()
{
}

void Projectile::Init(float lifetime)
{
	mLifetime = lifetime;
	mAlivetimer = 0.0f;

	float magnitude = 0.65f;

	//mInitVelocity = velocity;
	//mb2Body->SetLinearVelocity(velocity);

	//b2Vec2 forceDirection = mb2Body->GetWorldVector(b2Vec2(0, 1));

	b2Vec2 force = b2Vec2(-(std::cos(GetAngle(true) - 4.7f)), -(std::sin(GetAngle(true) - 4.7f)));
	force.Normalize();
	force *= magnitude;

	mb2Body->ApplyLinearImpulse(force, mb2Body->GetWorldCenter(), true);
}

void Projectile::Update(float dt)
{
	Entity::Update(dt);

	if (mAlivetimer >= mLifetime)
	{
		mIsAlive = false;
	}

	mAlivetimer += dt;
	//b2Vec2 stepVelocity = dt * mInitVelocity;

	//b2Vec2 forceDirection = mb2Body->GetWorldVector(b2Vec2(0, 1));
	//forceDirection *= 4.0f;
	//mb2Body->ApplyForce(-forceDirection, mb2Body->GetWorldCenter(), true);

	//b2Vec2 force = b2Vec2((std::cos(GetAngle(true))), (std::sin(GetAngle(true))));
	//mb2Body->ApplyLinearImpulse(force, mb2Body->GetWorldCenter(), true);
}
