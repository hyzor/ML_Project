#include "World.h"

World* World::mInstance = nullptr;

World* World::GetInstance()
{
	if (!mInstance)
	{
		mInstance = new World();
	}

	return mInstance;
}

bool World::HasInstance()
{
	return mInstance != nullptr;
}

void World::Init(b2World* _b2World)
{
	mb2World = _b2World;
}

void World::Destroy()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void World::Clear()
{
	for (unsigned int i = 0; i < mEntities.size(); ++i)
	{
		if (mEntities[i])
		{
			delete mEntities[i];
			mEntities[i] = nullptr;
		}
	}
	mEntities.clear();
}

b2World* World::Getb2World() const
{
	return mb2World;
}

void World::Update(float dt)
{
	for (unsigned int i = 0; i < mEntities.size(); ++i)
	{
		mEntities[i]->Update(dt);
	}
}

void World::Draw(SDL_Renderer* renderer)
{
	for (unsigned int i = 0; i < mEntities.size(); ++i)
	{
		mEntities[i]->Draw(renderer);
	}
}

World::World()
{
}

World::~World()
{
	Clear();
}