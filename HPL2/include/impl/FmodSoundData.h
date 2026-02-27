#if 0

#ifndef HPL_FMOD_SOUND_DATA_H
#define HPL_FMOD_SOUND_DATA_H

#include "sound/SoundData.h"

#include <fmod/fmod.h>

namespace hpl {

    class cFmodSoundData : public iSoundData
    {
    public:
        cFmodSoundData(const tString& asName, bool abStream);
        ~cFmodSoundData();

        bool CreateFromFile(const tWString &asFile);

        iSoundChannel* CreateChannel(int alPriority);

        bool IsStream(){ return mbStream;}

        bool IsStereo(){ return false;}

        //FMOD Specific
        FSOUND_SAMPLE *GetSample(){ return mpSample;}
        FSOUND_STREAM *GetStream(){ return mpStream;}
    
    private:
        FSOUND_SAMPLE * mpSample;
        FSOUND_STREAM * mpStream;
    };
};
#endif // HPL_FMOD_SOUND_DATA_H
#endif
