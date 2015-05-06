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

	b2Vec2 forceDirection = mb2Body->GetWorldVector(mb2LocalInitVec);
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

	if (mEventTriggers[Events::STRAFE_LEFT])
	{
		float dirX = std::cos((GetAngle(true) + MathHelper::DegreesToRadians(90.0f)) - 4.7f);
		float dirY = std::sin((GetAngle(true) + MathHelper::DegreesToRadians(90.0f)) - 4.7f);

		b2Vec2 force = b2Vec2(dirX*mMagnitude, dirY*mMagnitude);
		mb2Body->ApplyForce(force, mb2Body->GetWorldCenter(), true);
	}

	if (mEventTriggers[Events::STRAFE_RIGHT])
	{
		float dirX = std::cos((GetAngle(true) - MathHelper::DegreesToRadians(90.0f)) - 4.7f);
		float dirY = std::sin((GetAngle(true) - MathHelper::DegreesToRadians(90.0f)) - 4.7f);

		b2Vec2 force = b2Vec2(dirX*mMagnitude, dirY*mMagnitude);
		mb2Body->ApplyForce(force, mb2Body->GetWorldCenter(), true);
	}

	if (mEventTriggers[Events::STABILIZE])
	{
		mb2Body->SetLinearDamping(5.0f);
		mb2Body->SetAngularDamping(5.0f);
	}

	if (mEventTriggers[Events::SHOOT])
	{
		// Try to shoot
		Shoot();
	}
}

void Ship::Init()
{
	mTorque = 30.0f;
	mMagnitude = 20.0f;
	mCooldown = 0.15f;
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
	toTarget.Normalize();
	float desiredAngle = atan2f(-toTarget.x, toTarget.y);
	float totalRotation = desiredAngle - mb2Body->GetAngle();
	float newAngle = mb2Body->GetAngle() + std::min(degreesPerStep, std::max(-degreesPerStep, totalRotation));
	mb2Body->SetTransform(mb2Body->GetPosition(), newAngle);
}

void Ship::RotateTo_Torque(b2Vec2 point, float dt)
{
	float bodyAngle = GetAngle(true);

	b2Vec2 toTarget = point - GetPosition(true);
	toTarget.Normalize();
	float desiredAngle = atan2f(toTarget.x, -toTarget.y);

	float nextAngle = bodyAngle + GetAngularVelocity() / 2.0f;
	float totalRotation = desiredAngle - nextAngle;

	while (totalRotation < MathHelper::DegreesToRadians(-180)) totalRotation += MathHelper::DegreesToRadians(360);
	while (totalRotation > MathHelper::DegreesToRadians(180)) totalRotation -= MathHelper::DegreesToRadians(360);

	float desiredAngularVelocity = totalRotation * 60.0f;
	float torque = mb2Body->GetInertia() * desiredAngularVelocity / (dt);
	mb2Body->ApplyTorque(totalRotation < 0 ? -mTorque : mTorque, true);
}

void Ship::MoveTo(b2Vec2 point, float radius, float dt)
{
	b2Vec2 pos = GetPosition(true);
	float dist = sqrt(std::pow(point.x - pos.x, 2) + std::pow(point.y - pos.y, 2));

	if (dist <= radius)
	{
		//ActivateEventTrigger(Events::THRUST_FORWARD, false);

		mb2Body->SetLinearDamping(5.0f);
		mb2Body->SetAngularDamping(5.0f);
	}
	else
	{
		//ActivateEventTrigger(Events::THRUST_FORWARD, true);
		RotateTo_Torque(point, dt);

		mb2Body->SetLinearDamping(0.0f);
		mb2Body->SetAngularDamping(0.0f);
	}
}

bool Ship::Shoot()
{
	bool successful = false;

	if (mCurCooldown <= 0.0f)
	{
		int width, height;
		width = height = 4;

		// Spawn new projectile
		Projectile* newProjectile = World::GetInstance()->SpawnEntity<Projectile>(
			GetPosition(false).x - (GetDimensions(false).y*0.50f)*std::cos(GetAngle(true) - 4.7f) - ((float)height*0.7f)*std::cos(GetAngle(true) - 4.7f),
			GetPosition(false).y - (GetDimensions(false).y*0.50f)*std::sin(GetAngle(true) - 4.7f) - ((float)height*0.7f)*std::sin(GetAngle(true) - 4.7f),
			width, height, 1, 1, GetAngle(true), TextureManager::GetInstance()->LoadTexture("Projectile.png"));
		newProjectile->Init(2.0f);
		mCurCooldown = mCooldown;

		successful = true;
	}

	return successful;
}