#include "BasicSphere.h"

BasicSphere::BasicSphere(float _radius, float _x, float _y, float _z)
{
	body = new cyclone::RigidBody();
	body->setPosition(_x, _y, _z);
	radius = _radius;
	colour = cyclone::Vector3(0.0, 0.0, 0.0);
	SetupCollider();
}
BasicSphere::BasicSphere(float _radius, cyclone::Vector3 _pos)
{
	body = new cyclone::RigidBody();
	body->setPosition(_pos);
	radius = _radius;
	colour = cyclone::Vector3(0.0, 0.0, 0.0);
	SetupCollider();
} 

BasicSphere::~BasicSphere(void)
{
	delete body;
	body = NULL;
}

// Sets up collision body
void BasicSphere::SetupCollider()
{
    body->setOrientation(1,0,0,0);
    body->setVelocity(0,0,0);
    body->setRotation(cyclone::Vector3(0,0,0));

	halfSize = cyclone::Vector3(1,1,1);

    cyclone::real mass = halfSize.x * halfSize.y * halfSize.z * 8.0f;
    body->setMass(mass);

    cyclone::Matrix3 tensor;
    tensor.setBlockInertiaTensor(halfSize, mass);
    body->setInertiaTensor(tensor);

    body->setLinearDamping(0.95f);
    body->setAngularDamping(0.8f);
    body->clearAccumulators();
    body->setAcceleration(0,-10.0f,0);

    body->setCanSleep(false);
    body->setAwake();

    body->calculateDerivedData();
    calculateInternals();
}

// Visuals for launcher
void BasicSphere::Render ()
{
	// Draw a sphere at the firing point, and add a shadow projected
    // onto the ground plane.
    glColor3f(colour.x, colour.y, colour.z);
    glPushMatrix();
    //glTranslatef(body->getPosition().x, body->getPosition().y, body->getPosition().z);
    glutSolidSphere(1.5f, 25, 25);
    glTranslatef(0.0f, -1.5f, 0.0f);
    glColor3f(0.75f, 0.75f, 0.75f);
    glScalef(1.0f, 0.1f, 1.0f);
    glutSolidSphere(0.1f, 5, 5);
    glPopMatrix();
}