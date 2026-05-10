#ifndef HPL_WORLD_LOADER_H
#define HPL_WORLD_LOADER_H

#include "resources/ResourceLoader.h"

namespace hpl
{

class cScene;
class cWorld;
class cPhysics;
class cGraphics;
class cResources;

//----------------------------------------

class iWorldLoader : public iResourceLoader
{
    friend class cWorldLoaderHandler;
public:

    virtual cWorld* LoadWorld(const tWString& asFile, tWorldLoadFlag aFlags)=0;

protected:

    cScene *mpScene;
    cGraphics *mpGraphics;
    cResources *mpResources;
    cPhysics *mpPhysics;
};

};
#endif // HPL_MESH_LOADER_H
