#include "BasicSphere.h"

BasicSphere::BasicSphere(float _radius, float _x, float _y, float _z) 
	: zClamp(15.0f), 
	yClamp(0.1f), 
	xClamp(0),
	isInOrbit(false)
{
	body = new cyclone::RigidBody();
	body->setPosition(_x, _y, _z);
	radius = _radius;
	colour = cyclone::Vector3(0.3, 0.0, 0.0);
	SetupCollider();
}
BasicSphere::BasicSphere(float _radius, cyclone::Vector3 _pos)
	: zClamp(15.0f), 
	yClamp(0.1f),
	xClamp(0),
	isInOrbit(false)
{
	body = new cyclone::RigidBody();
	body->setPosition(_pos);
	radius = _radius;
	colour = cyclone::Vector3(0.3, 0.0, 0.0);
	SetupCollider();
} 

// Set limitations on z and y acceleration
// Currently sphere never moves on x plane
void BasicSphere::SetZYSpeedClamps(float z, float y)
{
	zClamp = z;
	yClamp = y;
}

// Sends ball into high orbit
void BasicSphere::LiftOff()
{
	if (isInOrbit == false)
	{
		isInOrbit = true;
		body->setVelocity(0.0, 400.0f, 0.0);
	}
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

void BasicSphere::MoveBody(float z, float y)
{
	cyclone::Vector3 vel = body->getVelocity();
	// Check y speed to ensure body is on ground
	// if it is not we will not change any speed values
	if (vel.y < yClamp && vel.y > -yClamp)
	{
		vel.y += y;

		// Check if desired direction is opposite of current direction
		// If so increase the force to get a faster turn around time
		if (z < 0 && vel.z > 0)
		{
			vel.z += 2*z;
		}
		else if (z > 0 && vel.z < 0)
		{
			vel.z += 2*z;
		}
		// Make sure velocity is within bounds
		else if (!(vel.z > zClamp) && !(vel.z < -zClamp))
		{
			vel.z += z;
		}
		// Finally apply new velocity value to the body
		body->setVelocity(vel);
	}
}

// Visuals for launcher
void BasicSphere::Render ()
{
	// Draw a sphere at the firing point, and add a shadow projected
    // onto the ground plane.
    glColor3f(colour.x, colour.y, colour.z);
    glPushMatrix();
    glTranslatef(body->getPosition().x, body->getPosition().y, body->getPosition().z);
    glutSolidSphere(radius, 25, 25);
    glPopMatrix();
}

BasicSphere::~BasicSphere(void)
{
	delete body;
	body = NULL;
}