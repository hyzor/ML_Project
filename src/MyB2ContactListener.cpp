#include "MyB2ContactListener.h"


MyB2ContactListener::MyB2ContactListener()
{
}


MyB2ContactListener::~MyB2ContactListener()
{
}

void MyB2ContactListener::BeginContact(b2Contact* contact)
{
	void* userdata1 = contact->GetFixtureA()->GetBody()->GetUserData();
	void* userdata2 = contact->GetFixtureB()->GetBody()->GetUserData();

	Entity* entity1 = nullptr;
	Entity* entity2 = nullptr;

	if (userdata1)
		entity1 = static_cast<Entity*>(userdata1);

	if (userdata2)
		entity2 = static_cast<Entity*>(userdata2);

	if (entity1 && entity2)
	{
		entity1->DoCollide(entity2->GetCollisionDamage());
		entity2->DoCollide(entity1->GetCollisionDamage());
	}
}

void MyB2ContactListener::EndContact(b2Contact* contact)
{

}