#ifndef HPL_WIDGET_LIST_BOX_H
#define HPL_WIDGET_LIST_BOX_H

#include "gui/Widget.h"
#include "gui/WidgetListBoxBase.h"
#include "gui/WidgetBaseClasses.h"

namespace hpl {

    class cGuiSkinFont;

    class cWidgetSlider;

    class cWidgetListBox : public iWidgetListBoxBase
    {
    public:
        cWidgetListBox(cGuiSet *apSet, cGuiSkin *apSkin);
        virtual ~cWidgetListBox();
    protected:
        /////////////////////////
        // Own functions
            
        /////////////////////////
        // Implemented iWidgetListBoxBase functions
        void DrawItems(float afTimeStep, cGuiClipRegion* apClipRegion);
        /////////////////////////
        // Implemented functions
    
            
        /////////////////////////
        // Data
    };

};
#endif // HPL_WIDGET_LIST_BOX_H
