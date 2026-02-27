#ifndef HPL_WIDGET_MENU
#define HPL_WIDGET_MENU

#include "gui/Widget.h"

#include "system/SystemTypes.h"

using namespace std;

namespace hpl {

    class cGuiSet;
    class cGuiSkin;
    class cWidgetMenuItem;

    typedef vector<cWidgetMenuItem*>        tWidgetMenuItemVector;
    typedef tWidgetMenuItemVector::iterator    tWidgetMenuItemVectorIt;

    
    class iWidgetMenu : public iWidget
    {
        friend class cWidgetMenuItem;
    public:
        iWidgetMenu(eWidgetType aeMenuType,cGuiSet *apSet, cGuiSkin *apSkin);
        virtual ~iWidgetMenu();

        virtual cWidgetMenuItem* AddMenuItem(const tWString &asText);
        virtual void AddSeparator() {}
        virtual void ClearMenuItems();

        

        void SetParentItem( cWidgetMenuItem* apParentItem );
        cWidgetMenuItem* GetParentItem() { return mpParentItem; }
        iWidgetMenu* GetParentMenu();

        void SetPrevAttention(iWidget* apWidget) { mpPrevAttention = apWidget; }

        void SetHighlightedItem( cWidgetMenuItem* apItem ) { if(apItem!=mpHighlightedItem) mpHighlightedItem = apItem; }
        cWidgetMenuItem* GetHightlightedItem () { return mpHighlightedItem; }

        /**
         * Hide() : Hides current menu and submenues
         */
        virtual void Hide(){}

        virtual void SetMustHide(bool abX);

        bool IsSubmenuOpen();

        iWidgetMenu* GetTopMostMenu();

        virtual void OnChildMenuHide() {};

        void SetTextPos(const cVector3f& avPos) { mvTextPos = avPos; }
        void SetArrowPos(const cVector3f& avPos) { mvArrowPos = avPos; }
        void SetCheckPos(const cVector3f& avPos) { mvCheckPos = avPos; }

        const cVector3f& GetTextOffset() { return mvTextPos; }
        const cVector3f& GetArrowOffset() { return mvArrowPos; }
        const cVector3f& GetCheckOffset() { return mvCheckPos; }

        virtual float GetMenuItemWidth() { return 0; }

    protected:

        /**
        * void UpdateMenuItemsPos(cWidgetMenuItem* apNewItem) : update item positions after adding apNewItem
        */ 

        virtual void UpdateMenuItemsPos(cWidgetMenuItem* apNewItem) {};

        /**
        * void UpdateMenuItemsSize(cWidgetMenuItem* apNewItem) : update item sizes after adding apNewItem
        */ 
        virtual void UpdateMenuItemsSize(cWidgetMenuItem* apNewItem) {};

        virtual void UpdateMenuWidth(float afWidth) {};



        ////////////////////////////
        // Data
        tWidgetMenuItemVector mvMenuItems;
        cWidgetMenuItem* mpHighlightedItem;

        cVector3f mvTextPos;
        cVector3f mvArrowPos;
        cVector3f mvCheckPos;

        cWidgetMenuItem* mpParentItem;

        iWidget* mpPrevAttention;

        bool mbMustHide;
    };

};

#endif    // HPL_WIDGET_MENU
