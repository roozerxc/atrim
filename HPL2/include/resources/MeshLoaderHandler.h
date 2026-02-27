#ifndef HPL_MESH_LOADER_HANDLER_H
#define HPL_MESH_LOADER_HANDLER_H

#include "resources/ResourceLoaderHandler.h"

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "resources/ResourcesTypes.h"

namespace hpl {
    
    class cMesh;
    class iMeshLoader;
    class cResources;
    class cWorld;
    class cScene;
    class cAnimation;

    //--------------------------------

    class cMeshLoaderHandler : public iResourceLoaderHandler
    {
    public:
        cMeshLoaderHandler(cResources* apResources, cScene *apScene);
        ~cMeshLoaderHandler();

        cMesh* LoadMesh(const tWString& asFile,tMeshLoadFlag aFlags);
        bool SaveMesh(cMesh* apMesh,const tWString& asFile);

        cAnimation *LoadAnimation(const tWString& asFile);

    private:
        void SetupLoader(iResourceLoader *apLoader);

        cResources* mpResources;
        cScene* mpScene;
    };

};
#endif // HPL_MESH_LOADER_HANDLER_H
