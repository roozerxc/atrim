#ifndef HPL_OPENAL_SOUND_DATA_H
#define HPL_OPENAL_SOUND_DATA_H

#include "sound/SoundData.h"

#ifdef USE_OALWRAPPER
# include "OALWrapper/OAL_Funcs.h"
#else
# include "OpenAL/OAL_Funcs.h"
#endif

namespace hpl {

    class cOpenALSoundData : public iSoundData
    {
    public:
        cOpenALSoundData(const tString& asName, bool abStream);
        ~cOpenALSoundData();

        bool CreateFromFile(const tWString &asFile);

        iSoundChannel* CreateChannel(int alPriority);

        bool IsStream(){ return mbStream;}

        bool IsStereo();

        cOAL_Sample*    GetSample(){ return ( mpSample ); } //static_cast<cOAL_Sample*> (mpSoundData));}
        cOAL_Stream*    GetStream(){ return ( mpStream ); } //static_cast<cOAL_Stream*> (mpSoundData));}
    
    private:
        cOAL_Sample*    mpSample;
        cOAL_Stream*    mpStream;

//iOAL_Loadable*    mpSoundData;
    };
};
#endif // HPL_OPENAL_SOUND_DATA_H
