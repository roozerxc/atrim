#ifndef HPL_GUI_SKIN_H
#define HPL_GUI_SKIN_H

#include <map>

#include "gui/GuiTypes.h"

namespace hpl {

    class cGui;
    class cGuiGfxElement;
    class iFontData;
    class iGuiMaterial;

    //-------------------------------------
    
    class cGuiSkinFont
    {
    public:
        cGuiSkinFont(cGui *apGui);
        ~cGuiSkinFont();

        iFontData* mpFont;
        cVector2f mvSize;
        cColor mColor;
        iGuiMaterial *mpMaterial;
    private:
        cGui *mpGui;
    };

    //-------------------------------------

    class cGuiSkin
    {
    public:
        cGuiSkin(const tString & asName,cGui *apGui);
        ~cGuiSkin();

        const tString& GetName(){ return msName;}

        bool LoadFromFile(const tWString &asFile);
        
        cGuiGfxElement* GetGfx(eGuiSkinGfx aType);
        cGuiSkinFont* GetFont(eGuiSkinFont aType);
        const cVector3f& GetAttribute(eGuiSkinAttribute aType);
    private:
                    
        tString msName;
        cGui *mpGui;

        std::vector<cGuiGfxElement*> mvGfxElements;
        std::vector<cGuiSkinFont*> mvFonts;
        std::vector<cVector3f> mvAttributes;
    };

};
#endif // HPL_GUI_SKIN_H
