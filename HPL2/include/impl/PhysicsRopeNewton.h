#ifndef HPL_PHYSICS_ROPE_NEWTON_H
#define HPL_PHYSICS_ROPE_NEWTON_H

#include "physics/PhysicsRope.h"

namespace hpl {

    //------------------------------------------

    class cPhysicsRopeNewton : public iPhysicsRope
    {
    public:
        cPhysicsRopeNewton(const tString &asName, iPhysicsWorld *apWorld, const cVector3f &avStartPos, const cVector3f &avEndPos);
        virtual ~cPhysicsRopeNewton();
    };
};
#endif // HPL_PHYSICS_ROPE_NEWTON_H
