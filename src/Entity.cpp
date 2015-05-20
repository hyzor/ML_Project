#include "Entity.h"


Entity::Entity()
{
	mHealth = 1;
	mInitialHealth = 1;
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
	mIsImmovable = false;
	mInitAngle = 0.0f;
	mb2Fixture = nullptr;
	mType = Type::STATIC;
}


Entity::Entity(float x, float y, int width, int height, int health, int damage, float angle, bool isImmovable, SDL_Wrapper::Texture* texture)
	: Entity()
{
	mSpawnPosX = x;
	mSpawnPosY = y;
	mHealth = health;
	mInitialHealth = health;
	mTexture = texture;
	mWidth = (float)width;
	mHeight = (float)height;
	mDamage = damage;

	mInitAngle = angle;

	mIsImmovable = isImmovable;

	Init_SDL();
}

Entity::Entity(float x, float y, int width, int height, int health, int damage, float angle, SDL_Wrapper::Texture* texture)
	: Entity()
{
	mSpawnPosX = x;
	mSpawnPosY = y;
	mHealth = health;
	mInitialHealth = health;
	mTexture = texture;
	mWidth = (float)width;
	mHeight = (float)height;
	mDamage = damage;

	mInitAngle = angle;

	Init_SDL();
}

Entity::~Entity()
{
	if (mSdlClipRect)
	{
		delete mSdlClipRect;
		mSdlClipRect = nullptr;
	}

	if (mSdlCenterPoint)
	{
		delete mSdlCenterPoint;
		mSdlCenterPoint = nullptr;
	}

	mb2Body = nullptr;
	mb2Fixture = nullptr;
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

int Entity::GetHealth() const
{
	return mHealth;
}

int Entity::GetHealth_Init() const
{
	return mInitialHealth;
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
	mHealth = mInitialHealth;
}

void Entity::DoCollide(int collisionDamage)
{
	mHealth -= collisionDamage;
}

int Entity::GetCollisionDamage() const
{
	return mDamage;
}

bool Entity::Init_b2(b2World* world, bool isBullet, unsigned int type)
{
	if (!mb2Body)
	{
		b2BodyDef bodyDef;

		if (mIsImmovable)
			bodyDef.type = b2_kinematicBody;
		else
			bodyDef.type = b2_dynamicBody;

		bodyDef.position.Set(mSpawnPosX*Box2dHelper::Units, mSpawnPosY*Box2dHelper::Units);
		bodyDef.angle = mInitAngle - 4.71238898038f;

		mb2Body = world->CreateBody(&bodyDef);

		mb2LocalInitVec = b2Vec2(std::cos(mInitAngle), std::sin(mInitAngle));

		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox((mWidth*Box2dHelper::Units)*0.5f, (mHeight*Box2dHelper::Units)*0.5f, b2Vec2(0.0f, 0.0f), 0.0f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.0f;
		fixtureDef.restitution = 0.0f;

		mType = type;
		fixtureDef.filter.categoryBits = type;

		mb2Body->SetBullet(isBullet);

		mb2Body->SetUserData(this);

		mb2Fixture = mb2Body->CreateFixture(&fixtureDef);

		if (mb2Body)
			return true;
	}

	return false;
}

bool Entity::Init_SDL()
{
	if (!mSdlClipRect)
	{
		mSdlClipRect = new SDL_Rect();
		mSdlClipRect->x = 0;
		mSdlClipRect->y = 0;
		mSdlClipRect->w = (int)mWidth;
		mSdlClipRect->h = (int)mHeight;
	}

	if (!mSdlCenterPoint)
	{
		mSdlCenterPoint = new SDL_Point();
		mSdlCenterPoint->x = (int)mWidth / 2;
		mSdlCenterPoint->y = (int)mHeight / 2;
	}

	return true;
}

void Entity::SetType(unsigned int Type)
{
	mType = Type;

	b2Filter filter = mb2Fixture->GetFilterData();
	filter.categoryBits = Type;
	mb2Fixture->SetFilterData(filter);
}

unsigned int Entity::GetType() const
{
	return mType;
}

void Entity::SetCollisionEnabled(bool enabled)
{
	b2Filter filter;

	for (b2Fixture* fixture = mb2Body->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{
		if (fixture == mb2Fixture)
			continue;

		filter = fixture->GetFilterData();

		if (enabled)
		{
			filter.categoryBits = Entity::Type::SENSOR;
			filter.maskBits = Entity::Type::SHIP;
		}
		else
		{
			filter.categoryBits = 0;
			filter.maskBits = 0;
		}

		fixture->SetFilterData(filter);
	}

	filter = mb2Fixture->GetFilterData();

	if (enabled)
	{
		filter.categoryBits = mType;
		filter.maskBits = 0xFFFF;
	}
	else
	{
		filter.categoryBits = 0;
		filter.maskBits = 0;
	}

	mb2Fixture->SetFilterData(filter);
}

void Entity::Setb2BodyType(b2BodyType bodyType)
{
	mb2Body->SetType(bodyType);
}

void Entity::SetPosition(b2Vec2 pos)
{
	mb2Body->SetTransform(pos, GetAngle(true));
}