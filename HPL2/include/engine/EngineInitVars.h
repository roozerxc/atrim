#ifndef HPL_ENGINE_INIT_VARS_H
#define HPL_ENGINE_INIT_VARS_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

namespace hpl {

    //---------------------------------------
    
    class cEngineInitVars
    {
    public:
        ////////////////////////////////
        // Game
        class cEngineVars
        {
        public:
            cEngineVars() :
                mlUpdateRate(60)
              {}

              int mlUpdateRate;
        };
        cEngineVars mGame;

        ////////////////////////////////
        // Graphics
        class cGraphicsVars
        {
        public:
            cGraphicsVars() :
                mvScreenSize(800,600),
                mlDisplay(0),
                mlScreenBpp(32),
                mbFullscreen(false),
                mlMultisampling(0),
                msWindowCaption(""),
                mvWindowPosition(-1),
                mGpuProgramFormat(eGpuProgramFormat_LastEnum)
            {}
        
            cVector2l mvScreenSize;
            int mlDisplay;
            int mlScreenBpp;
            bool mbFullscreen;
            int mlMultisampling;
            tString msWindowCaption;
            cVector2l mvWindowPosition;
            eGpuProgramFormat mGpuProgramFormat;
        };
        cGraphicsVars mGraphics;
        
        ////////////////////////////////
        // Sound
        class cSoundVars
        {
        public:
            cSoundVars() :
                mlSoundDeviceID(-1),
                mbUseEnvironmentalAudio(false),
                mbUseThreading(true),
                mbUseVoiceManagement(true),
                mbLowLevelLogging(false),
                mlStreamUpdateFreq(10),
                mlMaxChannels(32),
                mlMaxMonoChannelsHint(0),
                mlMaxStereoChannelsHint(0),
                mlStreamBufferSize(524288),
                mlStreamBufferCount(2)
            {}
                
            int    mlSoundDeviceID;
            bool mbUseEnvironmentalAudio;
            bool mbUseThreading;
            bool mbUseVoiceManagement;
            bool mbLowLevelLogging;
            int mlStreamUpdateFreq;
            int mlMaxChannels;
            int mlMaxMonoChannelsHint;
            int mlMaxStereoChannelsHint;
            int mlStreamBufferSize;
            int mlStreamBufferCount;
        };
        cSoundVars mSound;            

    };

    //---------------------------------------

};

#endif // HPL_ENGINE_INIT_VARS_H
