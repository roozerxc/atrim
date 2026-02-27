#ifndef HPL_WIDGET_GROUP_H
#define HPL_WIDGET_GROUP_H

#include "gui/Widget.h"


namespace hpl {

    class cWidgetLabel;
    class cGuiGfxElement;

    class cWidgetGroup : public iWidget
    {
    public:
        cWidgetGroup(cGuiSet* apSet, cGuiSkin* apSkin);
        ~cWidgetGroup();

        void SetHeaderText(const tWString& asText);
        const tWString& GetHeaderText();

        void SetDefaultFontSize(const cVector2f& avSize);
    protected:
        /////////////////////////////
        // Implemented functions
        void OnInit();
        void OnLoadGraphics();

        void OnChangeText();

        void OnDraw(float afTimeStep, cGuiClipRegion* apClipRegion);
        void OnDrawAfterClip(float afTimeStep, cGuiClipRegion *apClipRegion);

        /////////////////////////////
        // Data

        cWidgetLabel *mpHeader;
        cVector2f mvHeaderSize;

        cGuiGfxElement *mpGfxBackground;
        
        cGuiGfxElement *mvGfxBorders[4];
        cGuiGfxElement *mvGfxCorners[4];

        float mfHeaderOffset;
        bool mbHeaderBreaksUpperBorder;
    };
};


#endif // HPL_WIDGET_GROUP_H
