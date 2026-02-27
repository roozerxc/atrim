#include "LuxPlayerState_InteractSwingDoor.h"

#include "LuxPlayer.h"
#include "LuxMapHandler.h"
#include "LuxMap.h"
#include "LuxMapHelper.h"
#include "LuxProp.h"
#include "LuxMoveState_Normal.h"
#include "LuxInputHandler.h"

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPlayerState_InteractSwingDoor::cLuxPlayerState_InteractSwingDoor(cLuxPlayer *apPlayer) : iLuxPlayerState_InteractRotateBase(apPlayer, eLuxPlayerState_InteractSwingDoor)
{
    
}

//-----------------------------------------------------------------------

cLuxPlayerState_InteractSwingDoor::~cLuxPlayerState_InteractSwingDoor()
{
    
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

float cLuxPlayerState_InteractSwingDoor::GetSpeedAdd(cCamera *apCam)
{
    cVector3f vBodyCenter = cMath::MatrixMul(mpCurrentBody->GetLocalMatrix(), mpCurrentBody->GetMassCentre());
    cVector3f vJointToBody =  cMath::Vector3Normalize(vBodyCenter - mpCurrentJoint->GetPivotPoint());

    cVector3f vUp = apCam->GetUp();
    cVector3f vRight = apCam->GetRight();
    cVector3f vForward = apCam->GetForward();

    cVector3f vPushAmount = (vUp + vForward) * mvMouseAdd.y + vRight * -mvMouseAdd.x;
    cVector3f vPushRotateDir = cMath::Vector3Cross(vJointToBody, vPushAmount);

    float fSpeedAdd = cMath::Vector3Dot(vPushRotateDir, mpCurrentJoint->GetPinDir());
    
    return fSpeedAdd;
}

void cLuxPlayerState_InteractSwingDoor::OnThrow()
{
    cVector3f vBodyCenter = cMath::MatrixMul(mpCurrentBody->GetLocalMatrix(), mpCurrentBody->GetMassCentre());
    cVector3f vJointToBody =  cMath::Vector3Normalize(vBodyCenter - mpCurrentJoint->GetPivotPoint());
    mvJointForward = cMath::Vector3Cross(mpCurrentJoint->GetPinDir(), vJointToBody);

    cVector3f vImpulse = mvJointForward * mpMoveBaseData->mfMoveThrowImpulse;
    float fForwardDot = cMath::Vector3Dot(mvJointForward, mpPlayer->GetCamera()->GetForward());
    if(fForwardDot < 0) vImpulse = vImpulse * -1;

    mpCurrentBody->AddImpulse(vImpulse);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxPlayerState_InteractSwingDoor_SaveData, iLuxPlayerState_InteractRotateBase_SaveData)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxPlayerState_SaveData* cLuxPlayerState_InteractSwingDoor::CreateSaveData()
{
    return hplNew(cLuxPlayerState_InteractSwingDoor_SaveData, ());
}

//-----------------------------------------------------------------------


void cLuxPlayerState_InteractSwingDoor::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
    ///////////////////////
    // Init
    super_class::SaveToSaveData(apSaveData);
    cLuxPlayerState_InteractSwingDoor_SaveData *pData = static_cast<cLuxPlayerState_InteractSwingDoor_SaveData*>(apSaveData);


    ///////////////////////
    // Save vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractSwingDoor::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
    ///////////////////////
    // Init
    super_class::LoadFromSaveDataBeforeEnter(apMap,apSaveData);
    cLuxPlayerState_InteractSwingDoor_SaveData *pData = static_cast<cLuxPlayerState_InteractSwingDoor_SaveData*>(apSaveData);

    ///////////////////////
    // Load vars
}

//-----------------------------------------------------------------------

void cLuxPlayerState_InteractSwingDoor::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
    ///////////////////////
    // Init
    super_class::LoadFromSaveDataAfterEnter(apMap,apSaveData);
    cLuxPlayerState_InteractSwingDoor_SaveData *pData = static_cast<cLuxPlayerState_InteractSwingDoor_SaveData*>(apSaveData);

    ///////////////////////
    // Load vars
}

//-----------------------------------------------------------------------





