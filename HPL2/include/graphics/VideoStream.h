#ifndef HPL_VIDEO_STREAM_H
#define HPL_VIDEO_STREAM_H

#include "graphics/GraphicsTypes.h"
#include "resources/ResourceBase.h"

namespace hpl {

    //-----------------------------------------

    class iTexture;

    //-----------------------------------------
    
    class iVideoStream : public iResourceBase
    {
    public:
        iVideoStream(const tString& asName, const tWString& asFullPath) : iResourceBase(asName, asFullPath,0){}
        virtual ~iVideoStream(){}

        virtual void Update(float afTimeStep)=0;

        virtual void Play()=0;
        virtual void Stop()=0;

        virtual void Pause(bool abX)=0;
        virtual bool IsPaused()=0;
        
        virtual void SetLoop(bool abX)=0;
        virtual bool IsLooping()=0;

        virtual void CopyToTexture(iTexture *apTexture)=0;


        const tWString& GetFileName(){return msFilePath;}
        const cVector2l& GetSize(){return mvSize;}
        
        //////////////////////////////////
        // ResourceBase implementation
        bool Reload(){ return false;}
        void Unload(){}
        void Destroy(){}


    protected:
        tWString msFilePath;
        cVector2l mvSize;
    };
};
#endif // HPL_VIDEO_STREAM_H
