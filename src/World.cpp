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
			mb2World->DestroyBody(mEntities[i]->Getb2Body());
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
		if (!mEntities[i]->IsAlive())
		{
			mb2World->DestroyBody(mEntities[i]->Getb2Body());
			std::swap(mEntities[i], mEntities.back());
			mEntities.pop_back();
			continue;
		}

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

	if (mb2World)
	{
		delete mb2World;
		mb2World = nullptr;
	}
}