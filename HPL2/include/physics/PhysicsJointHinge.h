#ifndef HPL_PHYSICS_JOINT_HINGE_H
#define HPL_PHYSICS_JOINT_HINGE_H

#include "physics/PhysicsJoint.h"

namespace hpl {

    //-----------------------------------

    class iPhysicsJointHinge : public iPhysicsJoint
    {
    #ifdef __GNUC__
        typedef iPhysicsJoint __super;
    #endif
    public:
        iPhysicsJointHinge(const tString &asName, iPhysicsBody *apParentBody, iPhysicsBody *apChildBody,
            iPhysicsWorld *apWorld,const cVector3f &avPivotPoint,const cVector3f &avPinDir)
            : iPhysicsJoint(asName,apParentBody,apChildBody, apWorld,avPivotPoint,avPinDir){}
        virtual ~iPhysicsJointHinge(){}

        virtual void SetMaxAngle(float afAngle)=0;
        virtual void SetMinAngle(float afAngle)=0;
        virtual float GetMaxAngle()=0;
        virtual float GetMinAngle()=0;

        ePhysicsJointType GetType(){ return ePhysicsJointType_Hinge;}

    protected:
        float mfMaxAngle;
        float mfMinAngle;
    };
};
#endif // HPL_PHYSICS_JOINT_HINGE_H
