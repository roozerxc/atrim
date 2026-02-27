#ifndef HPL_PHYSICS_JOINT_SLIDER_NEWTON_H
#define HPL_PHYSICS_JOINT_SLIDER_NEWTON_H

#include "physics/PhysicsJointSlider.h"
#include "impl/PhysicsJointNewton.h"

namespace hpl {

    class cPhysicsJointSliderNewton : public iPhysicsJointNewton<iPhysicsJointSlider>
    {
    public:
        cPhysicsJointSliderNewton(const tString &asName, iPhysicsBody *apParentBody, iPhysicsBody *apChildBody, 
                            iPhysicsWorld *apWorld,const cVector3f &avPivotPoint, const cVector3f& avPinDir) ;
        ~cPhysicsJointSliderNewton();
        
        void SetMaxDistance(float afX);
        void SetMinDistance(float afX);
        float GetMaxDistance();
        float GetMinDistance();

        cVector3f GetVelocity();
        cVector3f GetAngularVelocity();
        float GetForceSize();

        float GetDistance();
        float GetAngle();

    private:
        static unsigned LimitCallback(const NewtonJoint* pSlider, NewtonHingeSliderUpdateDesc* pDesc);

        float mfPreviousDist;
    };
};
#endif // HPL_PHYSICS_JOINT_SLIDER_NEWTON_H
