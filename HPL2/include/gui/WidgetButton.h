#ifndef HPL_WIDGET_BUTTON_H
#define HPL_WIDGET_BUTTON_H

#include "gui/Widget.h"

namespace hpl {

    class cGuiSkinFont;

    class cWidgetButton : public iWidget
    {
    public:
        cWidgetButton(cGuiSet *apSet, cGuiSkin *apSkin);
        virtual ~cWidgetButton();

        void SetImage(cGuiGfxElement *apImage, bool abDestroyImage=true);
        cGuiGfxElement *GetImage(){return mpImage;}

        void SetRepeatActive(bool abX) { mbRepeatActive = abX; }
        bool GetRepeatActive() { return mbRepeatActive; }
        
        void SetRepeatFreq(float afFreq);
        
        void SetToggleable(bool abX) { mbToggleable = abX; }
        bool IsToggleable() { return mbToggleable; }

        void SetPressed(bool abX, bool abGenCallback=true);
        bool IsPressed() { return mbPressed; }
        
    protected:
        /////////////////////////
        // Implemented functions
        void OnLoadGraphics();
        
        void OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion);

        void OnUpdate(float afTimeStep);

        bool OnMouseMove(const cGuiMessageData& aData);
        bool OnMouseDown(const cGuiMessageData& aData);
        bool OnMouseUp(const cGuiMessageData& aData);
        bool OnMouseEnter(const cGuiMessageData& aData);
        bool OnMouseLeave(const cGuiMessageData& aData);

        bool OnMouseDoubleClick(const cGuiMessageData& aData) { return true; }

        bool OnLostFocus(const cGuiMessageData& aData);

        bool OnUIButtonPress(const cGuiMessageData& aData);
        bool OnUIButtonRelease(const cGuiMessageData& aData);

        /////////////////////////
        // Data
        bool mbPressed;

        bool mbToggleable;

        cGuiGfxElement *mpImage;
        bool mbDestroyImage;

        float mfTimer;
        bool mbRepeatActive;
        float mfRepeatPeriod;
        int mlRepeatTimes;
        cGuiMessageData mPressedData;

        cGuiGfxElement *mpGfxBackgroundUp;
        cGuiGfxElement *mvGfxBordersUp[4];
        cGuiGfxElement *mvGfxCornersUp[4];

        cGuiGfxElement *mpGfxBackgroundDown;
        cGuiGfxElement *mvGfxBordersDown[4];
        cGuiGfxElement *mvGfxCornersDown[4];
    };

};
#endif // HPL_WIDGET_BUTTON_H
