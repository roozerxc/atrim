#include "impl/PhysicsJointBallNewton.h"

#include "impl/PhysicsBodyNewton.h"
#include "impl/PhysicsWorldNewton.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cPhysicsJointBallNewton::cPhysicsJointBallNewton(const tString &asName,
        iPhysicsBody *apParentBody, iPhysicsBody *apChildBody,
        iPhysicsWorld *apWorld, const cVector3f &avPivotPoint, const cVector3f &avPinDir)
    : iPhysicsJointNewton<iPhysicsJointBall>(asName,apParentBody,apChildBody,apWorld,avPivotPoint,avPinDir)
{
    mpNewtonJoint = NewtonConstraintCreateBall(mpNewtonWorld,avPivotPoint.v,
                    mpNewtonChildBody, mpNewtonParentBody);

    mfMaxConeAngle =0;
    mfMaxTwistAngle = 0;
}

//-----------------------------------------------------------------------

cPhysicsJointBallNewton::~cPhysicsJointBallNewton()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cPhysicsJointBallNewton::SetConeLimits(float afMaxConeAngle, float afMaxTwistAngle)
{
    NewtonBallSetConeLimits(mpNewtonJoint, mvStartPinDir.v,afMaxConeAngle,afMaxTwistAngle);
    mfMaxConeAngle = afMaxConeAngle;
    mfMaxTwistAngle = afMaxTwistAngle;
}

cVector3f cPhysicsJointBallNewton::GetAngles()
{
    cVector3f vAngles;
    NewtonBallGetJointAngle(mpNewtonJoint,&vAngles.v[0]);
    return vAngles;
}

//-----------------------------------------------------------------------

cVector3f cPhysicsJointBallNewton::GetVelocity()
{
    return cVector3f(0,0,0);
}
cVector3f cPhysicsJointBallNewton::GetAngularVelocity()
{
    cVector3f vVel;
    NewtonBallGetJointOmega(mpNewtonJoint,&vVel.v[0]);
    return vVel;
}
float cPhysicsJointBallNewton::GetForceSize()
{
    cVector3f vForce;
    NewtonBallGetJointForce(mpNewtonJoint,&vForce.v[0]);
    return vForce.Length();
}

//-----------------------------------------------------------------------

float cPhysicsJointBallNewton::GetDistance()
{
    return 0;
}
float cPhysicsJointBallNewton::GetAngle()
{
    return 0;
}

//-----------------------------------------------------------------------

}
