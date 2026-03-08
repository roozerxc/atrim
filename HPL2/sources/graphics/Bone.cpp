#include "graphics/Bone.h"

#include "graphics/Skeleton.h"
#include "math/Math.h"

#include "system/LowLevelSystem.h"


namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cBone::cBone(const tString &asName,const tString &asSid, cSkeleton* apSkeleton)
{
    msName = asName;
    msSid = asSid;
    mpSkeleton = apSkeleton;
    mpParent = NULL;

    mbNeedsUpdate = true;

    mlValue = 0;
}

//-----------------------------------------------------------------------

cBone::~cBone()
{
    //STLDeleteAll(mlstChildren);
    tBoneListIt it = mlstChildren.begin();
    for(; it != mlstChildren.end(); it++)
    {
        cBone *pBone = *it;

        hplDelete( pBone );
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cBone* cBone::CreateChildBone(const tString &asName,const tString &asId)
{
    cBone *pBone = hplNew(cBone,(asName,asId, mpSkeleton) );
    pBone->mpParent = this;

    mlstChildren.push_back(pBone);
    mpSkeleton->AddBone(pBone);

    return pBone;
}

//-----------------------------------------------------------------------

void cBone::SetTransform(const cMatrixf &a_mtxTransform)
{
    m_mtxTransform = a_mtxTransform;

    NeedsUpdate();
}

//-----------------------------------------------------------------------

//// -- <UNSCALED> -- ///

void cBone::NeedsUpdateUnscaled()
{
    mbNeedsUnscaledUpdate = true;

    tBoneListIt it = mlstChildren.begin();
    for(; it != mlstChildren.end(); it++)
    {
        cBone* pBone = *it;

        pBone->NeedsUpdateUnscaled();
    }
}

void cBone::SetTransformUnscaled(const cMatrixf &a_mtxTransformUnscaled)
{
    m_mtxTransformUnscaled = a_mtxTransformUnscaled;

    NeedsUpdateUnscaled();
}

const cMatrixf& cBone::GetLocalTransformUnscaled()
{
    return m_mtxTransformUnscaled;
}

const cMatrixf& cBone::GetWorldTransformUnscaled()
{
    UpdateUnscaledMatrix();

    return m_mtxWorldTransformUnscaled;
}

void cBone::UpdateUnscaledMatrix()
{
    if(mbNeedsUnscaledUpdate == false) return;

    if(mpParent==NULL)
    {
        m_mtxWorldTransformUnscaled = m_mtxTransformUnscaled;
    }
    else
    {
        m_mtxWorldTransformUnscaled = cMath::MatrixMul(mpParent->GetWorldTransformUnscaled(),m_mtxTransformUnscaled);
    }

    //m_mtxInvWorldTransform = cMath::MatrixInverse(m_mtxWorldTransform);

    mbNeedsUnscaledUpdate = false;
}

//// -- </UNSCALED> -- ///


//-----------------------------------------------------------------------

const cMatrixf& cBone::GetLocalTransform()
{
    return m_mtxTransform;
}

const cMatrixf& cBone::GetWorldTransform()
{
    UpdateMatrix();

    return m_mtxWorldTransform;
}

const cMatrixf& cBone::GetInvWorldTransform()
{
    UpdateMatrix();

    return m_mtxInvWorldTransform;
}

//-----------------------------------------------------------------------

void cBone::Detach()
{
    if(mpParent == NULL) return;

    tBoneListIt it = mpParent->mlstChildren.begin();
    for(; it != mpParent->mlstChildren.end(); it++)
    {
        if(*it == this)
        {
            mpParent->mlstChildren.erase(it);
            break;
        }
    }

    mpSkeleton->RemoveBone(this);
}

//-----------------------------------------------------------------------

cBoneIterator cBone::GetChildIterator()
{
    return cBoneIterator(&mlstChildren);
}

//-----------------------------------------------------------------------

void cBone::UpdateMatrix()
{
    if(mbNeedsUpdate == false) return;

    if(mpParent==NULL)
    {
        m_mtxWorldTransform = m_mtxTransform;
    }
    else
    {
        m_mtxWorldTransform = cMath::MatrixMul(mpParent->GetWorldTransform(),m_mtxTransform);
    }

    m_mtxInvWorldTransform = cMath::MatrixInverse(m_mtxWorldTransform);

    mbNeedsUpdate = false;
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cBone::NeedsUpdate()
{
    mbNeedsUpdate = true;

    tBoneListIt it = mlstChildren.begin();
    for(; it != mlstChildren.end(); it++)
    {
        cBone* pBone = *it;

        pBone->NeedsUpdate();
    }
}
//-----------------------------------------------------------------------
}
