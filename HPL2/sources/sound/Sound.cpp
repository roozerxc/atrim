#include "sound/Sound.h"
#include "system/LowLevelSystem.h"
#include "sound/LowLevelSound.h"
#include "resources/Resources.h"
#include "sound/SoundHandler.h"
#include "sound/MusicHandler.h"


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

void cSound::Update(float afTimeStep)
{
    mpSoundHandler->Update(afTimeStep);
    mpMusicHandler->Update(afTimeStep);

    mpLowLevelSound->UpdateSound(afTimeStep);
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
