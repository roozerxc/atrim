#if 0

#include "impl/FmodSoundChannel.h"
#include "impl/FmodSoundData.h"
#include "resources/SoundManager.h"

#include "math/Math.h"

namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    
    cFmodSoundChannel::cFmodSoundChannel(iSoundData* apData, int alChannel,cSoundManager* apSoundManger)
    : iSoundChannel(apData, apSoundManger)
    {
        mlChannel = alChannel;
        mlDefaultFreq = FSOUND_GetFrequency(mlChannel);

        for(int i=0;i<3;i++){
            mfPosition[i] = 0;
            mfVelocity[i] = 0;
        }

        FSOUND_3D_SetAttributes(mlChannel,&mfPosition[0], &mfVelocity[0]);
        FSOUND_3D_SetMinMaxDistance(mlChannel, 100000.0f, 200000.0f);
    }
    
    //-----------------------------------------------------------------------

    cFmodSoundChannel::~cFmodSoundChannel()
    {
        if(mlChannel>0)
            FSOUND_StopSound(mlChannel);

        DestroyData();
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    
    void cFmodSoundChannel::Play()
    {
        SetPaused(false);

        mbStopUsed = false;
    }
    
    //-----------------------------------------------------------------------
    
    void cFmodSoundChannel::Stop()
    {
        FSOUND_StopSound(mlChannel);
        mlChannel = -1;

        mbStopUsed = true;
    }
    
    //-----------------------------------------------------------------------
    
    void cFmodSoundChannel::SetPaused(bool abX)
    {
        mbPaused = abX;
        FSOUND_SetPaused(mlChannel,abX);

        if(mbPaused)
        {
            //Log("Setting %s paused!\n",mpData->GetName().c_str());
        }
    }
    
    //-----------------------------------------------------------------------
    
    void cFmodSoundChannel::SetSpeed(float afSpeed)
    {
        mfSpeed = afSpeed;
        
        int lFreq = (int)(((float)mlDefaultFreq) * afSpeed);

        FSOUND_SetFrequency(mlChannel, lFreq);
    }
    
    //-----------------------------------------------------------------------
    
    void cFmodSoundChannel::SetVolume(float afVolume)
    {
        mfVolume = afVolume;
        
        int lVol = (int)(255.0f*afVolume);

        FSOUND_SetVolume(mlChannel, lVol);
    }
    
    //-----------------------------------------------------------------------
    
    void cFmodSoundChannel::SetLooping (bool abLoop)
    {
        mbLooping = abLoop;

        FSOUND_SetLoopMode(mlChannel, mbLooping?FSOUND_LOOP_NORMAL:FSOUND_LOOP_OFF);
    }

    //-----------------------------------------------------------------------

    void cFmodSoundChannel::SetPan (float afPan)
    {
        int lPan = (int)(255.0f*afPan);

        //Log("Pan: %d\n", lPan);
        
        FSOUND_SetPan(mlChannel, lPan);
    }
    
    //-----------------------------------------------------------------------

    void cFmodSoundChannel::Set3D(bool ab3D)
    {
        mb3D = ab3D;        
    }
    
    //-----------------------------------------------------------------------
    
    void cFmodSoundChannel::SetPositionIsRelative(bool abRelative)
    {
        mbPositionRelative = abRelative;
    }

    //-----------------------------------------------------------------------
    
    void cFmodSoundChannel::SetPosition(const cVector3f &avPos)
    {
        mvPosition = avPos;
        
        FSOUND_3D_SetAttributes(mlChannel,mvPosition.v,mvVelocity.v);
    }
    
    //-----------------------------------------------------------------------

    void cFmodSoundChannel::SetVelocity(const cVector3f &avVel)
    {
        mvVelocity = avVel;

        FSOUND_3D_SetAttributes(mlChannel,mvPosition.v,mvVelocity.v);
    }
    
    //-----------------------------------------------------------------------
    
    void cFmodSoundChannel::SetMinDistance(float afMin)
    {
        mfMinDistance = afMin;

        //FSOUND_3D_SetMinMaxDistance(mlChannel, mfMinDistance, mfMaxDistance);
    }

    //-----------------------------------------------------------------------
    
    void cFmodSoundChannel::SetMaxDistance(float afMax)
    {
        mfMaxDistance = afMax;

        //FSOUND_3D_SetMinMaxDistance(mlChannel, mfMinDistance, mfMaxDistance);
    }
    //-----------------------------------------------------------------------
    
    bool cFmodSoundChannel::IsPlaying()
    {
        return FSOUND_IsPlaying(mlChannel)?true:false;    
    }
    //-----------------------------------------------------------------------
    
    void cFmodSoundChannel::SetPriority(int alX)
    {
        int lPrio = alX+mlPriorityModifier;
        if(lPrio>255)lPrio =255;
        
        FSOUND_SetPriority(mlChannel,lPrio);
    }
    
    //-----------------------------------------------------------------------
    
    int cFmodSoundChannel::GetPriority()
    {
        return FSOUND_GetPriority(mlChannel);
    }
    
    //-----------------------------------------------------------------------

}
#endif
