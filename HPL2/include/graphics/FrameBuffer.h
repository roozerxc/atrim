#ifndef HPL_FRAME_BUFFER_H
#define HPL_FRAME_BUFFER_H

#include "graphics/GraphicsTypes.h"

namespace hpl {

    class iLowLevelGraphics;
    class iTexture;

    //-----------------------------------------------
    
    class iDepthStencilBuffer : public iFrameBufferAttachment
    {
    public:
        iDepthStencilBuffer(const cVector2l& avSize, int alDepthBits, int alStencilBits) :
                                    mvSize(avSize),
                                    mlDepthBits(alDepthBits),
                                    mlStencilBits(alStencilBits),
                                    mlUserCount(0){}
        virtual ~iDepthStencilBuffer(){}

        const cVector2l& GetSize(){ return mvSize;}
        int GetWidth(){ return mvSize.x;}
        int GetHeight(){ return mvSize.y;}

        int GetStencilBits(){ return mlStencilBits;}
        int GetDepthBits(){ return mlDepthBits;}

        eFrameBufferAttachment GetFrameBufferAttachmentType(){ return eFrameBufferAttachment_RenderBuffer;}

        void IncUserCount(){++mlUserCount;}
        void DecUserCount(){--mlUserCount;}
        bool HasUsers(){ return mlUserCount >0; }

    protected:
        cVector2l mvSize;
        int mlDepthBits;
        int mlStencilBits;

        int mlUserCount;
    };
    
    //-----------------------------------------------
    
    class iFrameBuffer
    {
    public:
        iFrameBuffer(const tString& asName, iLowLevelGraphics* apLowLevelGraphics) : 
                msName(asName), mpLowLevelGraphics(apLowLevelGraphics), mvSize(-1), mpDepthBuffer(NULL), mpStencilBuffer(NULL) 
        {
            for(int i=0; i<kMaxDrawColorBuffers; ++i) mpColorBuffer[i] = NULL;
        }
        virtual ~iFrameBuffer(){}

        const tString& GetName(){ return msName;}

        virtual void SetTexture2D(int alColorIdx, iTexture *apTexture, int alMipmapLevel=0)=0;
        virtual void SetTexture3D(int alColorIdx, iTexture *apTexture, int alZ, int alMipmapLevel=0)=0;
        virtual void SetTextureCubeMap(int alColorIdx, iTexture *apTexture, int alFace, int alMipmapLevel=0)=0;

        virtual void SetDepthTexture2D(iTexture *apTexture, int alMipmapLevel=0)=0;
        virtual void SetDepthTextureCubeMap(iTexture *apTexture, int alFace, int alMipmapLevel=0)=0;

        virtual void SetDepthStencilBuffer(iDepthStencilBuffer* apBuffer)=0;

        virtual bool CompileAndValidate()=0;

        /**
        * To be used internally!
        */
        virtual void PostBindUpdate()=0;


        iFrameBufferAttachment* GetColorBuffer(int alIdx){ return mpColorBuffer[alIdx];}
        iFrameBufferAttachment* GetDepthBuffer(){ return mpDepthBuffer;}
        iFrameBufferAttachment* GetStencilBuffer(){ return mpStencilBuffer;}

        inline const cVector2l& GetSize(){ return mvSize;}

    protected:
        iLowLevelGraphics* mpLowLevelGraphics;

        tString msName;

        iFrameBufferAttachment* mpColorBuffer[kMaxDrawColorBuffers];
        iFrameBufferAttachment* mpDepthBuffer;
        iFrameBufferAttachment* mpStencilBuffer;

        cVector2l mvSize;

    };
};
#endif // HPL_TEXTURE_H
