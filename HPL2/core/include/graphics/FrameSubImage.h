#ifndef HPL_RESOURCE_IMAGE_H
#define HPL_RESOURCE_IMAGE_H

#include <vector>

#include "resources/ResourceBase.h"

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"

namespace hpl
{

class cFrameTexture;
class cFrameBitmap;
class iTexture;
class cFBitmapImage;

//----------------------------------------------------

class cFrameSubImage : public iResourceBase
{
    friend class cFBitmapImage;
    friend class cFrameBitmap;
    friend class cImageManager;
public:
    cFrameSubImage(const tString& asName,const tWString& asFullPath, cFrameTexture *apFrameTex,
                   cFrameBitmap *apFrameBmp,
                   cRect2l aRect,
                   cVector2l avSrcSize, int alHandle,
                   cFBitmapImage *apFrameBitmapImage);
    ~cFrameSubImage();


    bool Reload();
    void Unload();
    void Destroy();

    //Image specific
    int GetHeight()const
    {
        return mRect.h;
    }
    int GetWidth()const
    {
        return mRect.w;
    }
    cVector2l GetSize()const
    {
        return cVector2l(mRect.w,mRect.h);
    }
    cVector2l GetPosition()const
    {
        return cVector2l(mRect.x,mRect.y);
    }

    int GetSourceWidth()const
    {
        return mvSourceSize.x;
    }
    int GetSourceHeight()const
    {
        return mvSourceSize.y;
    }

    iTexture *GetTexture()const;

    cFrameTexture *GetFrameTexture()const
    {
        return mpFrameTexture;
    }
    cFrameBitmap *GetFrameBitmap()const
    {
        return mpFrameBitmap;
    }

    tVertexVec GetVertexVecCopy(const cVector2f &avPos, const cVector2f &avSize);
    const tVertexVec& GetVertexVec()
    {
        return mvVtx;
    }

    int GetUpdateCount()
    {
        return mlUpdateCount;
    }

    void Flush();

    void SetNeedUpdateUvs();

private:
    void UpdateUvs();

    cFrameTexture *mpFrameTexture;
    cFrameBitmap *mpFrameBitmap;
    cFBitmapImage *mpFrameBitmapImage;

    cVector2l mvSourceSize;
    cRect2l mRect;
    tVertexVec mvVtx;

    int mlHandle;

    bool mbNeedUvUpdate;
    int mlUpdateCount;
};

typedef std::vector<cFrameSubImage*> tResourceImageVec;
typedef tResourceImageVec::iterator tResourceImageVecIt;

};

#endif // HPL_RESOURCE_IMAGE_H
