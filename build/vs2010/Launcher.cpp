#include "Launcher.h"

Launcher::Launcher(float _radius, float _x, float _y, float _z)
{
	body = new cyclone::RigidBody();
	body->setPosition(_x, _y, _z);
	radius = _radius;
	colour = cyclone::Vector3(0.0, 0.0, 0.0);
	SetupCollider();
}
Launcher::Launcher(float _radius, cyclone::Vector3 _pos)
{
	body = new cyclone::RigidBody();
	body->setPosition(_pos);
	radius = _radius;
	colour = cyclone::Vector3(0.0, 0.0, 0.0);
	SetupCollider();
} 

Launcher::~Launcher(void)
{
	delete body;
	body = NULL;
}

// Sets up collision body
void Launcher::SetupCollider()
{

}

// Visuals for launcher
void Launcher::Draw ()
{
	// Draw a sphere at the firing point, and add a shadow projected
    // onto the ground plane.
    glColor3f(colour.x, colour.y, colour.z);
    glPushMatrix();
    glTranslatef(body->getPosition().x, body->getPosition().y, body->getPosition().z);
    glutSolidSphere(1.5f, 25, 25);
    glTranslatef(0.0f, -1.5f, 0.0f);
    glColor3f(0.75f, 0.75f, 0.75f);
    glScalef(1.0f, 0.1f, 1.0f);
    glutSolidSphere(0.1f, 5, 5);
    glPopMatrix();
}