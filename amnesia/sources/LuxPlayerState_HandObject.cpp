#include "LuxPlayerState_HandObject.h"

#include "LuxPlayer.h"
#include "LuxMapHelper.h"
#include "LuxProp.h"
#include "LuxMoveState.h"
#include "LuxEffectRenderer.h"
#include "LuxPlayerHands.h"



//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerState_HandObject::cLuxPlayerState_HandObject(cLuxPlayer *apPlayer) : iLuxPlayerState_DefaultBase(apPlayer, eLuxPlayerState_HandObject)
{
}

//-----------------------------------------------------------------------

cLuxPlayerState_HandObject::~cLuxPlayerState_HandObject()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxPlayerState_HandObject::ImplementedDoAction(eLuxPlayerAction aAction,bool abPressed)
{
    ////////////////////
    // Holster
    if(aAction == eLuxPlayerAction_Holster && abPressed)
    {
        mpPlayer->ChangeState(eLuxPlayerState_Normal);
        return false;
    }
    ////////////////////
    // Attack
    if(aAction == eLuxPlayerAction_Attack)
    {
        mpPlayer->GetHands()->DoAction(aAction, abPressed);
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------

void cLuxPlayerState_HandObject::ImplementedOnEnterState(eLuxPlayerState aPrevState)
{
    /////////////////////////////////
    //Set the current hand object
    mpPlayer->SetCurrentHandObjectDrawn(true);
}

//-----------------------------------------------------------------------

void cLuxPlayerState_HandObject::ImplementedOnLeaveState(eLuxPlayerState aNewState)
{
    //TODO: More states here?
    if(    aNewState == eLuxPlayerState_Normal ||
            aNewState == eLuxPlayerState_InteractPush ||
            aNewState == eLuxPlayerState_InteractGrab)
    {
        mpPlayer->SetCurrentHandObjectDrawn(false);
    }
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxPlayerState_HandObject_SaveData, iLuxPlayerState_DefaultBase_SaveData)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxPlayerState_SaveData* cLuxPlayerState_HandObject::CreateSaveData()
{
    return hplNew(cLuxPlayerState_HandObject_SaveData, ());
}

//-----------------------------------------------------------------------


void cLuxPlayerState_HandObject::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
    ///////////////////////
    // Init
    super_class::SaveToSaveData(apSaveData);
    cLuxPlayerState_HandObject_SaveData *pData = static_cast<cLuxPlayerState_HandObject_SaveData*>(apSaveData);


    ///////////////////////
    // Save vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_HandObject::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
    ///////////////////////
    // Init
    super_class::LoadFromSaveDataBeforeEnter(apMap,apSaveData);
    cLuxPlayerState_HandObject_SaveData *pData = static_cast<cLuxPlayerState_HandObject_SaveData*>(apSaveData);

    ///////////////////////
    // Load vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_HandObject::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
    ///////////////////////
    // Init
    super_class::LoadFromSaveDataAfterEnter(apMap,apSaveData);
    cLuxPlayerState_HandObject_SaveData *pData = static_cast<cLuxPlayerState_HandObject_SaveData*>(apSaveData);

    ///////////////////////
    // Load vars
}

//-----------------------------------------------------------------------


