#ifndef LUX_ENEMY_MOVER_H
#define LUX_ENEMY_MOVER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class iLuxEnemy;
class cLuxEnemy_ManPig;

//----------------------------------------------

enum eLuxEnemyMoveState
{
    eLuxEnemyMoveState_Backward,
    eLuxEnemyMoveState_Stopped,
    eLuxEnemyMoveState_Walking,
    eLuxEnemyMoveState_Running,

    eLuxEnemyMoveState_LastEnum
};

//----------------------------------------------

class cLuxEnemyMover
{
    friend class cLuxEnemyMover_SaveData;
    friend class iLuxEnemy;
    friend class cLuxEnemy_ManPig;
public:
    cLuxEnemyMover(iLuxEnemy *apEnemy, iCharacterBody *apCharBody);
    virtual ~cLuxEnemyMover();

    //////////////////////
    //General
    void SetupAfterLoad(cWorld *apWorld);

    void OnUpdate(float afTimeStep);

    //////////////////////
    //Actions
    void MoveToPos(const cVector3f& avFeetPos);

    void TurnToPos(const cVector3f& avFeetPos);
    void TurnToAngle(float afAngle);

    void UseMoveStateAnimations();

    //////////////////////
    //Properties

    //This is used by enemy when caclulting final max speed.
    float CalculateSpeedMul(float afTimeStep);

    //This get the speed along the movement direction (so it does not accound for falling speeds, climbing, etc
    float GetMoveSpeed();

    //This gets speed / wanted_speed
    float GetWantedSpeedAmount();

    void SetOverideMoveState(bool abX)
    {
        mbOverideMoveState = abX;
    }
    bool GetOverideMoveState()
    {
        return mbOverideMoveState;
    }

    float GetStuckCounter()
    {
        return mfStuckCounter;
    }
    float GetMaxStuckCounter()
    {
        return mfMaxStuckCounter;
    }
    bool GetStuckCounterIsAtMax()
    {
        return mfStuckCounter >= mfMaxStuckCounter;
    }
    void ResetStuckCounter()
    {
        mfStuckCounter =0;
    }


    //////////////////////
    //Data

    //////////////////////
    //Save data stuff

private:
    void UpdateStuckCounter(float afTimeStep);
    void UpdateTurning(float afTimeStep);
    void UpdateMoveAnimation(float afTimeStep);
    void UpdateStepEffects(float afTimeStep);

    ///////////////////
    // Data
    iLuxEnemy *mpEnemy;
    iCharacterBody *mpCharBody;

    float mfStuckLimit;
    float mfMaxStuckCounter;


    ///////////////////
    // Variables
    bool mbTurning;
    float mfTurnGoalAngle;
    float mfTurnSpeed;
    float mfTurnBreakAcc;

    float mfStuckCounter;

    eLuxEnemyMoveState mMoveState;
    bool mbOverideMoveState;
};

//----------------------------------------------


#endif // LUX_ENEMY_MOVER_H
