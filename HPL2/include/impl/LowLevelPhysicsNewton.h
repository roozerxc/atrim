#ifndef HPL_LOWLEVELPHYSICS_NEWTON_H
#define HPL_LOWLEVELPHYSICS_NEWTON_H

#include "physics/LowLevelPhysics.h"
#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#endif
#include <Newton.h>

namespace hpl {

    class cLowLevelPhysicsNewton : public iLowLevelPhysics
    {
    public:
        cLowLevelPhysicsNewton();
        ~cLowLevelPhysicsNewton();

        iPhysicsWorld* CreateWorld();
    };
};
#endif // HPL_LOWLEVELPHYSICS_NEWTON_H
