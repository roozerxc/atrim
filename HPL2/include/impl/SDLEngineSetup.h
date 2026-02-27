#ifndef HPL_SDL_ENGINESETUP_H
#define HPL_SDL_ENGINESETUP_H

#include "system/SystemTypes.h"
#include "engine/LowLevelEngineSetup.h"

namespace hpl {

    class iLowLevelSystem;;
    class iLowLevelGraphics;
    class iLowLevelInput;
    class iLowLevelResources;
    class iLowLevelSound;
    class iLowLevelPhysics;

    class cSDLEngineSetup : public iLowLevelEngineSetup
    {
    public:
        cSDLEngineSetup(tFlag alHplSetupFlags);
        ~cSDLEngineSetup();
        
        cInput* CreateInput(cGraphics* apGraphics);
        cSystem* CreateSystem();
        cGraphics* CreateGraphics();
        cResources* CreateResources(cGraphics* apGraphics);
        cScene* CreateScene(cGraphics* apGraphics, cResources* apResources, cSound* apSound,
                            cPhysics *apPhysics, cSystem *apSystem,cAI *apAI,cGui *apGui);
        cSound* CreateSound();
        cPhysics* CreatePhysics();
        cAI* CreateAI();

    private:
        iLowLevelSystem *mpLowLevelSystem;
        iLowLevelGraphics *mpLowLevelGraphics;
        iLowLevelInput *mpLowLevelInput;
        iLowLevelResources *mpLowLevelResources;
        iLowLevelSound*    mpLowLevelSound;
        iLowLevelPhysics* mpLowLevelPhysics;
    };
};
#endif // HPL_SDL_ENGINESETUP_H
