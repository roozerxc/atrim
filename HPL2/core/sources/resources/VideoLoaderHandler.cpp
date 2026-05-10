#include "resources/VideoLoaderHandler.h"

#include "resources/VideoLoader.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "graphics/Graphics.h"

#include "scene/Scene.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cVideoLoaderHandler::cVideoLoaderHandler(cResources* apResources,cGraphics *apGraphics)
{
    mpResources = apResources;
    mpGraphics = apGraphics;
}

//-----------------------------------------------------------------------

cVideoLoaderHandler::~cVideoLoaderHandler()
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iVideoStream* cVideoLoaderHandler::LoadVideo(const tWString& asFile)
{
    iVideoLoader *pVideoLoader = static_cast<iVideoLoader*>(GetLoaderForFile(asFile));

    if(pVideoLoader)
    {
        return pVideoLoader->LoadVideo(asFile);
    }

    return NULL;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


void cVideoLoaderHandler::SetupLoader(iResourceLoader *apLoader)
{
    iVideoLoader *pVideoLoader = static_cast<iVideoLoader*>(apLoader);

    pVideoLoader->mpLowLevelGraphics = mpGraphics->GetLowLevel();
}

//-----------------------------------------------------------------------
}
