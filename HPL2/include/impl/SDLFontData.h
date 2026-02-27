#ifndef HPL_SDL_FONTDATA_H
#define HPL_SDL_FONTDATA_H

#include "graphics/FontData.h"

//#include <SDL/SDL.h>
//#include <SDL/SDL_ttf.h>

namespace hpl {

    class cSDLFontData : public iFontData
    {
    public:
        cSDLFontData(const tString &asName, iLowLevelGraphics* apLowLevelGraphics);
        cSDLFontData();

        bool CreateFromFontFile(const tWString &asFileName, int alSize,unsigned short alFirstChar, 
                                unsigned short alLastChar);
        bool CreateFromBitmapFile(const tWString &asFileName);

    private:
        //cGlyph* RenderGlyph(TTF_Font* apFont,unsigned short aChar, int alFontSize);

    };

};
#endif // HPL_FONTDATA_H
