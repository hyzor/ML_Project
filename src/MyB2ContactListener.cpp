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

		void* userdata1 = fixtureA->GetBody()->GetUserData();
		void* userdata2 = fixtureB->GetBody()->GetUserData();

		if (userdata1 != userdata2)
		{
			Entity* entity1 = nullptr;
			Entity* entity2 = nullptr;

			if (userdata1)
				entity1 = static_cast<Entity*>(userdata1);

			if (userdata2)
				entity2 = static_cast<Entity*>(userdata2);

			if (entity1 && entity2)
			{
				if (fixtureA->IsSensor() && !fixtureB->IsSensor())
				{
					Ship* entity1_Ship = static_cast<Ship*>(userdata1);
					entity1_Ship->ActivateEventTrigger(Ship::SHOOT, true);

				}
				else if (fixtureB->IsSensor() && !fixtureA->IsSensor())
				{
					Ship* entity2_Ship = static_cast<Ship*>(userdata2);
					entity2_Ship->ActivateEventTrigger(Ship::SHOOT, true);
				}
				else
				{
					if (filterA.categoryBits != Entity::Type::STATIC)
						entity1->DoCollide(entity2->GetCollisionDamage());

					if (filterB.categoryBits != Entity::Type::STATIC)
						entity2->DoCollide(entity1->GetCollisionDamage());
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

		void* userdata1 = fixtureA->GetBody()->GetUserData();
		void* userdata2 = fixtureB->GetBody()->GetUserData();

		if (userdata1 != userdata2)
		{
			Entity* entity1 = nullptr;
			Entity* entity2 = nullptr;

			if (userdata1)
				entity1 = static_cast<Entity*>(userdata1);

			if (userdata2)
				entity2 = static_cast<Entity*>(userdata2);

			if (entity1 && entity2)
			{
				if (fixtureA->IsSensor() && !fixtureB->IsSensor())
				{
					Ship* entity1_Ship = static_cast<Ship*>(userdata1);
					entity1_Ship->ActivateEventTrigger(Ship::SHOOT, false);

				}
				else if (fixtureB->IsSensor() && !fixtureA->IsSensor())
				{
					Ship* entity2_Ship = static_cast<Ship*>(userdata2);
					entity2_Ship->ActivateEventTrigger(Ship::SHOOT, false);
				}

				//entity1->DoCollide(entity2->GetCollisionDamage());
				//entity2->DoCollide(entity1->GetCollisionDamage());
			}
		}
	}
}