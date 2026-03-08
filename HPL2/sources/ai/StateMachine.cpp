#include "ai/StateMachine.h"

#include "system/LowLevelSystem.h"


namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// STATE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iAIState::iAIState()
{
    mfTimeCount = 0;
}

//-----------------------------------------------------------------------

void iAIState::Sleep(float afTime)
{

}

//-----------------------------------------------------------------------

void iAIState::Update(float afTime)
{
    //Update according to the time step
    mfTimeCount    += afTime;
    while(mfTimeCount >= mfUpdateStep)
    {
        OnUpdate(mfUpdateStep);
        mfTimeCount -= mfUpdateStep;
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cStateMachine::cStateMachine()
{
    mbActive = true;

    mpCurrentState = NULL;
}

//-----------------------------------------------------------------------

cStateMachine::~cStateMachine()
{
    STLMapDeleteAll(m_mapStates);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cStateMachine::Update(float afTime)
{
    if(mbActive== false || mpCurrentState==NULL) return;

    mpCurrentState->Update(afTime);
}

//-----------------------------------------------------------------------

void cStateMachine::AddState(iAIState *apState, const tString& asName, int alId, float afUpdateStep)
{
    apState->SetStateMachine(this);
    apState->mlId = alId;
    apState->msName = asName;
    apState->mfUpdateStep = afUpdateStep;

    if(m_mapStates.empty()) mpCurrentState = apState;

    m_mapStates.insert(tAIStateMap::value_type(alId, apState));
}

//-----------------------------------------------------------------------

void cStateMachine::ChangeState(int alId)
{
    if(alId == mpCurrentState->GetId()) return;

    iAIState *pState = GetState(alId);
    if(pState==NULL)
    {
        Warning("State %d does not exist!\n",alId);
        return;
    }

    if(mpCurrentState) mpCurrentState->OnLeaveState(pState->GetId());
    pState->OnEnterState(mpCurrentState==NULL ? -1 : mpCurrentState->GetId());

    mpCurrentState = pState;
}

//-----------------------------------------------------------------------

iAIState* cStateMachine::GetState(int alId)
{
    tAIStateMapIt it = m_mapStates.find(alId);
    if(it == m_mapStates.end()) return NULL;

    return it->second;
}
//-----------------------------------------------------------------------

iAIState* cStateMachine::CurrentState()
{
    return mpCurrentState;
}

//-----------------------------------------------------------------------
}
