#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <gl/glut.h>
#include <cyclone/cyclone.h>

class Launcher : public cyclone::CollisionSphere
{
private:
	// Color of launcher, drawn with openGL RGB (range 0.0f - 1.0f) 
	cyclone::Vector3 colour;
	// Store the shot power from last shot to display to player
	float shotPower;
	// Called inside constructor to setup physics collider on launcher
	void SetupCollider();

public:

	Launcher(float _radius, float _x = 0, float _y = 0, float _z = 0);
	Launcher(float _radius, cyclone::Vector3 _pos);
	~Launcher(void);

	// Get/Set launcher colour
	void SetColour(const cyclone::Vector3& _col) { colour = _col; }
	cyclone::Vector3 GetOColour() const { return colour; }

	// Get/Set shot power
	void SetPower(const float _pow) { shotPower = _pow; }
	float GetPower() const { return shotPower; }

	// Draw the visual representation of our launcher
	void Draw();
};
#endif

