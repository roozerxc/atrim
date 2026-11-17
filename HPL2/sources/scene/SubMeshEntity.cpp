#include "scene/SubMeshEntity.h"

#include "scene/MeshEntity.h"

#include "resources/MaterialManager.h"
#include "resources/MeshManager.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"

#include "graphics/Animation.h"
#include "graphics/AnimationTrack.h"
#include "graphics/Skeleton.h"
#include "graphics/Bone.h"

#include "scene/AnimationState.h"
#include "scene/NodeState.h"

#include "physics/PhysicsBody.h"

#include "math/Math.h"
#include "math/Frustum.h"

#include "graphics/GPUProgram.h"
#include "graphics/MaterialType.h"
#include "system/LowLevelSystem.h"
#include "system/FrameProfiler.h"

namespace hpl
{
//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

cSubMeshEntity::cSubMeshEntity(const tString &asName, cMeshEntity *apMeshEntity, cSubMesh * apSubMesh,
                               cMaterialManager* apMaterialManager) : iRenderable(asName)
{
    mpMeshEntity = apMeshEntity;
    mpSubMesh = apSubMesh;

    mbIsOneSided = mpSubMesh->GetIsOneSided();
    mvOneSidedNormal = mpSubMesh->GetOneSidedNormal();

    mpMaterialManager = apMaterialManager;

    mbGraphicsUpdated = false;

    mpDynVtxBuffer = NULL;
    mbGpuSkinning = false;
    if(mpMeshEntity->GetMesh()->GetSkeleton())
    {
        if(CanUseGpuSkinning(mpSubMesh->GetMaterial()))
        {
            mbGpuSkinning = true;
        }
        else
        {
            CreateCpuSkinBuffer();
        }
    }

    mpLocalNode = NULL;

    mpEntityCallback = hplNew( cSubMeshEntityBodyUpdate, () );
    mbUpdateBody = false;

    mpMaterial = NULL;

    mbIsOccluder = true;
    mpUserData = NULL;

    //This is used to see if null should be returned.
    // 0 = no check made, test if matrix is identity
    // -1 = Matrix was not identity
    // 1 = matrix was identiy
    mlStaticNullMatrixCount = 0;
    mlBoneMatricesUpdateCount = -2;
}

cSubMeshEntity::~cSubMeshEntity()
{
    hplDelete(mpEntityCallback);

    if(mpDynVtxBuffer)
    {
        hplDelete(mpDynVtxBuffer);
    }

    /* Clear any custom textures here*/
    if(mpMaterial)
    {
        mpMaterialManager->Destroy(mpMaterial);
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// BODY CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cSubMeshEntityBodyUpdate::OnTransformUpdate(iEntity3D* apEntity)
{
    /*cSubMeshEntity *pSubEntity = static_cast<cSubMeshEntity*>(apEntity);

    if(pSubEntity->GetBody())
    {
        if(apEntity->GetWorldMatrix() != pSubEntity->GetBody()->GetLocalMatrix())
        {
            Log("Setting matrix on %s from\n",pSubEntity->GetBody()->GetName().c_str());
            Log("  %s\n",apEntity->GetWorldMatrix().ToString().c_str());
            Log("  %s\n",pSubEntity->GetBody()->GetLocalMatrix().ToString().c_str());

            pSubEntity->GetBody()->SetMatrix(apEntity->GetWorldMatrix());
        }
    }*/
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cSubMeshEntity::UpdateLogic(double adFixedDelta)
{

}

//-----------------------------------------------------------------------


cMaterial *cSubMeshEntity::GetMaterial()
{
    if(mpMaterial == NULL && mpSubMesh->GetMaterial() == NULL)
    {
        //Error("Materials for sub entity %s are NULL!\n",GetName().c_str());
    }

    if(mpMaterial)
    {
        return mpMaterial;
    }
    else
    {
        return mpSubMesh->GetMaterial();
    }
}

//-----------------------------------------------------------------------

// Set Src as private variable to give this a little boost! Or?
static inline void MatrixFloatTransformSet(float *pDest, const cMatrixf &a_mtxA, const float* pSrc, const float fWeight)
{
    pDest[0] = ( a_mtxA.m[0][0] * pSrc[0] + a_mtxA.m[0][1] * pSrc[1] + a_mtxA.m[0][2] * pSrc[2] + a_mtxA.m[0][3] ) * fWeight;
    pDest[1] = ( a_mtxA.m[1][0] * pSrc[0] + a_mtxA.m[1][1] * pSrc[1] + a_mtxA.m[1][2] * pSrc[2] + a_mtxA.m[1][3] ) * fWeight;
    pDest[2] = ( a_mtxA.m[2][0] * pSrc[0] + a_mtxA.m[2][1] * pSrc[1] + a_mtxA.m[2][2] * pSrc[2] + a_mtxA.m[2][3] ) * fWeight;
}

static inline void MatrixFloatRotateSet(float *pDest, const cMatrixf &a_mtxA, const float* pSrc, const float fWeight)
{
    pDest[0] = ( a_mtxA.m[0][0] * pSrc[0] + a_mtxA.m[0][1] * pSrc[1] + a_mtxA.m[0][2] * pSrc[2] ) * fWeight;
    pDest[1] = ( a_mtxA.m[1][0] * pSrc[0] + a_mtxA.m[1][1] * pSrc[1] + a_mtxA.m[1][2] * pSrc[2] ) * fWeight;
    pDest[2] = ( a_mtxA.m[2][0] * pSrc[0] + a_mtxA.m[2][1] * pSrc[1] + a_mtxA.m[2][2] * pSrc[2] ) * fWeight;
}

//-----------------------------------------------------------------------

// Set Src as private variable to give this a little boost!Or?
static inline void MatrixFloatTransformAdd(float *pDest, const cMatrixf &a_mtxA, const float* pSrc, const float fWeight)
{
    pDest[0] += ( a_mtxA.m[0][0] * pSrc[0] + a_mtxA.m[0][1] * pSrc[1] + a_mtxA.m[0][2] * pSrc[2] + a_mtxA.m[0][3] ) * fWeight;
    pDest[1] += ( a_mtxA.m[1][0] * pSrc[0] + a_mtxA.m[1][1] * pSrc[1] + a_mtxA.m[1][2] * pSrc[2] + a_mtxA.m[1][3] ) * fWeight;
    pDest[2] += ( a_mtxA.m[2][0] * pSrc[0] + a_mtxA.m[2][1] * pSrc[1] + a_mtxA.m[2][2] * pSrc[2] + a_mtxA.m[2][3] ) * fWeight;
}

static inline void MatrixFloatRotateAdd(float *pDest, const cMatrixf &a_mtxA, const float* pSrc, const float fWeight)
{
    pDest[0] += ( a_mtxA.m[0][0] * pSrc[0] + a_mtxA.m[0][1] * pSrc[1] + a_mtxA.m[0][2] * pSrc[2] ) * fWeight;
    pDest[1] += ( a_mtxA.m[1][0] * pSrc[0] + a_mtxA.m[1][1] * pSrc[1] + a_mtxA.m[1][2] * pSrc[2] ) * fWeight;
    pDest[2] += ( a_mtxA.m[2][0] * pSrc[0] + a_mtxA.m[2][1] * pSrc[1] + a_mtxA.m[2][2] * pSrc[2] ) * fWeight;
}

//-----------------------------------------------------------------------

bool cSubMeshEntity::UpdateGraphicsForViewport(cFrustum *apFrustum, double adFrameTime)
{
    /////////////////
    // Get distance to frustum
    if(IsStatic() == false && apFrustum)
    {
        mfDistanceToFrustum = cMath::Vector3DistSqr(apFrustum->GetOrigin(), GetWorldPosition());
    }

    return true;
}

void cSubMeshEntity::UpdateGraphicsForFrame(double adFrameTime)
{
    ////////////////////////////////////
    //Update things in parent first.
    mpMeshEntity->UpdateGraphicsForFrame(adFrameTime);

    if(mbGpuSkinning)
    {
        UpdateGpuSkinData();
        return;
    }

    ////////////////////////////////////
    // If it has dynamic mesh, update it.
    if(mpDynVtxBuffer)
    {
        if(mpMeshEntity->mbSkeletonPhysicsSleeping && mbGraphicsUpdated)
        {
            return;
        }

        if(mpMeshEntity->mlBoneMatricesUpdateCount == mlBoneMatricesUpdateCount)
        {
            return; //dont update paused animations
        }

        mlBoneMatricesUpdateCount = mpMeshEntity->mlBoneMatricesUpdateCount;
        mbGraphicsUpdated = true;

        SkinOnCpu();
    }

}

//-----------------------------------------------------------------------

void cSubMeshEntity::SkinOnCpu()
{
    if(mpDynVtxBuffer == NULL || mpMeshEntity->mvBoneMatrices.empty())
    {
        return;
    }

    HPL_PROFILE_SCOPE("SkinCPU");

    const float *pBindPos = mpSubMesh->GetVertexBuffer()->GetFloatArray(eVertexBufferElement_Position);
    const float *pBindNormal = mpSubMesh->GetVertexBuffer()->GetFloatArray(eVertexBufferElement_Normal);
    const float *pBindTangent = mpSubMesh->GetVertexBuffer()->GetFloatArray(eVertexBufferElement_Texture1Tangent);

    float *pSkinPos = mpDynVtxBuffer->GetFloatArray(eVertexBufferElement_Position);
    float *pSkinNormal = mpDynVtxBuffer->GetFloatArray(eVertexBufferElement_Normal);
    float *pSkinTangent = mpDynVtxBuffer->GetFloatArray(eVertexBufferElement_Texture1Tangent);

    const int lVtxStride = mpDynVtxBuffer->GetElementNum(eVertexBufferElement_Position);
    const int lVtxNum = mpDynVtxBuffer->GetVertexNum();

    for(int vtx = 0; vtx < lVtxNum; vtx++)
    {
        //To count the bone bindings
        int lCount = 0;
        //Get pointer to weights and bone index.
        const float *pWeight = &mpSubMesh->mpVertexWeights[vtx * 4];
        if(*pWeight == 0)
        {
            //No bones: keeps the bind pose (copied when the buffer was made)
            pBindPos += lVtxStride;
            pSkinPos += lVtxStride;
            pBindNormal += 3;
            pSkinNormal += 3;
            pBindTangent += 4;
            pSkinTangent += 4;
            continue;
        }

        const unsigned char *pBoneIdx = &mpSubMesh->mpVertexBones[vtx * 4];

        const cMatrixf &mtxTransform = mpMeshEntity->mvBoneMatrices[*pBoneIdx];


        MatrixFloatTransformSet(pSkinPos, mtxTransform, pBindPos, *pWeight);

        MatrixFloatRotateSet(pSkinNormal, mtxTransform, pBindNormal, *pWeight);

        MatrixFloatRotateSet(pSkinTangent, mtxTransform, pBindTangent, *pWeight);

        ++pWeight;
        ++pBoneIdx;
        ++lCount;

        //Iterate weights until 0 is found or count < 4
        while(*pWeight != 0 && lCount < 4)
        {
            //Log("Boneidx: %d Count %d Weight: %f\n",(int)*pBoneIdx,lCount, *pWeight);
            const cMatrixf &mtxTransform = mpMeshEntity->mvBoneMatrices[*pBoneIdx];

            //Transform with the local movement of the bone.
            MatrixFloatTransformAdd(pSkinPos, mtxTransform, pBindPos, *pWeight);

            MatrixFloatRotateAdd(pSkinNormal, mtxTransform, pBindNormal, *pWeight);

            MatrixFloatRotateAdd(pSkinTangent, mtxTransform, pBindTangent, *pWeight);

            ++pWeight;
            ++pBoneIdx;
            ++lCount;
        }

        pBindPos += lVtxStride;
        pSkinPos += lVtxStride;

        pBindNormal += 3;
        pSkinNormal += 3;

        pBindTangent += 4;
        pSkinTangent += 4;
    }

    //No stencil shadows:
    /*float *pSkinPosArray = mpDynVtxBuffer->GetArray(eVertexElementFlag_Position);
    if(mpMeshEntity->GetRenderFlagBit(eRenderableFlag_ShadowCaster))
    {
        //Update the shadow double
        memcpy(&pSkinPosArray[lVtxStride*lVtxNum],pSkinPosArray,sizeof(float)*lVtxStride*lVtxNum);
        for(int vtx=lVtxStride*lVtxNum + lVtxStride-1; vtx < lVtxStride*lVtxNum*2; vtx+=lVtxStride)
        {
            pSkinPosArray[vtx] = 0;
        }
    }*/

    //Update buffer
    mpDynVtxBuffer->UpdateData(eVertexElementFlag_Position | eVertexElementFlag_Normal | eVertexElementFlag_Texture1, false);

    //No stencil shadows:
    /*if(mpMeshEntity->GetRenderFlagBit(eRenderableFlag_ShadowCaster))
    {
        //Update triangles
        cMath::CreateTriangleData(mvDynTriangles,
            mpDynVtxBuffer->GetIndices(), mpDynVtxBuffer->GetIndexNum(),
            pSkinPosArray, lVtxStride, lVtxNum);
    }*/
}

//-----------------------------------------------------------------------

bool cSubMeshEntity::CanUseGpuSkinning(cMaterial *apMaterial)
{
    if(apMaterial == NULL || apMaterial->GetType() == NULL || apMaterial->GetType()->SupportsHWSkinning() == false)
    {
        return false;
    }
    if(mpSubMesh->PrepareGpuSkinning() == false)
    {
        return false;
    }

    //Every pass the material is drawn in needs a skinned program
    for(int i = 0; i < eMaterialRenderMode_LastEnum; ++i)
    {
        eMaterialRenderMode mode = (eMaterialRenderMode)i;
        if(apMaterial->GetProgram(0, mode) && apMaterial->GetSkeletonProgram(mode, mpSubMesh->GetGpuSkinProgramType()) == NULL)
        {
            Log("GPU skinning: no skinned program for material '%s' (mode %d), '%s' is skinned on the CPU\n",
                apMaterial->GetName().c_str(), i, msName.c_str());
            return false;
        }
    }

    //Identity until the first bone update
    const size_t lBones = mpSubMesh->GetGpuSkinPalette().size();
    if(mpSubMesh->GetGpuSkinProgramType() == 2)
    {
        mvGpuSkinData.assign(lBones * 12, 0.0f);
        for(size_t i = 0; i < lBones; ++i)
        {
            mvGpuSkinData[i * 12 + 0] = 1.0f;
            mvGpuSkinData[i * 12 + 5] = 1.0f;
            mvGpuSkinData[i * 12 + 10] = 1.0f;
        }
    }
    else
    {
        mvGpuSkinData.assign(lBones * 8, 0.0f);
        for(size_t i = 0; i < lBones; ++i)
        {
            mvGpuSkinData[i * 8 + 3] = 1.0f;
            mvGpuSkinData[i * 8 + 7] = 1.0f;
        }
    }

    return true;
}

//-----------------------------------------------------------------------

void cSubMeshEntity::CreateCpuSkinBuffer()
{
    if(mpDynVtxBuffer)
    {
        return;
    }
    //The GPU skinning weights and indices are not needed in the CPU skinned copy
    tVertexElementFlag vtxFlags = eFlagBit_All;
    if(mpSubMesh->GetGpuSkinPalette().empty() == false)
    {
        vtxFlags = mpSubMesh->GetVertexBuffer()->GetVertexElementFlags() & ~(eVertexElementFlag_Texture2 | eVertexElementFlag_Texture3);
    }
    mpDynVtxBuffer = mpSubMesh->GetVertexBuffer()->CreateCopy(eVertexBufferType_Hardware, eVertexBufferUsageType_Dynamic, vtxFlags);
    mvDynTriangles = *mpSubMesh->GetTriangleVecPtr();
}

//-----------------------------------------------------------------------

void cSubMeshEntity::SwitchToCpuSkinning()
{
    if(mbGpuSkinning == false)
    {
        return;
    }
    mbGpuSkinning = false;
    mvGpuSkinData.clear();

    CreateCpuSkinBuffer();

    //Skin right away, this can be called in the middle of rendering a frame
    mlBoneMatricesUpdateCount = mpMeshEntity->mlBoneMatricesUpdateCount;
    mbGraphicsUpdated = true;
    SkinOnCpu();
}

//-----------------------------------------------------------------------

static bool BoneMatrixToQuatTransScale(const cMatrixf &a_mtxBone, float *apDest)
{
    cVector3f vCol0(a_mtxBone.m[0][0], a_mtxBone.m[1][0], a_mtxBone.m[2][0]);
    cVector3f vCol1(a_mtxBone.m[0][1], a_mtxBone.m[1][1], a_mtxBone.m[2][1]);
    cVector3f vCol2(a_mtxBone.m[0][2], a_mtxBone.m[1][2], a_mtxBone.m[2][2]);

    float fLen0 = vCol0.Length();
    float fLen1 = vCol1.Length();
    float fLen2 = vCol2.Length();
    float fScale = (fLen0 + fLen1 + fLen2) * (1.0f / 3.0f);

    //Mirrored, sheared or non uniformly scaled bones can not be a quaternion and a scale
    if(fScale < 0.000001f || cMath::Vector3Dot(vCol0, cMath::Vector3Cross(vCol1, vCol2)) <= 0)
    {
        return false;
    }
    float fMaxDiff = fScale * 0.005f;
    if(cMath::Abs(fLen0 - fScale) > fMaxDiff || cMath::Abs(fLen1 - fScale) > fMaxDiff || cMath::Abs(fLen2 - fScale) > fMaxDiff ||
       cMath::Abs(cMath::Vector3Dot(vCol0, vCol1)) > fScale * fScale * 0.005f ||
       cMath::Abs(cMath::Vector3Dot(vCol0, vCol2)) > fScale * fScale * 0.005f ||
       cMath::Abs(cMath::Vector3Dot(vCol1, vCol2)) > fScale * fScale * 0.005f)
    {
        return false;
    }

    float fInvScale = 1.0f / fScale;
    cMatrixf mtxRot = cMatrixf::Identity;
    for(int row = 0; row < 3; ++row)
    {
        for(int col = 0; col < 3; ++col)
        {
            mtxRot.m[row][col] = a_mtxBone.m[row][col] * fInvScale;
        }
    }

    cQuaternion qRot;
    qRot.FromRotationMatrix(mtxRot);
    qRot.Normalize();

    apDest[0] = qRot.v.x;
    apDest[1] = qRot.v.y;
    apDest[2] = qRot.v.z;
    apDest[3] = qRot.w;
    apDest[4] = a_mtxBone.m[0][3];
    apDest[5] = a_mtxBone.m[1][3];
    apDest[6] = a_mtxBone.m[2][3];
    apDest[7] = fScale;

    return true;
}

void cSubMeshEntity::UpdateGpuSkinData()
{
    if(mpMeshEntity->mbSkeletonPhysicsSleeping && mbGraphicsUpdated)
    {
        return;
    }
    if(mpMeshEntity->mlBoneMatricesUpdateCount == mlBoneMatricesUpdateCount)
    {
        return;
    }
    mlBoneMatricesUpdateCount = mpMeshEntity->mlBoneMatricesUpdateCount;
    mbGraphicsUpdated = true;

    HPL_PROFILE_SCOPE("SkinGPUData");

    const std::vector<int> &vPalette = mpSubMesh->GetGpuSkinPalette();
    const std::vector<cMatrixf> &vBoneMatrices = mpMeshEntity->mvBoneMatrices;

    if(mpSubMesh->GetGpuSkinProgramType() == 2)
    {
        mvGpuSkinData.resize(vPalette.size() * 12);
        for(size_t i = 0; i < vPalette.size(); ++i)
        {
            float *pDest = &mvGpuSkinData[i * 12];
            int lBone = vPalette[i];
            if(lBone >= (int)vBoneMatrices.size())
            {
                for(int j = 0; j < 12; ++j)
                {
                    pDest[j] = (j == 0 || j == 5 || j == 10) ? 1.0f : 0.0f;
                }
                continue;
            }
            const cMatrixf &mtxBone = vBoneMatrices[lBone];
            for(int row = 0; row < 3; ++row)
            {
                for(int col = 0; col < 4; ++col)
                {
                    pDest[row * 4 + col] = mtxBone.m[row][col];
                }
            }
        }
        return;
    }

    mvGpuSkinData.resize(vPalette.size() * 8);

    for(size_t i = 0; i < vPalette.size(); ++i)
    {
        float *pDest = &mvGpuSkinData[i * 8];
        int lBone = vPalette[i];
        if(lBone >= (int)vBoneMatrices.size())
        {
            pDest[0] = pDest[1] = pDest[2] = 0;
            pDest[3] = 1;
            pDest[4] = pDest[5] = pDest[6] = 0;
            pDest[7] = 1;
            continue;
        }

        if(BoneMatrixToQuatTransScale(vBoneMatrices[lBone], pDest) == false)
        {
            Log("GPU skinning: bone %d of '%s' is not a rotation + uniform scale, skinning it on the CPU\n", lBone, msName.c_str());
            SwitchToCpuSkinning();
            return;
        }
    }
}

//-----------------------------------------------------------------------

bool cSubMeshEntity::SetGpuSkinningUniforms(iGpuProgram *apProgram)
{
    if(apProgram == NULL || mvGpuSkinData.empty())
    {
        return false;
    }

    int lId = apProgram->GetVariableId("avSkinBones");
    if(lId < 0)
    {
        lId = apProgram->GetVariableId("avSkinBones[0]");
    }
    if(lId < 0)
    {
        return false;
    }

    return apProgram->SetVec4fArray(lId, &mvGpuSkinData[0], (int)mvGpuSkinData.size() / 4);
}

//-----------------------------------------------------------------------


iVertexBuffer *cSubMeshEntity::GetVertexBuffer()
{
    if(mpDynVtxBuffer)
    {
        return mpDynVtxBuffer;
    }
    else
    {
        return mpSubMesh->GetVertexBuffer();
    }
}

//-----------------------------------------------------------------------


cBoundingVolume *cSubMeshEntity::GetBoundingVolume()
{
    if(mpMeshEntity->GetMesh()->GetSkeleton())
    {
        return mpMeshEntity->GetBoundingVolume();
    }
    else
    {
        return GetSubMeshBoundingVolume();
    }
}

cBoundingVolume *cSubMeshEntity::GetSubMeshBoundingVolume()
{
    if(mbUpdateBoundingVolume)
    {
        mBoundingVolume.SetTransform(GetWorldMatrix());
        mbUpdateBoundingVolume = false;
    }

    return &mBoundingVolume;
}

//-----------------------------------------------------------------------


int cSubMeshEntity::GetMatrixUpdateCount()
{
    return GetTransformUpdateCount();
}

//-----------------------------------------------------------------------

cMatrixf *cSubMeshEntity::GetModelMatrix(cFrustum *apFrustum)
{
    ////////////////////////
    // Static entity
    if(IsStatic() && mlStaticNullMatrixCount >= 0)
    {
        if(mlStaticNullMatrixCount == 0)
        {
            if(GetWorldMatrix() == cMatrixf::Identity)
            {
                mlStaticNullMatrixCount = 1;
            }
            else
            {
                mlStaticNullMatrixCount = -1;
            }

            if(mlStaticNullMatrixCount == 1)
            {
                return NULL;
            }
            else
            {
                return &GetWorldMatrix();
            }

        }
        else
        {
            return NULL;
        }
    }
    ////////////////////////
    // Dynamic
    else
    {
        return &GetWorldMatrix();
    }
}

//-----------------------------------------------------------------------

void cSubMeshEntity::SetLocalNode(cNode3D *apNode)
{
    mpLocalNode = apNode;

    mpLocalNode->AddEntity(this);
}

//-----------------------------------------------------------------------

cNode3D *cSubMeshEntity::GetLocalNode()
{
    return mpLocalNode;
}

//-----------------------------------------------------------------------

void cSubMeshEntity::SetUpdateBody(bool abX)
{
    mbUpdateBody = abX;

    /*if(mbUpdateBody)
    {
        AddCallback(mpEntityCallback);
    }
    else
    {
        RemoveCallback(mpEntityCallback);
    }*/
}

bool cSubMeshEntity::GetUpdateBody()
{
    return mbUpdateBody;
}

//-----------------------------------------------------------------------

cTriangleData &cSubMeshEntity::GetTriangle(int alIndex)
{
    if(mpDynVtxBuffer)
    {
        return mvDynTriangles[alIndex];
    }
    else
    {
        return (*mpSubMesh->GetTriangleVecPtr())[alIndex];
    }
}
int cSubMeshEntity::GetTriangleNum()
{
    if(mpDynVtxBuffer)
    {
        return (int)mvDynTriangles.size();
    }
    else
    {
        return (int)mpSubMesh->GetTriangleVecPtr()->size();
    }
}

tTriangleDataVec *cSubMeshEntity::GetTriangleVecPtr()
{
    if(mpDynVtxBuffer)
    {
        return &mvDynTriangles;
    }
    else
    {
        return mpSubMesh->GetTriangleVecPtr();
    }
}

//-----------------------------------------------------------------------

void cSubMeshEntity::SetCustomMaterial(cMaterial *apMaterial, bool abDestroyOldCustom)
{
    if(abDestroyOldCustom)
    {
        if(mpMaterial)
        {
            mpMaterialManager->Destroy(mpMaterial);
        }
    }

    mpMaterial = apMaterial;

    if(mbGpuSkinning && CanUseGpuSkinning(GetMaterial()) == false)
    {
        SwitchToCpuSkinning();
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cSubMeshEntity::OnTransformUpdated()
{
    mpMeshEntity->mbUpdateBoundingVolume = true;
}

//-----------------------------------------------------------------------

}
