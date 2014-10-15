#ifndef BASICSPHERE_H
#define BASICSPHERE_H

#include <gl/glut.h>
#include <cyclone/cyclone.h>
#include "../src/demos/timing.h"

class BasicSphere : public cyclone::CollisionSphere
{
private:
	// Color of launcher, drawn with openGL RGB (range 0.0f - 1.0f) 
	cyclone::Vector3 colour;
	// Controls force for the next shot
	int shotPower;
	// Controls angle of force when shot is fired
	int shotAngle;
	// Called inside constructor to setup physics collider on launcher
	void SetupCollider();
	// Store halfsize value, used for inertia
	cyclone::Vector3 halfSize;

public:
	BasicSphere(float _radius, float _x = 0, float _y = 0, float _z = 0);
	BasicSphere(float _radius, cyclone::Vector3 _pos);
	~BasicSphere(void);

	// Get/Set launcher colour
	void SetColour(const cyclone::Vector3& _col) { colour = _col; }
	cyclone::Vector3 GetOColour() const { return colour; }

	// Get/Set shot power
	void SetPower(const float _pow) { shotPower = _pow; }
	float GetPower() const { return shotPower; }

	// Get/Set shot power
	void SetAngle(const float _pow) { shotAngle = _pow; }
	float GetAngle() const { return shotAngle; }

	// Draw the visual representation of our launcher
	void Render();
};
#endif
