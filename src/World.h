#ifndef WORLD_H_
#define WORLD_H_

#include <vector>
#include <algorithm>

#include <SDL/SDL.h>
#include <Box2D/Box2D.h>

#include "Entity.h"

class World
{
public:
	static World* GetInstance();

	void Init(b2World* _b2World, unsigned int width, unsigned int height);
	static void Destroy();

	void Clear();

	void Flush();

	template <class T>
	T* SpawnEntity(float xPos, float yPos, int width, int height, int health, int damage, float angle, bool isImmovable, SDL_Wrapper::Texture* texture);
	
	template <class T>
	void AddEntity(T* entity);

	template <class T>
	void RemoveEntity(T* entity);

	b2World* Getb2World() const;
	void Update(float dt);
	void Draw(SDL_Renderer* renderer);

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

private:
	World();
	~World();

	static World* mInstance;

	b2World* mb2World;
	std::vector<Entity*> mEntities;

	unsigned int mWidth, mHeight;

	static bool HasInstance();
};

template <class T>
T* World::SpawnEntity(float xPos, float yPos, int width, int height, int health, int damage, float angle, bool isImmovable, SDL_Wrapper::Texture* texture)
{
	T* newEntity = new T(xPos, yPos, width, height, health, damage, angle, isImmovable, texture);
	mEntities.push_back(newEntity);
	return newEntity;
}

template <class T>
void World::AddEntity(T* entity)
{
	mEntities.push_back(entity);
}

template <class T>
void World::RemoveEntity(T* entity)
{
	int index = -1;

	for (unsigned int i = 0; i < mEntities.size(); ++i)
	{
		if (mEntities[i] == entity)
		{
			index = i;
			break;
		}
	}

	if (index != -1)
		mEntities.erase(mEntities.begin()+index);
}

#endif