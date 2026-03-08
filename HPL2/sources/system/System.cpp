#include "system/System.h"
#include "system/LowLevelSystem.h"
#include "system/LogicTimer.h"
#include "system/String.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cSystem::cSystem(iLowLevelSystem *apLowLevelSystem)
{
    mpLowLevelSystem = apLowLevelSystem;
}

//-----------------------------------------------------------------------

cSystem::~cSystem()
{
    Log("Exiting System Module\n");
    Log("--------------------------------------------------------\n");

    Log("--------------------------------------------------------\n\n");
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
cLogicTimer * cSystem::CreateLogicTimer(unsigned int alUpdatesPerSec)
{
    return hplNew( cLogicTimer, (alUpdatesPerSec, mpLowLevelSystem) );
}

//-----------------------------------------------------------------------

iLowLevelSystem* cSystem::GetLowLevel()
{
    return mpLowLevelSystem;
}

//-----------------------------------------------------------------------

}
