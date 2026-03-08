#ifndef LUX_PLAYER_STATE_INTERACT_SLIDE_H
#define LUX_PLAYER_STATE_INTERACT_SLIDE_H

//----------------------------------------------

#include "LuxPlayerState_Interact.h"

//----------------------------------------------

class cLuxPlayer;

class cLuxInteractData_Slide;

//------------------------------------

class cLuxPlayerState_InteractSlide_SaveData : public iLuxPlayerState_Interact_SaveData
{
    kSerializableClassInit(cLuxPlayerState_InteractSlide_SaveData)
public:
};

//------------------------------------


class cLuxPlayerState_InteractSlide : public iLuxPlayerState_Interact
{
    typedef iLuxPlayerState_Interact super_class;
public:
    cLuxPlayerState_InteractSlide(cLuxPlayer *apPlayer);
    virtual ~cLuxPlayerState_InteractSlide();

    void OnEnterState(eLuxPlayerState aPrevState);
    void OnLeaveState(eLuxPlayerState aNewState);

    void Update(float afTimeStep);
    void PostUpdate(float afTimeStep);

    bool OnDoAction(eLuxPlayerAction aAction,bool abPressed);

    bool OnAddYaw(float afAmount);
    bool OnAddPitch(float afAmount);

    bool OnMove(eCharDir aDir, float afMul);

    cGuiGfxElement* GetCrosshair();

    void OnSaveBody(iPhysicsBody *apBody, float &afMass, bool &abCollideCharacter);

    float DrawDebug(cGuiSet *apSet,iFontData *apFont, float afStartY);

    void RenderSolid(cRendererCallbackFunctions* apFunctions);

    /////////////////////////////////
    //Save data stuff
    virtual bool IsSaved()
    {
        return true;
    }
    iLuxPlayerState_SaveData* CreateSaveData();

    void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
    void LoadFromSaveDataBeforeEnter(cLuxMap *apMap,iLuxPlayerState_SaveData* apSaveData);
    void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);

protected:
    cLuxInteractData_Slide *mpSlideData;

    cPidController<cVector3f> mForcePid;

    iPhysicsJoint *mpCurrentJoint;
    bool mbHasGravity;

    float mfMaxDistance;
    cVector3f mvLocalInteractPos;

    cVector2f mvMouseAdd;
    cVector2f mvLastMouseAdd;
    cVector3f mvLastForce;

    float mfSlideSpeed;

    float mfMoveToMouseAddFactor;
    float mfMaxForce;

};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_INTERACT_SLIDE_H
