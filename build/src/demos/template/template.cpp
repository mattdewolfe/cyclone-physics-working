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

#include <stdio.h>
#include <gl/glut.h>
#include <cyclone/cyclone.h>
#include "../app.h"
#include "../timing.h"

#include "BasicSphere.h"

class Template : public RigidBodyApplication
{
public:
	BasicSphere *testSphere;

public:
    /** Creates a new demo object. */
    Template();

	~Template();
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
};

// Method definitions
Template::Template() 
: RigidBodyApplication()
{
	pauseSimulation = false;

	testSphere = new BasicSphere(3, 1.0f, 1.0f, 1.0f);
	// Resets all ammo positions
    reset();
}

// Setup visuals such as open GL lights/textures
void Template::initGraphics()
{
    Application::initGraphics();
}

// Check for collisions between rigid bodies
void Template::generateContacts()
{
	 // Set up the collision data structure
    cData.reset(maxContacts);
    cData.friction = (cyclone::real)0.9;
    cData.restitution = (cyclone::real)0.1;
    cData.tolerance = (cyclone::real)0.1;
}

const char* Template::getTitle()
{
    return "Cyclone > Template";
}

// Reset to init state of project
void Template::reset()
{

}

// Iterate through all cyclone objects and update forces/positions/etc
void Template::updateObjects(cyclone::real duration)
{
	// Ensure frame time was not zero, in order to prevent any
	// possible divisions by zero
    if (duration <= 0.0f) return;
	
	/* Iterate through physics objects as needed and call the following
	**
	**	object->body->integrate(duration);
	**	object->calculateInternals();
	*/
	testSphere->body->integrate(duration);
	testSphere->calculateInternals();
}

// Render calls for all objects
void Template::display()
{

	// Clear the viewport and set the camera direction
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(-25.0, 8.0, 5.0,  0.0, 5.0, 22.0,  0.0, 1.0, 0.0);

	// Generally, iterate through all objects and call object->render()
	testSphere->Render();
}

// Mouse events/positioning
void Template::mouse(int button, int state, int x, int y)
{

}

// Key board events
void Template::key(unsigned char key)
{
   
}

// Destructor
Template::~Template()
{

}

/**
 * Called by the common demo framework to create an application
 * object (with new) and return a pointer.
 */
Application* getApplication()
{
    return new Template();
}