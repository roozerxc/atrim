#ifndef LUX_MAIN_MENU_START_GAME_H
#define LUX_MAIN_MENU_START_GAME_H

//----------------------------------------------

#include "LuxMainMenu.h"

//----------------------------------------------

class cLuxMainMenu_StartGame : public iLuxMainMenuWindow
{
public:
    cLuxMainMenu_StartGame(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin);
    ~cLuxMainMenu_StartGame();

    virtual void CreateGui();

    virtual void ExitPressed();

private:
    virtual void OnSetActive(bool abX);

    ////////////////////////
    // Properties
    cVector2f mvWindowSize;

    ////////////////////////
    // Layout
    cWidgetButton* mpNormalModeButton;
    cWidgetButton* mpHardModeButton;

    iWidget *mpStartButton;

    //Description
    cWidgetLabel* mpLDescription;

    ////////////////////////
    // Callbacks
    bool WindowOnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(WindowOnUpdate);

    bool PressStartGame(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(PressStartGame);

    bool PressCancel(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(PressCancel);

    bool UIPressStart(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(UIPressStart);

    bool UIPressCancel(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(UIPressCancel);

    bool ExitCallback(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(ExitCallback);

    //NORMAL
    bool PressNormalMode(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(PressNormalMode);

    bool ClickedStartGamePopup(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(ClickedStartGamePopup);

    bool UIPressNormalMode(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(UIPressNormalMode);

    //HARDMODE
    bool PressHardMode(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(PressHardMode);

    bool UIPressHardmode(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(UIPressHardmode);


};

//----------------------------------------------

#endif // LUX_MAIN_MENU_START_GAME_H