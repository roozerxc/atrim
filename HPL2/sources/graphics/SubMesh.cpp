#include "graphics/SubMesh.h"

#include "system/String.h"

#include "graphics/Mesh.h"
#include "resources/MaterialManager.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Material.h"
#include "graphics/Skeleton.h"
#include "graphics/Bone.h"
#include "graphics/LowLevelGraphics.h"
#include "system/LowLevelSystem.h"
#include "math/Math.h"

#include "physics/PhysicsWorld.h"

#include "system/MemoryManager.h"

#include <cstring>

namespace hpl
{

bool cSubMesh::mbGpuSkinningEnabled = true;
bool cSubMesh::mbGpuSkinningAuto = false;
int cSubMesh::mlMaxGpuSkinBones = 0;
bool cSubMesh::mbGpuSkinningLimitsSet = false;

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cSubMesh::cSubMesh(const tString &asName, cMaterialManager* apMaterialManager)
{
    mpMaterialManager = apMaterialManager;

    msName = asName;

    mpMaterial = NULL;
    mpVtxBuffer = NULL;

    mbDoubleSided = false;

    mbCollideShape = false;

    mpVertexWeights = NULL;
    mpVertexBones = NULL;

    mlGpuSkinState = 0;
    mbGpuSkinMatrices = false;

    m_mtxLocalTransform = cMatrixf::Identity;

    mbIsOneSided = false;
    mvOneSidedNormal = 0;
    mvOneSidedPoint = 0;
}

//-----------------------------------------------------------------------

cSubMesh::~cSubMesh()
{
    if(mpMaterial)
    {
        mpMaterialManager->Destroy(mpMaterial);
    }
    if(mpVtxBuffer)
    {
        hplDelete(mpVtxBuffer);
    }
    if(mpVertexBones)
    {
        hplDeleteArray(mpVertexBones);
    }
    if(mpVertexWeights)
    {
        hplDeleteArray(mpVertexWeights);
    }

    STLDeleteAll(mvColliders);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cSubMesh::SetMaterial(cMaterial* apMaterial)
{
    if(mpMaterial)
    {
        mpMaterialManager->Destroy(mpMaterial);
    }
    mpMaterial = apMaterial;
}

//-----------------------------------------------------------------------

void cSubMesh::SetVertexBuffer(iVertexBuffer* apVtxBuffer)
{
    if(mpVtxBuffer == apVtxBuffer)
    {
        return;
    }

    mpVtxBuffer = apVtxBuffer;
}

//-----------------------------------------------------------------------

cMaterial *cSubMesh::GetMaterial()
{
    return mpMaterial;
}

//-----------------------------------------------------------------------

iVertexBuffer *cSubMesh::GetVertexBuffer()
{
    return mpVtxBuffer;
}

//-----------------------------------------------------------------------

void cSubMesh::ResizeVertexBonePairs(int alSize)
{
    mvVtxBonePairs.resize(alSize);
}

int cSubMesh::GetVertexBonePairNum()
{
    return (int)mvVtxBonePairs.size();
}
cVertexBonePair &cSubMesh::GetVertexBonePair(int alNum)
{
    return mvVtxBonePairs[alNum];
}

void cSubMesh::AddVertexBonePair(const cVertexBonePair &aPair)
{
    mvVtxBonePairs.push_back(aPair);
}

void cSubMesh::ClearVertexBonePairs()
{
    mvVtxBonePairs.clear();
}



//-----------------------------------------------------------------------

cMeshCollider *cSubMesh::CreateCollider(eCollideShapeType aType)
{
    cMeshCollider* pColl = hplNew( cMeshCollider, () );
    pColl->mType = aType;

    mvColliders.push_back(pColl);

    return pColl;
}

cMeshCollider *cSubMesh::GetCollider(int alIdx)
{
    return mvColliders[alIdx];
}

int cSubMesh::GetColliderNum()
{
    return (int) mvColliders.size();
}

iCollideShape *cSubMesh::CreateCollideShapeFromCollider(cMeshCollider *pCollider, iPhysicsWorld *apWorld, const cVector3f& avSizeMul, cMatrixf *apMtxOffset)
{
    cMatrixf *pOffset = apMtxOffset ? apMtxOffset : &pCollider->m_mtxOffset;

    cVector3f vSize = pCollider->mvSize * avSizeMul;

    switch(pCollider->mType)
    {
    case eCollideShapeType_Box:
        return apWorld->CreateBoxShape(vSize, pOffset);
    case eCollideShapeType_Sphere:
        return apWorld->CreateSphereShape(vSize, pOffset);
    case eCollideShapeType_Cylinder:
        return apWorld->CreateCylinderShape(vSize.x, vSize.y, pOffset);
    case eCollideShapeType_Capsule:
        return apWorld->CreateCapsuleShape(vSize.x, vSize.y, pOffset);
    }

    return NULL;
}

iCollideShape *cSubMesh::CreateCollideShape(iPhysicsWorld *apWorld)
{
    if(mvColliders.empty())
    {
        return NULL;
    }

    //Create a single object
    if(mvColliders.size() == 1)
    {
        return CreateCollideShapeFromCollider(mvColliders[0], apWorld, 1, NULL);
    }
    //Create compound object
    else
    {
        tCollideShapeVec vShapes;
        vShapes.reserve(mvColliders.size());

        for(size_t i = 0; i < mvColliders.size(); ++i)
        {
            vShapes.push_back(CreateCollideShapeFromCollider(mvColliders[i], apWorld, 1, NULL));
        }

        return apWorld->CreateCompundShape(vShapes);
    }
}

//-----------------------------------------------------------------------


void cSubMesh::Compile()
{
    CheckOneSided();
    CompileBonePairs();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIAVTE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cSubMesh::CheckOneSided()
{
    //Log("--- %s\n",GetName().c_str());

    if(mpVtxBuffer == NULL)
    {
        return;
    }

    int lIdxNum = mpVtxBuffer->GetIndexNum();

    if(lIdxNum > 400 * 3)
    {
        return;    //Just skip larger buffers for now, they should never be planes.
    }

    unsigned int *pIndices = mpVtxBuffer->GetIndices();
    float *pPositions = mpVtxBuffer->GetFloatArray(eVertexBufferElement_Position);

    bool bFirst = true;
    cVector3f vNormalSum;
    cVector3f vFirstNormal;
    cVector3f vPosSum = 0;
    int vTri[3];
    const int lVtxStride = mpVtxBuffer->GetElementNum(eVertexBufferElement_Position);
    float fCount = 0;

    for(int i = 0; i < lIdxNum; i += 3)
    {
        //Log("%d \n",i);

        vTri[0] = pIndices[i + 0];
        vTri[1] = pIndices[i + 1];
        vTri[2] = pIndices[i + 2];

        const float *pVtx0 = &pPositions[vTri[0] *    lVtxStride];
        const float *pVtx1 = &pPositions[vTri[1] *    lVtxStride];
        const float *pVtx2 = &pPositions[vTri[2] *    lVtxStride];

        cVector3f vEdge1( pVtx1[0] - pVtx0[0], pVtx1[1] - pVtx0[1], pVtx1[2] - pVtx0[2]);
        cVector3f vEdge2( pVtx2[0] - pVtx0[0], pVtx2[1] - pVtx0[1], pVtx2[2] - pVtx0[2]);

        cVector3f vNormal = cMath::Vector3Normalize(cMath::Vector3Cross(vEdge2, vEdge1));

        //Log(" normal: %s\n",vNormal.ToString().c_str());

        vPosSum += cVector3f(pVtx0[0], pVtx0[1], pVtx0[2]);

        if(bFirst)
        {
            bFirst = false;
            vFirstNormal = vNormal;
            vNormalSum = vNormal;
        }
        else
        {
            if(cMath::Vector3Dot(vFirstNormal, vNormal) < 0.9f)
            {
                return;
            }
            vNormalSum += vNormal;
        }

        fCount += 1;
    }

    mbIsOneSided = true;
    mvOneSidedNormal = cMath::Vector3Normalize(vNormalSum / fCount);
    mvOneSidedPoint = vPosSum / fCount;
}

//-----------------------------------------------------------------------

void cSubMesh::CompileBonePairs()
{
    if(mvVtxBonePairs.empty())
    {
        return;
    }

    mpVertexWeights = hplNewArray( float, 4 * mpVtxBuffer->GetVertexNum());
    mpVertexBones = hplNewArray( unsigned char, 4 * mpVtxBuffer->GetVertexNum()) ;
    memset(mpVertexWeights, 0, 4 * mpVtxBuffer->GetVertexNum()*sizeof(float));
    bool bWarn = true;
    ///////////////////////////////////
    // Iterate pairs and fill arrays
    for(size_t i = 0; i < mvVtxBonePairs.size(); i++)
    {
        cVertexBonePair &Pair = mvVtxBonePairs[i];

        float *pWeight = &mpVertexWeights[Pair.vtxIdx * 4];
        unsigned char *pBoneIdx = &mpVertexBones[Pair.vtxIdx * 4];
        int lPos = -1;
        //Find out where to add the next weight.
        for(int j = 0; j < 4; j++)
        {
            if(pWeight[j] == 0)
            {
                lPos = j;
                break;
            }
        }
        //If no place was found there are too many bones on the vertex.
        if(lPos == -1)
        {
            if (bWarn)
            {
                Warning("More than 4 bones on a vertex in submesh '%s' in mesh '%s' !\n", GetName().c_str(), mpParent->GetName().c_str());
            }
            bWarn = false;
            continue;
        }

        pWeight[lPos] = Pair.weight;
        pBoneIdx[lPos] = Pair.boneIdx;
    }

    bool bUnconnectedVertexes = false;

    /////////////////////////////////
    //Normalize the weights
    for(int vtx = 0; vtx < mpVtxBuffer->GetVertexNum(); ++vtx)
    {
        float *pWeight = &mpVertexWeights[vtx * 4];

        //check if the vertex is missing bone connection
        if(pWeight[0] == 0)
        {
            bUnconnectedVertexes = true;
            continue;
        }

        float fTotal = 0;
        int lNum = 0;
        while(pWeight[lNum] != 0 && lNum < 4)
        {
            fTotal += pWeight[lNum];
            lNum++;
        }
        for(int i = 0; i < lNum; ++i)
        {
            pWeight[i] = pWeight[i] / fTotal;
        }
    }

    /////////////////////////////////
    // Check if any unconnected vertices
    if(bUnconnectedVertexes)
    {
        Warning("Some vertices in sub mesh '%s' in mesh '%s' are not connected to a bone!\n", GetName().c_str(), mpParent->GetName().c_str());
    }

}

//-----------------------------------------------------------------------

void cSubMesh::SetupGpuSkinningLimits(iLowLevelGraphics *apLowLevelGraphics)
{
    if(mbGpuSkinningLimitsSet)
    {
        return;
    }
    mbGpuSkinningLimitsSet = true;

    //Each bone takes two vec4s (rotation quaternion, translation + uniform scale).
    //Leave room for the matrices and other uniforms of the base vertex shader.
    int lComponents = apLowLevelGraphics->GetCaps(eGraphicCaps_MaxVertexUniformComponents);
    int lBones = (lComponents / 4 - 32) / 2;
    if(lBones > 64)
    {
        lBones = 64;
    }
    if(lBones < 8 || apLowLevelGraphics->GetCaps(eGraphicCaps_ShaderModel_2) == 0)
    {
        lBones = 0;
    }
    mlMaxGpuSkinBones = lBones;

    if(mbGpuSkinningEnabled && mbGpuSkinningAuto && apLowLevelGraphics->GetCaps(eGraphicCaps_ShaderModel_3) == 0)
    {
        mbGpuSkinningEnabled = false;
        Log("GPU skinning: off (auto: Shader Model 2 card, skinning on the CPU is faster here)\n");
    }
    else if(mbGpuSkinningEnabled == false)
    {
        Log("GPU skinning: off (config)\n");
    }
    else if(lBones == 0)
    {
        Log("GPU skinning: not supported (%d vertex uniform components), skinning on the CPU\n", lComponents);
    }
    else
    {
        Log("GPU skinning: up to %d bones per sub mesh (%d vertex uniform components)\n", lBones, lComponents);
    }
}

//-----------------------------------------------------------------------

bool cSubMesh::PrepareGpuSkinning()
{
    if(mlGpuSkinState != 0)
    {
        return mlGpuSkinState > 0;
    }
    mlGpuSkinState = -1;

    if(GetGpuSkinningEnabled() == false || mpVertexWeights == NULL || mpVertexBones == NULL || mpVtxBuffer == NULL)
    {
        return false;
    }
    if(mpVtxBuffer->GetElementNum(eVertexBufferElement_Texture2) > 0 || mpVtxBuffer->GetElementNum(eVertexBufferElement_Texture3) > 0)
    {
        Log("GPU skinning: '%s' in '%s' already uses texture coordinates 2/3, skinning on the CPU\n", msName.c_str(), mpParent->GetName().c_str());
        return false;
    }

    const int lVtxNum = mpVtxBuffer->GetVertexNum();

    ////////////////////////////
    //Palette of the bones used
    int vPaletteIdx[256];
    for(int i = 0; i < 256; ++i)
    {
        vPaletteIdx[i] = -1;
    }
    mvGpuSkinPalette.clear();
    for(int vtx = 0; vtx < lVtxNum; ++vtx)
    {
        if(mpVertexWeights[vtx * 4] == 0)
        {
            Log("GPU skinning: '%s' in '%s' has vertices without bones, skinning on the CPU\n", msName.c_str(), mpParent->GetName().c_str());
            mvGpuSkinPalette.clear();
            return false;
        }
        for(int j = 0; j < 4; ++j)
        {
            if(mpVertexWeights[vtx * 4 + j] == 0)
            {
                break;
            }
            int lBone = mpVertexBones[vtx * 4 + j];
            if(vPaletteIdx[lBone] < 0)
            {
                vPaletteIdx[lBone] = (int)mvGpuSkinPalette.size();
                mvGpuSkinPalette.push_back(lBone);
            }
        }
    }

    mbGpuSkinMatrices = (int)mvGpuSkinPalette.size() * 3 <= GetGpuSkinVecs();
    if(mvGpuSkinPalette.empty() || (int)mvGpuSkinPalette.size() > mlMaxGpuSkinBones)
    {
        Log("GPU skinning: '%s' in '%s' uses %d bones (max %d), skinning on the CPU\n", msName.c_str(), mpParent->GetName().c_str(),
            (int)mvGpuSkinPalette.size(), mlMaxGpuSkinBones);
        mvGpuSkinPalette.clear();
        return false;
    }

    ////////////////////////////
    //Weights and palette indices as vertex data
    mpVtxBuffer->CreateElementArray(eVertexBufferElement_Texture2, eVertexBufferElementFormat_Float, 4);
    mpVtxBuffer->CreateElementArray(eVertexBufferElement_Texture3, eVertexBufferElementFormat_Float, 4);
    mpVtxBuffer->ResizeArray(eVertexBufferElement_Texture2, lVtxNum * 4);
    mpVtxBuffer->ResizeArray(eVertexBufferElement_Texture3, lVtxNum * 4);

    float *pWeights = mpVtxBuffer->GetFloatArray(eVertexBufferElement_Texture2);
    float *pIndices = mpVtxBuffer->GetFloatArray(eVertexBufferElement_Texture3);
    if(pWeights == NULL || pIndices == NULL)
    {
        mvGpuSkinPalette.clear();
        return false;
    }

    for(int vtx = 0; vtx < lVtxNum; ++vtx)
    {
        bool bEnd = false;
        for(int j = 0; j < 4; ++j)
        {
            float fWeight = mpVertexWeights[vtx * 4 + j];
            if(fWeight == 0)
            {
                bEnd = true;
            }
            pWeights[vtx * 4 + j] = bEnd ? 0.0f : fWeight;
            pIndices[vtx * 4 + j] = bEnd ? 0.0f : (float)vPaletteIdx[mpVertexBones[vtx * 4 + j]];
        }
    }

    mpVtxBuffer->Compile(0);

    mlGpuSkinState = 1;
    return true;
}

//-----------------------------------------------------------------------
}
