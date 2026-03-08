#ifndef LUX_PROP_WHEEL_H
#define LUX_PROP_WHEEL_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_Wheel_SaveData : public iLuxProp_SaveData
{
    kSerializableClassInit(cLuxProp_Wheel_SaveData)
public:
    float mfAngle;
    float mfPrevAngle;

    float mfLastToMax;
    float mfLastToMin;

    int mlCurrentState;
    int mlStuckState;

    float mfJointAngle;
    float mfPrevJointAngle;

    bool mbAutoMoving;
    float mfAutoMoveGoal;

    float mfMinLimit;
    float mfMaxLimit;
    bool mbMinLimitStuck;
    bool mbMaxLimitStuck;

    bool mbInteractionDisablesStuck;

    int mlSpinDir;
};

//----------------------------------------------


class cLuxProp_Wheel : public iLuxProp
{
    typedef iLuxProp super_class;
    friend class cLuxPropLoader_Wheel;
public:
    cLuxProp_Wheel(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxProp_Wheel();

    //////////////////////
    //General
    bool CanInteract(iPhysicsBody *apBody);
    bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);

    void OnSetupAfterLoad(cWorld *apWorld);

    void OnResetProperties();

    void UpdatePropSpecific(float afTimeStep);

    void BeforePropDestruction();

    eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

    float OnInteractDebugDraw(cGuiSet *apSet,iFontData *apFont, float afStartY);

    //////////////////////
    //Properties
    iLuxInteractData_RotateBase* GetMoveBaseData()
    {
        return &mWheelData;
    }

    void SetStuckState(int alState, bool abEffects);
    int  GetStuckState()
    {
        return mlStuckState;
    }

    void SetAngle(float afX, bool abAutoMoveToAngle);
    float GetAngle()
    {
        return mfAngle;
    }

    float GetMinLimit()
    {
        return mfMinLimit;
    }
    float GetMaxLimit()
    {
        return mfMaxLimit;
    }

    void SetInteractionDisablesStuck(bool abX)
    {
        mbInteractionDisablesStuck = abX;
    }
    bool GetInteractionDisablesStuck(bool abX)
    {
        return mbInteractionDisablesStuck;
    }

    //////////////////////
    //Connection callbacks
    void OnConnectionStateChange(iLuxEntity *apEntity, int alState);

    //////////////////////
    //Save data stuff
    iLuxEntity_SaveData* CreateSaveData();
    void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
    void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
    void SetupSaveData(iLuxEntity_SaveData *apSaveData);

private:
    void UpdateAngle(float afTimeStep);
    void UpdateCheckLimit(float afTimeStep);
    void UpdateAutoRotation(float afTimeStep);

    void SetMaxJointAngle(float afMaxAngle);
    void SetMinJointAngle(float afMinAngle);

    void ChangeState(int alState, bool abEffects);

    void ResetAutoMove();

    ////////////////////////////
    // Data
    cLuxInteractData_Wheel mWheelData;

    cPidController<cVector3f> mRotatePid;

    iPhysicsJointHinge *mpHingeJoint;
    iPhysicsBody *mpWheelBody;

    ////////////////////////////
    // Settings
    bool mbCanInteractWithStaticBody;

    float mfMinLimitRange;
    float mfMaxLimitRange;

    float mfMinLimit;
    float mfMaxLimit;
    bool mbMinLimitStuck;
    bool mbMaxLimitStuck;

    int mlSpinDir;

    tString msMinLimitSound;
    tString msMaxLimitSound;

    tString msStuckSound;


    bool mbSlowDownRotation;
    bool mbSkipMiddleState;

    ////////////////////////////
    // Variables
    float mfAngle;
    float mfPrevAngle;

    float mfStuckSoundTimer;

    float mfLastToMax;
    float mfLastToMin;

    int mlCurrentState;
    int mlStuckState;

    float mfJointAngle;
    float mfPrevJointAngle;

    bool mbAutoMoving;
    float mfAutoMoveGoal;

    bool mbInteractionDisablesStuck;
};

//----------------------------------------------

class cLuxPropLoader_Wheel : public iLuxPropLoader
{
public:
    cLuxPropLoader_Wheel(const tString& asName);
    virtual ~cLuxPropLoader_Wheel() {}

    iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
    void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
    void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_PROP_WHEEL_H
