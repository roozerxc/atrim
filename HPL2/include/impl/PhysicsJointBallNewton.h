#ifndef HPL_PHYSICS_JOINT_BALL_NEWTON_H
#define HPL_PHYSICS_JOINT_BALL_NEWTON_H

#include "physics/PhysicsJointBall.h"
#include "impl/PhysicsJointNewton.h"

namespace hpl {

    class cPhysicsJointBallNewton : public iPhysicsJointNewton<iPhysicsJointBall>
    {
    public:
        cPhysicsJointBallNewton(const tString &asName, iPhysicsBody *apParentBody, iPhysicsBody *apChildBody, 
            iPhysicsWorld *apWorld, const cVector3f &avPivotPoint, const cVector3f &avPinDir);
        ~cPhysicsJointBallNewton();

        void SetConeLimits(float afMaxConeAngle, float afMaxTwistAngle);        
        cVector3f GetAngles();

        cVector3f GetVelocity();
        cVector3f GetAngularVelocity();
        float GetForceSize();

        float GetDistance();
        float GetAngle();
        
    private:
    };
};
#endif // HPL_PHYSICS_JOINT_BALL_NEWTON_H
