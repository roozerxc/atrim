#ifndef LUX_MOVE_STATE_CLIMB_LEDGE_H
#define LUX_MOVE_STATE_CLIMB_LEDGE_H

//----------------------------------------------

#include "LuxMoveState.h"

//----------------------------------------------

class cLuxMoveState_ClimbLedge : public iLuxMoveState
{
public:    
    cLuxMoveState_ClimbLedge(cLuxPlayer *apPlayer);
    ~cLuxMoveState_ClimbLedge();
    
    void OnMapEnter();

    void OnEnterState(eLuxMoveState aPrevState);
    void OnLeaveState(eLuxMoveState aNewState);

    void OnUpdate(float afTimeStep);

    bool GetAllowsInteraction();

    bool OnMove(eCharDir aDir, float afMul);
    bool OnAddYaw(float afAmount);
    bool OnAddPitch(float afAmount);

    void OnRun(bool abActive);
    void OnCrouch(bool abActive);
    void OnJump(bool abActive);

    void OnDraw(float afFrameTime);
    
private:
    float mfHeadMoveSpeed;
    float mfHeadMoveSlowdownDist;
    float mfGivePlayerControlDist;
    float mfMovePitchFactor;
    float mfMaxMovePitchSpeed;
};

//----------------------------------------------


#endif // LUX_MOVE_STATE_CLIMB_LEDGE_H
