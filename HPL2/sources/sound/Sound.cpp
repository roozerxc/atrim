#include "sound/Sound.h"
#include "system/LowLevelSystem.h"
#include "sound/LowLevelSound.h"
#include "resources/Resources.h"
#include "sound/SoundHandler.h"
#include "sound/MusicHandler.h"
#include "system/FrameProfiler.h"


namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cSound::cSound(iLowLevelSound *apLowLevelSound) : iUpdateable("HPL_Sound")
{
    mpLowLevelSound = apLowLevelSound;
}

//-----------------------------------------------------------------------

cSound::~cSound()
{
    Log("Exiting Sound Module\n");
    Log("--------------------------------------------------------\n");

    hplDelete(mpSoundHandler);
    hplDelete(mpMusicHandler);

    Log("--------------------------------------------------------\n\n");
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cSound::Update(double adFixedDelta)
{
    {
        HPL_PROFILE_SCOPE("SoundEntries");
        mpSoundHandler->Update(adFixedDelta);
    }
    {
        HPL_PROFILE_SCOPE("Music");
        mpMusicHandler->Update(adFixedDelta);
    }
    {
        HPL_PROFILE_SCOPE("SoundLowLevel");
        mpLowLevelSound->UpdateSound(adFixedDelta);
    }
}

//-----------------------------------------------------------------------

void cSound::Init(    cResources *apResources, int alSoundDeviceID, bool abUseEnvAudio, int alMaxChannels,
                      int alStreamUpdateFreq, bool abUseThreading, bool abUseVoiceManagement,
                      int alMaxMonoSourceHint, int alMaxStereoSourceHint,
                      int alStreamingBufferSize, int alStreamingBufferCount, bool abEnableLowLevelLog)
{
    mpResources = apResources;

    Log("Initializing Sound Module\n");
    Log("--------------------------------------------------------\n");

    mpLowLevelSound->Init(    alSoundDeviceID, abUseEnvAudio, alMaxChannels, alStreamUpdateFreq, abUseThreading,
                              abUseVoiceManagement, alMaxMonoSourceHint, alMaxStereoSourceHint,
                              alStreamingBufferSize, alStreamingBufferCount, abEnableLowLevelLog);

    mpSoundHandler = hplNew( cSoundHandler, (mpLowLevelSound, mpResources) );
    mpMusicHandler = hplNew( cMusicHandler, (mpLowLevelSound, mpResources) );

    Log("--------------------------------------------------------\n\n");
}

//-----------------------------------------------------------------------

}
