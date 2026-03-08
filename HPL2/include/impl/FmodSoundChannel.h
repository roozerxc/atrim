#if 0

#ifndef HPL_FMOD_SOUND_CHANNEL_H
#define HPL_FMOD_SOUND_CHANNEL_H

#include "sound/SoundChannel.h"
#include "sound/SoundData.h"

#include <fmod/fmod.h>

namespace hpl
{

class cFmodSoundChannel : public iSoundChannel
{
public:
    cFmodSoundChannel(iSoundData* apData, int alChannel, cSoundManager* apSoundManger);
    ~cFmodSoundChannel();

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
    bool IsBufferUnderrun()
    {
        return false;
    }
    double GetElapsedTime ()
    {
        return 0;
    }
    double GetTotalTime ()
    {
        return 0;
    }
    void SetElapsedTime(double afTime) {}

    void SetAffectedByEnv(bool abAffected) {}
    void SetFiltering ( bool abEnabled, int alFlags) {}
    void SetFilterGain(float afGain) {}
    void SetFilterGainHF(float afGainHF) {}


private:
    int mlChannel;
    int mlDefaultFreq;

    float mfPosition[3];
    float mfVelocity[3];
};
};
#endif // HPL_SOUND_CHANNEL_H
#endif
