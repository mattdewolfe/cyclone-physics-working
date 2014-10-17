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

#include "Box.h"
#include "BasicSphere.h"

class Template : public RigidBodyApplication
{
public:
	BasicSphere *testSphere;
	// Used to store the size of the ground plane
	// X width, Y width, and Z position
	cyclone::Vector3 groundPlane;

	const static int numBoxes = 3;
	/** Holds the box data. */
    Box boxData[numBoxes];

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

   void renderInterface();
};

// Method definitions
Template::Template() 
: RigidBodyApplication()
{
	groundPlane = cyclone::Vector3(100.0f, 7.0f, 0.0f);
	// Resets all positions
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
	// Define a plane for the ground
    cyclone::CollisionPlane plane;
    plane.direction = cyclone::Vector3(0,1,0);
    plane.offset = 0;

	// Set up the collision data structure
    cData.reset(maxContacts);
    cData.friction = (cyclone::real)0.9;
    cData.restitution = (cyclone::real)0.1;
    cData.tolerance = (cyclone::real)0.1;

	for (int i = 0; i < numBoxes; i++)
	{
		// If there is no cData, break out
		if (!cData.hasMoreContacts()) return;
		cyclone::CollisionDetector::boxAndSphere(boxData[i], *testSphere, &cData);
	}

	// Else check for plane and sphere collision
	cyclone::CollisionDetector::sphereAndHalfSpace(*testSphere, plane, &cData);
	
}

const char* Template::getTitle()
{
    return "Cyclone > Template";
}

// Reset to init state of project
void Template::reset()
{
	testSphere = new BasicSphere(2, 0.0f, 15.0f, 1.0f);
	

	// Put 3 boxes into the scene
	cyclone::real zVal = 15.0f;
	for (int i = 0; i < numBoxes; i++)
	{
		boxData[i].setState(zVal);
		zVal += 10.0f;
	}
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
    gluLookAt(-40.0, 10.0, 10.0,  0.0, 10.0, 10.0,  0.0, 1.0, 0.0);

	// Render the sphere
	testSphere->Render();

	// Render the boxes
	glColor3f(0.0, 0.0, 0.7f);
	for (int i = 0; i < numBoxes; i++)
	{
		boxData[i].render();
	}

	renderInterface();
	
}

// Mouse events/positioning
void Template::mouse(int button, int state, int x, int y)
{
	testSphere->body->addForce(cyclone::Vector3(50.0, 0.0, 0.0));
}

// Key board events
void Template::key(unsigned char key)
{
   switch (key)
   {
	   case 's':
		   pauseSimulation = !pauseSimulation;
		   break;
	   case 'S':
		   pauseSimulation = !pauseSimulation;
		   break;
	   case 'r':
		   reset();
		   break;
	   case 'R':
		   reset();
		   break;
	   case 'w':
		   testSphere->MoveBody(0.0, 1.0f);
		   break;
	   case 'a':
		   testSphere->MoveBody(-1.0f, 0.0);
		   break;
	   case 'd':
		   testSphere->MoveBody(1.0f, 0.0);
		   break;
   }
}

// Render on screen elements such as feedback and grind lines
void Template::renderInterface()
{
	// Draw a ground plane
	glColor3f(0.0, 0.5, 0.1);
	glPushMatrix();
	glBegin(GL_POLYGON);
		glVertex3f(0.0, 0.0, -groundPlane.x);
		glVertex3f(groundPlane.y, 0, -groundPlane.x);
		glVertex3f(groundPlane.y, 0, groundPlane.x);
		glVertex3f(0, 0, groundPlane.x);
	glEnd();
	glPopMatrix();

	// Draw some scale lines
    glColor3f(1.0f, 0, 0);
    glBegin(GL_LINES);
    for (unsigned i = 0; i < 200; i += 10)
    {
        glVertex3f(-5.0f, 0.0f, i);
        glVertex3f(5.0f, 0.0f, i);
    }
    glEnd();

	// Ouptut text for user
	glColor3f(0.5f, 0.0, 0.0);
	if (pauseSimulation == true)
		renderText(10.0f, 24.0f, "Simulation is paused - Press S to unpause");
	else
		renderText(10.0f, 24.0f, " Simulation is running - Press S to pause");
}
// Destructor
Template::~Template()
{
	delete testSphere;
	testSphere = NULL;
}

/**
 * Called by the common demo framework to create an application
 * object (with new) and return a pointer.
 */
Application* getApplication()
{
    return new Template();
}