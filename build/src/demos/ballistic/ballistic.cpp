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

#include <string>
#include <gl/glut.h>
#include <cyclone/cyclone.h>
#include "../app.h"
#include "../timing.h"
// GAME3002 - Launcher class
#include "Launcher.h"
#include "AmmoRound.h"
#include <stdio.h>

#define NUM_LAUNCHERS 2
#define MAX_SHOT_POWER 10
#define MIN_SHOT_ANGLE 10
#define MAX_SHOT_ANGLE 85
/**
 * The main demo class definition.
 */

class BallisticDemo : public RigidBodyApplication
{

    /**
     * Holds the maximum number of  rounds that can be
     * fired.
     */
    const static unsigned ammoRounds = 32;

    /** Holds the particle data. */
    AmmoRound ammo[ammoRounds];

    /** Holds the current shot type. */
    ShotType currentShotType;

	// GAME3002 - Holds all the launchers on the screen
	Launcher *launchers[NUM_LAUNCHERS];

	// GAME3002 - Used to access array of launchers for the current player/launcher
	int currentLauncher; 

    /** Dispatches a round. */
    void fire();

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

	/** Sets up the rendering. */
    virtual void initGraphics();
    
    /** Processes the objects in the simulation forward in time. */
    virtual void updateObjects(cyclone::real duration);

    /** Display the particle positions. */
    virtual void display();

    /** Handle a mouse click. */
    virtual void mouse(int button, int state, int x, int y);

    /** Handle a keypress. */
   virtual void key(unsigned char key);

   // GAME3002 - Functions for change in shot power and angle
   void ChangePower(int _change);
   void ChangeAngle(int _change);
};

// Method definitions
BallisticDemo::BallisticDemo() 
: RigidBodyApplication(),
currentShotType(LASER)
{

	// Game3002 - Create our launchers
	for (int i = 0; i < NUM_LAUNCHERS; i++)
	{
		launchers[i] = new Launcher(5.0f, 1.0f, 1.5f, 80.0f*i);
		launchers[i]->SetColour(cyclone::Vector3(0.0f, 0.75f*i, 0.25f));
	}
	currentLauncher = 0;
	pauseSimulation = false;
	// Resets all ammo positions
    reset();
}

void BallisticDemo::initGraphics()
{
    GLfloat lightAmbient[] = {0.8f,0.8f,0.8f,1.0f};
    GLfloat lightDiffuse[] = {0.9f,0.95f,1.0f,1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    glEnable(GL_LIGHT0);

    Application::initGraphics();
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

    // Check ground plane collisions
    for (int i = 0; i < NUM_LAUNCHERS; i++)
    {
        if (!cData.hasMoreContacts()) return;
			cyclone::CollisionDetector::sphereAndHalfSpace(*launchers[i], plane, &cData);

        // Check for collisions with each shot
        for (AmmoRound *shot = ammo; shot < ammo+ammoRounds; shot++)
        {
            if (shot->type != UNUSED)
            {
                if (!cData.hasMoreContacts()) return;

                // When we get a collision, remove the shot
                if (cyclone::CollisionDetector::sphereAndSphere(*launchers[i], *shot, &cData))
                {
                    shot->type = UNUSED;
                }
			}
		}
	}
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

    // Else, setup the shot with a vector 3 to offset it from it's orientation
	cyclone::Vector3 direction;
	if (currentLauncher == 0)
	{
		direction = cyclone::Vector3(0, 0, 2.0f);
	}
	else
	{
		direction = cyclone::Vector3(0, 0, -2.0f);
	}


    shot->setState( currentShotType, (launchers[currentLauncher]->body->getPosition() + direction) );

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
            if (shot->body->getPosition().x < 0.0f ||
                shot->startTime+5000 < TimingData::get().lastFrameTimestamp ||
                shot->body->getPosition().z > 200.0f)
            {
                // We simply set the shot type to be unused, so the
                // memory it occupies can be reused by another shot.
                shot->type = UNUSED;
            }
        }
    }
}

void BallisticDemo::display()
{

	const static GLfloat lightPosition[] = {-1,1,0,0};

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

    // Render each shot in turn
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
	// Render the shot power and angle
	renderText(190.0f, 22.0f, "Current Power: ");
	renderText(190.0f, 10.0f, "Current Angle: ");
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
		case 'w': ChangePower(1); break;
		case 'W': ChangePower(1); break;
		case 's': ChangePower(-1); break;
		case 'S': ChangePower(-1); break;
		case 'a': ChangeAngle(1); break;
		case 'A': ChangeAngle(1); break;
		case 'd': ChangeAngle(-1); break;
		case 'D': ChangeAngle(-1); break;
    }
}

void BallisticDemo::ChangePower(int _change)
{
	// Pull the power value of the current launcher
	int tempPower = launchers[currentLauncher]->GetPower();
	tempPower += _change;
	// Ensure value is within bounds for launcher power
	if (tempPower < 1)
		tempPower = 1;
	else if (tempPower > MAX_SHOT_POWER)
		tempPower = MAX_SHOT_POWER;
	// Pass that value back into the current launcher
	launchers[currentLauncher]->SetPower(tempPower);
}

void BallisticDemo::ChangeAngle(int _change)
{
	// Pull the power value of the current launcher
	int tempAngle = launchers[currentLauncher]->GetAngle();
	tempAngle += _change;
	// Ensure value is within bounds for launcher power
	if (tempAngle < MIN_SHOT_ANGLE)
		tempAngle = MIN_SHOT_ANGLE;
	else if (tempAngle > MAX_SHOT_ANGLE)
		tempAngle = MAX_SHOT_ANGLE;
	// Pass that value back into the current launcher
	launchers[currentLauncher]->SetAngle(tempAngle);
}

// Destructor
BallisticDemo::~BallisticDemo()
{

}

/**
 * Called by the common demo framework to create an application
 * object (with new) and return a pointer.
 */
Application* getApplication()
{
    return new BallisticDemo();
}