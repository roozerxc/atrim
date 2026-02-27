#ifndef HPL_WIDGET_MAIN_MENU
#define HPL_WIDGET_MAIN_MENU

#include "gui/WidgetMenu.h"

#include "gui/GuiSkin.h"

namespace hpl {

    class cWidgetMainMenu : public iWidgetMenu
    {
    public:
        cWidgetMainMenu(cGuiSet *apSet, cGuiSkin *apSkin);
        ~cWidgetMainMenu();

        //////////////////////////////
        // Implemented iWidgetMenu functions
        void Hide();

    protected:

        //////////////////////////////
        // Implemented iWidgetMenu functions

        void UpdateMenuItemsPos(cWidgetMenuItem* apNewItem);
        void UpdateMenuItemsSize(cWidgetMenuItem* apNewItem);

        void OnChildMenuHide();

        //////////////////////////////
        // Implemented iWidget functions
        void OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion);

        void OnUpdate(float afTimeStep);

        void OnInit();

        void OnLoadGraphics();

        /////////////////////////////
        // Data

        float mfItemSeparation;
        float mfItemVPadding;
        float mfItemTextPadding;

        cGuiGfxElement* mpGfxBackground;
        
    };
};

#endif // HPL_WIDGET_MAIN_MENU


