#ifndef HPL_WIDGET_SLIDER_H
#define HPL_WIDGET_SLIDER_H

#include "gui/Widget.h"

namespace hpl {

    class cGuiSkinFont;

    class cWidgetButton;

    //---------------------------------------------

    class cWidgetSlider : public iWidget
    {
    public:
        cWidgetSlider(cGuiSet *apSet, cGuiSkin *apSkin, eWidgetSliderOrientation aOrientation);
        virtual ~cWidgetSlider();

        int GetValue(){ return mlValue;}
        void SetValue(int alValue, bool abGenCallback=true);
        
        int GetMaxValue(){ return mlMaxValue;}
        void SetMaxValue(int alMax);
        
        int GetButtonValueAdd(){ return mlButtonValueAdd; }
        void SetButtonValueAdd(int alAdd);

        int GetBarClickValueAdd() { return mlBarClickValueAdd; }
        void SetBarClickValueAdd(int alAdd);

        int GetBarValueSize(){ return mlBarValueSize;}
        void SetBarValueSize(int alSize);

        float GetButtonSize() const{return mfButtonSize;}

    protected:
        /////////////////////////
        // Own functions
        void UpdateBarProperties();

        bool ArrowButtonDown(iWidget* apWidget, const cGuiMessageData& aData);
        kGuiCallbackDeclarationEnd(ArrowButtonDown);        

        /////////////////////////
        // Implemented functions
        void OnInit();
        void OnLoadGraphics();
        void OnChangeSize();
        void OnChangePosition();

        
        void OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion);

        bool OnMouseMove(const cGuiMessageData& aData);
        bool OnMouseDown(const cGuiMessageData& aData);
        bool OnMouseUp(const cGuiMessageData& aData);
        bool OnMouseEnter(const cGuiMessageData& aData);
        bool OnMouseLeave(const cGuiMessageData& aData);

        bool OnMouseDoubleClick(const cGuiMessageData& aData);

        bool OnLostFocus(const cGuiMessageData& aData);

        bool OnUIArrowPress(const cGuiMessageData& aData);
        bool OnUIArrowRelease(const cGuiMessageData& aData);

        bool OnUIButtonPress(const cGuiMessageData& aData);
        bool OnUIButtonRelease(const cGuiMessageData& aData);

        /////////////////////////
        // Data
        eWidgetSliderOrientation mOrientation;

        bool mbPressed;

        float mfButtonSize;

        int mlValue;
        int mlMaxValue;
        int mlButtonValueAdd;
        int mlBarClickValueAdd;
        int mlBarValueSize; //This is how big the bar is compared to the max value of the slider
        float mfValueStep;

        cVector3f mvBarPos;
        cVector2f mvBarSize;
        float mfSliderSize;
        cRect2f mBarRect;
        float mfMaxPos;
        float mfMinPos;

        cVector2f mvRelMousePos;

        cWidgetButton* mvButtons[2];

        cGuiGfxElement *mpGfxButtonBackground;
        cGuiGfxElement *mvGfxBorders[4];
        cGuiGfxElement *mvGfxCorners[4];

        cGuiGfxElement *mvGfxArrow[2];
        cGuiGfxElement *mpGfxBackground;
    };

};
#endif // HPL_WIDGET_SLIDER_H
