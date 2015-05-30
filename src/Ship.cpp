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
				if (mWaypoints.front()->isIntermediate)
					//mWaypoints.pop_front();
					mCurWaypoint = mCurWaypoint->next;
			}
		}

		Entity::Update(dt);


		if (mCurCooldown > 0.0f)
			mCurCooldown -= dt;

		if (mType == Type::STATIONARY)
		{
			RotateTo_Torque(mTarget, dt);
			//Shoot();
			if (mEventTriggers[Events::SHOOT])
			{
				// Try to shoot
				Shoot();
			}

			SetPosition(b2Vec2(mSpawnPosX*Box2dHelper::Units, mSpawnPosY*Box2dHelper::Units));
			mb2Body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
		}
		else
		{
			if (mDoProcessWaypoints)
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
				mb2Body->SetLinearDamping(20.0f);
				mb2Body->SetAngularDamping(20.0f);
			}

			if (mEventTriggers[Events::SHOOT])
			{
				DoProcessWaypoints(false);
				float desiredAngle = RotateTo_Torque(mTarget, dt);

				float diff = std::abs(desiredAngle - ConstrainAngle180(GetAngle(true)));

				if (diff < MathHelper::DegreesToRadians(1.5f))
				{
					if (Shoot())
					{
						mb2Body->SetLinearDamping(0.0f);
						DoProcessWaypoints(true);
						mEventTriggers[Events::SHOOT] = false;
					}
					//mEventTriggers[Events::STABILIZE] = false;
				}
				else
				{
					mb2Body->SetLinearDamping(10.0f);
					//mEventTriggers[Events::STABILIZE] = true;
				}

				// Try to shoot
				//Shoot();
			}
		}


}

void Ship::InitShip()
{
	mTorque = 30.0f;
	mMagnitude = 20.0f;
	mCooldown = 3.0f;
	mBaseCooldown = 3.0f;
	mCurCooldown = 0.0f;
	mType = Type::NON_STATIONARY;
	mCurWaypoint = nullptr;
	mSensorFixture = nullptr;
	mDoProcessWaypoints = true;
}

Ship::~Ship()
{
	for (unsigned int i = 0; i < mWaypoints.size(); ++i)
	{
		if (mWaypoints[i])
			delete mWaypoints[i];
	}

	mWaypoints.clear();
}

void Ship::ActivateEventTrigger(Events movement, bool activate)
{
	mEventTriggers[movement] = activate;
}

/*
float Ship::RotateTo_Torque(b2Vec2 point, float degreesPerStep)
{
	b2Vec2 toTarget = point - mb2Body->GetPosition();
	toTarget.Normalize();
	float desiredAngle = atan2f(toTarget.x, -toTarget.y);
	float totalRotation = desiredAngle - mb2Body->GetAngle();
	float newAngle = desiredAngle;
	mb2Body->SetTransform(mb2Body->GetPosition(), newAngle);

	return desiredAngle;
}
*/

float Ship::RotateTo_Torque(b2Vec2 point, float dt)
{
	float bodyAngle = GetAngle(true);
	
	b2Vec2 toTarget = point - GetPosition(true);
	toTarget.Normalize();
	float desiredAngle = atan2f(toTarget.x, -toTarget.y);

	float nextAngle = bodyAngle + GetAngularVelocity() / 4.0f;
	//float nextAngle = bodyAngle + (((GetAngularVelocity()) * dt) / 4.0f);

	float totalRotation = desiredAngle - nextAngle;

	//while (totalRotation < MathHelper::DegreesToRadians(-180)) totalRotation += MathHelper::DegreesToRadians(360);
	//while (totalRotation > MathHelper::DegreesToRadians(180)) totalRotation -= MathHelper::DegreesToRadians(360);

	totalRotation = ConstrainAngle180(totalRotation);

	//float diff = std::abs(desiredAngle - ConstrainAngle180(GetAngle(true)));


	//if (diff < MathHelper::DegreesToRadians(5.0f))
	//{
	//	mb2Body->SetAngularDamping(diff * 100.0f);
	//}


	//float desiredAngularVelocity = totalRotation / (dt);
	//float torque = mb2Body->GetInertia() * desiredAngularVelocity / dt;

	mb2Body->ApplyTorque(totalRotation < 0 ? -mTorque : mTorque, true);
	//mb2Body->ApplyTorque(torque, true);

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
		b2Vec2 destination = GetCorrectionWaypoint(point, dt);

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

void Ship::MoveToWaypoint(Waypoint* waypoint, float radius, float dt)
{
	b2Vec2 pos = GetPosition(true);
	float dist = sqrt(std::pow(waypoint->position.x - pos.x, 2) + std::pow(waypoint->position.y - pos.y, 2));

	if (waypoint->isIntermediate)
		dist = sqrt(std::pow(waypoint->next->position.x - pos.x, 2) + std::pow(waypoint->next->position.y - pos.y, 2));

	if (dist <= radius*1.0f + mb2Body->GetLinearVelocity().Length() * 0.5)
	{
		mb2Body->SetLinearDamping(15.0f);
		mb2Body->SetAngularDamping(0.0f);

		//mb2Body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
		//mReachedCurWaypoint = true;

		if (dist <= (0.965f)*(radius + mb2Body->GetLinearVelocity().Length() * 0.5))
		{
			mReachedCurWaypoint = true;
		}
	}
	else
	{
		b2Vec2 destination = GetCorrectionWaypoint(waypoint->position, dt);
		//destination = waypoint->position;

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
		newProjectile->Init_b2(World::GetInstance()->Getb2World(), true, Entity::Type::PROJECTILE);
		newProjectile->SetType(Entity::Type::PROJECTILE);
		newProjectile->Fire(2.0f);
		mCurCooldown = mCooldown;

		successful = true;
	}

	return successful;
}

b2Vec2 Ship::GetCorrectionWaypoint(b2Vec2 waypoint, float dt)
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
		Waypoint* newWaypoint = new Waypoint(intermediateWaypoint, true);

		if (mWaypoints.size() > 0)
			mWaypoints.back()->next = newWaypoint;

		mWaypoints.push_back(newWaypoint);
		//mWaypoints.back(mWaypoints.size() - 1).next = &mWaypoints.back();
		//mWaypoints.back().next = &mWaypoints.front();
	}

	Waypoint* newWaypoint = new Waypoint(waypoint, false);

	if (mWaypoints.size() > 0)
		mWaypoints.back()->next = newWaypoint;

	mWaypoints.push_back(newWaypoint);
	mWaypoints.back()->next = mWaypoints.front();

	return waypoint;
}

void Ship::ProcessWaypoints(float dt)
{
	if (mWaypoints.size() > 0)
	{
		if (!mCurWaypoint)
		{
			mCurWaypoint = mWaypoints.front();
		}

		if (mReachedCurWaypoint)
		{
			if (mCurWaypoint->isIntermediate)
			{
				//mWaypoints.pop_front();

				mCurWaypoint = mCurWaypoint->next;
			}

			mCurWaypoint = mCurWaypoint->next;
			//mWaypoints.pop_front();
			mReachedCurWaypoint = false;
		}
	}

	if (mWaypoints.size() > 0)
	{
		ActivateEventTrigger(Events::STABILIZE, false);
		//MoveToWaypoint(mWaypoints.front(), 40.0f*Box2dHelper::Units, dt);
		MoveToWaypoint(mCurWaypoint, (40.0f*Box2dHelper::Units), dt);
	}
	else
	{
		ActivateEventTrigger(Events::STABILIZE, true);
	}
}

b2Vec2 Ship::GetCurrentWaypoint() const
{
	if (mWaypoints.size() > 0)
		return mWaypoints.front()->position;
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

void Ship::Draw(SDL_Renderer* renderer, double alpha)
{
	if (mCurWaypoint)
		TextureManager::GetInstance()->LoadTexture("Waypoint.png")->Render((int)(mCurWaypoint->position.x*Box2dHelper::PixelsPerMeter - (16.0f*0.5f)),
		(int)(mCurWaypoint->position.y*Box2dHelper::PixelsPerMeter - (16.0f*0.5f)),
		renderer);

	Entity::Draw(renderer, alpha);
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
	for (unsigned int i = 0; i < mWaypoints.size(); ++i)
	{
		if (mWaypoints[i])
			delete mWaypoints[i];
	}

	mWaypoints.clear();
	mCurWaypoint = nullptr;
}

bool Ship::Init_b2(b2World* world, bool isBullet, unsigned int type, float sensorGene)
{
	Entity::Init_b2(world, isBullet, type);

	if (mSensorFixture)
	{
		mb2Body->DestroyFixture(mSensorFixture);
		mSensorFixture = nullptr;
	}

	b2FixtureDef sensorFixture;
	b2PolygonShape polygonShape;

	sensorFixture.isSensor = true;
	sensorFixture.filter.categoryBits = Entity::Type::SENSOR;
	sensorFixture.filter.maskBits = Entity::Type::SHIP;

	float radius = 30.0f;
	b2Vec2 vertices[8];
	vertices[0].Set(0, 0);

	float sensorAngle = 30.0f;

	if (sensorGene != 0.0f)
	{
		if (sensorGene > 0.9f)
			sensorGene = std::fmod(sensorGene, 0.9f);

		sensorAngle = sensorAngle * sensorGene;
		mCooldown = 2.0f * mBaseCooldown * (1.0f - sensorGene);
	}

	float angle = 0.0f;

	for (int i = 0; i < 7; ++i)
	{
		angle = ((i / 6.0f * MathHelper::DegreesToRadians(sensorAngle)) - 4.71238898038f - MathHelper::DegreesToRadians(sensorAngle * 0.5f)) * -1.0f;
		vertices[i + 1].Set(radius*cosf(angle), radius*sinf(angle));
	}

	polygonShape.Set(vertices, 8);

	sensorFixture.shape = &polygonShape;

	mSensorFixture = mb2Body->CreateFixture(&sensorFixture);

	return true;
}

void Ship::Reset()
{
	Entity::Reset();

	mCurCooldown = 0.0f;
	mDoProcessWaypoints = true;
	for (int i = 0; i < Events::NUM_EVENTS; ++i)
	{
		mEventTriggers[i] = false;
	}
}

void Ship::DoProcessWaypoints(bool doProcess)
{
	mDoProcessWaypoints = doProcess;
}
