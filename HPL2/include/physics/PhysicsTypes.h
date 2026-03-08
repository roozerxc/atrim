#ifndef HPL_PHYSICS_TYPES_H
#define HPL_PHYSICS_TYPES_H

#include "math/MathTypes.h"

namespace hpl
{

//-----------------------------------------

enum eCollideShapeType
{
    eCollideShapeType_Null,
    eCollideShapeType_Box,
    eCollideShapeType_Sphere,
    eCollideShapeType_Cylinder,
    eCollideShapeType_Capsule,
    eCollideShapeType_ConvexHull,
    eCollideShapeType_Mesh,
    eCollideShapeType_Compound,
    eCollideShapeType_StaticScene,
    eCollideShapeType_LastEnum
};

//-----------------------------------------

enum ePhysicsAccuracy
{
    ePhysicsAccuracy_Low,
    ePhysicsAccuracy_Medium,
    ePhysicsAccuracy_High,
    ePhysicsAccuracy_LastEnum
};

//----------------------------------------------------

enum eVelocityAxes
{
    eVelocityAxes_XZ,
    eVelocityAxes_Y,
    eVelocityAxes_XYZ,
    eVelocityAxes_LastEnum
};

//----------------------------------------------------

typedef tFlag tPhysicsConnectionFlag;

#define ePhysicsConnectionFlag_DependOnBodyRotation        (0x00000001)
#define ePhysicsConnectionFlag_AlignBodyCenter            (0x00000002)
#define ePhysicsConnectionFlag_AlignBodyRotation        (0x00000004)
#define ePhysicsConnectionFlag_DisableBodyGravity        (0x00000008)
#define ePhysicsConnectionFlag_AffectChar                (0x00000010)
#define ePhysicsConnectionFlag_AlignCharRotation        (0x00000020)
#define ePhysicsConnectionFlag_DependOnCharRotation        (0x00000040)
#define ePhysicsConnectionFlag_AllowCollision            (0x00000080)

//----------------------------------------------------

class iVerletParticleContainer;

typedef std::list<iVerletParticleContainer*> tVerletParticleContainerList;
typedef tVerletParticleContainerList::iterator tVerletParticleContainerListIt;


//----------------------------------------------------

class cPhysicsRayParams
{
public:
    float mfT;
    float mfDist;
    cVector3f mvNormal;
    cVector3f mvPoint;
};

//----------------------------------------------------

class iPhysicsBody;

class iPhysicsRayCallback
{
public:
    virtual bool BeforeIntersect(iPhysicsBody *pBody)
    {
        return true;
    }
    virtual bool OnIntersect(iPhysicsBody *pBody,cPhysicsRayParams *apParams)=0;
};

//----------------------------------------------------

class cCollideData;

class iPhysicsWorldCollisionCallback
{
public:
    virtual void OnCollision(iPhysicsBody *apBody, cCollideData *apCollideData)=0;
};


//----------------------------------------------------
}

#endif // HPL_PHYSICS_TYPES_H
