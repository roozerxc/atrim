#include "sound/LowLevelSound.h"

#include "sound/SoundEnvironment.h"

namespace hpl
{

tString iLowLevelSound::msSoundDeviceNameFilter = "";
tSoundDeviceVec iLowLevelSound::mvSoundDevices = tSoundDeviceVec();
bool iLowLevelSound::mbSoundDeviceNameFilterChanged = true;
tSoundDeviceVec iLowLevelSound::mvFilteredSoundDevices = tSoundDeviceVec();

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLowLevelSound::iLowLevelSound()
{
    mfVolume=1;
    mfEnvVolume=0;
    mbListenerAttenuation=true;
    mbHardwareAcc = false;
    mbEnvAudioEnabled = false;
}

//-----------------------------------------------------------------------

iLowLevelSound::~iLowLevelSound()
{
    STLDeleteAll(mvSoundDevices);
    STLDeleteAll(mlstSoundEnv);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iSoundEnvironment* iLowLevelSound::GetSoundEnvironmentFromFileName (const tString& asName)
{
    tString sLowName = cString::ToLowerCase(asName);
    for (tSoundEnvListIt SEIt = mlstSoundEnv.begin(); SEIt != mlstSoundEnv.end(); ++SEIt)
    {
        iSoundEnvironment* pSoundEnv = *SEIt;

        if(sLowName == pSoundEnv->GetFileName()) return pSoundEnv;
    }
    return NULL;
}

//-----------------------------------------------------------------------

/*void iLowLevelSound::DestroySoundEnvironment( iSoundEnvironment* apSoundEnv)
{
    Log(" Destroy %d\n",apSoundEnv);

    STLFindAndDelete(mlstSoundEnv,apSoundEnv);
}*/

//-----------------------------------------------------------------------

const tSoundDeviceVec& iLowLevelSound::GetAvailableSoundDevices()
{
    if(mvSoundDevices.empty())
        PopulateAvailableSoundDevices(mvSoundDevices);

    return mvSoundDevices;
}

const tSoundDeviceVec& iLowLevelSound::GetFilteredSoundDevices()
{
    if(mbSoundDeviceNameFilterChanged)
    {
        mbSoundDeviceNameFilterChanged = false;
        mvFilteredSoundDevices.clear();

        const tSoundDeviceVec& vSndDevices = GetAvailableSoundDevices();

        tString sLowCaseFilter = cString::ToLowerCase(msSoundDeviceNameFilter);
        for(int i=0; i<(int)mvSoundDevices.size(); ++i)
        {
            iSoundDeviceIdentifier* pSndDev = mvSoundDevices[i];
            tString sLowCaseName = cString::ToLowerCase(pSndDev->GetName());

            if(cString::GetFirstStringPos(sLowCaseName, sLowCaseFilter)!=-1)
                mvFilteredSoundDevices.push_back(pSndDev);
        }
    }

    return mvFilteredSoundDevices;
}
}
