#ifndef HPL_BITMAP_LOADER_HANDLER_H
#define HPL_BITMAP_LOADER_HANDLER_H

#include "resources/ResourceLoaderHandler.h"

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "resources/ResourcesTypes.h"

namespace hpl
{

//------------------------------------------------------------

class cBitmap;
class iBitmapLoader;
class cResources;
class cGraphics;

//------------------------------------------------------------

class cBitmapLoaderHandler : public iResourceLoaderHandler
{
public:
    cBitmapLoaderHandler(cResources* apResources, cGraphics* apGraphics);
    ~cBitmapLoaderHandler();

    cBitmap* LoadBitmap(const tWString& asFile, tBitmapLoadFlag aFlags);
    bool SaveBitmap(cBitmap* apBitmap, const tWString& asFile, tBitmapSaveFlag aFlags);

private:
    void SetupLoader(iResourceLoader *apLoader);

    cResources* mpResources;
    cGraphics* mpGraphics;
};

};
#endif // HPL_MESH_LOADER_HANDLER_H
