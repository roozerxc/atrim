#ifndef HPL_MESHCREATOR_H
#define HPL_MESHCREATOR_H

#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"

namespace hpl {

    class cMesh2D;
    class iVertexBuffer;
    class iLowLevelGraphics;
    class cResources;
    class cMesh;

    class cMeshCreator
    {
    public:
        cMeshCreator(iLowLevelGraphics *apLowLevelGraphics, cResources *apResources);
        ~cMeshCreator();

        iVertexBuffer* CreateWireframeVertexBuffer(iVertexBuffer *apSrc);
        
        cMesh* CreateBox(const tString &asName, cVector3f avSize, const tString &asMaterial);

        cMesh* CreatePlane(const tString &asName, const cVector3f& avCorner1, const cVector3f& avCorner2, 
                           const cVector2f& avCorner1UV, const cVector2f& avCorner2UV, const cVector2f& avCorner3UV, const cVector2f& avCorner4UV, 
                           const tString& asMaterial);

        cMesh* CreateSphere(const tString &asName, float afRadius, int alSections, int alSlices, const tString& asMaterial);

        cMesh* CreateCylinder(const tString &asName, const cVector2f &avSize, int alSections, const tString &asMaterial);
        
        cMesh* CreateCapsule(const tString &asName, const cVector2f &avSize, int alSections, int alSlices, const tString &asMaterial);

        cMesh* CreateCone(const tString &asName, const cVector2f &avSize, int alSections, const tString &asMaterial);
        
        iVertexBuffer* CreateSkyBoxVertexBuffer(float afSize);
        iVertexBuffer* CreateBoxVertexBuffer(cVector3f avSize);

    private:
        cVector3f GetBoxTex(int i,int x, int y, int z, cVector3f *vAdd);
        int GetBoxIdx(int i,int x, int y, int z);
        
        void CreateCircumference(float afRadius, float afSectionStep, float afHeight, tVector3fVec& avPositions);
        void WrapSides(iVertexBuffer* apVtxBuffer, int alStartVertexIdx, int alSections);
        void WrapUpperCap(iVertexBuffer* apVtxBuffer, int alCenterVertexIdx, int alStartVertexIdx, int alSections);
        void WrapLowerCap(iVertexBuffer* apVtxBuffer, int alCenterVertexIdx, int alStartVertexIdx, int alSections);

        iLowLevelGraphics *mpLowLevelGraphics;
        cResources *mpResources;
    };

};
#endif // HPL_MESHCREATOR_H


