#include "resources/WorldLoaderHandler.h"

#include "resources/WorldLoader.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"

#include "scene/Scene.h"

namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    
    cWorldLoaderHandler::cWorldLoaderHandler(cResources* apResources,cGraphics *apGraphics, cScene *apScene, cPhysics *apPhysics)
    {
        mpResources = apResources;
        mpScene = apScene;
        mpGraphics = apGraphics;
        mpPhysics = apPhysics;
    }
    
    //-----------------------------------------------------------------------

    cWorldLoaderHandler::~cWorldLoaderHandler()
    {
        
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    
    cWorld* cWorldLoaderHandler::LoadWorld(const tWString& asFile,tWorldLoadFlag aFlags)
    {
        iWorldLoader *pWorldLoader = static_cast<iWorldLoader*>(GetLoaderForFile(asFile));

        if(pWorldLoader)
        {
                return pWorldLoader->LoadWorld(asFile,aFlags);
        }
        
        return NULL;
    }

    //-----------------------------------------------------------------------

    
    //////////////////////////////////////////////////////////////////////////
    // PRIVATE METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    
    void cWorldLoaderHandler::SetupLoader(iResourceLoader *apLoader)
    {
        iWorldLoader *pWorldLoader = static_cast<iWorldLoader*>(apLoader);
        
        pWorldLoader->mpResources = mpResources;
        pWorldLoader->mpGraphics = mpGraphics;
        pWorldLoader->mpScene = mpScene;
        pWorldLoader->mpPhysics = mpPhysics;
    }

    //-----------------------------------------------------------------------
}
