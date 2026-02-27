#ifndef LUX_PLAYER_STATE_INTERACT_LEVER_H
#define LUX_PLAYER_STATE_INTERACT_LEVER_H

//----------------------------------------------

#include "LuxPlayerState_InteractRotateBase.h"

//----------------------------------------------

class cLuxInteractData_Lever;

//------------------------------------

class cLuxPlayerState_InteractLever_SaveData : public iLuxPlayerState_InteractRotateBase_SaveData
{
    kSerializableClassInit(cLuxPlayerState_InteractLever_SaveData)
public:

};
//----------------------------------------------

class cLuxPlayerState_InteractLever : public iLuxPlayerState_InteractRotateBase
{
typedef iLuxPlayerState_InteractRotateBase super_class;
public:    
    cLuxPlayerState_InteractLever(cLuxPlayer *apPlayer);
    virtual ~cLuxPlayerState_InteractLever();

    void RenderSolid(cRendererCallbackFunctions* apFunctions);

    /////////////////////////////////
    //Save data stuff
    virtual bool IsSaved(){ return false; }
    iLuxPlayerState_SaveData* CreateSaveData();

    void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
    void LoadFromSaveDataBeforeEnter(cLuxMap *apMap,iLuxPlayerState_SaveData* apSaveData);
    void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);

protected:
    float GetSpeedAdd(cCamera *apCam);
    void OnThrow();

    cLuxInteractData_Lever *mpLeverData;

};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_INTERACT_LEVER_H
