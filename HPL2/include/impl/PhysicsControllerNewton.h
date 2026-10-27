#ifndef HPL_PHYSICS_CONTROLLER_NEWTON_H
#define HPL_PHYSICS_CONTROLLER_NEWTON_H

#include "physics/PhysicsController.h"

namespace hpl
{

class iPhysicsWorld;

class cPhysicsControllerNewton : public iPhysicsController
{
public:
    cPhysicsControllerNewton(const tString &asName, iPhysicsWorld *apWorld);
    ~cPhysicsControllerNewton();
};
};
#endif // HPL_PHYSICS_CONTROLLER_NEWTON_H
