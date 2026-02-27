#ifndef LUX_PLAYER_STATE_HANDOBJECT_H
#define LUX_PLAYER_STATE_HANDOBJECT_H

//----------------------------------------------

#include "LuxPlayerState_DefaultBase.h"

//----------------------------------------------

class cLuxPlayer;

//------------------------------------

class cLuxPlayerState_HandObject_SaveData : public iLuxPlayerState_DefaultBase_SaveData
{
    kSerializableClassInit(cLuxPlayerState_HandObject_SaveData)
public:

};

//----------------------------------------------

class cLuxPlayerState_HandObject : public iLuxPlayerState_DefaultBase
{
typedef iLuxPlayerState_DefaultBase super_class;
public:    
    cLuxPlayerState_HandObject(cLuxPlayer *apPlayer);
    virtual ~cLuxPlayerState_HandObject();

    
    bool ImplementedDoAction(eLuxPlayerAction aAction,bool abPressed);
    void ImplementedOnEnterState(eLuxPlayerState aPrevState);
    void ImplementedOnLeaveState(eLuxPlayerState aNewState);
    
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


#endif // LUX_PLAYER_STATE_HANDOBJECT_H
