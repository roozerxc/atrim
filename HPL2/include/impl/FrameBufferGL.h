#ifndef HPL_FRAME_BUFFER_GL_H
#define HPL_FRAME_BUFFER_GL_H

#include "graphics/FrameBuffer.h"
#include "impl/LowLevelGraphicsSDL.h"

namespace hpl
{

class cDepthStencilBufferGL : public iDepthStencilBuffer
{
public:
    cDepthStencilBufferGL(const cVector2l& avSize, int alDepthBits, int alStencilBits);
    ~cDepthStencilBufferGL();

    //GL Specific
    inline unsigned int GetHandle()
    {
        return mlHandle;
    }
public:
    GLuint mlHandle;
};

//-----------------------------------------------

class cFrameBufferGL : public iFrameBuffer
{
public:
    cFrameBufferGL(const tString& asName, iLowLevelGraphics* apLowLevelGraphics);
    ~cFrameBufferGL();

    void SetTexture2D(int alColorIdx, iTexture *apTexture, int alMipmapLevel=0);
    void SetTexture3D(int alColorIdx, iTexture *apTexture, int alZ, int alMipmapLevel=0);
    void SetTextureCubeMap(int alColorIdx, iTexture *apTexture, int alFace, int alMipmapLevel=0);

    void SetDepthTexture2D(iTexture *apTexture, int alMipmapLevel=0);
    void SetDepthTextureCubeMap(iTexture *apTexture, int alFace, int alMipmapLevel=0);

    void SetDepthStencilBuffer(iDepthStencilBuffer* apBuffer);

    bool CompileAndValidate();

    void PostBindUpdate();

    //GL Specific
    inline unsigned int GetHandle()
    {
        return mlHandle;
    }
    inline bool GetIsUpdated()
    {
        return mbIsUpdated;
    }
    inline void SetIsUpdated(bool abX)
    {
        mbIsUpdated = abX;
    }


private:
    void PostBindUpdateAttachment(iFrameBufferAttachment *apAttachment);
    bool CheckIfNullTexture(int alAttachmentType, int alAttachmentIdx,iTexture *apTexture);
    void AttachTexture(int alAttachmentType, int alAttachmentIdx,iTexture *apTexture, int alMipmapLevel, int alExtra);
    void SetFirstSize(const cVector2l &avSize);

    GLuint mlHandle;

    bool mbIsUpdated;
};
};
#endif // HPL_TEXTURE_H
