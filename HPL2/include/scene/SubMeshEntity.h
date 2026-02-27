#ifndef HPL_SUB_MESH_ENTITY_H
#define HPL_SUB_MESH_ENTITY_H

#include <vector>
#include <map>

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "scene/Entity3D.h"
#include "graphics/Renderable.h"
#include "math/MeshTypes.h"

namespace hpl {

    class cMaterialManager;
    class cMeshManager;
    class cMesh;
    class cSubMesh;
    class cMeshEntity;
    class cAnimationState;
    class cNodeState;
    class cBone;
    class cNode3D;
    class iPhysicsBody;
    class cMaterial;
    class cBoneState;

    //-----------------------------------------------------------------------

    
    

    //-----------------------------------------------------------------------

    class cSubMeshEntityBodyUpdate : public iEntityCallback
    {
    public:
        void OnTransformUpdate(iEntity3D * apEntity);
    };

    //-----------------------------------------------------------------------

    class cSubMeshEntity : public iRenderable
    {
    #ifdef __GNUC__
        typedef iRenderable __super;
    #endif
        friend class cMeshEntity;
    public:
        cSubMeshEntity(const tString &asName,cMeshEntity *apMeshEntity, cSubMesh * apSubMesh,cMaterialManager* apMaterialManager);
        ~cSubMeshEntity();

        cMaterial *GetMaterial();

        void UpdateGraphicsForFrame(float afFrameTime);

        iVertexBuffer* GetVertexBuffer();

        cBoundingVolume* GetBoundingVolume();

        cMatrixf* GetModelMatrix(cFrustum *apFrustum);

        int GetMatrixUpdateCount();

        eRenderableType GetRenderType(){ return eRenderableType_SubMesh;}

        cSubMesh* GetSubMesh() const { return mpSubMesh;}

        void SetLocalNode(cNode3D *apNode);
        cNode3D* GetLocalNode();

        void* GetUserData(){ return mpUserData;}
        void SetUserData(void *apData){ mpUserData = apData;}

        //Entity implementation
        tString GetEntityType(){ return "SubMesh";}
        
        void UpdateLogic(float afTimeStep);

        cTriangleData& GetTriangle(int alIndex);
        int GetTriangleNum();
        tTriangleDataVec* GetTriangleVecPtr();

        void SetUpdateBody(bool abX);
        bool GetUpdateBody();

        void SetCustomMaterial(cMaterial *apMaterial, bool abDestroyOldCustom=true);
        cMaterial* GetCustomMaterial(){ return mpMaterial;}

    private:
        void OnTransformUpdated();

        cSubMesh *mpSubMesh;
        cMeshEntity *mpMeshEntity;

        cMaterial *mpMaterial;

        cNode3D *mpLocalNode;

        cMaterialManager* mpMaterialManager;

        iVertexBuffer* mpDynVtxBuffer;
        tTriangleDataVec mvDynTriangles;

        cSubMeshEntityBodyUpdate* mpEntityCallback;
        bool mbUpdateBody;

        bool mbGraphicsUpdated;

        char mlStaticNullMatrixCount;
        void *mpUserData;
    };

    typedef std::vector<cSubMeshEntity*> tSubMeshEntityVec;
    typedef std::vector<cSubMeshEntity*>::iterator tSubMeshEntityVecIt;

    typedef std::multimap<tString,cSubMeshEntity*> tSubMeshEntityMap;
    typedef tSubMeshEntityMap::iterator tSubMeshEntityMapIt;

};
#endif // HPL_SUB_MESH_ENTITY_H
