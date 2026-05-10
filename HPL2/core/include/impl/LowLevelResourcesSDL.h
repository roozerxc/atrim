#ifndef HPL_LOWLEVELRESOURCES_SDL_H
#define HPL_LOWLEVELRESOURCES_SDL_H

#include "resources/LowLevelResources.h"
#include "system/SystemTypes.h"

namespace hpl
{

//----------------------------------------

class iLowLevelGraphics;

//----------------------------------------

class cLowLevelResourcesSDL : public iLowLevelResources
{
public:
    cLowLevelResourcesSDL(iLowLevelGraphics *apLowLevelGraphics);
    ~cLowLevelResourcesSDL();

    void AddBitmapLoaders(cBitmapLoaderHandler* apHandler);
    void AddMeshLoaders(cMeshLoaderHandler* apHandler);
    void AddVideoLoaders(cVideoLoaderHandler* apHandler);

    iXmlDocument* CreateXmlDocument(const tString& asName="");

private:
    iLowLevelGraphics *mpLowLevelGraphics;
};
};
#endif // HPL_LOWLEVELRESOURCES_SDL_H
