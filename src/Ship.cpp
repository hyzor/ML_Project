#include "Ship.h"

Ship::Ship()
	: Entity()
{
	Init();
}

Ship::Ship(float x, float y, int width, int height, int health, int damage, float angle, SDL_Wrapper::Texture* texture, b2World* world)
	: Entity(x, y, width, height, health, damage, angle, texture, world)
{
	Init();
}

void Ship::Update(float dt)
{
	Entity::Update(dt);

	if (mCurCooldown > 0.0f)
		mCurCooldown -= dt;

	b2Vec2 forceDirection = mb2Body->GetWorldVector(b2Vec2(0, 1));
	forceDirection *= mMagnitude;

	if (mEventTriggers[Events::THRUST_FORWARD])
	{
		mb2Body->ApplyForce(-forceDirection, mb2Body->GetWorldCenter(), true);
	}

	if (mEventTriggers[Events::THRUST_BACKWARD])
	{
		mb2Body->ApplyForce(forceDirection, mb2Body->GetWorldCenter(), true);
	}

	if (mEventTriggers[Events::TORQUE_LEFT])
	{
		mb2Body->ApplyTorque(-mTorque, true);
	}

	if (mEventTriggers[Events::TORQUE_RIGHT])
	{
		mb2Body->ApplyTorque(mTorque, true);
	}

	if (mEventTriggers[Events::SHOOT])
	{
		// Try to shoot
		Shoot();
	}
}

void Ship::Init()
{
	mTorque = 4000000.0f;
	mMagnitude = 40000.0f;
	mCooldown = 0.5f;
	mCurCooldown = 0.0f;
}

Ship::~Ship()
{}

void Ship::ActivateEventTrigger(Events movement, bool activate)
{
	mEventTriggers[movement] = activate;
}

void Ship::RotateTo(b2Vec2 point, float degreesPerStep)
{
	b2Vec2 toTarget = point - mb2Body->GetPosition();
	float desiredAngle = atan2f(-toTarget.x, toTarget.y);
	float totalRotation = desiredAngle - mb2Body->GetAngle();
	float newAngle = mb2Body->GetAngle() + std::min(degreesPerStep, std::max(-degreesPerStep, totalRotation));
	mb2Body->SetTransform(mb2Body->GetPosition(), newAngle);
}

void Ship::RotateTo_Torque(b2Vec2 point, float dt)
{
	float bodyAngle = GetAngle(true);

	b2Vec2 toTarget = point - GetPosition();
	float desiredAngle = atan2f(toTarget.x, -toTarget.y);

	float nextAngle = bodyAngle + GetAngularVelocity() / 6.0f;
	float totalRotation = desiredAngle - nextAngle;

	while (totalRotation < MathHelper::DegreesToRadians(-180)) totalRotation += MathHelper::DegreesToRadians(360);
	while (totalRotation > MathHelper::DegreesToRadians(180)) totalRotation -= MathHelper::DegreesToRadians(360);

	float desiredAngularVelocity = totalRotation * 60.0f;
	float torque = mb2Body->GetInertia() * desiredAngularVelocity / (dt);
	mb2Body->ApplyTorque(totalRotation < 0 ? -mTorque : mTorque, true);
}

void Ship::MoveTo(b2Vec2 point, float radius, float dt)
{
	b2Vec2 pos = GetPosition();
	float dist = sqrt(std::pow(point.x - pos.x, 2) + std::pow(point.y - pos.y, 2));

	if (dist <= radius)
	{
		ActivateEventTrigger(Events::THRUST_FORWARD, false);

		mb2Body->SetLinearDamping(5.0f);
		mb2Body->SetAngularDamping(5.0f);
	}
	else
	{
		ActivateEventTrigger(Events::THRUST_FORWARD, true);
		RotateTo_Torque(point, dt);
	}
}

bool Ship::Shoot()
{
	bool successful = false;

	if (mCurCooldown <= 0.0f)
	{
		// Spawn new projectile
		Projectile* newProjectile = World::GetInstance()->SpawnEntity<Projectile>(GetPosition().x, GetPosition().y, 4, 4, 1, 1, GetAngle(true), mTexture);
		//newProjectile->Init()
		mCurCooldown = mCooldown;

		successful = true;
	}

	return successful;
}