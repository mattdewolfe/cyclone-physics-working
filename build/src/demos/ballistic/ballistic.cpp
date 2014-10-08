/*
 * The ballistic demo.
 *
 * Part of the Cyclone physics system.
 *
 * Copyright (c) Icosagon 2003. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */

#include <gl/glut.h>
#include <cyclone/cyclone.h>
#include "../app.h"
#include "../timing.h"
// GAME3002 - Launcher class
#include "Launcher.h"
#include "AmmoRound.h"
#include <stdio.h>

#define NUM_LAUNCHERS 2
/**
 * The main demo class definition.
 */

class BallisticDemo : public RigidBodyApplication
{

    /**
     * Holds the maximum number of  rounds that can be
     * fired.
     */
    const static unsigned ammoRounds = 16;

    /** Holds the particle data. */
    AmmoRound ammo[ammoRounds];

    /** Holds the current shot type. */
    ShotType currentShotType;

	// GAME3002 - Holds all the launchers on the screen
	Launcher *launchers[NUM_LAUNCHERS];

    /** Dispatches a round. */
    void fire();
	// GAME3002 - Stuff we added, force registry for tracking all forces
	cyclone::ParticleForceRegistry m_ParticleForceRegistry;
	// Gravity - should be added to shots we want effected by gravity
	cyclone::ParticleGravity m_GravityForce;
	// Spring force - added to artillery shot when fired
	cyclone::ParticleFakeSpring *m_FakeSpringForce;

public:
    /** Creates a new demo object. */
    BallisticDemo();

	~BallisticDemo();
    /** Returns the window title for the demo. */
    virtual const char* getTitle();

    /** Resets the position of all the boxes and primes the explosion. */
    virtual void reset();

    /** Build the contacts for the current situation. */
    virtual void generateContacts();

    /** Processes the objects in the simulation forward in time. */
    virtual void updateObjects(cyclone::real duration);

    /** Display the particle positions. */
    virtual void display();

    /** Handle a mouse click. */
    virtual void mouse(int button, int state, int x, int y);

    /** Handle a keypress. */
   virtual void key(unsigned char key);
};

// Method definitions
BallisticDemo::BallisticDemo()
: currentShotType(LASER)
{
	// GAME3002 - Create some gravity
	m_GravityForce.SetGravity(cyclone::Vector3::GRAVITY);
	// GAME3002 - Create a fake spring force (TEMPORARILY zero'd out)
	m_FakeSpringForce = new cyclone::ParticleFakeSpring(new cyclone::Vector3(0.0, 0.0f, 0.0f), 0.0f, 0.0f);

    // Make all shots unused
    for (AmmoRound *shot = ammo; shot < ammo+ammoRounds; shot++)
    {
        shot->type = UNUSED;
    }

	// Game3002 - Create our launchers
	for (int i = 0; i < NUM_LAUNCHERS; i++)
	{
		launchers[i] = new Launcher(5.0f, 1.0f, 1.0f, 80.0f*i);
		launchers[i]->SetColour(cyclone::Vector3(0.0f, 0.75f*i, 0.25f));
	}
}

void BallisticDemo::generateContacts()
{
    // Create the ground plane data
    cyclone::CollisionPlane plane;
    plane.direction = cyclone::Vector3(0,1,0);
    plane.offset = 0;

    // Set up the collision data structure
    cData.reset(maxContacts);
    cData.friction = (cyclone::real)0.9;
    cData.restitution = (cyclone::real)0.1;
    cData.tolerance = (cyclone::real)0.1;

    // Check for collisions with each shot
    for (AmmoRound *shot = ammo; shot < ammo+ammoRounds; shot++)
    {
        if (shot->type != UNUSED)
        {
            if (!cData.hasMoreContacts()) return;
               shot->type = UNUSED;            
        }
    }
    // NB We aren't checking box-box collisions.
}


const char* BallisticDemo::getTitle()
{
    return "Cyclone > Ballistic Demo";
}

void BallisticDemo::fire()
{
    // Find the first available round.
    AmmoRound *shot;
    for (shot = ammo; shot < ammo+ammoRounds; shot++)
    {
        if (shot->type == UNUSED) break;
    }

    // If we didn't find a round, then exit - we can't fire.
    if (shot >= ammo+ammoRounds) return;

    // Else, setup the shot
    shot->setState(currentShotType, 
		(launchers[0]->body->getPosition() + cyclone::Vector3(0.0, 0.0, 2.0f))
		);

}

void BallisticDemo::reset()
{
    // Make all shots unused
    for (AmmoRound *shot = ammo; shot < ammo+ammoRounds; shot++)
    {
        shot->type = UNUSED;
    }
}

void BallisticDemo::updateObjects(cyclone::real duration)
{
    if (duration <= 0.0f) return;
	for (int i = 0; i < NUM_LAUNCHERS; i++)
	{
		launchers[i]->body->integrate(duration);
		launchers[i]->calculateInternals();
	}
    // Update the physics of each particle in turn
    for (AmmoRound *shot = ammo; shot < ammo+ammoRounds; shot++)
    {
        if (shot->type != UNUSED)
        {
            // Run the physics
            shot->body->integrate(duration);
			shot->calculateInternals();

            // Check if the particle is now invalid
            if (shot->body->getPosition().y < -1.0f ||
                shot->startTime+5000 < TimingData::get().lastFrameTimestamp ||
                shot->body->getPosition().z > 200.0f)
            {
                // We simply set the shot type to be unused, so the
                // memory it occupies can be reused by another shot.
                shot->type = UNUSED;
            }
        }
    }

    Application::update();
}

void BallisticDemo::display()
{
    // Clear the viewport and set the camera direction
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(-25.0, 8.0, 5.0,  0.0, 5.0, 22.0,  0.0, 1.0, 0.0);

	// Game3002 - Draw our launchers to the screen
	for (int i = 0; i < NUM_LAUNCHERS; i++)
	{
		launchers[i]->Draw();
	}
    // Draw some scale lines
    glColor3f(0.75f, 0.75f, 0.75f);
    glBegin(GL_LINES);
    for (unsigned i = 0; i < 200; i += 10)
    {
        glVertex3f(-5.0f, 0.0f, i);
        glVertex3f(5.0f, 0.0f, i);
    }
    glEnd();

    // Render each particle in turn
    for (AmmoRound *shot = ammo; shot < ammo+ammoRounds; shot++)
    {
        if (shot->type != UNUSED)
        {
            shot->render();
        }
    }

    // Render the description
    glColor3f(0.0f, 0.0f, 0.0f);
    renderText(10.0f, 34.0f, "Click: Fire\n1-4: Select Ammo");

    // Render the name of the current shot type
    switch(currentShotType)
    {
    case PISTOL: renderText(10.0f, 10.0f, "Current Ammo: Pistol"); break;
    case ARTILLERY: renderText(10.0f, 10.0f, "Current Ammo: Artillery"); break;
    case FIREBALL: renderText(10.0f, 10.0f, "Current Ammo: Fireball"); break;
    case LASER: renderText(10.0f, 10.0f, "Current Ammo: Laser"); break;
    }
}

void BallisticDemo::mouse(int button, int state, int x, int y)
{
    // Fire the current weapon.
    if (state == GLUT_DOWN) fire();
}

void BallisticDemo::key(unsigned char key)
{
    switch(key)
    {
    case '1': currentShotType = PISTOL; break;
    case '2': currentShotType = ARTILLERY; break;
    case '3': currentShotType = FIREBALL; break;
    case '4': currentShotType = LASER; break;
    }
}



// Destructor
BallisticDemo::~BallisticDemo()
{
	if (m_FakeSpringForce)
		delete m_FakeSpringForce;
}

/**
 * Called by the common demo framework to create an application
 * object (with new) and return a pointer.
 */
Application* getApplication()
{
    return new BallisticDemo();
}