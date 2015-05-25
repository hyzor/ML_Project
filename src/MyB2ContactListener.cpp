#include "MyB2ContactListener.h"


MyB2ContactListener::MyB2ContactListener()
{
}


MyB2ContactListener::~MyB2ContactListener()
{
}

void MyB2ContactListener::BeginContact(b2Contact* contact)
{
	b2Filter filterA = contact->GetFixtureA()->GetFilterData();
	b2Filter filterB = contact->GetFixtureB()->GetFilterData();

	bool doCollide = (filterA.maskBits & filterB.categoryBits) != 0
		&& (filterA.categoryBits & filterB.maskBits) != 0;

	if (doCollide)
	{
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		void* userdataA = fixtureA->GetBody()->GetUserData();
		void* userdataB = fixtureB->GetBody()->GetUserData();

		if (userdataA != userdataB)
		{
			Entity* entityA = nullptr;
			Entity* entityB = nullptr;

			if (userdataA)
				entityA = static_cast<Entity*>(userdataA);

			if (userdataB)
				entityB = static_cast<Entity*>(userdataB);

			if (entityA && entityB)
			{
				if (fixtureA->IsSensor() && !fixtureB->IsSensor())
				{
					Ship* entityA_Ship = (Ship*)(entityA);
					entityA_Ship->ActivateEventTrigger(Ship::SHOOT, true);
					entityA_Ship->SetTarget(entityB->GetPosition(true));
				}
				else if (fixtureB->IsSensor() && !fixtureA->IsSensor())
				{
					Ship* entityB_Ship = (Ship*)(entityB);
					entityB_Ship->ActivateEventTrigger(Ship::SHOOT, true);
					entityB_Ship->SetTarget(entityA->GetPosition(true));
				}
				else
				{
					if (filterA.categoryBits != Entity::Type::STATIC)
					{
						entityA->DoCollide(entityB->GetCollisionDamage());
						entityA->AddCollisionEntity(entityB);
					}

					if (filterB.categoryBits != Entity::Type::STATIC)
					{
						entityB->DoCollide(entityA->GetCollisionDamage());
						entityB->AddCollisionEntity(entityA);
					}
				}
			}
		}
	}
}

void MyB2ContactListener::EndContact(b2Contact* contact)
{
	b2Filter filterA = contact->GetFixtureA()->GetFilterData();
	b2Filter filterB = contact->GetFixtureB()->GetFilterData();

	bool doCollide = (filterA.maskBits & filterB.categoryBits) != 0
		&& (filterA.categoryBits & filterB.maskBits) != 0;

	if (doCollide)
	{
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		void* userdataA = fixtureA->GetBody()->GetUserData();
		void* userdataB = fixtureB->GetBody()->GetUserData();

		if (userdataA != userdataB)
		{
			Entity* entityA = nullptr;
			Entity* entityB = nullptr;

			if (userdataA)
				entityA = static_cast<Entity*>(userdataA);

			if (userdataB)
				entityB = static_cast<Entity*>(userdataB);

			if (entityA && entityB)
			{
				if (fixtureA->IsSensor() && !fixtureB->IsSensor())
				{
					Ship* entityA_Ship = (Ship*)(entityA);
					entityA_Ship->ActivateEventTrigger(Ship::SHOOT, false);
					entityA_Ship->DoProcessWaypoints(true);
				}
				else if (fixtureB->IsSensor() && !fixtureA->IsSensor())
				{
					Ship* entityB_Ship = (Ship*)(entityB);
					entityB_Ship->ActivateEventTrigger(Ship::SHOOT, false);
					entityB_Ship->DoProcessWaypoints(true);
				}
				else
				{
					if (filterA.categoryBits != Entity::Type::STATIC)
					{
						//entityA->DoCollide(entityB->GetCollisionDamage());
						entityA->RemoveCollisionEntity(entityB);
					}

					if (filterB.categoryBits != Entity::Type::STATIC)
					{
						//entityB->DoCollide(entityA->GetCollisionDamage());
						entityB->RemoveCollisionEntity(entityA);
					}
				}

				//entity1->DoCollide(entity2->GetCollisionDamage());
				//entity2->DoCollide(entity1->GetCollisionDamage());
			}
		}
	}
}