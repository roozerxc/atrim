#ifndef HPL_BITMAP_LOADER_H
#define HPL_BITMAP_LOADER_H

#include "resources/ResourceLoader.h"

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "resources/ResourcesTypes.h"

namespace hpl
{

class iLowLevelGraphics;
class cBitmap;

class iBitmapLoader : public iResourceLoader
{
    friend class cBitmapLoaderHandler;
public:
    virtual ~iBitmapLoader() {}

    virtual cBitmap* LoadBitmap(const tWString& asFile, tBitmapLoadFlag aFlags)=0;
    virtual bool SaveBitmap(cBitmap* apBitmap,const tWString& asFile, tBitmapLoadFlag aFlags)=0;

protected:
    iLowLevelGraphics *mpLowLevelGraphics;
};

};
#endif // HPL_BITMAP_LOADER_H
