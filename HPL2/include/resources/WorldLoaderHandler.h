#ifndef HPL_WORLD_LOADER_HANDLER_H
#define HPL_WORLD_LOADER_HANDLER_H

#include "resources/ResourceLoaderHandler.h"

#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "resources/ResourcesTypes.h"

namespace hpl {
    
    class cResources;
    class cWorld;
    class cScene;
    class cGraphics;
    class cPhysics;
    
    //--------------------------------

    class cWorldLoaderHandler : public iResourceLoaderHandler
    {
    public:
        cWorldLoaderHandler(cResources* apResources,cGraphics *apGraphics, cScene *apScene, cPhysics *apPhysics);
        ~cWorldLoaderHandler();

        cWorld* LoadWorld(const tWString& asFile,tWorldLoadFlag aFlags);
    
    private:
        void SetupLoader(iResourceLoader *apLoader);

        cGraphics *mpGraphics;
        cResources* mpResources;
        cScene* mpScene;
        cPhysics *mpPhysics;
    };

};
#endif // HPL_WORLD_LOADER_HANDLER_H
