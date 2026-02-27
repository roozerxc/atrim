#include "impl/PhysicsJointSliderNewton.h"

#include "impl/PhysicsBodyNewton.h"
#include "impl/PhysicsWorldNewton.h"

#include "system/LowLevelSystem.h"

namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cPhysicsJointSliderNewton::cPhysicsJointSliderNewton(const tString &asName, 
        iPhysicsBody *apParentBody, iPhysicsBody *apChildBody, 
        iPhysicsWorld *apWorld,const cVector3f &avPivotPoint, const cVector3f& avPinDir)
        : iPhysicsJointNewton<iPhysicsJointSlider>(asName,apParentBody,apChildBody,apWorld,avPivotPoint,avPinDir)
    {
        mpNewtonJoint = NewtonConstraintCreateSlider(mpNewtonWorld, avPivotPoint.v, avPinDir.v, mpNewtonChildBody,
                                                    mpNewtonParentBody);

        NewtonJointSetUserData(mpNewtonJoint, (void*) this);
        NewtonSliderSetUserCallback(mpNewtonJoint,LimitCallback);

        mfMaxDistance =0;
        mfMinDistance =0;

        mfPreviousDist =0;
    }

    //-----------------------------------------------------------------------

    cPhysicsJointSliderNewton::~cPhysicsJointSliderNewton()
    {

    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    
    void cPhysicsJointSliderNewton::SetMaxDistance(float afX)
    {
        mfMaxDistance = afX;
    }
    void cPhysicsJointSliderNewton::SetMinDistance(float afX)
    {
        mfMinDistance = afX;
    }
    float cPhysicsJointSliderNewton::GetMaxDistance()
    {
        return mfMaxDistance;
    }
    float cPhysicsJointSliderNewton::GetMinDistance()
    {
        return mfMinDistance;
    }
    
    //-----------------------------------------------------------------------

    cVector3f cPhysicsJointSliderNewton::GetVelocity()
    {
        float fSpeed = NewtonSliderGetJointVeloc(mpNewtonJoint);
        return GetPinDir() * fSpeed;
    }
    cVector3f cPhysicsJointSliderNewton::GetAngularVelocity()
    {
        return cVector3f(0,0,0);
    }
    float cPhysicsJointSliderNewton::GetForceSize()
    {
        cVector3f vForce;
        NewtonSliderGetJointForce(mpNewtonJoint,&vForce.v[0]);
        return vForce.Length();
    }


    //-----------------------------------------------------------------------

    float cPhysicsJointSliderNewton::GetDistance()
    {
        return NewtonSliderGetJointPosit(mpNewtonJoint);
    }
    float cPhysicsJointSliderNewton::GetAngle()
    {
        return 0;
    }
    
    //-----------------------------------------------------------------------
    
    //////////////////////////////////////////////////////////////////////////
    // STATIC CALLBACKS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    unsigned cPhysicsJointSliderNewton::LimitCallback(const NewtonJoint* pSlider, NewtonHingeSliderUpdateDesc* pDesc)
    {
        cPhysicsJointSliderNewton* pSliderJoint = (cPhysicsJointSliderNewton*)NewtonJointGetUserData(pSlider);
        
        //pSliderJoint->OnPhysicsUpdate();
        
        float fDistance = NewtonSliderGetJointPosit (pSlider);
        //Log("Dist: %f\n",fDistance);

        if(pSliderJoint->mfMinDistance == 0 && pSliderJoint->mfMaxDistance == 0) return 0;

        //Avoid oscillation
        CheckLimitAutoSleep(pSliderJoint, pSliderJoint->mfMinDistance,pSliderJoint->mfMaxDistance,fDistance);

        if (fDistance < pSliderJoint->mfMinDistance)
        {
            pSliderJoint->OnMinLimit();

            pDesc->m_accel = NewtonSliderCalculateStopAccel (pSlider, pDesc, pSliderJoint->mfMinDistance);
            pDesc->m_minFriction =0;

            //Log("Under Min. Acc: %f Dist %f\n",pDesc->m_accel,fDistance);
            
            pSliderJoint->mfPreviousDist = fDistance;

            return 1;
        } 
        else if (fDistance > pSliderJoint->mfMaxDistance)
        {
            pSliderJoint->OnMaxLimit();
            
            pDesc->m_accel = NewtonSliderCalculateStopAccel (pSlider, pDesc, pSliderJoint->mfMaxDistance);
            pDesc->m_maxFriction =0;

            pSliderJoint->mfPreviousDist = fDistance;

            //Log("Over Max. Acc: %f Dist %f\n",pDesc->m_accel,fDistance);
            return 1;
        }
        else
        {
            if(pSliderJoint->mpParentBody ==NULL || pSliderJoint->mpParentBody->GetMass()==0)
            {
                if( (pSliderJoint->mbStickyMaxLimit && pSliderJoint->mfPreviousDist > pSliderJoint->mfMaxDistance) || 
                    (pSliderJoint->mbStickyMinLimit && pSliderJoint->mfPreviousDist < pSliderJoint->mfMinDistance) )
                {
                    pSliderJoint->mpChildBody->SetAngularVelocity(0);
                    pSliderJoint->mpChildBody->SetLinearVelocity(0);
                }
            }

            pSliderJoint->OnNoLimit();
        }

        pSliderJoint->mfPreviousDist = fDistance;

        //Log("Nothing, Dist %f\n",fDistance);

        return 0;
    }
    //-----------------------------------------------------------------------

}
