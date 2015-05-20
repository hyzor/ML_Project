#ifndef CONTACTLISTENER_H_
#define CONTACTLISTENER_H_

#include <Box2D/Box2D.h>

#include "Entity.h"
#include "Ship.h"

class MyB2ContactListener : public b2ContactListener
{
public:
	MyB2ContactListener();
	~MyB2ContactListener();

	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
};

#endif