#ifndef HPL_SOUND_H
#define HPL_SOUND_H

#include "engine/Updateable.h"

namespace hpl {
    
    class iLowLevelSound;
    class cResources;
    class cSoundHandler;
    class cMusicHandler;

    class cSound : public iUpdateable
    {
    public:
        cSound(iLowLevelSound *apLowLevelSound);
        ~cSound();

        void Init(    cResources *apResources, int alSoundDeviceID, bool abUseEnvAudio, int alMaxChannels, 
                        int alStreamUpdateFreq, bool abUseThreading, bool abUseVoiceManagement,
                        int alMaxMonoSourceHint, int alMaxStereoSourceHint,
                        int alStreamingBufferSize, int alStreamingBufferCount, bool abEnableLowLevelLog);

        void Update(float afTimeStep);

        iLowLevelSound* GetLowLevel(){ return mpLowLevelSound;}
        cSoundHandler* GetSoundHandler(){ return mpSoundHandler; }
        cMusicHandler* GetMusicHandler(){ return mpMusicHandler; }
        
    private:
        iLowLevelSound *mpLowLevelSound;
        cResources* mpResources;
        cSoundHandler* mpSoundHandler;
        cMusicHandler* mpMusicHandler;
    };

};
#endif // HPL_SOUND_H
