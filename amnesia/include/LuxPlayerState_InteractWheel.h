#ifndef LUX_PLAYER_STATE_INTERACT_WHEEL_H
#define LUX_PLAYER_STATE_INTERACT_WHEEL_H

//----------------------------------------------

#include "LuxPlayerState_InteractRotateBase.h"

//----------------------------------------------

class cLuxInteractData_Wheel;

//------------------------------------

class cLuxPlayerState_InteractWheel_SaveData : public iLuxPlayerState_InteractRotateBase_SaveData
{
    kSerializableClassInit(cLuxPlayerState_InteractWheel_SaveData)
public:

};
//----------------------------------------------

class cLuxPlayerState_InteractWheel : public iLuxPlayerState_InteractRotateBase
{
typedef iLuxPlayerState_InteractRotateBase super_class;
public:    
    cLuxPlayerState_InteractWheel(cLuxPlayer *apPlayer);
    virtual ~cLuxPlayerState_InteractWheel();

    void RenderSolid(cRendererCallbackFunctions* apFunctions);

    /////////////////////////////////
    //Save data stuff
    virtual bool IsSaved(){ return false; }
    iLuxPlayerState_SaveData* CreateSaveData();

    void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
    void LoadFromSaveDataBeforeEnter(cLuxMap *apMap,iLuxPlayerState_SaveData* apSaveData);
    void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);

protected:
    void EnterRotateBase(eLuxPlayerState aPrevState);
    void LeaveRotateBase(eLuxPlayerState aNewState);

    float GetSpeedAdd(cCamera *apCam);
    void OnThrow();

    cLuxInteractData_Wheel *mpWheelData;

    std::list<cVector2f> mlstMoveAdds;
};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_INTERACT_WHEEL_H
