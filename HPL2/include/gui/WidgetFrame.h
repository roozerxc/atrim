#ifndef HPL_WIDGET_FRAME_H
#define HPL_WIDGET_FRAME_H

#include "gui/Widget.h"

namespace hpl {

    class cWidgetSlider;
    class cGuiSkinFont;

    class cWidgetFrame : public iWidget
    {
    public:
        cWidgetFrame(cGuiSet *apSet, cGuiSkin *apSkin, bool abHScrollBar=false, bool abVScrollBar=false);
        virtual ~cWidgetFrame();

        void SetDrawFrame(bool abX){ mbDrawFrame = abX;}
        bool GetDrawFrame(){ return mbDrawFrame;}
        
        void SetDrawBackground(bool abX){mbDrawBackground = abX;}
        bool GetDrawBackground(){ return mbDrawBackground;}
        
        void SetBackgroundZ(float afZ){mfBackgroundZ = afZ;}
        float GetBackgroundZ(){ return mfBackgroundZ;}

        void SetBackGroundColor(const cColor &aColor){ mBackGroundColor = aColor;}
        const cColor& GetBackGroundColor(){ return mBackGroundColor;}

        void OnAttachChild(iWidget* apChild);
        void OnRemoveChild(iWidget* apChild);

        void ScrollToPosition(const cVector2f& avPos);
        
    protected:
        /////////////////////////
        // Own functions
        bool OnSliderMove(iWidget* apWidget, const cGuiMessageData& aData);
        kGuiCallbackDeclarationEnd(OnSliderMove);

        /////////////////////////
        // Implemented functions
        void OnLoadGraphics();

        void OnInit();
        void OnChangeSize();

        void OnChildUpdate(iWidget* apChild);
        void OnUpdate(float afTimeStep);
        
        void OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion);
        void OnDrawAfterClip(float afTimeStep, cGuiClipRegion *apClipRegion);

        bool OnMouseMove(const cGuiMessageData& aData);
        bool OnMouseDown(const cGuiMessageData& aData);
        bool OnMouseUp(const cGuiMessageData& aData);
        bool OnMouseEnter(const cGuiMessageData& aData);
        bool OnMouseLeave(const cGuiMessageData& aData);

        /////////////////////////
        // Data
        bool mbAutoScroll;
        bool mbHScrollBar, mbVScrollBar;
        bool mbDrawFrame;
        bool mbDrawBackground;
        float mfBackgroundZ;
        cColor mBackGroundColor;

        cGuiGfxElement *mpGfxBackground;
        
        cGuiGfxElement *mvGfxBorders[4];
        cGuiGfxElement *mvGfxCorners[4];

        cVector3f mvMinWidgetCoord;
        cVector3f mvMaxWidgetCoord;

        cWidgetSlider* mpHSlider;
        cWidgetSlider* mpVSlider;
        
        bool mbScrollBarsNeedUpdate;
        bool mbScrollUpdated;

        cVector2f mvAutoScrollMargin;
    };

};
#endif // HPL_WIDGET_FRAME_H
