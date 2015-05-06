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

	void Init(b2World* _b2World);
	static void Destroy();

	void Clear();

	template <class T>
	T* SpawnEntity(float xPos, float yPos, int width, int height, int health, int damage, float angle, SDL_Wrapper::Texture* texture);

	b2World* Getb2World() const;
	void Update(float dt);
	void Draw(SDL_Renderer* renderer);

private:
	World();
	~World();

	static World* mInstance;

	b2World* mb2World;
	std::vector<Entity*> mEntities;

	static bool HasInstance();
};

template <class T>
T* World::SpawnEntity(float xPos, float yPos, int width, int height, int health, int damage, float angle, SDL_Wrapper::Texture* texture)
{
	T* newEntity = new T(xPos, yPos, width, height, health, damage, angle, texture, mb2World);
	mEntities.push_back(newEntity);
	return newEntity;
}

#endif