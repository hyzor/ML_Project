#include "Entity.h"


Entity::Entity()
{
	mHealth = 1;
	mSpawnPosX = 0.0f;
	mSpawnPosY = 0.0f;
	mTexture = nullptr;
	mWidth = 0;
	mHeight = 0;
	mSdlRect = nullptr;
	mb2Body = nullptr;
	mDamage = 0;
	mIsAlive = true;
}


Entity::Entity(float x, float y, int width, int height, int health, int damage, float angle, SDL_Wrapper::Texture* texture, b2World* world)
	: Entity()
{
	mSpawnPosX = x;
	mSpawnPosY = y;
	mHealth = health;
	mTexture = texture;
	mWidth = width;
	mHeight = height;
	mDamage = damage;

	/*
	mSdlRect = new SDL_Rect();
	mSdlRect->x = (int)x;
	mSdlRect->y = (int)y;
	mSdlRect->w = mWidth;
	mSdlRect->h = mHeight;
	*/

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(x, y);
	bodyDef.angle = angle;
	mb2Body = world->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox((float)mWidth, (float)mHeight);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 0.01f;
	fixtureDef.friction = 0.0f;

	mb2Body->CreateFixture(&fixtureDef);
}

Entity::~Entity()
{
	if (mSdlRect)
	{
		delete mSdlRect;
		mSdlRect = nullptr;
	}
}

b2Vec2 Entity::GetPosition() const
{
	return mb2Body->GetPosition();
}

float Entity::GetAngle(bool radians) const
{
	if (radians)
		return mb2Body->GetAngle();
	else
		return MathHelper::RadiansToDegrees(mb2Body->GetAngle());
}

b2Vec2 Entity::GetLinearVelocity() const
{
	return mb2Body->GetLinearVelocity();
}

float Entity::GetAngularVelocity() const
{
	return mb2Body->GetAngularVelocity();
}

void Entity::Draw(SDL_Renderer* renderer)
{
	mTexture->Render((int)GetPosition().x, (int)GetPosition().y, renderer, NULL, GetAngle(false));
}

void Entity::Update(float dt){}

void Entity::Reset()
{
	mb2Body->SetTransform(b2Vec2(mSpawnPosX, mSpawnPosY), 0.0f);
	mb2Body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	mb2Body->SetAngularVelocity(0.0f);
}
