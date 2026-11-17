#ifndef LUX_CONFIG_HANDLER_H
#define LUX_CONFIG_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxConfigHandler
{
public:
    cLuxConfigHandler();
    ~cLuxConfigHandler();

    void LoadMainConfig();
    void SaveMainConfig();

    bool ShowRestartWarning(cGuiSet* apSet, void* apObject, tGuiCallbackFunc apCallback);

    void SetGameNeedsRestart()
    {
        mbGameNeedsRestart = true;
        mbRestartDialogShown = false;
    }

    // Variables
    bool mbLoadDebugMenu;
    bool mbFirstStart;
    tString msScreenShotExt;

    bool mbCreateAndLoadCompressedMaps;
    bool mbForceCacheLoadingAndSkipSaving;

    tString msLangFile;

    cVector2l mvScreenSize;
    bool mbFullscreen;
    bool mbVSync;
    bool mbRenderInterpolation;
    bool mbFrameRateMouseLook;
    int mlFrameRateLimit; //0 = no limit
    int mlTextureQuality;
    int mlTextureFilter;
    float mfTextureAnisotropy;
    int mlShadowQuality;
    int mlShadowRes;

    bool mbSSAOActive;
    int mlSSAOSamples;
    int mlSSAOResolution; //0= medium(div2), 1=high (same as screen resolution)

    int mlParallaxQuality;
    bool mbParallaxEnabled;

    bool mbOcclusionTestLights;

    bool mbWorldReflection;
    bool mbRefraction;
    bool mbShadowsActive;

    bool mbForceShaderModel3And4Off;
    bool mbForceGLSL110;
    bool mbForceNoStencil;
    bool mbForceRectAs2D;
    bool mbLogGLErrors;
    bool mbForceVertexArrays;
    bool mbDisablePostEffects;
    bool mbRenderTrace;
    bool mbRenderTraceSync;
    bool mbOcclusionCulling;
    bool mbShaderWarming;
    int mlMaxFramesAhead;
    int mlFullscreenMode;
    int mlLightColorGradeMode;
    int mlPhysicsPreSettleSteps;
    bool mbLogMoveObjects;

    bool mbFrameProfiler;
    int mlFrameProfilerFrames;
    float mfFrameProfilerSpikeMs;

    bool mbFastPhysicsLoad;
    bool mbFastStaticLoad;
    bool mbFastEntityLoad;

    int mlSoundDevID;
    int mlMaxSoundChannels;
    int mlSoundStreamBuffers;
    int mlSoundStreamBufferSize;
    int mlSoundAutoStreamKB;
    bool mbSoundPreload;


private:
    bool mbGameNeedsRestart;
    bool mbRestartDialogShown;
};

//----------------------------------------------


#endif // LUX_DEBUG_HANDLER_H
