#ifndef HPL_LOWLEVELSOUND_FMOD_H
#define HPL_LOWLEVELSOUND_FMOD_H

#include "sound/LowLevelSound.h"

namespace hpl {

    class cLowLevelSoundFmod : public iLowLevelSound
    {
    public:
        cLowLevelSoundFmod();
        ~cLowLevelSoundFmod();

        void GetSupportedFormats(tStringList &alstFormats);

        iSoundData* LoadSoundData(const tString& asName,const tString& asFilePath,
                                    const tString& asType, bool abStream,bool abLoopStream);

        void UpdateSound(float afTimeStep);

        void SetListenerAttributes (const cVector3f &avPos,const cVector3f &avVel,
                                const cVector3f &avForward,const cVector3f &avUp);
        void SetListenerPosition(const cVector3f &avPos);

        void SetSetRolloffFactor(float afFactor);

        void SetListenerAttenuation (bool abEnabled);

        virtual void SetVolume(float afVolume);

    private:
        tString mvFormats[30];
    };
};
#endif // HPL_LOWLEVELSOUND_FMOD_H
