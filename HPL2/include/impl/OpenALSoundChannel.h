#ifndef HPL_OPENAL_SOUND_CHANNEL_H
#define HPL_OPENAL_SOUND_CHANNEL_H

#include "sound/SoundChannel.h"
#include "sound/SoundData.h"

#include "OpenAL/OAL_Funcs.h"

namespace hpl {

    class cOpenALSoundChannel : public iSoundChannel
    {
    public:
        cOpenALSoundChannel(iSoundData* apData, int alChannel, cSoundManager* apSoundManger);
        ~cOpenALSoundChannel();

        void Play();
        void Stop();

        void SetPaused(bool abX);
        void SetSpeed(float afSpeed);
        void SetVolume (float afVolume); 
        void SetLooping (bool abLoop);
        void SetPan (float afPan);
        void Set3D(bool ab3D);

        void SetPriority(int alX);
        int GetPriority();

        void SetPositionIsRelative(bool abRelative);
        void SetPosition(const cVector3f &avPos);
        void SetVelocity(const cVector3f &avVel);
        
        void SetMinDistance(float fMin);
        void SetMaxDistance(float fMax); 

        bool IsPlaying();

        bool IsBufferUnderrun();
        double GetElapsedTime();
        double GetTotalTime();
        void SetElapsedTime(double afTime);

        void SetAffectedByEnv(bool abAffected);
        void SetFiltering ( bool abEnabled, int alFlags);
        void SetFilterGain(float afGain);
        void SetFilterGainHF(float afGainHF);
    
    private:
        int mlChannel;
        int mlDefaultFreq;

        float mfPosition[3];
        float mfVelocity[3];
    };
};
#endif // HPL_OPENAL_CHANNEL_H
