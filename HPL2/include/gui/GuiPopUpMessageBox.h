#ifndef HPL_GUI_POP_UP_MESSAGE_BOX_H
#define HPL_GUI_POP_UP_MESSAGE_BOX_H

#include "gui/GuiPopUp.h"

namespace hpl {

    class cWidgetWindow;
    class cWidgetLabel;
    class cWidgetButton;

    class cGuiPopUpMessageBox : public iGuiPopUp
    {
    public:
        cGuiPopUpMessageBox(cGuiSet *apSet, 
            const tWString& asLabel, const tWString& asText,
            const tWString& asButton1, const tWString& asButton2,
            void *apCallbackObject, tGuiCallbackFunc apCallback);
        virtual ~cGuiPopUpMessageBox();
        
    protected:
        bool ButtonPress(iWidget* apWidget, const cGuiMessageData& aData);
        kGuiCallbackDeclarationEnd(ButtonPress);
        bool GamepadButtonPress(iWidget* apWidget, const cGuiMessageData& aData);
        kGuiCallbackDeclarationEnd(GamepadButtonPress);

        cWidgetButton *mvButtons[2];
        cWidgetLabel *mpLabel;
        
        void * mpCallbackObject;
        tGuiCallbackFunc mpCallback;
    };

};
#endif // HPL_GUI_POP_UP_MESSAGE_BOX_H
