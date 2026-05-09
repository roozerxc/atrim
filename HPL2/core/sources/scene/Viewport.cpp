#include "scene/Viewport.h"

#include "graphics/Renderer.h"

#include "scene/Scene.h"
#include "scene/World.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cViewport::cViewport(cScene *apScene)
{
    mpScene = apScene;

    mbActive = true;
    mbVisible = true;

    mpRenderSettings = hplNew( cRenderSettings, () );

    mpWorld = NULL;
    mpCamera = NULL;
    mpRenderer = NULL;
    mpPostEffectComposite = NULL;

    mbIsListener = false;
}

//-----------------------------------------------------------------------

cViewport::~cViewport()
{
    hplDelete( mpRenderSettings );
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cViewport::SetWorld(cWorld *apWorld)
{
    if(mpWorld != NULL && mpScene->WorldExists(mpWorld))
    {
        mpWorld->SetIsSoundEmitter(false);
    }

    mpWorld = apWorld;
    if(mpWorld) mpWorld->SetIsSoundEmitter(true);

    mpRenderSettings->ResetVariables();
}


//-----------------------------------------------------------------------

void cViewport::AddGuiSet(cGuiSet *apSet)
{
    mlstGuiSets.push_back(apSet);
}
void cViewport::RemoveGuiSet(cGuiSet *apSet)
{
    STLFindAndRemove(mlstGuiSets, apSet);
}
cGuiSetListIterator cViewport::GetGuiSetIterator()
{
    return cGuiSetListIterator(&mlstGuiSets);
}

//-----------------------------------------------------------------------

void cViewport::AddViewportCallback(iViewportCallback *apCallback)
{
    mlstCallbacks.push_back(apCallback);
}

void cViewport::RemoveViewportCallback(iViewportCallback *apCallback)
{
    STLFindAndRemove(mlstCallbacks, apCallback);
}

void cViewport::RunViewportCallbackMessage(eViewportMessage aMessage)
{
    tViewportCallbackListIt it = mlstCallbacks.begin();
    for(; it != mlstCallbacks.begin(); ++it)
    {
        iViewportCallback *pCallback = *it;

        pCallback->RunMessage(aMessage);
    }

}

//-----------------------------------------------------------------------

void cViewport::AddRendererCallback(iRendererCallback *apCallback)
{
    mlstRendererCallbacks.push_back(apCallback);
}

void cViewport::RemoveRendererCallback(iRendererCallback *apCallback)
{
    STLFindAndRemove(mlstRendererCallbacks, apCallback);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------

}
