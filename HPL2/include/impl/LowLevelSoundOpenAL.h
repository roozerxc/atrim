#ifndef HPL_LOWLEVELSOUND_OPENAL_H
#define HPL_LOWLEVELSOUND_OPENAL_H

#include "sound/LowLevelSound.h"

class cOAL_Effect_Reverb;

namespace hpl
{

class cSoundDeviceIdentifierOpenAL : public iSoundDeviceIdentifier
{
public:
    cSoundDeviceIdentifierOpenAL(int alID, const tString& asName, bool abDefault);

    int GetID()
    {
        return mlID;
    }
    const tString& GetName()
    {
        return msName;
    }
    bool IsDefault()
    {
        return mbDefault;
    }
private:
    int mlID;
    tString msName;
    bool mbDefault;
};

class cLowLevelSoundOpenAL : public iLowLevelSound
{
public:
    cLowLevelSoundOpenAL();
    ~cLowLevelSoundOpenAL();

    void GetSupportedFormats(tStringList &alstFormats);

    iSoundData* LoadSoundData(const tString& asName,const tWString& asFilePath,
                              const tString& asType, bool abStream,bool abLoopStream);

    void UpdateSound(float afTimeStep);

    void SetListenerAttributes (const cVector3f &avPos,const cVector3f &avVel,
                                const cVector3f &avForward,const cVector3f &avUp);
    void SetListenerPosition(const cVector3f &avPos);

    void SetSetRolloffFactor(float afFactor);

    void SetListenerAttenuation (bool abEnabled);

//        void LogSoundStatus();

    void Init(int alSoundDeviceID, bool abUseEnvAudio,int alMaxChannels,
              int alStreamUpdateFreq, bool abUseThreading, bool abUseVoiceManagement,
              int alMaxMonoSourceHint, int alMaxStereoSourceHint,
              int alStreamingBufferSize, int alStreamingBufferCount, bool abEnableLowLevelLog);

    void SetVolume(float afVolume);

    void SetEnvVolume( float afEnvVolume );

    iSoundEnvironment* LoadSoundEnvironment (const tString& asFilePath);
    void SetSoundEnvironment ( iSoundEnvironment* apSoundEnv );
    void FadeSoundEnvironment( iSoundEnvironment* apSourceSoundEnv, iSoundEnvironment* apDestSoundEnv, float afT );

    iSoundDeviceIdentifier* GetCurrentSoundDevice();

private:
    iSoundDeviceIdentifier* GetFirstValidDefaultDevice();
    iSoundDeviceIdentifier* GetFirstDefaultDevice();

    tString mvFormats[30];
    bool    mbLogSounds;
    bool    mbInitialized;
    int        mlEffectSlotId;
    bool    mbNullEffectAttached;

    cOAL_Effect_Reverb* mpEffect;
    int mlCurrentSoundDevID;
};
};
#endif // HPL_LOWLEVELSOUND_OPENAL_H

