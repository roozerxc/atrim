#ifndef HPL_MESH_MANAGER_H
#define HPL_MESH_MANAGER_H

#include "resources/ResourceManager.h"
#include "resources/ResourcesTypes.h"
#include "graphics/GraphicsTypes.h"

namespace hpl {

    class cGraphics;
    class cResources;
    class cMesh;
    class iVertexBuffer;

    class cMeshManager : public iResourceManager
    {
    public:
        cMeshManager(cGraphics* apGraphics,cResources *apResources);
        ~cMeshManager();

        cMesh* CreateMesh(const tString& asName, tMeshLoadFlag aFlag=0);

        /**
         * Loads only the vertex buffer from the first submesh. Vertexbuffer must be deleted!
         */
        iVertexBuffer* CreateVertexBufferFromMesh(const tString& asName, tVertexElementFlag alVtxToCopy);
    
        void Destroy(iResourceBase* apResource);
        void Unload(iResourceBase* apResource);

        void SetFastloadMaterial(const tString& asFile){ msFastloadMaterial = asFile;}
        void SetUseFastloadMaterial(bool abX){ mbUseFastloadMaterial = abX;}
        const tString& GetFastloadMaterial(){ return msFastloadMaterial;}
        bool GetUseFastloadMaterial(){ return mbUseFastloadMaterial;}

    private:
        cGraphics* mpGraphics;
        cResources* mpResources;

        tString msFastloadMaterial;
        bool mbUseFastloadMaterial;
    };

};
#endif // HPL_MESH_MANAGER_H
