#ifndef AMMO_ROUND_H
#define AMMO_ROUND_H

#include <gl/glut.h>
#include <cyclone/cyclone.h>

enum ShotType
{
    UNUSED = 0,
    PISTOL,
    ARTILLERY,
    FIREBALL,
    LASER
};

// Create an ammo class that inherits collision properties
class AmmoRound : public cyclone::CollisionSphere
{
public:
    ShotType type;
    unsigned startTime;

    AmmoRound()
    {
        body = new cyclone::RigidBody;
    }

    ~AmmoRound()
    {
        delete body;
    }

    /** Draws the box, excluding its shadow. */
    void render()
    {
        // Get the OpenGL transformation
        GLfloat mat[16];
        body->getGLTransform(mat);

        glPushMatrix();
        glMultMatrixf(mat);
		glColor3f(1.0f, 0, 0);
        glutSolidSphere(radius, 20, 20);
        glPopMatrix();
    }

    /** Sets the box to a specific location. */
    void setState(ShotType shotType)
    {
        type = shotType;

        // Set the properties of the particle
        switch(type)
        {
        case PISTOL:
            body->setMass(1.5f);
            body->setVelocity(0.0f, 0.0f, 20.0f);
            body->setAcceleration(0.0f, -0.5f, 0.0f);
            body->setDamping(0.99f, 0.8f);
            radius = 0.2f;
            break;

        case ARTILLERY:
            body->setMass(200.0f); // 200.0kg
            body->setVelocity(0.0f, 30.0f, 40.0f); // 50m/s
            body->setAcceleration(0.0f, -21.0f, 0.0f);
            body->setDamping(0.99f, 0.8f);
            radius = 0.4f;
            break;

        case FIREBALL:
            body->setMass(4.0f); // 4.0kg - mostly blast damage
            body->setVelocity(0.0f, -0.5f, 10.0); // 10m/s
            body->setAcceleration(0.0f, 0.3f, 0.0f); // Floats up
            body->setDamping(0.9f, 0.8f);
            radius = 0.6f;
            break;

        case LASER:
            // Note that this is the kind of laser bolt seen in films,
            // not a realistic laser beam!
            body->setMass(0.1f); // 0.1kg - almost no weight
            body->setVelocity(0.0f, 0.0f, 100.0f); // 100m/s
            body->setAcceleration(0.0f, 0.0f, 0.0f); // No gravity
            body->setDamping(0.99f, 0.8f);
            radius = 0.2f;
            break;
        }

        body->setCanSleep(false);
        body->setAwake();

        cyclone::Matrix3 tensor;
        cyclone::real coeff = 0.4f*body->getMass()*radius*radius;
        tensor.setInertiaTensorCoeffs(coeff,coeff,coeff);
        body->setInertiaTensor(tensor);

        // Set the data common to all particle types
        body->setPosition(0.0f, 1.5f, 0.0f);
        startTime = TimingData::get().lastFrameTimestamp;

        // Clear the force accumulators
        body->calculateDerivedData();
        calculateInternals();
    }
};

#endif