#ifndef HPL_SUB_MESH_H
#define HPL_SUB_MESH_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "math/MeshTypes.h"
#include "physics/PhysicsTypes.h"

namespace hpl
{

class cMaterial;
class iVertexBuffer;

class cMesh;
class iPhysicsWorld;
class iCollideShape;

class cMaterialManager;
class iLowLevelGraphics;

//--------------------------------------------------

class cMeshCollider
{
public:
    tString msGroup; // Only used as temp var when loading!

    eCollideShapeType mType;
    cVector3f mvSize;
    cMatrixf m_mtxOffset;
    bool mbCharCollider;
};

typedef std::vector<cMeshCollider *> tMeshColliderVec;
typedef tMeshColliderVec::iterator tMeshColliderVecIt;

//--------------------------------------------------

class cSubMesh
{
    friend class cMesh;
    friend class cSubMeshEntity;
public:
    cSubMesh(const tString &asName, cMaterialManager* apMaterialManager);
    ~cSubMesh();

    void SetMaterial(cMaterial* apMaterial);
    void SetVertexBuffer(iVertexBuffer* apVtxBuffer);

    //Renderable implementation.
    cMaterial *GetMaterial();
    iVertexBuffer *GetVertexBuffer();

    const tString &GetName()
    {
        return msName;
    }

    //Vertex-Bone pairs
    void ResizeVertexBonePairs(int alSize);
    int GetVertexBonePairNum();
    cVertexBonePair &GetVertexBonePair(int alNum);

    void AddVertexBonePair(const cVertexBonePair &aPair);
    void ClearVertexBonePairs();

    //Colliders
    cMeshCollider *CreateCollider(eCollideShapeType aType);
    cMeshCollider *GetCollider(int alIdx);
    int GetColliderNum();
    iCollideShape *CreateCollideShape(iPhysicsWorld *apWorld);
    static iCollideShape *CreateCollideShapeFromCollider(cMeshCollider *pCollider, iPhysicsWorld *apWorld, const cVector3f& avSizeMul, cMatrixf *apMtxOffset);

    void SetIsCollideShape(bool abX)
    {
        mbCollideShape = abX;
    }
    bool IsCollideShape()
    {
        return mbCollideShape;
    }

    const cTriEdge &GetEdge(int alIndex) const
    {
        return mvEdges[alIndex];
    }
    int GetEdgeNum()
    {
        return (int)mvEdges.size();
    }

    tTriEdgeVec *GetEdgeVecPtr()
    {
        return &mvEdges;
    }

    tTriangleDataVec *GetTriangleVecPtr()
    {
        return &mvTriangles;
    }

    void SetDoubleSided(bool abX)
    {
        mbDoubleSided = abX;
    }
    bool GetDoubleSided()
    {
        return mbDoubleSided;
    }

    void SetModelScale(const cVector3f& avScale)
    {
        mvModelScale = avScale;
    }
    cVector3f GetModelScale()
    {
        return mvModelScale;
    }

    const cMatrixf &GetLocalTransform()
    {
        return m_mtxLocalTransform;
    }
    void SetLocalTransform(const cMatrixf& a_mtxTrans)
    {
        m_mtxLocalTransform = a_mtxTrans;
    }

    bool GetIsOneSided()
    {
        return mbIsOneSided;
    }
    const cVector3f &GetOneSidedNormal()
    {
        return mvOneSidedNormal;
    }
    const cVector3f &GetOneSidedPoint()
    {
        return mvOneSidedPoint;
    }

    void SetMaterialName(const tString& asName)
    {
        msMaterialName = asName;
    }
    const tString &GetMaterialName()
    {
        return msMaterialName;
    }

    void Compile();

    //GPU skinning: per vertex weights (texcoord 2) and palette indices (texcoord 3) are
    //added to the vertex buffer, each palette entry is a skeleton bone index.
    static void SetGpuSkinningEnabled(bool abX)
    {
        mbGpuSkinningEnabled = abX;
    }
    static bool GetGpuSkinningEnabled()
    {
        return mbGpuSkinningEnabled && mlMaxGpuSkinBones > 0;
    }
    static void SetupGpuSkinningLimits(iLowLevelGraphics *apLowLevelGraphics);
    static int GetMaxGpuSkinBones()
    {
        return mlMaxGpuSkinBones;
    }
    //Size of the bone uniform array, in vec4s
    static int GetGpuSkinVecs()
    {
        return (mlMaxGpuSkinBones > 1 ? mlMaxGpuSkinBones : 1) * 2;
    }

    bool PrepareGpuSkinning();
    const std::vector<int> &GetGpuSkinPalette() const
    {
        return mvGpuSkinPalette;
    }
    //Palettes small enough are sent as 3x4 matrices (exact for any bone), bigger ones as
    //quaternion + translation/scale (needs rigid bones). Same numbers as cMaterial::GetSkeletonProgram.
    char GetGpuSkinProgramType() const
    {
        return mbGpuSkinMatrices ? 2 : 1;
    }

private:
    void CheckOneSided();
    void CompileBonePairs();

    static bool mbGpuSkinningEnabled;
    static int mlMaxGpuSkinBones;
    static bool mbGpuSkinningLimitsSet;

    int mlGpuSkinState;
    bool mbGpuSkinMatrices;
    std::vector<int> mvGpuSkinPalette;

    tString msName;

    tString msMaterialName;
    cMaterial *mpMaterial;
    iVertexBuffer *mpVtxBuffer;

    cMatrixf m_mtxLocalTransform;

    tVertexBonePairVec mvVtxBonePairs;

    tMeshColliderVec mvColliders;

    float *mpVertexWeights;
    unsigned char *mpVertexBones;

    tTriEdgeVec mvEdges;
    tTriangleDataVec mvTriangles;

    cVector3f mvModelScale;

    bool mbDoubleSided;

    bool mbCollideShape;

    bool mbIsOneSided;
    cVector3f mvOneSidedNormal;
    cVector3f mvOneSidedPoint;

    cMaterialManager *mpMaterialManager;

    cMesh *mpParent;
};

};
#endif // HPL_SUB_MESH_H
