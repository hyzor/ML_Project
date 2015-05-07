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

	ProcessWaypoints(dt);

	if (mCurCooldown > 0.0f)
		mCurCooldown -= dt;

	//b2Vec2 forceDirection = mb2Body->GetWorldVector(mb2LocalInitVec);
	//forceDirection *= mMagnitude;

	float dirX = std::cos((GetAngle_NonRetarded(true)));
	float dirY = std::sin((GetAngle_NonRetarded(true)));

	b2Vec2 forceDirection = b2Vec2(dirX, dirY);
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
		float dirX = std::cos((GetAngle_NonRetarded(true) + MathHelper::DegreesToRadians(90.0f)));
		float dirY = std::sin((GetAngle_NonRetarded(true) + MathHelper::DegreesToRadians(90.0f)));

		b2Vec2 force = b2Vec2(dirX*mMagnitude, dirY*mMagnitude);
		mb2Body->ApplyForce(force, mb2Body->GetWorldCenter(), true);
	}

	if (mEventTriggers[Events::STRAFE_RIGHT])
	{
		float dirX = std::cos((GetAngle_NonRetarded(true) - MathHelper::DegreesToRadians(90.0f)));
		float dirY = std::sin((GetAngle_NonRetarded(true) - MathHelper::DegreesToRadians(90.0f)));

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

float Ship::RotateTo_Torque(b2Vec2 point, float dt)
{
	float bodyAngle = GetAngle(true);

	b2Vec2 toTarget = point - GetPosition(true);
	toTarget.Normalize();
	float desiredAngle = atan2f(toTarget.x, -toTarget.y);

	float nextAngle = bodyAngle + GetAngularVelocity() / 2.0f;
	float totalRotation = desiredAngle - nextAngle;

	//while (totalRotation < MathHelper::DegreesToRadians(-180)) totalRotation += MathHelper::DegreesToRadians(360);
	//while (totalRotation > MathHelper::DegreesToRadians(180)) totalRotation -= MathHelper::DegreesToRadians(360);

	totalRotation = ConstrainAngle180(totalRotation);

	float desiredAngularVelocity = totalRotation * 60.0f;
	float torque = mb2Body->GetInertia() * desiredAngularVelocity / (dt);
	mb2Body->ApplyTorque(totalRotation < 0 ? -mTorque : mTorque, true);

	return desiredAngle;
}

void Ship::MoveTo(b2Vec2 point, float radius, float dt)
{
	b2Vec2 pos = GetPosition(true);
	float dist = sqrt(std::pow(point.x - pos.x, 2) + std::pow(point.y - pos.y, 2));

	if (dist <= radius + (mb2Body->GetLinearVelocity()).Length())
	{
		ActivateEventTrigger(Events::THRUST_FORWARD, false);

		mb2Body->SetLinearDamping(5.0f);
		mb2Body->SetAngularDamping(5.0f);

		if (dist <= radius * 3.0f)
		{
			mReachedCurWaypoint = true;
		}
	}
	else
	{
		//ActivateEventTrigger(Events::THRUST_FORWARD, true);

		b2Vec2 destination = GetCorrectionWaypoint(point);

		std::cout << "Dest x: " << destination.x << "\n";
		std::cout << "Dest y: " << destination.y << "\n";

		float desiredAngle = RotateTo_Torque(destination, dt);

		if (std::abs(desiredAngle - ConstrainAngle180(GetAngle(true))) < MathHelper::DegreesToRadians(20.0f))
		{
			ActivateEventTrigger(Events::THRUST_FORWARD, true);
			mb2Body->SetLinearDamping(0.0f);

		}
		else
		{
			ActivateEventTrigger(Events::THRUST_FORWARD, false);
			mb2Body->SetLinearDamping(0.8f);
		}

		//RotateTo_Torque(destination, dt);

		//mb2Body->SetLinearDamping(0.0f);
		mb2Body->SetAngularDamping(0.0f);

		//mReachedCurWaypoint = false;
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
			GetPosition(false).x - (GetDimensions(false).y*0.50f)*std::cos(GetAngle_NonRetarded(true)) - ((float)height*0.7f)*std::cos(GetAngle_NonRetarded(true)),
			GetPosition(false).y - (GetDimensions(false).y*0.50f)*std::sin(GetAngle_NonRetarded(true)) - ((float)height*0.7f)*std::sin(GetAngle_NonRetarded(true)),
			width, height, 1, 1, GetAngle(true), TextureManager::GetInstance()->LoadTexture("Projectile.png"));
		newProjectile->Init(2.0f);
		mCurCooldown = mCooldown;

		successful = true;
	}

	return successful;
}

b2Vec2 Ship::GetCorrectionWaypoint(b2Vec2 waypoint)
{
	b2Vec2 selfVel = mb2Body->GetLinearVelocity();
	b2Vec2 selfPos = GetPosition(true);

	b2Vec2 desiredVel = waypoint - selfPos;
	b2Vec2 steeringCorrection = desiredVel - selfVel;

	return waypoint + steeringCorrection;
}

float Ship::ConstrainAngle180(float angle)
{
	while (angle < MathHelper::DegreesToRadians(-180)) angle += MathHelper::DegreesToRadians(360);
	while (angle > MathHelper::DegreesToRadians(180)) angle -= MathHelper::DegreesToRadians(360);

	return angle;
}

void Ship::AddWaypoint(b2Vec2 waypoint)
{
	mWaypoints.push_back(waypoint);
}

void Ship::ProcessWaypoints(float dt)
{
	if (mWaypoints.size() > 0)
	{
		if (mReachedCurWaypoint)
		{
			mWaypoints.pop_front();
			mReachedCurWaypoint = false;
		}
	}

	if (mWaypoints.size() > 0)
	{
		ActivateEventTrigger(Events::STABILIZE, false);
		MoveTo(mWaypoints.front(), 40.0f*Box2dHelper::Units, dt);
	}
	else
	{
		ActivateEventTrigger(Events::STABILIZE, true);
	}
}

b2Vec2 Ship::GetCurrentWaypoint() const
{
	if (mWaypoints.size() > 0)
		return mWaypoints.front();
	else
		return b2Vec2(-1.0f, -1.0f);
}

void Ship::AddMovementPattern(int movementPattern, bool immediate)
{
	switch (movementPattern)
	{
	case Attributes::MOVEMENT_CIRCLE:
		break;
	case Attributes::MOVEMENT_ZIGZAG:
		if (immediate)
			mWaypoints.clear();

		b2Vec2 curPos = GetPosition(true);
		float angle = GetAngle_NonRetarded(true);
		//b2Vec2 curDir = mb2Body->GetWorldVector(mb2LocalInitVec);
		//float angle = std::atan2f(curDir.x, -curDir.y);
		//angle += MathHelper::DegreesToRadians(45.0f);

		b2Vec2 newWaypoint;
		newWaypoint.x = -((std::cos(angle))*10.0f + curPos.x);
		newWaypoint.y = (std::sin(angle))*10.0f + curPos.y;
		AddWaypoint(newWaypoint);

		//angle -= MathHelper::DegreesToRadians(90.0f);

		/*
		b2Vec2 newWaypoint2;
		newWaypoint2.x = -((std::cos(angle)) + newWaypoint.x);
		newWaypoint2.y = ((std::sin(angle)) + newWaypoint.y);
		AddWaypoint(newWaypoint2);
		*/

		break;
	}
}