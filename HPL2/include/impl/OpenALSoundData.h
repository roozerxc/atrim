#ifndef HPL_OPENAL_SOUND_DATA_H
#define HPL_OPENAL_SOUND_DATA_H

#include "sound/SoundData.h"

#include "OpenAL/OAL_Funcs.h"

namespace hpl
{

class cOpenALSoundData : public iSoundData
{
public:
    cOpenALSoundData(const tString& asName, bool abStream);
    ~cOpenALSoundData();

    bool CreateFromFile(const tWString &asFile);

    iSoundChannel* CreateChannel(int alPriority);

    bool IsStream()
    {
        return mbStream;
    }

    bool IsStereo();

    cOAL_Sample*    GetSample()
    {
        return ( mpSample );
    }
    cOAL_Stream*    GetStream()
    {
        return ( mpStream );
    }

private:
    cOAL_Sample*    mpSample;
    cOAL_Stream*    mpStream;
};
};
#endif // HPL_OPENAL_SOUND_DATA_H
