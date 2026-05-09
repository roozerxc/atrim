#ifndef HPL_GUI_POP_UP_H
#define HPL_GUI_POP_UP_H

#include "gui/GuiTypes.h"

namespace hpl
{

class cGuiSet;
class cGuiSkin;

class cWidgetWindow;

class iGuiPopUp
{
public:
    iGuiPopUp(cGuiSet *apSet, bool abAddCloseButton, const cVector2f& avPopUpSize);
    virtual ~iGuiPopUp();

    void AddOnDestroyCallback(void *apCallbackObject, tGuiCallbackFunc apCallback);

    void SetKillOnEscapeKey(bool abX);
    bool GetKillOnEscapeKey();

    void SetNextFocusWidget(iWidget *apNewFocused)
    {
        mpNewFocused = apNewFocused;
        mbFocusChanged = apNewFocused != NULL;
    }
    cGuiSet* GetGuiSet()
    {
        return mpSet;
    }

protected:
    bool Window_OnClose(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(Window_OnClose);

    virtual void OnCloseSpecific() {}

    void SelfDestruct();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Use this func to safely execute callbacks (makes sure right attention and focus widgets are set after destroying popup)
    bool RunCallback(void* apObj, tGuiCallbackFunc apCallback,
                     iWidget* apWidget, const cGuiMessageData& aData, bool abRunFocusChangeChecks);

    void SetUpDefaultFocus(iWidget* apWidget);

    cGuiSet *mpSet;
    cGuiSkin *mpSkin;

    cWidgetWindow* mpWindow;

    void* mpDestroyCallbackObject;
    tGuiCallbackFunc mpDestroyCallback;

    bool mbAttChanged;
    iWidget* mpNewAttention;
    bool mbFocusChanged;
    iWidget* mpNewFocused;
    bool mbDefaultUIFocusChanged;
    iWidget* mpNewDefaultUIFocus;
};

};
#endif // HPL_GUI_POP_UP_H

