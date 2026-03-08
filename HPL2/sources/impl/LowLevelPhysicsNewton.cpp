#pragma comment(lib, "newton.lib")

#include "impl/LowLevelPhysicsNewton.h"

#include "impl/PhysicsWorldNewton.h"
#include "system/LowLevelSystem.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLowLevelPhysicsNewton::cLowLevelPhysicsNewton()
{

}

//-----------------------------------------------------------------------

cLowLevelPhysicsNewton::~cLowLevelPhysicsNewton()
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHOD
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


iPhysicsWorld* cLowLevelPhysicsNewton::CreateWorld()
{
    cPhysicsWorldNewton* pWorld = hplNew( cPhysicsWorldNewton, () );
    return pWorld;
}

//-----------------------------------------------------------------------

}
