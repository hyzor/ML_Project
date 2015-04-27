#ifndef MYENTITY_H_
#define MYENTITY_H_

class MyEntity
{
public:
	MyEntity();
	~MyEntity();

protected:
	float mVelocity;
	float mHealth;
	float mPosX, mPosY;
	float mAngle;
};

#endif