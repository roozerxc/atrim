#ifndef LUX_PLAYER_STATE_NORMAL_H
#define LUX_PLAYER_STATE_NORMAL_H

//----------------------------------------------

#include "LuxPlayerState_DefaultBase.h"

//----------------------------------------------

class cLuxPlayer;

//------------------------------------

class cLuxPlayerState_Normal_SaveData : public iLuxPlayerState_DefaultBase_SaveData
{
    kSerializableClassInit(cLuxPlayerState_Normal_SaveData)
public:

};

//----------------------------------------------

class cLuxPlayerState_Normal : public iLuxPlayerState_DefaultBase
{
typedef iLuxPlayerState_DefaultBase super_class;
public:    
    cLuxPlayerState_Normal(cLuxPlayer *apPlayer);
    virtual ~cLuxPlayerState_Normal();

    bool ImplementedDoAction(eLuxPlayerAction aAction,bool abPressed);
    
    /////////////////////////////////
    //Save data stuff
    virtual bool IsSaved(){ return true; }
    iLuxPlayerState_SaveData* CreateSaveData();

    void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
    void LoadFromSaveDataBeforeEnter(cLuxMap *apMap,iLuxPlayerState_SaveData* apSaveData);
    void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);

protected:
};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_NORMAL_H
