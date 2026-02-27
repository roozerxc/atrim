#include "graphics/Animation.h"

#include "math/Math.h"
#include "graphics/AnimationTrack.h"

namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cAnimation::cAnimation(const tString &asName, const tWString &asFullPath, const tString &asFile) : iResourceBase(asName, asFullPath,0)
    {
        msAnimName = "";
        msFileName = asFile;
    }

    //-----------------------------------------------------------------------

    cAnimation::~cAnimation()
    {
        STLDeleteAll(mvTracks);
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    
    float cAnimation::GetLength()
    {
        return mfLength;
    }

    //-----------------------------------------------------------------------

    void cAnimation::SetLength(float afTime)
    {
        mfLength = afTime;
    }

    //-----------------------------------------------------------------------

    cAnimationTrack* cAnimation::CreateTrack(const tString &asName, tAnimTransformFlag aFlags)
    {
        cAnimationTrack *pTrack = hplNew( cAnimationTrack,(asName, aFlags, this) );

        mvTracks.push_back(pTrack);

        return pTrack;
    }
    
    //-----------------------------------------------------------------------

    cAnimationTrack* cAnimation::GetTrack(int alIndex)
    {
        return mvTracks[alIndex];
    }

    //-----------------------------------------------------------------------

    cAnimationTrack* cAnimation::GetTrackByName(const tString &asName)
    {
        for(size_t i=0; i< mvTracks.size(); ++i)
        {
            if(asName == tString(mvTracks[i]->GetName()))
            {
                return mvTracks[i];
            }
        }

        return NULL;
    }
    
    //-----------------------------------------------------------------------
    
    void cAnimation::ReserveTrackNum(int alNum)
    {
        mvTracks.reserve(alNum);
    }
    
    //-----------------------------------------------------------------------

    int cAnimation::GetTrackNum()
    {
        return (int)mvTracks.size();
    }

    //-----------------------------------------------------------------------

    void cAnimation::SmoothAllTracks(float afAmount, float afPow, int alSamples,bool abTranslation, bool abRotation)
    {
        for(size_t i=0; i< mvTracks.size(); ++i)
        {
            mvTracks[i]->Smooth(afAmount, afPow, alSamples, abTranslation, abRotation);
        }
    }

    //-----------------------------------------------------------------------


    //////////////////////////////////////////////////////////////////////////
    // PRIVATE METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
}
