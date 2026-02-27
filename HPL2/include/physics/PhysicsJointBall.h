#ifndef HPL_PHYSICS_JOINT_BALL_H
#define HPL_PHYSICS_JOINT_BALL_H

#include "physics/PhysicsJoint.h"

namespace hpl {

    //-----------------------------------

    class iPhysicsJointBall : public iPhysicsJoint
    {
    #ifdef __GNUC__
        typedef iPhysicsJoint __super;
    #endif
    public:
        iPhysicsJointBall(const tString &asName, iPhysicsBody *apParentBody, iPhysicsBody *apChildBody,
            iPhysicsWorld *apWorld, const cVector3f &avPivotPoint,const cVector3f &avPinDir)
            : iPhysicsJoint(asName,apParentBody,apChildBody, apWorld,avPivotPoint,avPinDir){}
        virtual ~iPhysicsJointBall(){}

        virtual void SetConeLimits(float afMaxConeAngle, float afMaxTwistAngle)=0;
        virtual cVector3f GetAngles()=0;

        float GetMaxConeAngle(){ return mfMaxConeAngle;}
        float GetMaxTwistAngle(){ return mfMaxTwistAngle;}
        
        ePhysicsJointType GetType(){ return ePhysicsJointType_Ball;}
    
    protected:
        float mfMaxConeAngle;
        float mfMaxTwistAngle;
    };
};
#endif // HPL_PHYSICS_JOINT_BALL_H
