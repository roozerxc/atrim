#ifndef HPL_SCRIPT_FUNCS_H
#define HPL_SCRIPT_FUNCS_H

#include "physics/PhysicsJoint.h"

namespace hpl {
    
    class cGraphics;
    class cResources;
    class cSystem;
    class cSound;
    class cScene;
    class cInput;
    class cEngine;


    
    //---------------------------------------

    class cScriptJointCallback : public iPhysicsJointCallback
    {
    public:
        cScriptJointCallback(cScene *apScene);

        void OnMinLimit(iPhysicsJoint *apJoint);
        void OnMaxLimit(iPhysicsJoint *apJoint);

        bool IsScript(){ return true;}

        tString msMaxFunc;
        tString msMinFunc;

        cScene *mpScene;
    };
    
    //---------------------------------------

    class cScriptFuncs
    {
    public:
        static void Init(    cGraphics* apGraphics,
                    cResources *apResources,
                    cSystem *apSystem,
                    cInput *apInput,
                    cScene *apScene,
                    cSound *apSound,
                    cEngine *apGame
                    );
    };

};
#endif // HPL_SCRIPT_FUNCS_H
