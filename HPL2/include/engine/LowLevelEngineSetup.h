#ifndef HPL_LOWLEVELENGINESETUP_H
#define HPL_LOWLEVELENGINESETUP_H

namespace hpl
{

class cInput;
class cSystem;
class cGraphics;
class cResources;
class cScene;
class cSound;
class cPhysics;
class cAI;
class cGui;

class iLowLevelEngineSetup
{
public:
    virtual ~iLowLevelEngineSetup() {}

    virtual cInput* CreateInput(cGraphics* apGraphics)=0;
    virtual cSystem* CreateSystem()=0;
    virtual cGraphics* CreateGraphics()=0;
    virtual cResources* CreateResources(cGraphics* apGraphics)=0;
    virtual cScene* CreateScene(cGraphics* apGraphics, cResources* apResources, cSound* apSound,
                                cPhysics *apPhysics, cSystem *apSystem,cAI *apAI,cGui *apGui)=0;
    virtual cSound* CreateSound()=0;
    virtual cPhysics* CreatePhysics()=0;
    virtual cAI* CreateAI()=0;
};
};
#endif // HPL_LOWLEVELENGINESETUP_H
