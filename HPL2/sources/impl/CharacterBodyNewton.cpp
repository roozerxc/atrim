#include "impl/CharacterBodyNewton.h"

#include "physics/CollideShape.h"
#include "physics/PhysicsWorld.h"
#include "system/LowLevelSystem.h"

namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cCharacterBodyNewton::cCharacterBodyNewton(const tString &asName,iPhysicsWorld *apWorld, const cVector3f avSize)
        : iCharacterBody(asName,apWorld, avSize)
    {
    }

    //-----------------------------------------------------------------------

    cCharacterBodyNewton::~cCharacterBodyNewton()
    {
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------


    //-----------------------------------------------------------------------

}
