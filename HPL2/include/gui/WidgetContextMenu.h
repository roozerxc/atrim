#ifndef HPL_WIDGET_CONTEXT_MENU
#define HPL_WIDGET_CONTEXT_MENU

#include "gui/WidgetMenu.h"

#include "gui/GuiSkin.h"

namespace hpl {

    class cWidgetContextMenu : public iWidgetMenu
    {
    public:
        cWidgetContextMenu(cGuiSet *apSet, cGuiSkin *apSkin);
        ~cWidgetContextMenu();

        ////////////////////////////////////
        // iWidgetMenu implemented functions
        void AddSeparator();
        void Hide();

        void SavePrevAttentionWidget();

        float GetMenuItemWidth();

    protected:

        ////////////////////////////////////
        // iWidgetMenu implemented functions

        void UpdateMenuItemsPos(cWidgetMenuItem* apNewItem);
        void UpdateMenuItemsSize(cWidgetMenuItem* apNewItem);
        void UpdateMenuWidth(float afWidth);

        void OnChildMenuHide();

        ////////////////////////////////////
        // iWidget implemented functions

        void OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion);

        void OnUpdate(float afTimeStep);

        void OnInit();

        bool OnMouseDown(const cGuiMessageData& aData) {return true;}
        bool OnMouseUp(const cGuiMessageData& aData) {return true;}

        bool OnMouseEnter(const cGuiMessageData& aData);
        bool OnMouseLeave(const cGuiMessageData& aData);

        bool OnKeyPress(const cGuiMessageData& aData);

        bool OnLostFocus(const cGuiMessageData& aData);
        bool OnGotFocus(const cGuiMessageData& aData);

        void OnLoadGraphics();

        /////////////////////////
        // Data
        float mfTimer;
        float mfHideTime;

        // Skin attributes
        float mfItemHPadding;
        float mfItemTextLeftPadding;
        float mfItemTextRightPadding;
        float mfItemVPadding;
        float mfItemSeparation;
        float mfItemArrowIconSize;
        float mfItemCheckIconSize;
        
        // Graphics
        cGuiGfxElement* mpGfxBackground;

        cGuiGfxElement* mvGfxBorders[4];
        cGuiGfxElement* mvGfxCorners[4];
    };
};

#endif // HPL_WIDGET_CONTEXT_MENU


