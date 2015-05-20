#ifndef MYENTITY_H_
#define MYENTITY_H_

#include <cmath>

#include <deque>

#include "Entity.h"
#include "Projectile.h"
#include "World.h"
#include "TextureManager.h"
#include "Common.h"

class World;

class Ship : public Entity
{
public:
	static enum Attributes
	{
		SHOOTFREQ,
		MOVEMENT_ZIGZAG,
		MOVEMENT_CIRCLE,
		MOVEMENT_LINE
	};

	static enum Events
	{
		THRUST_FORWARD,
		THRUST_BACKWARD,
		TORQUE_LEFT,
		TORQUE_RIGHT,
		STRAFE_LEFT,
		STRAFE_RIGHT,
		STABILIZE,
		SHOOT,
		NUM_EVENTS
	};

	static enum Type
	{
		NON_STATIONARY,
		STATIONARY
	};

	struct Waypoint
	{
		b2Vec2 position;
		bool isIntermediate;

		Waypoint()
		{
			position = b2Vec2(0.0f, 0.0f);
			isIntermediate = false;
		}

		Waypoint(b2Vec2 _position, bool _isIntermediate)
		{
			position = _position;
			isIntermediate = _isIntermediate;
		}
	};

public:
	Ship();
	Ship(float x, float y, int width, int height, int health, int damage, float angle, bool isImmovable, SDL_Wrapper::Texture* texture);
	~Ship();

	void ActivateEventTrigger(Events event, bool activate);

	void Update(float dt);

	void RotateTo(b2Vec2 point, float degreesPerStep);
	float RotateTo_Torque(b2Vec2 point, float dt);

	void MoveTo(b2Vec2 point, float radius, float dt);
	void MoveToWaypoint(Waypoint waypoint, float radius, float dt);

	b2Vec2 AddWaypoint(b2Vec2 waypoint);
	b2Vec2 GetCurrentWaypoint() const;

	void AddMovementPattern(int movementPattern, bool immediate);

	void SetTarget(b2Vec2 target);
	b2Vec2 GetTarget() const;

	void Init(int type);

	void ClearWaypoints();

	bool Init_b2(b2World* world, bool isBullet, unsigned int type);

	void Reset();

protected:
	float mTorque;
	float mMagnitude;

	float mCooldown;
	float mCurCooldown;

	void InitShip();

	bool mEventTriggers[Events::NUM_EVENTS];
	bool Shoot();

	b2Vec2 GetCorrectionWaypoint(b2Vec2 waypoint);
	float ConstrainAngle180(float angle);

	void ProcessWaypoints(float dt);

	//std::vector<b2Vec2> mWaypoints;

	//std::deque<b2Vec2> mWaypoints;
	std::deque<Waypoint> mWaypoints;

	bool mReachedCurWaypoint;

	b2Vec2 mTarget;
	int mType;
};

#endif