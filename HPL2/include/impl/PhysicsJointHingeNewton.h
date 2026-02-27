#ifndef HPL_PHYSICS_JOINT_HINGE_NEWTON_H
#define HPL_PHYSICS_JOINT_HINGE_NEWTON_H

#include "physics/PhysicsJointHinge.h"
#include "impl/PhysicsJointNewton.h"

namespace hpl {

    class cPhysicsJointHingeNewton : public iPhysicsJointNewton<iPhysicsJointHinge>
    {
    public:
        cPhysicsJointHingeNewton(const tString &asName, iPhysicsBody *apParentBody, iPhysicsBody *apChildBody, 
            iPhysicsWorld *apWorld,const cVector3f &avPivotPoint, const cVector3f &avPinDir);
        ~cPhysicsJointHingeNewton();
        
        void SetMaxAngle(float afAngle);
        void SetMinAngle(float afAngle);
        float GetMaxAngle();
        float GetMinAngle();

        cVector3f GetVelocity();
        cVector3f GetAngularVelocity();
        float GetForceSize();

        float GetDistance();
        float GetAngle();
    
    private:
        cMatrixf m_mtxLocalPinPivot0;
        cMatrixf m_mtxLocalPinPivot1;
        
        float mfPreviousAngle;
        
        void SubmitConstraints (dFloat afTimestep, int alThreadIndex);
        void GetInfo (NewtonJointRecord* apInfo);

        //static unsigned LimitCallback(const NewtonJoint* pHinge, NewtonHingeSliderUpdateDesc* pDesc);
    };
};
#endif // HPL_PHYSICS_JOINT_HINGE_NEWTON_H
