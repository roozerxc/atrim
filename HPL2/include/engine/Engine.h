#ifndef HPL_ENGINE_H
#define HPL_ENGINE_H

#include "system/SystemTypes.h"
#include "engine/EngineTypes.h"

namespace hpl {

    class cUpdater;
    class iLowLevelEngineSetup;
    class iLowLevelSystem;
    class cLogicTimer;

    class cSystem;
    class cInput;
    class cResources;
    class cGraphics;
    class cScene;
    class cSound;
    class cPhysics;
    class cAI;
    class cGui;
    class cGenerate;
    class cEngine;
    class cEngineInitVars;
    class iTimer;
    class iMutex;

    //------------------------------------------------------
    
    class cFPSCounter
    {
    public:
        cFPSCounter(iLowLevelSystem* apLowLevelSystem);
        
        void AddFrame();
        
        float mfFPS;
        float mfUpdateRate;
    private:
        iLowLevelSystem* mpLowLevelSystem;
        int mlFramecounter;
        float mfFrametimestart;
        float mfFrametime;
    };

    //---------------------------------------------------

    class cSetupVarContainer
    {
    public:
        cSetupVarContainer();

        void AddString(const tString& asName, const tString& asValue);
        
        void AddInt(const tString& asName, int alValue);
        void AddFloat(const tString& asName, float afValue);
        void AddBool(const tString& asName, bool abValue);

        const tString& GetString(const tString& asName);

        float GetFloat(const tString& asName, float afDefault);
        int GetInt(const tString& asName,int alDefault);
        bool GetBool(const tString& asName, bool abDefault);
    private:
        std::map<tString, tString>  m_mapVars; 
        tString msBlank;
    };

    //---------------------------------------------------
        
    extern cEngine* CreateHPLEngine(eHplAPI aApi, tFlag alHplModuleFlags, cEngineInitVars *apVars);
    extern void DestroyHPLEngine(cEngine* apGame);

    //---------------------------------------------------

    class cEngine
    {
    public:
        cEngine(iLowLevelEngineSetup *apGameSetup,tFlag alHplSetupFlags, cEngineInitVars *apVars);
        ~cEngine();
    private:
        void GameInit(iLowLevelEngineSetup *apGameSetup,tFlag alHplSetupFlags, cEngineInitVars *apVars);

    public:

        /**
         * Starts the game loop. To make stuff run they must be added as updatables..
         */
        void Run();
        /**
         * Exists the game. 
         * \todo is this a good way to do it? Should game be global. If so, make a singleton.
         */
        void Exit();
        bool GetGameIsDone();
        
        cScene* GetScene(){ return mpScene;}
        cResources* GetResources(){ return mpResources;}
        cUpdater* GetUpdater(){ return mpUpdater;}
        cSystem* GetSystem(){ return mpSystem;}
        cInput* GetInput(){ return mpInput;}
        cGraphics* GetGraphics(){ return mpGraphics;}
        cSound* GetSound(){ return mpSound;}
        cPhysics* GetPhysics(){ return mpPhysics;}
        cAI* GetAI(){ return mpAI;}
        cGui* GetGui(){ return mpGui;}
        cGenerate* GetGenerate(){ return mpGenerate;}
        
        void ResetLogicTimer();
        void SetUpdatesPerSec(int alUpdatesPerSec);
        int GetUpdatesPerSec();
        float GetStepSize();

        cLogicTimer* GetLogicTimer(){ return mpLogicTimer;}

        void SetSpeedMul(float afX);

        float GetFPS();
        float GetAvgFrameTimeInMS();

        void SetFPSUpdateRate(float afSec);
        float GetFPSUpdateRate();

        void SetRenderOnce(bool abX){mbRenderOnce = abX;}

        float GetFrameTime(){ return mfFrameTime;}

        double GetGameTime(){ return mfGameTime;}

        void SetLimitFPS(bool abX){ mbLimitFPS = abX;}
        bool GetLimitFPS(){ return mbLimitFPS;}

        void SetPaused(bool abPaused);
        bool GetPaused();

        static void SetDeviceWasPlugged() { mbDevicePlugged = true; }
        static void SetDeviceWasRemoved() { mbDeviceRemoved = true; }
        
        ///// SCRIPT VAR METHODS ////////////////////

        cScriptVar* CreateLocalVar(const tString& asName);
        cScriptVar* GetLocalVar(const tString& asName);
        tScriptVarMap* GetLocalVarMap();
        cScriptVar* CreateGlobalVar(const tString& asName);
        cScriptVar* GetGlobalVar(const tString& asName);
        tScriptVarMap* GetGlobalVarMap();

        void ClearAllVariables();

        ////// ENGINE VARIABLES /////////////////////

        eVariableType GetEngineTypeFromString(const tString& asType);
        eVariableType GetEngineTypeFromStringW(const tWString& asType);
        const tString& GetEngineTypeString(eVariableType aType) { return mvEngineTypeStrings[aType]; }
        

    private:
        void UpdateFrameTimer();

        bool mbGameIsDone;

        bool mbRenderOnce;

        bool mbPaused;

        float mfFrameTime;

        double mfGameTime;

        iLowLevelEngineSetup *mpGameSetup;
        cUpdater *mpUpdater;
        cLogicTimer *mpLogicTimer;

        iMutex *mpMutex;

        cFPSCounter* mpFPSCounter;
        
        iTimer *mpFrameTimer;
        
        bool mbLimitFPS;

        tScriptVarMap m_mapLocalVars;
        tScriptVarMap m_mapGlobalVars;

        bool mbApplicationHasInputFocus;
        bool mbApplicationHasMouseFocus;
        bool mbApplicationIsVisible;

        static bool mbDevicePlugged;
        static bool mbDeviceRemoved;
        
        tStringVec mvEngineTypeStrings;

        //Modules that Game connnect to:
        cResources *mpResources;
        cSystem *mpSystem;
        cInput *mpInput;
        cGraphics *mpGraphics;
        cScene *mpScene;
        cSound *mpSound;
        cPhysics *mpPhysics;
        cAI *mpAI;
        cGui *mpGui;
        cGenerate* mpGenerate;
    };

};
#endif // HPL_ENGINE_H
