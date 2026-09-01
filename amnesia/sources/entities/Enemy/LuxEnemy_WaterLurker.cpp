#include "LuxEnemy_WaterLurker.h"

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

cLuxEnemyLoader_WaterLurker::cLuxEnemyLoader_WaterLurker(const tString& asName) : iLuxEnemyLoader(asName)
{
}

//-----------------------------------------------------------------------

iLuxEnemy *cLuxEnemyLoader_WaterLurker::CreateEnemy(const tString& asName, int alID, cLuxMap *apMap)
{
    return hplNew(cLuxEnemy_WaterLurker, (asName, alID,apMap));
}

//-----------------------------------------------------------------------

void cLuxEnemyLoader_WaterLurker::LoadVariables(iLuxEnemy *apEnemy, cXmlElement *apRootElem)
{
    cLuxEnemy_WaterLurker *pWaterLurker = static_cast<cLuxEnemy_WaterLurker*>(apEnemy);

    pWaterLurker->mfPlayerDetectionHeight  = GetVarFloat("PlayerDetectionHeight", 0.6f);
    pWaterLurker->mfEatDamage              = GetVarFloat("EatDamage", 0.0f);
    pWaterLurker->mfAfterAttackPauseTime   = GetVarFloat("AfterAttackPauseTime", 0.0f);

    pWaterLurker->msSplashPS_Walk          = GetVarString("SplashPS_Walk", "");
    pWaterLurker->msSplashPS_Run           = GetVarString("SplashPS_Run", "");
    pWaterLurker->msSplashPS_Eat           = GetVarString("SplashPS_Eat", "");
    pWaterLurker->msSplashPS_Attack        = GetVarString("SplashPS_Attack", "");

    pWaterLurker->msSplashSound_Walk       = GetVarString("SplashSound_Walk", "");
    pWaterLurker->msSplashSound_Run        = GetVarString("SplashSound_Run", "");
    pWaterLurker->msSplashSound_Eat        = GetVarString("SplashSound_Eat", "");
    pWaterLurker->msSplashSound_Attack     = GetVarString("SplashSound_Attack", "");

    gpBase->PreloadParticleSystem(pWaterLurker->msSplashPS_Walk);
    gpBase->PreloadParticleSystem(pWaterLurker->msSplashPS_Run);
    gpBase->PreloadParticleSystem(pWaterLurker->msSplashPS_Eat);
    gpBase->PreloadParticleSystem(pWaterLurker->msSplashPS_Attack);

    gpBase->PreloadSound(pWaterLurker->msSplashSound_Walk);
    gpBase->PreloadSound(pWaterLurker->msSplashSound_Run);
    gpBase->PreloadSound(pWaterLurker->msSplashSound_Eat);
    gpBase->PreloadSound(pWaterLurker->msSplashSound_Attack);
}

//-----------------------------------------------------------------------

void cLuxEnemyLoader_WaterLurker::LoadInstanceVariables(iLuxEnemy *apEnemy, cResourceVarsObject *apInstanceVars)
{
    cLuxEnemy_WaterLurker *pWaterLurker = static_cast<cLuxEnemy_WaterLurker*>(apEnemy);

    float fHeight = apInstanceVars->GetVarFloat("PlayerDetectionHeight", 0.0f);
    if(fHeight > 0.0f)
    {
        pWaterLurker->mfPlayerDetectionHeight = fHeight;
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxEnemy_WaterLurker::cLuxEnemy_WaterLurker(const tString &asName, int alID, cLuxMap *apMap) : iLuxEnemy(asName,alID,apMap, eLuxEnemyType_WaterLurker)
{
    mbUseAnimations = false;
    mbCausesSanityDecrease = false;
    mbCausesSanityDecreaseAsDefault = false;

    mfPlayerLostTimer = 0.0f;

    mReturnState = eLuxEnemyState_Idle;
}

//-----------------------------------------------------------------------

cLuxEnemy_WaterLurker::~cLuxEnemy_WaterLurker()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxEnemy_WaterLurker::StateEventImplement(int alState, eLuxEnemyStateEvent aEvent, cLuxStateMessage *apMessage)
{
    ////////////////////////////////
    // atrim state machine rewrite
    // by RoozerXC -- 09-01-2026

    kLuxBeginStateMachine
        // ------------------- MESSAGES ------------------- //

        // DO NOTHING
        kLuxOnMessage(eLuxEnemyMessage_TakeHit) { }

        ////////////////////////////////
        // Reset all to default values
        kLuxOnMessage(eLuxEnemyMessage_Reset)
        {
            gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack, this);
            gpBase->mpPlayer->RemoveTerrorEnemy(this);

            ChangeState(eLuxEnemyState_Idle);
        }

        // Enemy spots player!
        kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
        {
            ChangeState(eLuxEnemyState_Hunt);
        }

        // Enemy found something to eat
        kLuxOnMessage(eLuxEnemyMessage_FoodInRange)
        {
            cAINode *pNode = mpPathfinder->GetNodeAtPos(apMessage->mvCustomValue, 0.0f, 1.5f, true, true, true, NULL);
            if(pNode)
            {
                gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack, this);
                gpBase->mpPlayer->RemoveTerrorEnemy(this);

                mvTempPos = pNode->GetPosition();
                ChangeState(eLuxEnemyState_Search);

                gpBase->mpDebugHandler->AddMessage(_W("Enemy ") +
                    cString::To16Char(msName) + _W(" found food!"), false);
            }
            else
            {
                gpBase->mpDebugHandler->AddMessage(_W("Enemy ") +
                    cString::To16Char(msName) + _W(" found food but no node..."), false);
            }
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
            kLuxOnEnter
            {
                ChangeSoundState(eLuxEnemySoundState_Idle);
            }

            kLuxOnUpdate
            {
                if(PlayerIsDetected()) 
                {
                    ChangeState(eLuxEnemyState_Hunt);
                }
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
        // Investigate (heard a sound)
        kLuxState(eLuxEnemyState_Investigate)
        {
            kLuxOnUpdate
            {
                // BREAK DOOR CHECK
                if(mbStuckAtDoor)
                {
                    CheckStuckDoor();
                }

                if(mpMover->GetStuckCounter() > 0.9f)
                {
                    mpMover->ResetStuckCounter();

                    ChangeState(eLuxEnemyState_Idle);
                }
            }

            kLuxOnEnter
            {
                mpPathfinder->Stop();

                cAINode *pNode = mpPathfinder->GetNodeAtPos(mvTempPos, 0.0f, 10.0f, true, true, true, NULL, 1);
                if(pNode)
                {
                    mpPathfinder->MoveTo(pNode->GetPosition());
                }
                else
                {
                    mpPathfinder->MoveTo(mvTempPos);

                    gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName) +
                        _W(" could not find a node near sound!"), false);
                }

                SendMessage(eLuxEnemyMessage_TimeOut_2, 0.1f, true);

                // Output: Enemy servant_grunt_1 heard a sound!
                //         Volume: 0.755555555555555555f
                gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName) + _W(" heard a sound!"), false);
                gpBase->mpDebugHandler->AddMessage(_W("Volume: ") + cString::ToStringW(mfTempVal) + _W("f"), false);
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

            kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
            {
                SplashWater(eWaterLurkerSplash_Walk);
                SendMessage(eLuxEnemyMessage_TimeOut_2, 1.0f, true);
            }

            kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
            {
                ChangeState(eLuxEnemyState_Idle);
            }
        }

        ////////////////////////////////
        // Search (Scavenge for food!)
        kLuxState(eLuxEnemyState_Search)
        {
            // Overload global
            kLuxOnMessage(eLuxEnemyMessage_FoodInRange) { }

            kLuxOnUpdate
            {
                // BREAK DOOR CHECK
                if(mbStuckAtDoor)
                {
                    CheckStuckDoor();
                }

                if(mpMover->GetStuckCounter() > 0.9f)
                {
                    mpMover->ResetStuckCounter();

                    ChangeState(eLuxEnemyState_Idle);
                }
            }

            kLuxOnEnter
            {
                ChangeSoundState(eLuxEnemySoundState_Idle);

                mpPathfinder->MoveTo(mvTempPos);
                SendMessage(eLuxEnemyMessage_TimeOut_2, 0.1f, true);
            }

            kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
            {
                SplashWater(eWaterLurkerSplash_Walk);
                SendMessage(eLuxEnemyMessage_TimeOut_2, 1.0f, true);
            }

            kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
            {
                ChangeState(eLuxEnemyState_Eat);
                gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName) +
                    _W(" is eating some yummy food. Om nom nom!\n"), false);
            }
        }

        ////////////////////////////////
        // Eat
        kLuxState(eLuxEnemyState_Eat)
        {
            // Overload global
            kLuxOnMessage(eLuxEnemyMessage_FoodInRange) { }

            kLuxOnUpdate
            {
                // BREAK DOOR CHECK
                if(mbStuckAtDoor)
                {
                    CheckStuckDoor();
                }

                if(Dist2D(mvTempPos) > mpCharBody->GetSize().x * 0.75f)
                {
                    mpMover->MoveToPos(mvTempPos);
                }
            }

            kLuxOnEnter
            {
                mpPathfinder->Stop();

                ChangeSoundState(eLuxEnemySoundState_Idle);

                SendMessage(eLuxEnemyMessage_TimeOut, 0.1f, true);

                if(mPreviousState != eLuxEnemyState_BreakDoor)
                {
                    mvTempPos = mpCharBody->GetFeetPosition();
                }
            }

            //Search for food and eat it!
            kLuxOnMessage(eLuxEnemyMessage_TimeOut)
            {
                cLuxProp_Object *pFood = GetClosestFood(3.5f,mfPlayerDetectionHeight);
                if(pFood)
                {
                    iPhysicsBody *pFoodBody = pFood->GetBody(0);
                    cVector3f vToFeet(mpCharBody->GetFeetPosition() - pFoodBody->GetLocalPosition());
                    vToFeet.Normalize();

                    pFoodBody->AddImpulse(vToFeet*2.5f);

                    SplashWater(eWaterLurkerSplash_Eat);
                    pFood->GiveDamage(mfEatDamage, 10);
                    gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName) + _W(" took a bite!"), false);
                    gpBase->mpDebugHandler->AddMessage(_W("Health: ") + cString::ToStringW(pFood->GetHealth()) + _W("f"), false);

                    SendMessage(eLuxEnemyMessage_TimeOut, 1.2f, true);
                    mvTempPos = pFood->GetBody(0)->GetLocalPosition();
                }
                else
                {
                    gpBase->mpDebugHandler->AddMessage(_W("No more food for enemy ") + cString::To16Char(msName) + _W("!"),false);
                    ChangeState(eLuxEnemyState_GoHome);
                }
            }

            //Player detection, only hunt if player is really close.
            kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)
            {
                if(DistToPlayer2D() < 2.0f)
                {
                    ChangeState(eLuxEnemyState_Hunt);
                }
                else
                {
                    mbPlayerDetected = false;
                }
            }
        }

        ////////////////////////////////
        // Hunt
        kLuxState(eLuxEnemyState_Hunt)
        {
            // Overload global
            kLuxOnMessage(eLuxEnemyMessage_FoodInRange) { }

            kLuxOnUpdate
            {
                // BREAK DOOR CHECK
                if(mbStuckAtDoor)
                {
                    CheckStuckDoor();
                }

                if(mpMover->GetStuckCounter() > 0.9f)
                {
                    mpMover->ResetStuckCounter();

                    ChangeState(eLuxEnemyState_Idle);
                }

                if(DistToPlayer() < mfNormalAttackDistance)
                {
                    ChangeState(eLuxEnemyState_AttackMeleeShort);
                }
            }

            kLuxOnEnter
            {
                mfPlayerLostTimer = 0.0f;

                if(PlayerIsDetected()==false)
                {
                    ChangeState(eLuxEnemyState_GoHome);
                    mbCausesSanityDecrease = false;
                }
                else if(mPreviousState == eLuxEnemyState_AttackMeleeShort)
                {
                    mlAttackHitCounter = 0;

                    gpBase->mpMusicHandler->AddEnemy(eLuxEnemyMusic_Attack,this);

                    ChangeState(eLuxEnemyState_HuntPause);
                }
                else
                {
                    gpBase->mpMusicHandler->AddEnemy(eLuxEnemyMusic_Attack,this);

                    ChangeSoundState(eLuxEnemySoundState_Hunt);

                    SetMoveSpeed(eLuxEnemyMoveSpeed_Run);

                    SendMessage(eLuxEnemyMessage_TimeOut, 0.1f, true);
                    SendMessage(eLuxEnemyMessage_TimeOut_2, 0.1f, true);

                    mbCausesSanityDecrease = true;
                }
            }

            kLuxOnMessage(eLuxEnemyMessage_TimeOut)
            {
                mpPathfinder->MoveTo(gpBase->mpPlayer->GetCharacterBody()->GetFeetPosition());
                SendMessage(eLuxEnemyMessage_TimeOut, 0.4f, true);

                if(PlayerIsDetected()==false)
                {
                    mfPlayerLostTimer += 0.4f;
                    if(mfPlayerLostTimer >= 1.2f)
                    {
                        ChangeState(eLuxEnemyState_GoHome);
                        mbCausesSanityDecrease = false;
                    }
                }
                else
                {
                    mfPlayerLostTimer = 0.0f;
                }
            }

            kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
            {
                if(mpMover->GetWantedSpeedAmount() > 0.2f)
                {
                    SplashWater(eWaterLurkerSplash_Run);
                }

                SendMessage(eLuxEnemyMessage_TimeOut_2, 0.6f, true);
            }

            kLuxOnLeave
            {
                SetMoveSpeed(eLuxEnemyMoveSpeed_Walk);
            }
        }

        ////////////////////////////////
        // Hunt Pause
        kLuxState(eLuxEnemyState_HuntPause)
        {
            // Overload globals
            kLuxOnMessage(eLuxEnemyMessage_FoodInRange) { }
            kLuxOnMessage(eLuxEnemyMessage_PlayerDetected) { }

            kLuxOnEnter
            {
                mpPathfinder->Stop();

                SendMessage(eLuxEnemyMessage_TimeOut, mfAfterAttackPauseTime, true);
            }

            kLuxOnMessage(eLuxEnemyMessage_TimeOut)
            {
                ChangeState(eLuxEnemyState_Hunt);
            }
        }

        ////////////////////////////////
        // Break door
        kLuxState(eLuxEnemyState_BreakDoor)
        {
            // Overload global
            kLuxOnMessage(eLuxEnemyMessage_FoodInRange) { }
            kLuxOnMessage(eLuxEnemyMessage_PlayerDetected) { }

            kLuxOnUpdate
            {
                // Turn towards the door
                mpMover->TurnToPos(mvTempPos);
            }

            kLuxOnEnter
            {
                mpMover->ResetStuckCounter();
                mpPathfinder->Stop();

                mlTempVal = 0;

                SendMessage(eLuxEnemyMessage_TimeOut, 0.01f, true);
            }

            // Damage door
            kLuxOnMessage(eLuxEnemyMessage_TimeOut)
            {
                Attack(mNormalAttackSize, mBreakDoorAttackDamage);

                SendMessage(eLuxEnemyMessage_TimeOut_2, cMath::RandRectf(0.5,1.5f), true);
            }

            // Check if door is destroyed
            kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
            {
                mlTempVal++;
                const int kMaxBreakDoorAttempts = 10;

                iLuxEntity *pDoorEntity = mpMap->GetEntityByID(mlStuckDoorID);
                bool bDoorIsBroken = false;

                if(pDoorEntity == NULL || pDoorEntity->GetEntityType() != eLuxEntityType_Prop)
                {
                    bDoorIsBroken = true;
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

                if(bDoorIsBroken || bEnemyIsTooFar || mlTempVal >= kMaxBreakDoorAttempts)
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

                    if(PlayerIsDetected())
                    {
                        ChangeState(eLuxEnemyState_Hunt);
                    }
                    else
                    {
                        if(mReturnState == eLuxEnemyState_BreakDoor)
                        {
                            ChangeState(eLuxEnemyState_Idle);
                        }
                        else
                        {
                            ChangeState(mReturnState);
                        }
                    }
                }
                else
                {
                    SendMessage(eLuxEnemyMessage_TimeOut, 0.01f, true);
                }
            }

            kLuxOnLeave
            {
                mpMover->ResetStuckCounter();
                mpPathfinder->Stop();

                mbStuckAtDoor = false;
                mlAttackHitCounter = 0;

                if(mNextState != eLuxEnemyState_Hunt &&
                    mNextState != eLuxEnemyState_HuntPause)
                {
                    gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack, this);
                    gpBase->mpPlayer->RemoveTerrorEnemy(this);
                }
            }
        }

        ////////////////////////////////
        // Attack Short
        kLuxState(eLuxEnemyState_AttackMeleeShort)
        {
            // Overload global
            kLuxOnMessage(eLuxEnemyMessage_FoodInRange) { }
            kLuxOnMessage(eLuxEnemyMessage_PlayerDetected) { }

            kLuxOnEnter
            {
                mpMover->ResetStuckCounter();
                mpPathfinder->Stop();

                SendMessage(eLuxEnemyMessage_TimeOut, 0.3f, true);
            }

            kLuxOnMessage(eLuxEnemyMessage_TimeOut)
            {
                SplashWater(eWaterLurkerSplash_Attack);

                Attack(mNormalAttackSize, mNormalAttackDamage);

                SendMessage(eLuxEnemyMessage_TimeOut_2, 0.5f, true);
            }

            kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
            {
                ChangeState(mPreviousState);
            }
        }

        ////////////////////////////////
        // Go Home
        kLuxState(eLuxEnemyState_GoHome)
        {
            kLuxOnUpdate
            {
                // BREAK DOOR CHECK
                if(mbStuckAtDoor)
                {
                    CheckStuckDoor();
                }

                if(mpMover->GetStuckCounter() > 0.9f)
                {
                    mpMover->ResetStuckCounter();

                    gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack, this);
                    gpBase->mpPlayer->RemoveTerrorEnemy(this);

                    ChangeState(eLuxEnemyState_Idle);
                }
            }

            kLuxOnEnter
            {
                ChangeSoundState(eLuxEnemySoundState_Idle);

                cAINode *pNode = mpPathfinder->GetNodeAtPos(GetPlayerFeetPos(), 0.0f, 6.0f, false, false, true, NULL);
                if(pNode)
                {
                    mpPathfinder->MoveTo(pNode->GetPosition());

                    gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName) + _W(" approaching node!"), false);
                }
                else
                {
                    mpPathfinder->MoveTo(mvStartPosition);

                    gpBase->mpDebugHandler->AddMessage(_W("Enemy ") + cString::To16Char(msName) + _W(" approaching start position!"), false);
                }

                SendMessage(eLuxEnemyMessage_TimeOut_2, 0.1f, true);
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

            kLuxOnMessage(eLuxEnemyMessage_TimeOut_2)
            {
                SplashWater(eWaterLurkerSplash_Walk);
                SendMessage(eLuxEnemyMessage_TimeOut_2, 1.0f, true);
            }

            kLuxOnMessage(eLuxEnemyMessage_EndOfPath)
            {
                gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack, this);
                gpBase->mpPlayer->RemoveTerrorEnemy(this);

                ChangeState(eLuxEnemyState_Idle);
            }
        }

        ////////////////////////////////
        // Dead
        kLuxState(eLuxEnemyState_Dead)
        {
            kLuxOnMessage(eLuxEnemyMessage_HelpMe) { }
            kLuxOnMessage(eLuxEnemyMessage_PlayerDetected)  { }

            kLuxOnEnter
            {
                mpMover->ResetStuckCounter();
                mpPathfinder->Stop();

                mpCharBody->SetActive(false);
                ChangeSoundState(eLuxEnemySoundState_Silent);

                gpBase->mpMusicHandler->RemoveEnemy(eLuxEnemyMusic_Attack, this);
                gpBase->mpPlayer->RemoveTerrorEnemy(this);

                mbCausesSanityDecrease = false;
            }
        }
    kLuxEndStateMachine
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::OnSetupAfterLoad(cWorld *apWorld)
{
    mpMeshEntity->SetActive(false);
    mpMeshEntity->SetVisible(false);
}

//-----------------------------------------------------------------------

bool cLuxEnemy_WaterLurker::InRangeOfFood(iPhysicsBody *apFoodBody)
{
    float fYDist = apFoodBody->GetLocalPosition().y - mpCharBody->GetFeetPosition().y;
    return cMath::Abs(fYDist) <= mfPlayerDetectionHeight;
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::OnRenderSolidImplemented(cRendererCallbackFunctions* apFunctions)
{
    iPhysicsWorld *pPhysicsWorld = mpMap->GetPhysicsWorld();

    mpCharBody->GetCurrentBody()->RenderDebugGeometry(apFunctions->GetLowLevelGfx(), cColor(1,1));

    if(mCurrentState == eLuxEnemyState_AttackMeleeShort || mCurrentState == eLuxEnemyState_BreakDoor)
    {
        pPhysicsWorld->RenderShapeDebugGeometry(GetAttackShape(0), GetDamageShapeMatrix(cVector3f(0,0,1)),
            apFunctions->GetLowLevelGfx(), cColor(1,0,0,1));
    }
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::OnAfterWorldLoad()
{
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::UpdateEnemySpecific(double adFixedDelta)
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::SplashWater(eWaterLurkerSplash aType)
{
    tString sPS = "";
    if(aType == eWaterLurkerSplash_Walk)
    {
        sPS = msSplashPS_Walk;
    }
    else if(aType == eWaterLurkerSplash_Run)
    {
        sPS = msSplashPS_Run;
    }
    else if(aType == eWaterLurkerSplash_Eat)
    {
        sPS = msSplashPS_Eat;
    }
    else if(aType == eWaterLurkerSplash_Attack)
    {
        sPS = msSplashPS_Attack;
    }

    if(sPS!="")
    {
        cParticleSystem *pPS = mpMap->GetWorld()->CreateParticleSystem("LurkerSplash", sPS,1);
        if(pPS)
        {
            pPS->SetPosition(mpCharBody->GetFeetPosition() + cVector3f(0, 0.7f, 0));
        }
    }

    tString sSound = "";
    if(aType == eWaterLurkerSplash_Walk)
    {
        sSound = msSplashSound_Walk;
    }
    else if(aType == eWaterLurkerSplash_Run)
    {
        sSound = msSplashSound_Run;
    }
    else if(aType == eWaterLurkerSplash_Eat)
    {
        sSound = msSplashSound_Eat;
    }
    else if(aType == eWaterLurkerSplash_Attack)
    {
        sSound = msSplashSound_Attack;
    }

    PlaySound(sSound);
}

//-----------------------------------------------------------------------

bool cLuxEnemy_WaterLurker::PlayerIsDetected()
{
    return AbsHeightDistToPlayer() <= mfPlayerDetectionHeight;
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::OnSetActiveEnemySpecific(bool abX)
{
    // Make sure the mesh is invisible!
    mpMeshEntity->SetActive(false);
    mpMeshEntity->SetVisible(false);
}

//-----------------------------------------------------------------------

float cLuxEnemy_WaterLurker::GetDamageMul(float afAmount, int alStrength)
{
    if(    mCurrentState == eLuxEnemyState_Idle ||
            mCurrentState == eLuxEnemyState_Wait ||
            mCurrentState == eLuxEnemyState_Patrol ||
            mCurrentState == eLuxEnemyState_Investigate)
    {
        return 1.0f;
    }
    if( mCurrentState == eLuxEnemyState_Hurt)
    {
        return 0.5f;
    }

    return 0.2f;
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::PatrolUpdateGoal()
{

}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::PatrolEndOfPath()
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxEnemy_WaterLurker_SaveData, iLuxEnemy_SaveData)
kSerializeVar(mfPlayerDetectionHeight, eSerializeType_Float32)
kSerializeVar(mfPlayerLostTimer, eSerializeType_Float32)
kSerializeVar(mlReturnState, eSerializeType_Int32)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxEnemy_WaterLurker::CreateSaveData()
{
    return hplNew(cLuxEnemy_WaterLurker_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
    super_class::SaveToSaveData(apSaveData);
    cLuxEnemy_WaterLurker_SaveData *pData = static_cast<cLuxEnemy_WaterLurker_SaveData*>(apSaveData);

    kCopyToVar(pData,mfPlayerDetectionHeight);
    kCopyToVar(pData,mfPlayerLostTimer);

    pData->mlReturnState = (int)mReturnState;
}

//-----------------------------------------------------------------------

void cLuxEnemy_WaterLurker::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
    super_class::LoadFromSaveData(apSaveData);
    cLuxEnemy_WaterLurker_SaveData *pData = static_cast<cLuxEnemy_WaterLurker_SaveData*>(apSaveData);

    kCopyFromVar(pData,mfPlayerDetectionHeight);
    kCopyFromVar(pData,mfPlayerLostTimer);

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

void cLuxEnemy_WaterLurker::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
    super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
