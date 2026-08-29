#ifndef LUX_PLAYER_STATE_INTERACT_SWING_DOOR_H
#define LUX_PLAYER_STATE_INTERACT_SWING_DOOR_H

//----------------------------------------------

#include "LuxPlayerState_InteractRotateBase.h"

//----------------------------------------------

class cLuxInteractData_SwingDoor;

//------------------------------------

class cLuxPlayerState_InteractSwingDoor_SaveData : public iLuxPlayerState_InteractRotateBase_SaveData
{
    kSerializableClassInit(cLuxPlayerState_InteractSwingDoor_SaveData)
public:

};
//----------------------------------------------

class cLuxPlayerState_InteractSwingDoor : public iLuxPlayerState_InteractRotateBase
{
    typedef iLuxPlayerState_InteractRotateBase super_class;
public:
    cLuxPlayerState_InteractSwingDoor(cLuxPlayer *apPlayer);
    virtual ~cLuxPlayerState_InteractSwingDoor();

    /////////////////////////////////
    //Save data stuff
    virtual bool IsSaved()
    {
        return false;
    }
    iLuxPlayerState_SaveData* CreateSaveData();

    void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
    void LoadFromSaveDataBeforeEnter(cLuxMap *apMap,iLuxPlayerState_SaveData* apSaveData);
    void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);


protected:
    float GetSpeedAdd(cCamera *apCam);
    void OnThrow();

    cLuxInteractData_SwingDoor *mpSwingDoorData;
};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_INTERACT_SWING_DOOR_H
