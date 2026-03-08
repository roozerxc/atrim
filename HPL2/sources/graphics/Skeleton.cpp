#include "graphics/Skeleton.h"

#include "graphics/Bone.h"
#include "system/MemoryManager.h"
#include "system/LowLevelSystem.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cSkeleton::cSkeleton()
{
    mpRootBone = hplNew( cBone, ("__root_bone","",this) );
    mpRootBone->SetTransform(cMatrixf::Identity);
    mpRootBone->SetTransformUnscaled(cMatrixf::Identity);
}

//-----------------------------------------------------------------------

cSkeleton::~cSkeleton()
{
    //All bones are deleted in this call.
    //No need to delete the bones in the containers.
    hplDelete(mpRootBone);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cSkeleton::AddBone(cBone* apBone)
{
    mvBones.push_back(apBone);

    m_mapBonesIdxByName.insert(tBoneIdxNameMap::value_type(apBone->GetName(), (int)mvBones.size()-1));
}

void cSkeleton::RemoveBone(cBone* apBone)
{
    tBoneVecIt vecIt = mvBones.begin();
    for(; vecIt != mvBones.end(); ++vecIt)
    {
        if(*vecIt == apBone)
        {
            mvBones.erase(vecIt);
            break;
        }
    }

    //Rebuild the map
    m_mapBonesIdxByName.clear();

    for(int i=0; i< (int) mvBones.size(); i++)
    {
        m_mapBonesIdxByName.insert(tBoneIdxNameMap::value_type(
                                       mvBones[i]->GetName(),i));
    }
}

//-----------------------------------------------------------------------

cBone* cSkeleton::GetRootBone()
{
    return mpRootBone;
}

//-----------------------------------------------------------------------

cBone* cSkeleton::GetBoneByIndex(int alIndex)
{
    return mvBones[alIndex];
}

cBone* cSkeleton::GetBoneByName(const tString &asName)
{
    int alIdx = GetBoneIndexByName(asName);
    if(alIdx <0) return NULL;

    return mvBones[alIdx];
}
cBone* cSkeleton::GetBoneBySid(const tString &asSid)
{
    for(size_t i=0; i<mvBones.size(); ++i)
    {
        if(mvBones[i]->GetSid() == asSid) return mvBones[i];
    }
    return NULL;
}

int cSkeleton::GetBoneIndexByName(const tString &asName)
{
    tBoneIdxNameMapIt it = m_mapBonesIdxByName.find(asName);
    if(it == m_mapBonesIdxByName.end()) return -1;

    return it->second;
}
int cSkeleton::GetBoneIndexBySid(const tString &asSid)
{
    for(size_t i=0; i<mvBones.size(); ++i)
    {
        if(mvBones[i]->GetSid() == asSid) return (int)i;
    }
    return-1;
}
int cSkeleton::GetBoneNum()
{
    return (int)mvBones.size();
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
}
