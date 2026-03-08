#include "impl/SDLEngineSetup.h"

#include "system/System.h"
#include "input/Input.h"
#include "graphics/Graphics.h"
#include "resources/Resources.h"
#include "scene/Scene.h"
#include "sound/Sound.h"
#include "physics/Physics.h"
#include "ai/AI.h"

#include "impl/KeyboardSDL.h"
#include "impl/MouseSDL.h"
#include "impl/LowLevelGraphicsSDL.h"
#include "impl/LowLevelResourcesSDL.h"
#include "impl/LowLevelSystemSDL.h"
#include "impl/LowLevelInputSDL.h"
#include "impl/LowLevelSoundFmod.h"
#include "impl/LowLevelSoundOpenAL.h"
#include "impl/LowLevelPhysicsNewton.h"

#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"

#ifdef _WIN32
#include "Windows.h"
#include "Dbt.h"
#endif

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cSDLEngineSetup::cSDLEngineSetup(tFlag alHplSetupFlags)
{
    if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0)
    {
        FatalError("Error Initializing Display: %s",SDL_GetError());
        exit(1);
    }

    //////////////////////////
    // System
    mpLowLevelSystem = hplNew( cLowLevelSystemSDL, () );

    //////////////////////////
    // Graphics
    mpLowLevelGraphics = hplNew( cLowLevelGraphicsSDL,() );

    //////////////////////////
    // Input
    mpLowLevelInput = hplNew( cLowLevelInputSDL,(mpLowLevelGraphics) );

    //////////////////////////
    // Resources
    mpLowLevelResources = hplNew( cLowLevelResourcesSDL,(mpLowLevelGraphics) );

    //////////////////////////
    // Sound
    mpLowLevelSound    = hplNew( cLowLevelSoundOpenAL,() );

    //////////////////////////
    // Physics
    mpLowLevelPhysics = hplNew( cLowLevelPhysicsNewton,() );

}

//-----------------------------------------------------------------------

cSDLEngineSetup::~cSDLEngineSetup()
{
    Log("- Deleting lowlevel stuff.\n");

    Log("  Physics\n");
    hplDelete(mpLowLevelPhysics);
    Log("  Sound\n");
    hplDelete(mpLowLevelSound);
    Log("  Input\n");
    hplDelete(mpLowLevelInput);
    Log("  Resources\n");
    hplDelete(mpLowLevelResources);
    Log("  System\n");
    hplDelete(mpLowLevelSystem);
    Log("  Graphics\n");
    hplDelete(mpLowLevelGraphics);

    SDL_Quit();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cScene* cSDLEngineSetup::CreateScene(cGraphics* apGraphics, cResources *apResources, cSound* apSound,
                                     cPhysics *apPhysics, cSystem *apSystem,cAI *apAI,cGui *apGui)
{
    cScene *pScene = hplNew( cScene, (apGraphics,apResources, apSound,apPhysics, apSystem,apAI,apGui) );
    return pScene;
}

//-----------------------------------------------------------------------


/**
 * \todo Lowlevelresource and resource both use lowlevel graphics. Can this be fixed??
 * \param apGraphics
 * \return
 */
cResources* cSDLEngineSetup::CreateResources(cGraphics* apGraphics)
{
    cResources *pResources = hplNew( cResources, (mpLowLevelResources,mpLowLevelGraphics) );
    return pResources;
}

//-----------------------------------------------------------------------

cInput* cSDLEngineSetup::CreateInput(cGraphics* apGraphics)
{
    cInput *pInput = hplNew( cInput, (mpLowLevelInput) );
    return pInput;
}

//-----------------------------------------------------------------------

cSystem* cSDLEngineSetup::CreateSystem()
{
    cSystem *pSystem = hplNew( cSystem, (mpLowLevelSystem) );
    return pSystem;
}

//-----------------------------------------------------------------------

cGraphics* cSDLEngineSetup::CreateGraphics()
{
    cGraphics *pGraphics = hplNew( cGraphics, (mpLowLevelGraphics,mpLowLevelResources) );
    return pGraphics;
}
//-----------------------------------------------------------------------

cSound* cSDLEngineSetup::CreateSound()
{
    cSound *pSound = hplNew( cSound, (mpLowLevelSound) );
    return pSound;
}

//-----------------------------------------------------------------------

cPhysics* cSDLEngineSetup::CreatePhysics()
{
    cPhysics *pPhysics = hplNew( cPhysics, (mpLowLevelPhysics) );
    return pPhysics;
}

//-----------------------------------------------------------------------

cAI* cSDLEngineSetup::CreateAI()
{
    cAI *pAI = hplNew( cAI,() );
    return pAI;
}

//-----------------------------------------------------------------------

}
