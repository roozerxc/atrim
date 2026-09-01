#include "LuxEnemy_Grunt.h"

#include "LuxEnemyMover.h"
#include "LuxEnemyPathfinder.h"

#include "LuxMap.h"
#include "LuxMapHelper.h"
#include "LuxMusicHandler.h"
#include "LuxDebugHandler.h"
#include "LuxGlobalDataHandler.h"

#include "LuxPlayer.h"
#include "LuxPlayerHelpers.h"

#include "LuxProp.h"
#include "LuxProp_Object.h"
#include "LuxProp_SwingDoor.h"

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxEnemyLoader_Grunt::cLuxEnemyLoader_Grunt(const tString& asName) : iLuxEnemyLoader(asName)
{
}

//-----------------------------------------------------------------------

iLuxEnemy *cLuxEnemyLoader_Grunt::CreateEnemy(const tString& asName, int alID, cLuxMap *apMap)
{
    return hplNew(cLuxEnemy_Grunt, (asName, alID,apMap));
}

//-----------------------------------------------------------------------

void cLuxEnemyLoader_Grunt::LoadVariables(iLuxEnemy *apEnemy, cXmlElement *apRootElem)
{
    cLuxEnemy_Grunt *pGrunt = static_cast<cLuxEnemy_Grunt*>(apEnemy);

    pGrunt->msNoticeSound                   = GetVarString("NoticeSound");
    pGrunt->msGiveUpNoticeSound             = GetVarString("GiveUpNoticeSound");
    pGrunt->msEnabledSound                  = GetVarString("EnabledSound");

    pGrunt->mfGroggyDamageCount             = GetVarFloat("GroggyDamageCount", 0.0f);
    pGrunt->mfAlertToHuntDistance           = GetVarFloat("AlertToHuntDistance", 0.0f);
    pGrunt->mfAlertToInstantHuntDistance    = GetVarFloat("AlertToInstantHuntDistance", 0.0f);
    pGrunt->mfHuntPauseMinTime              = GetVarFloat("HuntPauseMinTime", 0.0f);
    pGrunt->mfHuntPauseMaxTime              = GetVarFloat("HuntPauseMaxTime", 0.0f);
    pGrunt->mfIncreaseAlertSpeedDistance    = GetVarFloat("IncreaseAlertSpeedDistance", 0.0f);
    pGrunt->mfIncreasedAlertSpeedMul        = GetVarFloat("IncreasedAlertSpeedMul", 0.0f);

    pGrunt->mfAlertRunTowardsToHuntLimit    = GetVarFloat("AlertRunTowardsToHuntLimit", 0.0f);
    pGrunt->mfAlertRunTowardsCheckDistance  = GetVarFloat("AlertRunTowardsCheckDistance", 0.0f);

    pGrunt->mfIdleExtraTimeMin              = GetVarFloat("IdleExtraTimeMin", 2.0f);
    pGrunt->mfIdleExtraTimeMax              = GetVarFloat("IdleExtraTimeMax", 6.0f);
    pGrunt->mlIdleExtraNum                  = GetVarInt("IdleExtraNum", 3);

    gpBase->PreloadSound(pGrunt->msNoticeSound);
    gpBase->PreloadSound(pGrunt->msGiveUpNoticeSound);
    gpBase->PreloadSound(pGrunt->msEnabledSound);
}

//-----------------------------------------------------------------------

void cLuxEnemyLoader_Grunt::LoadInstanceVariables(iLuxEnemy *apEnemy, cResourceVarsObject *apInstanceVars)
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxEnemy_Grunt::cLuxEnemy_Grunt(const tString &asName, int alID, cLuxMap *apMap) : iLuxEnemy(asName,alID,apMap, eLuxEnemyType_Grunt)
{
    mfAlertRunTowardsCount = 0.0f;
    mfWaitTime = 0.0f;

    mlHuntWanderCount = 0;

    mbAlignEntityWithGroundRay = true;

    mReturnState = eLuxEnemyState_Patrol;
}

//-----------------------------------------------------------------------

cLuxEnemy_Grunt::~cLuxEnemy_Grunt()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxEnemy_Grunt::StateEventImplement(int alState, eLuxEnemyStateEvent aEvent, cLuxStateMessage *apMessage)
{
    ////////////////////////////////
    // atrim state machine rewrite
    // by RoozerXC -- 09-01-2026

    kLuxBeginStateMachine
    {
        ////////////////////////////////
        // Reset all to default values
        kLuxOnMessage(eLuxEnemyMessage_Reset)
        {
            gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack, this);
            gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search, this);
            gpBase->mpPlayer->RemoveTerrorEnemy(this);
            
            ChangeState(eLuxEnemyState_Wait);
        }

        // Enemy spots player!
        kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
        {
            ChangeState(eLuxEnemyState_Alert);
        }

        // Enemy takes damage from an object
        kLuxOnMessage(eLuxEnemyMessage_TakeHit)
        {
            mfDamageCount += apMessage->mfCustomValue;
            if(mfDamageCount > mfGroggyDamageCount)
            {
                ChangeState(eLuxEnemyState_Hurt);
                mfDamageCount = 0.0f;
            }
        }

        // Enemy was hurt within proximity of the player
        kLuxOnMessage(eLuxEnemyMessage_HelpMe)
        {
            ShowPlayerPosition();
            ChangeState(eLuxEnemyState_Hunt);
        }

        // Player is out of range, despawn
        kLuxOnMessage(eLuxEnemyMessage_PlayerOutOfRange)
        {
            SetActive(false);
        }

        // Player is dead, don't do anything else yet!
        kLuxOnMessage(eLuxEnemyMessage_PlayerDead)
        {
            ChangeState(eLuxEnemyState_Idle);
        }
        
        // ------------------   STATES   ------------------ //
        
        ////////////////////////////////
        // Idle
        kLuxState(eLuxEnemyState_Idle)
        {
            // Global override
            kLuxOnMessage(eLuxEnemyMessage_PlayerDetected) { }

            kLuxOnEnter
            {
                mbPlayerInRange = false;
                SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
            }
            
            // Enemy is within the vicinity of the player
            kLuxOnMessage(eLuxEnemyMessage_PlayerInRange)
            {
                gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName) + _W(" enabled!"), false);

                if(gpBase->mpGlobalDataHandler->GetEnemyActivateSoundAllowed())
                {
                    gpBase->mpGlobalDataHandler->SetEnemyActivateSoundMade();
                    PlaySound(msEnabledSound);
                }

                if(mvPatrolNodes.empty())
                {
                    ChangeState(eLuxEnemyState_Wait);
                }
                else
                {
                    ChangeState(eLuxEnemyState_Patrol);
                }
            }
        }

        ////////////////////////////////
        // Wait
        kLuxState(eLuxEnemyState_Wait)
        {
            kLuxOnEnter
            {
                mpMover->ResetStuckCounter();
                mpPathfinder->Stop();

                if(mfWaitTime <= 0.0f)
                {
                    SendMessage(eLuxEnemyMessage_TimeOut, cMath::RandRectf(1.0f, 3.0f), true);
                }
                else
                {
                    SendMessage(eLuxEnemyMessage_TimeOut, mfWaitTime, true);
                }
                mfWaitTime = 0.0f;

                SendMessage(eLuxEnemyMessage_TimeOut_2, cMath::RandRectf(mfIdleExtraTimeMin, mfIdleExtraTimeMax), true);
                SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
            }

            kLuxOnMessage(eLuxEnemyMessage_TimeOut)
            {
                if(mPreviousState == eLuxEnemyState_Search)
                {
                    FadeOutCurrentAnim(0.2f);
                    ChangeState(eLuxEnemyState_Search);
                }
                else
                {
                    if(GetPatrolNodeNum() > 0)
                    {
                        FadeOutCurrentAnim(0.2f);
                        ChangeState(eLuxEnemyState_Patrol);
                    }
                    else
                    {
                        SendMessage(eLuxEnemyMessage_TimeOut, cMath::RandRectf(3.0f, 5.0f), true);
                    }
                }
            }

            kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
            {
                PlayAnim("IdleExtra" + cString::ToString(cMath::RandRectl(1, mlIdleExtraNum)), false, 0.3f);
            }

            kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
            {
                SendMessage(eLuxEnemyMessage_TimeOut_2, cMath::RandRectf(2.0f, 6.0f), true);
            }

            kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
            {
                if(apMessage->mfCustomValue > mfHearVolume)
                {
                    ChangeState(eLuxEnemyState_Investigate);
                    mvTempPos = apMessage->mvCustomValue;
                    mfTempVal = apMessage->mfCustomValue;
                }
            }

            kLuxOnLeave
            {
                mpMover->SetOverideMoveState(false);
            }
        }

        ////////////////////////////////
        // Patrol
        kLuxState(eLuxEnemyState_Patrol)
        {
            kLuxOnUpdate
            {
                // BREAK DOOR CHECK
                if(mbStuckAtDoor)
                {
                    iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
                    bool bShouldBreak = false;

                    if(pDoorEnt && pDoorEnt->GetEntityType() == eLuxEntityType_Prop)
                    {
                        iLuxProp* pDoorProp = static_cast<iLuxProp*>(pDoorEnt);
                        if(pDoorProp->GetHealth() > 0.0f && !mpMap->DoorIsBroken(mlStuckDoorID))
                        {
                            bShouldBreak = true;
                        }
                    }

                    if(bShouldBreak)
                    {
                        mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();

                        mReturnState = mCurrentState;
                        ChangeState(eLuxEnemyState_BreakDoor);
                    }
                    else
                    {
                        mpMover->ResetStuckCounter();

                        mbStuckAtDoor = false;
                        mlStuckDoorID = -1;

                        ChangeState(eLuxEnemyState_Wait);
                    }
                }
            }

            kLuxOnEnter
            {
                ChangeSoundState(eLuxEnemySoundState_Idle);
                SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);

                gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack, this);
                gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search, this);
                gpBase->mpPlayer->RemoveTerrorEnemy(this);

                PatrolUpdateGoal();
            }

            kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
            {
                if(apMessage->mfCustomValue > mfHearVolume)
                {
                    ChangeState(eLuxEnemyState_Investigate);
                    mvTempPos = apMessage->mvCustomValue;
                    mfTempVal = apMessage->mfCustomValue;
                }
            }

            kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
            {
                PatrolEndOfPath();
            }
        }

        ////////////////////////////////
        // Investigate
        kLuxState(eLuxEnemyState_Investigate)
        {
            kLuxOnUpdate
            {
                // BREAK DOOR CHECK
                if(mbStuckAtDoor)
                {
                    iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
                    bool bShouldBreak = false;

                    if(pDoorEnt && pDoorEnt->GetEntityType() == eLuxEntityType_Prop)
                    {
                        iLuxProp* pDoorProp = static_cast<iLuxProp*>(pDoorEnt);
                        if(pDoorProp->GetHealth() > 0.0f && !mpMap->DoorIsBroken(mlStuckDoorID))
                        {
                            bShouldBreak = true;
                        }
                    }

                    if(bShouldBreak)
                    {
                        mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();

                        mReturnState = mCurrentState;
                        ChangeState(eLuxEnemyState_BreakDoor);
                    }
                    else
                    {
                        mpMover->ResetStuckCounter();

                        mbStuckAtDoor = false;
                        mlStuckDoorID = -1;

                        ChangeState(eLuxEnemyState_Wait);
                    }
                }
            }

            kLuxOnEnter
            {
                if(mPreviousState != eLuxEnemyState_BreakDoor)
                {
                    mReturnState = mPreviousState;
                }
                if(mReturnState == eLuxEnemyState_BreakDoor)
                {
                    mReturnState = eLuxEnemyState_Patrol;
                }

                ChangeSoundState(eLuxEnemySoundState_Alert);
                SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);

                mpPathfinder->Stop();

                PlayAnim("Notice"+cString::ToString(cMath::RandRectl(1, 2)), false, 0.3f);
                PlaySound(msNoticeSound);

                mfFOVMul = 4.0f;

                /*
                if(gpBase->mbHardMode)
                {
                    mfForwardSpeed *= 1.5f;
                }
                */

                // Output: Enemy servant_grunt_1 heard a sound!
                //         Volume: 0.755555555555555555f
                gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName) + _W(" heard a sound!"), false);
                gpBase->mpDebugHandler->AddMessage(_W("Volume: ") + cString::ToStringW(mfTempVal) + _W("f"), false);
            }
            
            kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
            {
                SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
                
                cAINode *pNode = mpPathfinder->GetNodeAtPos(mvTempPos, 0.0f, 10.0f, true, true, true, NULL, 1);
                if(pNode)
                {
                    mpPathfinder->MoveTo(pNode->GetPosition());
                }
                else
                {
                    gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName) +
                        _W(" could not find node near sound!"), false);
                    mpPathfinder->MoveTo(mvTempPos);
                }
            }

            // Enemy gets tired of trying to find the player
            kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
            {
                PlaySound(msGiveUpNoticeSound);
                
                if(mReturnState == eLuxEnemyState_Search)
                {
                    ChangeState(eLuxEnemyState_Search);
                }
                else
                {
                    ChangeState(eLuxEnemyState_Patrol);
                }
            }

            kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
            {
                if(mpPathfinder->IsMoving() && apMessage->mfCustomValue > mfTempVal)
                {
                    cAINode *pNode = mpPathfinder->GetNodeAtPos(apMessage->mvCustomValue, 0.0f, 10.0f, true, true, true, NULL, 1);
                    if(pNode)
                    {
                        mpPathfinder->MoveTo(pNode->GetPosition());
                    }

                    mvTempPos = apMessage->mvCustomValue;
                    mfTempVal = apMessage->mfCustomValue;
                }
            }

            kLuxOnLeave
            {
                FadeOutCurrentAnim(0.2f);
                mfFOVMul = 1.0f;

                SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
            }
        }

        ////////////////////////////////
        // Alert
        kLuxState(eLuxEnemyState_Alert)
        {
            kLuxOnUpdate
            {
                // BREAK DOOR CHECK
                if(mbStuckAtDoor && gpBase->mpPlayer->GetTerror() >= 1.0f)
                {
                    iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
                    bool bShouldBreak = false;

                    if(pDoorEnt && pDoorEnt->GetEntityType() == eLuxEntityType_Prop)
                    {
                        iLuxProp* pDoorProp = static_cast<iLuxProp*>(pDoorEnt);
                        if(pDoorProp->GetHealth() > 0.0f && !mpMap->DoorIsBroken(mlStuckDoorID))
                        {
                            bShouldBreak = true;
                        }
                    }

                    if(bShouldBreak)
                    {
                        mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();

                        mReturnState = mCurrentState;
                        ChangeState(eLuxEnemyState_BreakDoor);
                    }
                    else
                    {
                        mpMover->ResetStuckCounter();

                        mbStuckAtDoor = false;
                        mlStuckDoorID = -1;

                        ChangeState(eLuxEnemyState_Wait);
                    }
                }

                // Player is less than the check distance for pursuit
                if(DistToPlayer() < mfAlertRunTowardsCheckDistance)
                {
                    mfAlertRunTowardsCount += GetPlayerMovementTowardEnemyAmount();
                }
                else
                {
                    mfAlertRunTowardsCount -= 1.0f;
                }

                // Clamp it!
                if(mfAlertRunTowardsCount < 0.0f)
                {
                    mfAlertRunTowardsCount = 0.0f;
                }

                // Player is over the increased alert distance!
                if(DistToPlayer() > mfIncreaseAlertSpeedDistance)
                {
                    SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
                    mfForwardSpeed *= 1.2f * mfIncreasedAlertSpeedMul;
                }
                else
                {
                    SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
                    mfForwardSpeed *= 1.2f;
                }

                // Player is no longer seen! See if there's time to search...
                if(PlayerIsDetected() == false)
                {
                    if(gpBase->mpPlayer->GetTerror() < 1.0f)
                    {
                        ChangeState(eLuxEnemyState_Search);
                    }
                }
                // Player is seen! See if he's close enough to hunt him
                else if((gpBase->mpPlayer->GetTerror() >= 1.0f &&
                    (DistToPlayer() > mfAlertToHuntDistance ||
                    mfAlertRunTowardsCount > mfAlertRunTowardsToHuntLimit)) ||
                    DistToPlayer() < mfAlertToInstantHuntDistance)
                {
                    gpBase->mpPlayer->SetTerror(1.0f);
                    ChangeState(eLuxEnemyState_Hunt);
                }
            }

            kLuxOnEnter
            {
                ChangeSoundState(eLuxEnemySoundState_Alert);

                SendMessage(eLuxEnemyMessage_TimeOut, 0.3f, true);
                mpPathfinder->MoveTo(mvLastKnownPlayerPos);

                gpBase->mpPlayer->AddTerrorEnemy(this);

                SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
                mfForwardSpeed *= 1.2f;

                mfFOVMul = 4.0f;
                mfAlertRunTowardsCount = 0.0f;
            }

            kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
            {
                if(PlayerIsDetected() == false || (apMessage->mlCustomValue == 1 && DistToPlayer() > 5.0f))
                {
                    ChangeState(eLuxEnemyState_Search);
                }
            }

            kLuxOnMessage(eLuxEnemyMessage_TimeOut)
            {
                if(PlayerIsDetected())
                {
                    mpPathfinder->MoveTo(mvLastKnownPlayerPos);
                }

                SendMessage(eLuxEnemyMessage_TimeOut, 0.3f, true);
            }

            kLuxOnMessage(eLuxEnemyMessage_TakeHit)
            {
                gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search, this);

                ChangeState(eLuxEnemyState_Hurt);
            }

            kLuxOnLeave
            {
                mfFOVMul = 1.0f;
                
                if(mNextState != eLuxEnemyState_BreakDoor &&
                    mNextState != eLuxEnemyState_Hunt)
                {
                    SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
                    gpBase->mpPlayer->RemoveTerrorEnemy(this);
                }
            }
        }

        ////////////////////////////////
        // Search
        kLuxState(eLuxEnemyState_Search)
        {
            kLuxOnUpdate
            {
                // BREAK DOOR CHECK
                if(mbStuckAtDoor)
                {
                    iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
                    bool bShouldBreak = false;

                    if(pDoorEnt && pDoorEnt->GetEntityType() == eLuxEntityType_Prop)
                    {
                        iLuxProp* pDoorProp = static_cast<iLuxProp*>(pDoorEnt);
                        if(pDoorProp->GetHealth() > 0.0f && !mpMap->DoorIsBroken(mlStuckDoorID))
                        {
                            bShouldBreak = true;
                        }
                    }

                    if(bShouldBreak)
                    {
                        mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();

                        mReturnState = mCurrentState;
                        ChangeState(eLuxEnemyState_BreakDoor);
                    }
                    else
                    {
                        mpMover->ResetStuckCounter();

                        mbStuckAtDoor = false;
                        mlStuckDoorID = -1;

                        ChangeState(eLuxEnemyState_Wait);
                    }
                }
            }

            kLuxOnEnter
            {
                ChangeSoundState(eLuxEnemySoundState_Alert);

                SendMessage(eLuxEnemyMessage_TimeOut, mfPlayerSearchTime, true);
                SendMessage(eLuxEnemyMessage_TimeOut_2, cMath::RandRectf(0.0f, 1.0f), true);

                gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack, this);
                gpBase->mpMusicHandler->AddEnemy(eLuxEnemyMusic_Search, this);
                gpBase->mpPlayer->RemoveTerrorEnemy(this);

                SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
                mfForwardSpeed *= 1.0f;
            }

            kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
            {
                if(apMessage->mfCustomValue > mfHearVolume)
                {
                    ChangeState(eLuxEnemyState_Investigate);
                    mvTempPos = apMessage->mvCustomValue;
                    mfTempVal = apMessage->mfCustomValue;
                }
            }

            kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
            {
                mpPathfinder->Stop();

                SendMessage(eLuxEnemyMessage_TimeOut_2, cMath::RandRectf(1.0f, 3.0f), true);
            }
            
            kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
            {
                cAINode *pNode = mpPathfinder->GetNodeAtPos(gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition(), 4.0f, 12.0f, false, false, true, NULL);
                if(pNode)
                {
                    mpPathfinder->MoveTo(pNode->GetPosition());
                }
                else
                {
                    ChangeState(eLuxEnemyState_Patrol);
                }
            }
            
            kLuxOnMessage(eLuxEnemyMessage_TimeOut)
            {
                gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack, this);
                gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search, this);
                gpBase->mpPlayer->RemoveTerrorEnemy(this);

                ChangeState(eLuxEnemyState_Patrol);
            }

            kLuxOnLeave
            {
                SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
            }
        }

        ////////////////////////////////
        // Hunt
        kLuxState(eLuxEnemyState_Hunt)
        {
            // Overload globals
            kLuxOnMessage(eLuxEnemyMessage_PlayerDetected) { }
            kLuxOnMessage(eLuxEnemyMessage_HelpMe) { }

            kLuxOnUpdate
            {
                // BREAK DOOR CHECK
                if(mbStuckAtDoor)
                {
                    iLuxEntity *pDoorEnt = mpMap->GetEntityByID(mlStuckDoorID);
                    bool bShouldBreak = false;

                    if(pDoorEnt && pDoorEnt->GetEntityType() == eLuxEntityType_Prop)
                    {
                        iLuxProp* pDoorProp = static_cast<iLuxProp*>(pDoorEnt);
                        if(pDoorProp->GetHealth() > 0.0f && !mpMap->DoorIsBroken(mlStuckDoorID))
                        {
                            bShouldBreak = true;
                        }
                    }

                    if(bShouldBreak)
                    {
                        mvTempPos = pDoorEnt->GetAttachEntity()->GetWorldPosition();

                        mReturnState = mCurrentState;
                        ChangeState(eLuxEnemyState_BreakDoor);
                    }
                    else
                    {
                        mpMover->ResetStuckCounter();

                        mbStuckAtDoor = false;
                        mlStuckDoorID = -1;

                        ChangeState(eLuxEnemyState_Wait);
                    }
                }

                // Short attack
                if(CanSeePlayer() && DistToPlayer() < mfNormalAttackDistance)
                {
                    ChangeState(eLuxEnemyState_AttackMeleeShort);
                }
            }

            kLuxOnEnter
            {
                if(mlAttackHitCounter >= 1)
                {
                    mlAttackHitCounter = 0;

                    gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search, this);
                    ChangeState(eLuxEnemyState_HuntPause);
                }
                else
                {
                    ChangeSoundState(eLuxEnemySoundState_Hunt);

                    SetMoveSpeed(eLuxEnemyMoveSpeed_Run);
                    SendMessage(eLuxEnemyMessage_TimeOut, 0.1f, true);

                    mfFOVMul = 4.0f;

                    gpBase->mpMusicHandler->AddEnemy(eLuxEnemyMusic_Attack, this);
                    gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search, this);
                    gpBase->mpPlayer->AddTerrorEnemy(this);

                    mpPathfinder->MoveTo(mvLastKnownPlayerPos);
                    SendMessage(eLuxEnemyMessage_TimeOut_2, 0.6f, true);
                }
            }

            // Check if enemy is close enough to lunge towards the player.
            kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
            {
                if(CanSeePlayer() && DistToPlayer() > mfNormalAttackDistance &&
                    DistToPlayer() < mfNormalAttackDistance * 2.0f &&
                    mpMover->GetStuckCounter() < 0.5f)
                {
                    ChangeState(eLuxEnemyState_AttackMeleeLong);
                }
                SendMessage(eLuxEnemyMessage_TimeOut_2, 0.6f, true);
            }

            kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
            {
                if(apMessage->mlCustomValue == 1 && DistToPlayer() >= mfNormalAttackDistance)
                {
                    if(PlayerIsDetected() == false)
                    {
                        // Give him some time to find the player!
                        SendMessage(eLuxEnemyMessage_TimeOut_3, 1.5f, true);
                    }
                    else
                    {
                        mlHuntWanderCount++;
                        
                        if(mlHuntWanderCount > 3)
                        {
                            mlHuntWanderCount = 0;
                            ChangeState(eLuxEnemyState_HuntPause);
                        }
                        else
                        {
                            ChangeState(eLuxEnemyState_HuntWander);
                        }
                    }
                }
                else
                {
                    mpPathfinder->MoveTo(mvLastKnownPlayerPos);
                    
                    if(PlayerIsDetected() == false)
                    {
                        // Give him extra time to catch up and find the player
                        SendMessage(eLuxEnemyMessage_TimeOut_3, 1.5f, true);
                    }
                }
            }

            // Update the path and call for help
            kLuxOnMessage(eLuxEnemyMessage_TimeOut)
            {
                if(mpMover->GetWantedSpeedAmount() > 0.5f)
                {
                    mlHuntWanderCount = 0;
                }
                else
                {
                    mlHuntWanderCount = cMath::Max(0, mlHuntWanderCount - 1);
                }

                mpMap->BroadcastEnemyMessage(eLuxEnemyMessage_HelpMe, true, mpCharBody->GetPosition(),
                    mfActivationDistance * 0.5f, 0.0f, false, mpCharBody->GetFeetPosition());

                mpPathfinder->MoveTo(mvLastKnownPlayerPos);
                SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true);
            }

            // Still lost player, give him one more chance...
            kLuxOnMessage(eLuxEnemyMessage_TimeOut_3)
            {
                if(PlayerIsDetected() == false)
                {
                    gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack, this);
                    gpBase->mpPlayer->RemoveTerrorEnemy(this);

                    ChangeState(eLuxEnemyState_Search);
                }
                else
                {
                    SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true);
                }
            }

            kLuxOnLeave
            {
                SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
                mfFOVMul = 1.0f;
            }
        }

        ////////////////////////////////
        // Hunt Wander (Keep hunting but go to a nearby reachable node)
        kLuxState(eLuxEnemyState_HuntWander)
        {
            // Overload globals
            kLuxOnMessage(eLuxEnemyMessage_PlayerDetected) { }
            kLuxOnMessage(eLuxEnemyMessage_HelpMe) { }

            kLuxOnEnter
            {
                SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);

                mfForwardSpeed *= 1.2f;
                mfFOVMul = 4.0f;

                cAINode *pNode = mpPathfinder->GetNodeAtPos(mpCharBody->GetFeetPosition(), 2.0f, 5.0f, false, true, true, NULL);
                if(pNode)
                {
                    mpPathfinder->MoveTo(pNode->GetPosition());
                }
                else
                {
                    ChangeState(eLuxEnemyState_Hunt);
                }
            }

            kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
            {
                ChangeState(eLuxEnemyState_Hunt);
            }

            kLuxOnLeave
            {
                mfFOVMul = 1.0f;
            }
        }

        ////////////////////////////////
        // Hunt Pause (Slowly walk towards the player)
        kLuxState(eLuxEnemyState_HuntPause)
        {
            // Overload globals
            kLuxOnMessage(eLuxEnemyMessage_PlayerDetected) { }
            kLuxOnMessage(eLuxEnemyMessage_HelpMe) { }

            kLuxOnEnter
            {
                mpMover->ResetStuckCounter();
                mpPathfinder->Stop();

                SendMessage(eLuxEnemyMessage_TimeOut_2, cMath::RandRectf(mfHuntPauseMinTime, mfHuntPauseMaxTime), true);

                SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
                mfForwardSpeed *= 0.6f;
                mfFOVMul = 4.0f;

                SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true);
            }

            kLuxOnMessage(eLuxEnemyMessage_TimeOut)
            {
                if(DistToPlayer2D() > 2.0f)
                {
                    mpPathfinder->MoveTo(gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition());
                }

                if(CanSeePlayer())
                {
                    SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true);
                }
            }
            
            kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
            {
                ChangeState(eLuxEnemyState_Alert);
            }

            kLuxOnLeave
            {
                mfFOVMul = 1.0f;
            }
        }

        ////////////////////////////////
        // Break Door
        kLuxState(eLuxEnemyState_BreakDoor)
        {
            // Overload globals
            kLuxOnMessage(eLuxEnemyMessage_PlayerDetected) { }
            kLuxOnMessage(eLuxEnemyMessage_HelpMe) { }

            kLuxOnUpdate
            {
                // Turn towards the door
                mpMover->TurnToPos(mvTempPos);
            }

            kLuxOnEnter
            {
                mpMover->ResetStuckCounter();
                mpPathfinder->Stop();

                PlayAnim("BreakDoor", false, 0.3f);
                
                mfFOVMul = 4.0f;
                mlTempVal = 0;
            }

            kLuxOnMessage(eLuxEnemyMessage_AnimationSpecialEvent)
            {
                Attack(mNormalAttackSize, mBreakDoorAttackDamage);
            }

            kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
            {
                mlTempVal++;
                const int kMaxBreakDoorAttempts = 10;

                iLuxEntity *pDoorEntity = mpMap->GetEntityByID(mlStuckDoorID);
                bool bDoorIsBroken = false;

                if(pDoorEntity == NULL || pDoorEntity->GetEntityType() != eLuxEntityType_Prop)
                {
                    bDoorIsBroken = true;

                    // Map issue?
                    if(pDoorEntity == NULL)
                    {
                        gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName) + 
                            _W(" is stuck at invalid door ID: ") + cString::ToStringW(mlStuckDoorID), false);
                    }
                    else
                    {
                        gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName) + 
                            _W(" is stuck at non-prop entity ID: ") + cString::ToStringW(mlStuckDoorID), false);
                    }
                }
                else
                {
                    iLuxProp* pDoorProp = static_cast<iLuxProp*>(pDoorEntity);

                    if(pDoorProp->GetHealth() <= 0.01f)
                    {
                        bDoorIsBroken = true;
                    }
                    else if(pDoorProp->GetPropType() == eLuxPropType_SwingDoor)
                    {
                        cLuxProp_SwingDoor* pSwingDoor = static_cast<cLuxProp_SwingDoor*>(pDoorProp);

                        if(pSwingDoor->IsBroken())
                        {
                            bDoorIsBroken = true;
                        }
                    }
                }

                float fDistanceToDoor = cMath::Vector3Dist(mpCharBody->GetPosition(), mvTempPos);
                bool bEnemyIsTooFar = (fDistanceToDoor > 2.5f);

                if(PlayerIsDetected())
                {
                    ChangeState(eLuxEnemyState_Alert);
                }
                else if(bDoorIsBroken || bEnemyIsTooFar || mlTempVal >= kMaxBreakDoorAttempts)
                {
                    mpMover->ResetStuckCounter();
                    mpPathfinder->Stop();

                    mbStuckAtDoor = false;
                    mlStuckDoorID = -1;

                    if(bDoorIsBroken)
                    {
                        gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName) + 
                            _W(" successfully broke door!"), false);
                    }
                    else if(bEnemyIsTooFar)
                    {
                        gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName) + 
                            _W(" is too far from door! (") + cString::ToStringW(fDistanceToDoor) + _W(" units)"), false);
                    }
                    else if(mlTempVal >= kMaxBreakDoorAttempts)
                    {
                        gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName) + 
                            _W(" gave up breaking door after ") + cString::ToStringW(mlTempVal) + _W(" attempts!"), false);
                    }

                    if(mReturnState == eLuxEnemyState_BreakDoor ||
                        mReturnState == eLuxEnemyState_Alert ||
                        mReturnState == eLuxEnemyState_Hunt ||
                        mReturnState == eLuxEnemyState_HuntPause ||
                        mReturnState == eLuxEnemyState_HuntWander)
                    {
                        ChangeState(eLuxEnemyState_Hunt);
                    }
                    else
                    {
                        ChangeState(mReturnState);
                    }
                }
                else
                {
                    PlayAnim("BreakDoor", false, 0.3f);
                }
            }

            kLuxOnLeave
            {
                mpMover->ResetStuckCounter();
                mpPathfinder->Stop();

                mbStuckAtDoor = false;
                mlAttackHitCounter = 0;
                mfFOVMul = 1.0f;

                if(mReturnState == eLuxEnemyState_BreakDoor)
                {
                    mReturnState = eLuxEnemyState_Patrol;
                }

                if(mNextState != eLuxEnemyState_Alert &&
                    mNextState != eLuxEnemyState_Hunt &&
                    mNextState != eLuxEnemyState_HuntPause &&
                    mNextState != eLuxEnemyState_HuntWander)
                {
                    gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack, this);
                    gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search, this);
                    gpBase->mpPlayer->RemoveTerrorEnemy(this);
                }
            }
        }

        ////////////////////////////////
        // Attack Melee Short
        kLuxState(eLuxEnemyState_AttackMeleeShort)
        {
            // Overload globals
            kLuxOnMessage(eLuxEnemyMessage_PlayerDetected) { }
            kLuxOnMessage(eLuxEnemyMessage_HelpMe) { }

            kLuxOnEnter
            {
                mpMover->ResetStuckCounter();
                mpPathfinder->Stop();

                int lNum = cMath::RandRectl(1, 2);

                PlayAnim("SwingClaws0" + cString::ToString(lNum), false, 0.3f);
                mfFOVMul = 4.0f;
            }

            kLuxOnMessage(eLuxEnemyMessage_AnimationSpecialEvent)
            {
                Attack(mNormalAttackSize, mNormalAttackDamage);
            }

            kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
            {
                if(mPreviousState == eLuxEnemyState_Hurt)
                {
                    ChangeState(eLuxEnemyState_Hunt);
                }
                else
                {
                    ChangeState(mPreviousState);
                }
            }

            kLuxOnLeave
            {
                mfFOVMul = 1.0f;
            }
        }

        ////////////////////////////////
        // Attack Melee Long
        kLuxState(eLuxEnemyState_AttackMeleeLong)
        {
            // Overload globals
            kLuxOnMessage(eLuxEnemyMessage_PlayerDetected) { }
            kLuxOnMessage(eLuxEnemyMessage_HelpMe) { }

            kLuxOnEnter
            {
                mpMover->ResetStuckCounter();
                mpPathfinder->Stop();
                mlTempVal = 0;

                SetMoveSpeed(eLuxEnemyMoveSpeed_Run);
                mfForwardSpeed *= 1.5f;
                
                PlayAnim("SwingLaunch", false, 0.3f);
                
                mfFOVMul = 4.0f;
            }

            kLuxOnMessage(eLuxEnemyMessage_AnimationSpecialEvent)
            {
                mlTempVal = 1;

                Attack(mNormalAttackSize, mNormalAttackDamage);
            }

            kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
            {
                ChangeState(mPreviousState);
            }

            kLuxOnUpdate
            {
                if(mlTempVal == 0)
                {
                    mpMover->MoveToPos(gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition());
                }
            }

            kLuxOnLeave
            {
                SetMoveSpeed(eLuxEnemyMoveSpeed_Run);
                mfFOVMul = 1.0f;
            }
        }

        ////////////////////////////////
        // Hurt
        kLuxState(eLuxEnemyState_Hurt)
        {
            // Overload globals
            kLuxOnMessage(eLuxEnemyMessage_PlayerDetected) { }
            kLuxOnMessage(eLuxEnemyMessage_HelpMe) { }

            kLuxOnEnter
            {
                mpMover->ResetStuckCounter();
                mpPathfinder->Stop();

                PlayAnim("Flinch", false, 0.5f);

                SendMessage(eLuxEnemyMessage_TimeOut, 0.2f, true);
                mfFOVMul = 4.0f;
            }

            kLuxOnMessage(eLuxEnemyMessage_AnimationOver)
            {
                if(PlayerIsDetected())
                {
                    ChangeState(eLuxEnemyState_Hunt);
                }
                else
                {
                    ChangeState(eLuxEnemyState_Alert);
                }
            }
            
            kLuxOnMessage(eLuxEnemyMessage_TimeOut)
            {
                if(cMath::RandRectl(0, 1) == 0 && DistToPlayer() < mfNormalAttackDistance * 1.3f)
                {
                    ChangeState(eLuxEnemyState_AttackMeleeShort);
                }
            }

            kLuxOnLeave
            {
                mfFOVMul = 1.0f;
            }
        }

        ////////////////////////////////
        // Go Home
        kLuxState(eLuxEnemyState_GoHome)
        {
            kLuxOnEnter
            {
                mpPathfinder->MoveTo(mvStartPosition);
                SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
            }

            kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
            {
                ChangeState(eLuxEnemyState_Idle);
            }

            kLuxOnMessage(eLuxEnemyMessage_SoundHeard)
            {
                if(apMessage->mfCustomValue > mfHearVolume)
                {
                    ChangeState(eLuxEnemyState_Investigate);
                    mvTempPos = apMessage->mvCustomValue;
                    mfTempVal = apMessage->mfCustomValue;
                }
            }
        }

        ////////////////////////////////
        // Death
        kLuxState(eLuxEnemyState_Dead)
        {
            // Overload globals
            kLuxOnMessage(eLuxEnemyMessage_PlayerDetected) { }
            kLuxOnMessage(eLuxEnemyMessage_HelpMe) { }

            kLuxOnEnter
            {
                mpMover->ResetStuckCounter();
                mpPathfinder->Stop();

                PlayAnim("Dead", false, 0.3f, false, 1.0f, false, true, false);

                gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack,this);
                gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Search,this);
                gpBase->mpPlayer->RemoveTerrorEnemy(this);

                mbCausesSanityDecrease = false;

                mpCharBody->SetActive(false);
            }
        }
    }
    kLuxEndStateMachine
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::OnRenderSolidImplemented(cRendererCallbackFunctions* apFunctions)
{
    iPhysicsWorld *pPhysicsWorld = mpMap->GetPhysicsWorld();

    if(mCurrentState == eLuxEnemyState_AttackMeleeShort)
    {
        pPhysicsWorld->RenderShapeDebugGeometry(GetAttackShape(0),
            GetDamageShapeMatrix(cVector3f(0.0f, 0.0f, 1.0f)), apFunctions->GetLowLevelGfx(),
            cColor(1.0f, 0.0f, 0.0f, 1.0f));
    }
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::OnSetupAfterLoad(cWorld *apWorld)
{
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::OnAfterWorldLoad()
{
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::UpdateEnemySpecific(double adFixedDelta)
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxEnemy_Grunt::PlayerIsDetected()
{
    if(CanSeePlayer())
    {
        return true;
    }
    else
    {
        return (DistToPlayer() < mpCharBody->GetSize().x && PlayerInFOV());
    }
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::OnDisableTriggers()
{
    if(mCurrentState == eLuxEnemyState_Idle)
    {
        return;
    }

    ChangeState(eLuxEnemyState_Patrol);
}

//-----------------------------------------------------------------------

float cLuxEnemy_Grunt::GetDamageMul(float afAmount, int alStrength)
{
    if(mCurrentState == eLuxEnemyState_Idle ||
        mCurrentState == eLuxEnemyState_Wait ||
        mCurrentState == eLuxEnemyState_Patrol ||
        mCurrentState == eLuxEnemyState_Investigate)
    {
        return 1.0f;
    }
    if(mCurrentState == eLuxEnemyState_Hurt)
    {
        return 0.5f;
    }

    return 0.2f;
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::PatrolUpdateGoal()
{
    if(mvPatrolNodes.empty())
    {
        ChangeState(eLuxEnemyState_Wait);
        return;
    }

    cLuxEnemyPatrolNode* pPatrolNode = GetCurrentPatrolNode();
    mpPathfinder->MoveTo(pPatrolNode->mpNode->GetPosition());
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::PatrolEndOfPath()
{
    if(IsAtLastPatrolNode())
    {
        if(mbIsSeenByPlayer==false && DistToPlayer() > 10.0f && mbAutoRemoveAtPathEnd)
        {
            SetActive(false);
            RunCallbackFunc("OnAutoDisabled");

            return;
        }
    }

    cLuxEnemyPatrolNode *pNode = GetCurrentPatrolNode();
    if(pNode)
    {
        mfWaitTime = pNode->mfWaitTime;
    }
    else
    {
        mfWaitTime = 0;
    }

    ChangeState(eLuxEnemyState_Wait);
    IncCurrentPatrolNode(true);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxEnemy_Grunt_SaveData, iLuxEnemy_SaveData)
kSerializeVar(mlReturnState, eSerializeType_Int32)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxEnemy_Grunt::CreateSaveData()
{
    return hplNew(cLuxEnemy_Grunt_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
    super_class::SaveToSaveData(apSaveData);
    cLuxEnemy_Grunt_SaveData *pData = static_cast<cLuxEnemy_Grunt_SaveData*>(apSaveData);

    pData->mlReturnState = (int)mReturnState;
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
    super_class::LoadFromSaveData(apSaveData);
    cLuxEnemy_Grunt_SaveData *pData = static_cast<cLuxEnemy_Grunt_SaveData*>(apSaveData);

    mReturnState = (eLuxEnemyState)pData->mlReturnState;

#if LUX_ENEMY_MANPIG
    ////////////////////////
    // Handle changed enums
    if(mCurrentState >= eLuxEnemyState_PigEnumStart)
    {
        mCurrentState = eLuxEnemyState_LastEnum;
    }
    if(mNextState >= eLuxEnemyState_PigEnumStart)
    {
        mNextState = eLuxEnemyState_LastEnum;
    }
    if(mPreviousState >= eLuxEnemyState_PigEnumStart)
    {
        mPreviousState = eLuxEnemyState_LastEnum;
    }
    if(mReturnState >= eLuxEnemyState_PigEnumStart)
    {
        mReturnState = eLuxEnemyState_LastEnum;
    }
#endif
}

//-----------------------------------------------------------------------

void cLuxEnemy_Grunt::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
    super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
