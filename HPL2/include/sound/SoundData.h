#ifndef HPL_SOUND_DATA_H
#define HPL_SOUND_DATA_H

#include "system/SystemTypes.h"
#include "resources/ResourceBase.h"

namespace hpl {

    class cSoundManager;
    class iSoundChannel;
    
    class iSoundData : public iResourceBase
    {
    public:
        iSoundData(const tString& asName, const tWString& asFullPath, bool abStream) : iResourceBase(asName, asFullPath, 0),
        mpSoundManger(NULL), mbStream(abStream){}
        
        virtual ~iSoundData(){}

        virtual bool CreateFromFile(const tWString &asFile)=0;

        virtual iSoundChannel* CreateChannel(int alPriority)=0;
        

        virtual bool IsStereo()=0;

        bool IsStream(){ return mbStream;}
        void SetLoopStream(bool abX){mbLoopStream = abX;}
        bool GetLoopStream(){ return mbLoopStream;}

        bool Reload(){ return false;}
        void Unload(){}
        void Destroy(){}

        void SetSoundManager(cSoundManager* apSoundManager){
            mpSoundManger = apSoundManager;
        }
    
    protected:    
        bool mbStream;
        bool mbLoopStream;
        cSoundManager* mpSoundManger;
    };
};
#endif // HPL_SOUND_DATA_H
