#include "Entity.h"


Entity::Entity()
{
	mHealth = 1;
	mSpawnPosX = 0.0f;
	mSpawnPosY = 0.0f;
	mTexture = nullptr;
	mWidth = 0;
	mHeight = 0;
	mSdlClipRect = nullptr;
	mb2Body = nullptr;
	mDamage = 0;
	mIsAlive = true;
	mSdlCenterPoint = nullptr;
	mb2LocalInitVec = b2Vec2(0.0f, 0.0f);
}


Entity::Entity(float x, float y, int width, int height, int health, int damage, float angle, SDL_Wrapper::Texture* texture, b2World* world)
	: Entity()
{
	mSpawnPosX = x;
	mSpawnPosY = y;
	mHealth = health;
	mTexture = texture;
	mWidth = (float)width;
	mHeight = (float)height;
	mDamage = damage;

	mSdlClipRect = new SDL_Rect();
	mSdlClipRect->x = 0;
	mSdlClipRect->y = 0;
	mSdlClipRect->w = width;
	mSdlClipRect->h = height;

	mSdlCenterPoint = new SDL_Point();
	mSdlCenterPoint->x = width / 2;
	mSdlCenterPoint->y = height / 2;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(x*Box2dHelper::Units, y*Box2dHelper::Units);
	bodyDef.angle = angle - 4.71238898038f;
	mb2Body = world->CreateBody(&bodyDef);

	mb2LocalInitVec = b2Vec2(std::cos(angle), std::sin(angle));

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox((mWidth*Box2dHelper::Units)*0.5f, (mHeight*Box2dHelper::Units)*0.5f, b2Vec2(0.0f, 0.0f), 0.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 0.0f;

	mb2Body->SetUserData(this);

	mb2Body->CreateFixture(&fixtureDef);
}

Entity::~Entity()
{
	if (mSdlClipRect)
	{
		delete mSdlClipRect;
		mSdlClipRect = nullptr;
	}

	mb2Body = nullptr;
}

b2Body* Entity::Getb2Body() const
{
	return mb2Body;
}

b2Vec2 Entity::GetPosition(bool inMeters) const
{
	if (inMeters)
		return mb2Body->GetPosition();
	else
		return b2Vec2(mb2Body->GetPosition().x*Box2dHelper::PixelsPerMeter, mb2Body->GetPosition().y*Box2dHelper::PixelsPerMeter);
}

float Entity::GetAngle(bool inRadians) const
{
	if (inRadians)
		return mb2Body->GetAngle();
	else
		return MathHelper::RadiansToDegrees(mb2Body->GetAngle());
}

float Entity::GetAngle_NonRetarded(bool inRadians) const
{
	if (inRadians)
		return mb2Body->GetAngle() - 4.71238898038f; // 3pi/2
	else
		return MathHelper::RadiansToDegrees(mb2Body->GetAngle() - 4.71238898038f); //3pi/2
}

b2Vec2 Entity::GetLinearVelocity() const
{
	return mb2Body->GetLinearVelocity();
}

float Entity::GetAngularVelocity() const
{
	return mb2Body->GetAngularVelocity();
}

bool Entity::IsAlive() const
{
	return mIsAlive;
}

b2Vec2 Entity::GetDimensions(bool inMeters) const
{
	if (inMeters)
		return b2Vec2(mWidth*Box2dHelper::Units, mHeight*Box2dHelper::Units);
	else
		return b2Vec2(mWidth, mHeight);
}

void Entity::Draw(SDL_Renderer* renderer)
{
	mTexture->Render((int)(GetPosition(false).x-(mWidth*0.5f)), (int)(GetPosition(false).y-(mHeight*0.5f)), renderer, mSdlClipRect, GetAngle(false), mSdlCenterPoint);
	//TextureManager::GetInstance()->LoadTexture("BlueDot.png")->Render((int)GetPosition(false).x, (int)GetPosition(false).y, renderer, NULL, GetAngle(false));
}

void Entity::Update(float dt)
{
	if (mHealth <= 0)
	{
		mIsAlive = false;
	}

	if (GetPosition(true).y < 0.0f)
		SetPosition(b2Vec2(GetPosition(true).x, 600.0f*Box2dHelper::Units));

	if (GetPosition(true).y > 600.0f * Box2dHelper::Units)
		SetPosition(b2Vec2(GetPosition(true).x, 0.0f));

	if (GetPosition(true).x < 0.0f)
		SetPosition(b2Vec2(800.0f * Box2dHelper::Units, GetPosition(true).y));

	if (GetPosition(true).x > 800.0f * Box2dHelper::Units)
		SetPosition(b2Vec2(0.0f, GetPosition(true).y));
}

void Entity::Reset()
{
	mb2Body->SetTransform(b2Vec2(mSpawnPosX*Box2dHelper::Units, mSpawnPosY*Box2dHelper::Units), 0.0f);
	mb2Body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	mb2Body->SetAngularVelocity(0.0f);
	mIsAlive = true;
}

void Entity::DoCollide(int collisionDamage)
{
	mHealth -= collisionDamage;
}

int Entity::GetCollisionDamage() const
{
	return mDamage;
}

void Entity::SetPosition(b2Vec2 pos)
{
	mb2Body->SetTransform(pos, GetAngle(true));
}