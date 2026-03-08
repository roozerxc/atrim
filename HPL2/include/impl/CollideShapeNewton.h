#ifndef HPL_COLLIDE_SHAPE_NEWTON_H
#define HPL_COLLIDE_SHAPE_NEWTON_H

#include <Newton.h>
#include "physics/CollideShape.h"

#include "math/MathTypes.h"

namespace hpl
{

class iVertexBuffer;
class iCollideShape;
class cBinaryBuffer;

typedef std::vector<iCollideShape*> tCollideShapeVec;
typedef tCollideShapeVec::iterator tCollideShapeVecIt;

class cCollideShapeNewton : public iCollideShape
{
public:
    cCollideShapeNewton(eCollideShapeType aType, const cVector3f &avSize,
                        cMatrixf* apOffsetMtx,NewtonWorld* apNewtonWorld,
                        iPhysicsWorld *apWorld);
    ~cCollideShapeNewton();

    iCollideShape* GetSubShape(int alIdx);
    int GetSubShapeNum();

    cVector3f GetInertia(float afMass);

    void CreateStaticSceneFromShapeVec(tCollideShapeVec &avShapes, tMatrixfVec *apMatrices);
    void CreateCompoundFromShapeVec(tCollideShapeVec &avShapes);
    void CreateFromVertices(const unsigned int* apIndexArray, int alIndexNum,
                            const float *apVertexArray, int alVtxStride, int alVtxNum);

    void SaveToSerializedData(cBinaryBuffer* apBinBuffer);
    void CreateFromSerializedData(cBinaryBuffer* apBinBuffer);

    NewtonCollision* GetNewtonCollision()
    {
        return mpNewtonCollision;
    }

private:
    NewtonCollision* mpNewtonCollision;
    NewtonWorld *mpNewtonWorld;

    tCollideShapeVec mvSubShapes;
};
};
#endif // HPL_COLLIDE_SHAPE_NEWTON_H
