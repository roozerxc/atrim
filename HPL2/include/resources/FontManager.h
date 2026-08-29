#ifndef HPL_FONT_MANAGER_H
#define HPL_FONT_MANAGER_H

#include "resources/ResourceManager.h"

namespace hpl
{

class cGraphics;
class cResources;
class cGui;
class iFontData;

class cFontManager : public iResourceManager
{
public:
    cFontManager(cGraphics* apGraphics,cGui *apGui,cResources *apResources);
    ~cFontManager();

    /**
     * Create a new font
     * \param asName name of the font
     * \param alSize size the characters are rendered in
     * \param alFirstChar first ASCII character to be rendered
     * \param alLastChar last ASCII character to be rendered
     * \return
     */
    iFontData* CreateFontData(const tString& asName, int alSize=16,unsigned short alFirstChar=32,
                              unsigned short alLastChar=255);

    void Destroy(iResourceBase* apResource);
    void Unload(iResourceBase* apResource);

private:
    cGraphics* mpGraphics;
    cResources *mpResources;
    cGui *mpGui;
};

};
#endif // HPL_FONT_MANAGER_H
