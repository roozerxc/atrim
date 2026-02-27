#ifndef HPL_WIDGET_IMAGE_H
#define HPL_WIDGET_IMAGE_H

#include "gui/Widget.h"

namespace hpl {

    class cGuiSkinFont;

    class cWidgetImage : public iWidget
    {
    public:
        cWidgetImage(cGuiSet *apSet, cGuiSkin *apSkin);
        virtual ~cWidgetImage();

        void SetImage(cGuiGfxElement *apGfx);
        cGuiGfxElement* GetImage(){ return mpGfxImage;}

    protected:
        /////////////////////////
        // Implemented functions
        void OnLoadGraphics();
        void OnChangeSize();
        
        void OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion);

        bool OnMouseMove(const cGuiMessageData& aData);
        bool OnMouseDown(const cGuiMessageData& aData);
        bool OnMouseUp(const cGuiMessageData& aData);
        bool OnMouseEnter(const cGuiMessageData& aData);
        bool OnMouseLeave(const cGuiMessageData& aData);

        /////////////////////////
        // Data
        cGuiGfxElement *mpGfxImage;
    };

};
#endif // HPL_WIDGET_IMAGE_H
