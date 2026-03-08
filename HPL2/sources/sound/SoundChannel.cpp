#include "sound/SoundChannel.h"

#include "sound/SoundData.h"
#include "resources/SoundManager.h"


namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iSoundChannel::iSoundChannel(iSoundData* apData,cSoundManager* apSoundManger)
{
    mbLooping=false;
    mbPaused=true;
    mbPositionRelative=false;

    mfSpeed=1;
    mfVolume=1;
    mfMaxDistance=0;
    mfMinDistance=0;

    mpData = apData;
    mpData->IncUserCount();

    mpSoundManger = apSoundManger;

    mvVelocity = cVector3f(0,0,0);
    mvPosition = cVector3f(0,0,0);
    mvRelPosition = cVector3f(0,0,0);

    mbBlockable = false;
    mfBlockVolumeMul = 1;

    mbAffectedByEnv = false;

    mlPriorityModifier =0;

    mbStopUsed = false;
}

//-----------------------------------------------------------------------

iSoundChannel::~iSoundChannel()
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iSoundChannel::DestroyData()
{
    if(mpSoundManger)
    {
        mpSoundManger->Destroy(mpData);
    }
}

//-----------------------------------------------------------------------

}
