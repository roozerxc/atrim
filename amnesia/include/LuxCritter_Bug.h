#ifndef LUX_CRITTER_BUG_H
#define LUX_CRITTER_BUG_H

//----------------------------------------------

#include "LuxProp_CritterBase.h"

//----------------------------------------------

class cLuxCritter_Bug_SaveData : public iLuxProp_CritterBase_SaveData
{
    kSerializableClassInit(cLuxCritter_Bug_SaveData)
public:
    float mfPlaySoundCount;
};

//----------------------------------------------

class cLuxCritter_Bug : public iLuxProp_CritterBase
{
    typedef iLuxProp_CritterBase super_class;
    friend class cLuxPropLoader_Critter_Bug;
    friend class cLuxCritter_BugRayCallback;
public:
    cLuxCritter_Bug(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxCritter_Bug();

    //////////////////////
    //General
    void UpdateCritterSpecific(float afTimeStep);

    //////////////////////
    //Properties

    //////////////////////
    //Connection callbacks
    void OnConnectionStateChange(iLuxEntity *apEntity, int alState) {}

    //////////////////////
    //Save data stuff
    iLuxEntity_SaveData* CreateSaveData();
    void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
    void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
    void SetupSaveData(iLuxEntity_SaveData *apSaveData);


private:
    void UpdateVelocity(float afTimeStep);
    void UpdateMovement(float afTimeStep);

    //General

    //Vars
    float mfPlaySoundCount;
    float mfPauseCount;
    bool mbPaused;

    //Data
    float mfMaxSpeed;
    float mfMaxSpeedNormal;
    float mfMaxSpeedAfraid;
    float mfGravity;

    float mfPlayerFleeDistance;

    float mfFleeMul;
    float mfBackToSpawnPointMul;
    float mfWanderCircleRadius;
    float mfWanderCircleDist;

    bool mbHasRandomPauses;
    cVector2f mvRandomPauseTimeMinMax;
    cVector2f mvRandomMoveTimeMinMax;

    tString msNormalSound;
    cVector2f mvNormalSoundRandMinMax;

    tString msScaredSound;
    cVector2f mvScaredSoundRandMinMax;
};

//----------------------------------------------

class cLuxPropLoader_Critter_Bug : public iLuxPropLoader_Critter
{
public:
    cLuxPropLoader_Critter_Bug(const tString& asName);
    virtual ~cLuxPropLoader_Critter_Bug() {}

    iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
    void LoadCritterVariables(iLuxProp *apProp, cXmlElement *apRootElem);
    void LoadCritterInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_CRITTER_BUG_H
