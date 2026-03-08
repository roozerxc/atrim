#ifndef HPL_BITMAP_LOADER_DEVIL_H
#define HPL_BITMAP_LOADER_DEVIL_H

#include "resources/BitmapLoader.h"

#include <IL/il.h>

namespace hpl
{


class iBitmapLoaderDevil : public iBitmapLoader
{
public:
    iBitmapLoaderDevil();
    virtual ~iBitmapLoaderDevil();

    bool SaveBitmap(cBitmap* apBitmap,const tWString& asFile, tBitmapSaveFlag aFlags);

protected:
    void Initialize();

    bool LoadDevilImageW(const tWString& asFile);

    ePixelFormat DevilPixelFormatToHPL(int alFormat);
    ILenum HPLPixelFormatToDevil(ePixelFormat aFormat);
    ILenum FileNameToDevilTypeW(const tWString& asFile);

    static bool mbIsInitialized;
};

};
#endif // HPL_BITMAP_LOADER_DEVIL_H
