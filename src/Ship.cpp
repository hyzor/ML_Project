#include "Ship.h"

Ship::Ship()
	: Entity()
{
	InitShip();
}

Ship::Ship(float x, float y, int width, int height, int health, int damage, float angle, bool isImmovable, SDL_Wrapper::Texture* texture)
	: Entity(x, y, width, height, health, damage, angle, isImmovable, texture)
{
	InitShip();
}

void Ship::Update(float dt)
{
		if (GetPosition(true).y < 0.0f || GetPosition(true).y >(600.0f * Box2dHelper::Units)
			|| GetPosition(true).x < 0.0f || GetPosition(true).x >(800.0f * Box2dHelper::Units))
		{
			if (mWaypoints.size() > 0)
			{
				if (mWaypoints.front().isIntermediate)
					mWaypoints.pop_front();
			}
		}

		Entity::Update(dt);

		if (mCurCooldown > 0.0f)
			mCurCooldown -= dt;

		if (mType == Type::STATIONARY)
		{
			RotateTo_Torque(mTarget, dt);
			Shoot();
		}
		else
		{
			ProcessWaypoints(dt);

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


}

void Ship::InitShip()
{
	mTorque = 30.0f;
	mMagnitude = 20.0f;
	mCooldown = 1.0f;
	mCurCooldown = 0.0f;
	mType = Type::NON_STATIONARY;
}

Ship::~Ship()
{

}

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

	float nextAngle = bodyAngle + GetAngularVelocity() / 4.0f;
	float totalRotation = desiredAngle - nextAngle;

	//while (totalRotation < MathHelper::DegreesToRadians(-180)) totalRotation += MathHelper::DegreesToRadians(360);
	//while (totalRotation > MathHelper::DegreesToRadians(180)) totalRotation -= MathHelper::DegreesToRadians(360);

	totalRotation = ConstrainAngle180(totalRotation);

	float diff = std::abs(desiredAngle - ConstrainAngle180(GetAngle(true)));

	/*
	if (diff < MathHelper::DegreesToRadians(5.0f))
	{
		mb2Body->SetAngularDamping(diff * 100.0f);
	}
	*/

	float desiredAngularVelocity = totalRotation * 60.0f;
	float torque = mb2Body->GetInertia() * desiredAngularVelocity / (dt);
	mb2Body->ApplyTorque(totalRotation < 0 ? -mTorque : mTorque, true);

	return desiredAngle;
}

void Ship::MoveTo(b2Vec2 point, float radius, float dt)
{
	b2Vec2 pos = GetPosition(true);
	float dist = sqrt(std::pow(point.x - pos.x, 2) + std::pow(point.y - pos.y, 2));

	if (dist <= radius*1.0f + (mb2Body->GetLinearVelocity()).Length()*0.5f)
	{
		mb2Body->SetLinearDamping(10.0f);
		mb2Body->SetAngularDamping(0.0f);

		if (dist <= 0.965f*(radius + (mb2Body->GetLinearVelocity()).Length()*0.5f))
		{
			mReachedCurWaypoint = true;
		}
	}
	else
	{
		b2Vec2 destination = GetCorrectionWaypoint(point);

		float desiredAngle = RotateTo_Torque(destination, dt);

		if (std::abs(desiredAngle - ConstrainAngle180(GetAngle(true))) < MathHelper::DegreesToRadians(30.0f))
		{
			ActivateEventTrigger(Events::THRUST_FORWARD, true);
			mb2Body->SetLinearDamping(0.0f);
		}
		else
		{
			ActivateEventTrigger(Events::THRUST_FORWARD, false);
			mb2Body->SetLinearDamping(0.8f);
		}
	}
}

void Ship::MoveToWaypoint(Waypoint waypoint, float radius, float dt)
{
	b2Vec2 pos = GetPosition(true);
	float dist = sqrt(std::pow(waypoint.position.x - pos.x, 2) + std::pow(waypoint.position.y - pos.y, 2));

	if (waypoint.isIntermediate)
		dist = sqrt(std::pow(mWaypoints.at(1).position.x - pos.x, 2) + std::pow(mWaypoints.at(1).position.y - pos.y, 2));

	if (dist <= radius*1.0f + (mb2Body->GetLinearVelocity()).Length()*0.5f)
	{
		mb2Body->SetLinearDamping(15.0f);
		mb2Body->SetAngularDamping(0.0f);

		if (dist <= 0.965f*(radius + (mb2Body->GetLinearVelocity()).Length()*0.5f))
		{
			mReachedCurWaypoint = true;
		}
	}
	else
	{
		b2Vec2 destination = GetCorrectionWaypoint(waypoint.position);
		//destination = waypoint.position;

		float desiredAngle = RotateTo_Torque(destination, dt);

		if (std::abs(desiredAngle - ConstrainAngle180(GetAngle(true))) < MathHelper::DegreesToRadians(30.0f))
		{
			ActivateEventTrigger(Events::THRUST_FORWARD, true);
			mb2Body->SetLinearDamping(0.0f);
		}
		else
		{
			ActivateEventTrigger(Events::THRUST_FORWARD, false);
			mb2Body->SetLinearDamping(0.8f);
		}
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
			width, height, 1, 1, GetAngle(true), false, TextureManager::GetInstance()->LoadTexture("Projectile.png"));
		newProjectile->Init_b2(World::GetInstance()->Getb2World(), true);
		newProjectile->Fire(2.0f);
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

	if (desiredVel.y < 0.0f && selfVel.y < 0.0f && steeringCorrection.y > 0.0f)
		steeringCorrection.y *= -1.0f;

	if (desiredVel.x < 0.0f && selfVel.x < 0.0f && steeringCorrection.x > 0.0f)
		steeringCorrection.x *= -1.0f;

	return waypoint + steeringCorrection;
}

float Ship::ConstrainAngle180(float angle)
{
	while (angle < MathHelper::DegreesToRadians(-180)) angle += MathHelper::DegreesToRadians(360);
	while (angle > MathHelper::DegreesToRadians(180)) angle -= MathHelper::DegreesToRadians(360);

	return angle;
}

b2Vec2 Ship::AddWaypoint(b2Vec2 waypoint)
{
	b2Vec2 intermediateWaypoint = waypoint;

	float boundW = 800.0f * Box2dHelper::Units;
	float boundH = 600.0f * Box2dHelper::Units;

	if (waypoint.x < 0.0f)
	{
		//intermediateWaypoint.x = 0.0f;
		//waypoint.x = 0.0f;
		//mWaypoints.push_back(waypoint);
		waypoint.x = (boundW) - (waypoint.x * -1.0f);
	}
	else if (waypoint.x > boundW)
	{
		//intermediateWaypoint.x = boundW;
		//waypoint.x = 640.0f;
		//mWaypoints.push_back(waypoint);
		waypoint.x = 0.0f + (waypoint.x - boundW);
	}

	if (waypoint.y < 0.0f)
	{
		//intermediateWaypoint.y = 0.0f;
		//waypoint.y = 0.0f;
		//mWaypoints.push_back(waypoint);
		waypoint.y = boundH - (waypoint.y * -1.0f);
	}

	else if (waypoint.y > boundH)
	{
		//intermediateWaypoint.y = boundH;
		//waypoint.y = 480.0f;
		//mWaypoints.push_back(waypoint);
		waypoint.y = 0.0f + (waypoint.y - boundH);
	}

	if (intermediateWaypoint.x != waypoint.x
		|| intermediateWaypoint.y != waypoint.y)
	{
		mWaypoints.push_back(Waypoint(intermediateWaypoint, true));
	}

	mWaypoints.push_back(Waypoint(waypoint, false));

	return waypoint;
}

void Ship::ProcessWaypoints(float dt)
{
	if (mWaypoints.size() > 0)
	{
		if (mReachedCurWaypoint)
		{
			if (mWaypoints.front().isIntermediate)
			{
				mWaypoints.pop_front();
			}
			mWaypoints.pop_front();
			mReachedCurWaypoint = false;
		}
	}

	if (mWaypoints.size() > 0)
	{
		ActivateEventTrigger(Events::STABILIZE, false);
		MoveToWaypoint(mWaypoints.front(), 40.0f*Box2dHelper::Units, dt);
	}
	else
	{
		ActivateEventTrigger(Events::STABILIZE, true);
	}
}

b2Vec2 Ship::GetCurrentWaypoint() const
{
	if (mWaypoints.size() > 0)
		return mWaypoints.front().position;
	else
		return b2Vec2(-1.0f, -1.0f);
}

void Ship::AddMovementPattern(int movementPattern, bool immediate)
{
	switch (movementPattern)
	{
	case Attributes::MOVEMENT_CIRCLE:
	{
		if (immediate)
			mWaypoints.clear();

		b2Vec2 curPos = GetPosition(true);
		float angle = GetAngle_NonRetarded(true);

		float dist = 5.0f;

		float angleInc = 60.0f;

		angle += MathHelper::DegreesToRadians(angleInc);

		b2Vec2 newWaypoint;
		newWaypoint.x = (-(std::cos(angle))*dist + curPos.x);
		newWaypoint.y = (std::sin(angle))*dist + curPos.y;
		newWaypoint = AddWaypoint(b2Vec2(newWaypoint.x, newWaypoint.y));

		angle += MathHelper::DegreesToRadians(angleInc);

		b2Vec2 newWaypoint2;
		newWaypoint2.x = (-(std::cos(angle))*dist + newWaypoint.x);
		newWaypoint2.y = (std::sin(angle))*dist + newWaypoint.y;
		newWaypoint2 = AddWaypoint(b2Vec2(newWaypoint2.x, newWaypoint2.y));

		angle += MathHelper::DegreesToRadians(angleInc);

		b2Vec2 newWaypoint3;
		newWaypoint3.x = (-(std::cos(angle))*dist + newWaypoint2.x);
		newWaypoint3.y = ((std::sin(angle))*dist + newWaypoint2.y);
		newWaypoint3 = AddWaypoint(b2Vec2(newWaypoint3.x, newWaypoint3.y));

		angle += MathHelper::DegreesToRadians(angleInc);

		b2Vec2 newWaypoint4;
		newWaypoint4.x = (-(std::cos(angle))*dist + newWaypoint3.x);
		newWaypoint4.y = ((std::sin(angle))*dist + newWaypoint3.y);
		newWaypoint4 = AddWaypoint(b2Vec2(newWaypoint4.x, newWaypoint4.y));

		angle += MathHelper::DegreesToRadians(angleInc);

		b2Vec2 newWaypoint5;
		newWaypoint5.x = (-(std::cos(angle))*dist + newWaypoint4.x);
		newWaypoint5.y = ((std::sin(angle))*dist + newWaypoint4.y);
		newWaypoint5 = AddWaypoint(b2Vec2(newWaypoint5.x, newWaypoint5.y));

		angle += MathHelper::DegreesToRadians(angleInc);

		b2Vec2 newWaypoint6;
		newWaypoint6.x = (-(std::cos(angle))*dist + newWaypoint5.x);
		newWaypoint6.y = ((std::sin(angle))*dist + newWaypoint5.y);
		newWaypoint6 = AddWaypoint(b2Vec2(newWaypoint6.x, newWaypoint6.y));

		break;
	}
	case Attributes::MOVEMENT_ZIGZAG:
	{
		if (immediate)
			mWaypoints.clear();

		b2Vec2 curPos = GetPosition(true);
		float angle = ConstrainAngle180(GetAngle_NonRetarded(true));

		float dist = 7.0f;

		angle += MathHelper::DegreesToRadians(45.0f);

		b2Vec2 newWaypoint;
		newWaypoint.x = (-(std::cos(angle))*dist + curPos.x);
		newWaypoint.y = (std::sin(angle))*dist + curPos.y;
		newWaypoint = AddWaypoint(b2Vec2(newWaypoint.x, newWaypoint.y));

		angle -= MathHelper::DegreesToRadians(90.0f);

		b2Vec2 newWaypoint2;
		newWaypoint2.x = (-(std::cos(angle))*dist + newWaypoint.x);
		newWaypoint2.y = ((std::sin(angle))*dist + newWaypoint.y);
		newWaypoint2 = AddWaypoint(b2Vec2(newWaypoint2.x, newWaypoint2.y));

		angle += MathHelper::DegreesToRadians(90.0f);

		b2Vec2 newWaypoint3;
		newWaypoint3.x = (-(std::cos(angle))*dist + newWaypoint2.x);
		newWaypoint3.y = ((std::sin(angle))*dist + newWaypoint2.y);
		newWaypoint3 = AddWaypoint(b2Vec2(newWaypoint3.x, newWaypoint3.y));

		angle -= MathHelper::DegreesToRadians(90.0f);

		b2Vec2 newWaypoint4;
		newWaypoint4.x = (-(std::cos(angle))*dist + newWaypoint3.x);
		newWaypoint4.y = ((std::sin(angle))*dist + newWaypoint3.y);
		newWaypoint4 = AddWaypoint(b2Vec2(newWaypoint4.x, newWaypoint4.y));

		break;
	}
	}
}

void Ship::SetTarget(b2Vec2 target)
{
	mTarget = target;
}

b2Vec2 Ship::GetTarget() const
{
	return mTarget;
}

void Ship::Init(int type)
{
	//mIsImmovable = isImmovable;
	mType = type;
}

void Ship::ClearWaypoints()
{
	mWaypoints.clear();
}
