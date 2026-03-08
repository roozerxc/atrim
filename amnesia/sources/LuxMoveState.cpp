#include "LuxMoveState.h"

#include "LuxPlayer.h"

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxMoveState::iLuxMoveState(cLuxPlayer *apPlayer)
{
    mpPlayer = apPlayer;

    mfMaxForwardSpeed = 0.0f;
    mfMaxBackwardSpeed = 0.0f;
    mfMaxSidwaySpeed = 0.0f;

    mfForwardAcc = gpBase->mpGameCfg->GetFloat("Player_Movement_Default","ForwardAcc",0);
    mfSidewayAcc = gpBase->mpGameCfg->GetFloat("Player_Movement_Default","SidewayAcc",0);
    mfForwardDeacc = gpBase->mpGameCfg->GetFloat("Player_Movement_Default","ForwardDeacc",0);
    mfSidewayDeacc = gpBase->mpGameCfg->GetFloat("Player_Movement_Default","SidewayDeacc",0);

    mfForwardOppositeDirAccMul = gpBase->mpGameCfg->GetFloat("Player_Movement_Default","ForwardOppositeDirAccMul",0);
    mfSidewayOppositeDirAccMul = gpBase->mpGameCfg->GetFloat("Player_Movement_Default","SidewayOppositeDirAccMul",0);

    mfMaxForwardSpeedMul = 1.0f;
    mfMaxBackwardSpeedMul = 1.0f;
    mfMaxSidwaySpeedMul = 1.0f;
}

//-----------------------------------------------------------------------

iLuxMoveState::~iLuxMoveState()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxMoveState::Update(float afTimeStep)
{
    //////////////////////////////
    // Move state specific update
    OnUpdate(afTimeStep);

    //////////////////////////////
    // Set move state variables
    iCharacterBody *pCharBody = gpBase->mpPlayer->GetCharacterBody();

    pCharBody->SetMaxPositiveMoveSpeed(eCharDir_Forward,mfMaxForwardSpeed * mfMaxForwardSpeedMul);
    pCharBody->SetMaxNegativeMoveSpeed(eCharDir_Forward,-mfMaxBackwardSpeed * mfMaxBackwardSpeedMul);
    pCharBody->SetMaxPositiveMoveSpeed(eCharDir_Right,mfMaxSidwaySpeed * mfMaxSidwaySpeedMul);
    pCharBody->SetMaxNegativeMoveSpeed(eCharDir_Right,-mfMaxSidwaySpeed * mfMaxSidwaySpeedMul);
    pCharBody->SetMoveAcc(eCharDir_Forward,mfForwardAcc);
    pCharBody->SetMoveAcc(eCharDir_Right,mfSidewayAcc);
    pCharBody->SetMoveDeacc(eCharDir_Forward,mfForwardDeacc);
    pCharBody->SetMoveDeacc(eCharDir_Right,mfSidewayDeacc);
    pCharBody->SetMoveOppositeDirAccMul(eCharDir_Forward,mfForwardOppositeDirAccMul);
    pCharBody->SetMoveOppositeDirAccMul(eCharDir_Right,    mfSidewayOppositeDirAccMul);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------



