#include "MyEntity.h"

MyEntity::MyEntity()
{
	mVelocity = 0.0f;
	mHealth = 1;
	mPosX = 0.0f;
	mPosY = 0.0f;
	mAngle = 0.0f;
	mDmg = 0;
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	mSdlRect = new SDL_Rect();
	mb2Body = NULL;
}

MyEntity::MyEntity(float xPos, float yPos, int width, int height, int hp, int dmg, SDL_Wrapper::Texture& texture, b2World* world)
	: MyEntity()
{
	mPosX = xPos;
	mPosY = yPos;
	mHealth = hp;
	mDmg = dmg;
	mTexture = &texture;
	mWidth = width;
	mHeight = height;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(mPosX, mPosY);
	mb2Body = world->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox((float)mWidth, (float)mHeight);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 0.1f;
	fixtureDef.friction = 0.0f;

	mb2Body->CreateFixture(&fixtureDef);
}

void MyEntity::Update(float dt)
{
	b2Vec2 forceDirection = mb2Body->GetWorldVector(b2Vec2(0, 1));
	float magnitude = 400000.0f;
	forceDirection *= magnitude;
	float torque = 7000000.0f;

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
		mb2Body->ApplyTorque(-torque, true);
	}

	if (mEventTriggers[Events::TORQUE_RIGHT])
	{
		mb2Body->ApplyTorque(torque, true);
	}

	if (mEventTriggers[Events::SHOOT])
	{
		// Try to shoot
	}

	mPosX = mb2Body->GetPosition().x;
	mPosY = mb2Body->GetPosition().y;
	mAngle = mb2Body->GetAngle();

	mSdlRect->x = (int)mPosX;
	mSdlRect->y = (int)mPosY;
	mSdlRect->w = mWidth;
	mSdlRect->h = mHeight;
}

b2Body* MyEntity::Getb2Body() const
{
	return mb2Body;
}

float MyEntity::GetAngle() const
{
	return mAngle;
}

float MyEntity::GetAngle_Degrees() const
{
	return MathHelper::RadiansToDegrees(mAngle);
}

void MyEntity::Draw(SDL_Renderer* renderer)
{
	mTexture->Render((int)mPosX, (int)mPosY, renderer, NULL, GetAngle_Degrees());
}

MyEntity::~MyEntity()
{
	delete mSdlRect;
}

void MyEntity::ActivateEventTrigger(Events movement, bool activate)
{
	mEventTriggers[movement] = activate;
}