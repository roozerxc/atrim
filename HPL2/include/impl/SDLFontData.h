#ifndef HPL_SDL_FONTDATA_H
#define HPL_SDL_FONTDATA_H

#include "graphics/FontData.h"

//#include <SDL/SDL.h>
//#include <SDL/SDL_ttf.h>

namespace hpl
{

class cSDLFontData : public iFontData
{
public:
    cSDLFontData(const tString &asName, iLowLevelGraphics* apLowLevelGraphics);
    cSDLFontData();

    bool CreateFromBitmapFile(const tWString &asFileName);
};

};
#endif // HPL_FONTDATA_H
