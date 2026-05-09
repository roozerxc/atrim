#ifndef HPL_CHARACTER_BODY_NEWTON_H
#define HPL_CHARACTER_BODY_NEWTON_H

#include "physics/CharacterBody.h"
#include <Newton.h>

namespace hpl
{

class iPhysicsWorld;

class cCharacterBodyNewton : public iCharacterBody
{
public:
    cCharacterBodyNewton(const tString &asName,iPhysicsWorld *apWorld, const cVector3f avSize);
    ~cCharacterBodyNewton();

private:
};
};
#endif // HPL_CHARACTER_BODY_NEWTON_H
