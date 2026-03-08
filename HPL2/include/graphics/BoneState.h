#ifndef HPL_BONE_STATE_H
#define HPL_BONE_STATE_H

#include "scene/Node3D.h"

namespace hpl
{

class iPhysicsBody;

class cBoneState : public cNode3D
{
public:
    cBoneState(const tString &asName, bool abAutoDeleteChildren);

    void SetBody(iPhysicsBody *apBody);
    iPhysicsBody* GetBody();

    void SetColliderBody(iPhysicsBody *apBody);
    iPhysicsBody* GetColliderBody();

    void SetBodyMatrix(const cMatrixf &a_mtxBody);
    const cMatrixf& GetBodyMatrix();
    const cMatrixf& GetInvBodyMatrix();

private:
    iPhysicsBody *mpBody;
    cMatrixf m_mtxBody;
    cMatrixf m_mtxInvBody;

    iPhysicsBody *mpColliderBody;
};

};
#endif // HPL_BONE_STATE_H
