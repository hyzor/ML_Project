#ifndef MYENTITY_H_
#define MYENTITY_H_

#include <cmath>

#include <deque>

#include "Entity.h"
#include "Projectile.h"
#include "World.h"
#include "TextureManager.h"

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

public:
	Ship();
	Ship(float x, float y, int width, int height, int health, int damage, float angle, SDL_Wrapper::Texture* texture, b2World* world);
	~Ship();

	void ActivateEventTrigger(Events event, bool activate);

	void Update(float dt);

	void RotateTo(b2Vec2 point, float degreesPerStep);
	float RotateTo_Torque(b2Vec2 point, float dt);

	void MoveTo(b2Vec2 point, float radius, float dt);

	void AddWaypoint(b2Vec2 waypoint);
	b2Vec2 GetCurrentWaypoint() const;

	void AddMovementPattern(int movementPattern, bool immediate);

private:
	float mTorque;
	float mMagnitude;

	float mCooldown;
	float mCurCooldown;

	bool mEventTriggers[Events::NUM_EVENTS];

	void Init();
	bool Shoot();

	b2Vec2 GetCorrectionWaypoint(b2Vec2 waypoint);
	float ConstrainAngle180(float angle);

	void ProcessWaypoints(float dt);

	//std::vector<b2Vec2> mWaypoints;

	std::deque<b2Vec2> mWaypoints;

	bool mReachedCurWaypoint;
};

#endif