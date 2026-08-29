#ifndef HPL_DECALCREATOR_H
#define HPL_DECALCREATOR_H

#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"
#include "math/BoundingVolume.h"

namespace hpl
{

class cMesh2D;
class iVertexBuffer;
class iLowLevelGraphics;
class cResources;
class cMesh;
class cSubMeshEntity;
class cRendererCallbackFunctions;
class cBoundingVolume;

class cDecalCreator
{
public:
    cDecalCreator(iLowLevelGraphics *apLowLevelGraphics, cResources *apResources);
    ~cDecalCreator();

    bool AddSubMesh(cSubMeshEntity* apSubMesh);
    void AddAffectedSubMesh(cSubMeshEntity* apSubMesh);
    void ClearMeshes();

    void SetMaxTrianglesPerDecal(int alMaxTris)
    {
        mlMaxDecalTriangleCount = alMaxTris;
    }

    void SetDecalPosition(const cVector3f& avPosition);
    void SetDecalUp(const cVector3f& avUp, bool abComputeBasis=true);
    void SetDecalRight(const cVector3f& avRight, bool abComputeBasis=true);
    void SetDecalForward(const cVector3f& avForward, bool abComputeBasis=true);
    void SetDecalOffset(float afOffset);
    void SetDecalSize(const cVector3f& avSize);
    void SetMaterial(const tString& asMat);
    void SetColor(const cColor& aCol);
    void SetUVSubDivisions(const cVector2l& avSubDiv);
    void SetCurrentSubDiv(int alSubDiv);

    bool IsUpdated()
    {
        return mbUpdated;
    }
    bool Compile();
    bool CanCreateDecal();


    cMesh* CreateDecalMesh();

    ////////////////////////////////////////////////////////////
    // Debug
    const cVector3f& GetPosition()
    {
        return mvDecalPosition;
    }
    const cVector3f& GetUp()
    {
        return mvDecalUp;
    }
    const cVector3f& GetRight()
    {
        return mvDecalRight;
    }
    const cVector3f& GetForward()
    {
        return mvDecalForward;
    }
    const cVector3f& GetSize()
    {
        return mvDecalSize;
    }
    int GetCurrentSubDiv()
    {
        return mlCurrentSubDiv;
    }

    iVertexBuffer* GetVB()
    {
        return mpDecalVB;
    }
    cMaterial* GetMaterial()
    {
        return mpDecalMaterial;
    }

    void DrawDebug(cRendererCallbackFunctions* apFunctions, bool abDrawAxes, bool abDrawWireframe);

    cBoundingVolume* GetDecalBoundingVolume();

private:
    void ComputeBasis();

    bool AddPolygon(int alVertexCount, const cVector3f* apVertices, const cVector3f* apNormals, iVertexBuffer* apDecalVB,
                    const cMatrixf& amtxWorldMatrix,const cMatrixf& amtxWorldNormalRot);
    void ClipMesh(cSubMeshEntity* apMesh, iVertexBuffer* apDecalVB);
    int ClipPolygon(int alVertexCount, const cVector3f* apVertices, const cVector3f* apNormals,
                    cVector3f* apNewVertices, cVector3f* apNewNormals, const std::vector<cPlanef>& avPlanes);
    int ClipPolygonAgainstPlane(const cPlanef& aPlane, int alVertexCount,
                                const cVector3f* apVertices, const cVector3f* apNormals,
                                cVector3f* apNewVertices, cVector3f* apNewNormals);


    iLowLevelGraphics *mpLowLevelGraphics;
    cResources *mpResources;

    std::vector<cSubMeshEntity*> mvMeshes;

    bool mbCompiled;
    bool mbUpdated;
    bool mbCanCreate;

    bool mbBVUpdated;
    cBoundingVolume mDecalBV;

    cVector3f mvDecalPosition;
    cVector3f mvDecalUp;
    cVector3f mvDecalRight;
    cVector3f mvDecalForward;
    float mfDecalOffset;
    cVector3f mvDecalSize;

    cVector3f mvDebugDecalPos;
    cVector3f mvDebugDecalUp;
    cVector3f mvDebugDecalRight;
    cVector3f mvDebugDecalForward;

    int mlDecalVertexCount;
    int mlDecalTriangleCount;
    int mlMaxDecalTriangleCount;

    tPlanefVec mvClipPlanes;

    tString msMaterial;
    cVector2l mvSubDiv;
    int        mlCurrentSubDiv;
    cColor mColor;

    cMaterial* mpDecalMaterial;
    iVertexBuffer* mpDecalVB;

    tVector3fVec mvTransformedBases;
    tMatrixfVec mvMatrices;
};

};
#endif // HPL_DECALCREATOR_H


