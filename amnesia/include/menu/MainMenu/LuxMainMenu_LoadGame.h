#ifndef LUX_MAIN_MENU_LOAD_GAME_H
#define LUX_MAIN_MENU_LOAD_GAME_H

//----------------------------------------------

#include "LuxMainMenu.h"

//----------------------------------------------

class cLuxMainMenu_LoadGame : public iLuxMainMenuWindow
{
    friend class cLuxInputMenuEntry;
public:
    cLuxMainMenu_LoadGame(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin);
    ~cLuxMainMenu_LoadGame();

    void CreateGui();

    void ExitPressed();

private:
    void OnSetActive(bool abX);

    void PopulateSavedGameList();

    ////////////////////////
    // Properties
    cVector2f mvWindowSize;

    tWStringVec mvSavedGameFileNames;

    ////////////////////////
    // Layout
    cWidgetListBox* mpLBSavedGames;
    cWidgetImage* mpISavedGameSnapShot;
    iWidget *mpLoadButton;

    ////////////////////////
    // Callbacks
    void LoadGame(int alIdx);

    bool WindowOnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(WindowOnUpdate);

    bool PressOK(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(PressOK);

    bool PressCancel(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(PressCancel);

    bool UIPressList(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(UIPressList);

    bool UIPress(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(UIPress);

    bool UIPressCancel(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(UIPressCancel);

    bool LoadGameCallback(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(LoadGameCallback);

    bool ExitCallback(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(ExitCallback);

    bool LoadSelectionClickChange(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(LoadSelectionClickChange);

    bool LockLoadList(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(LockLoadList);
};

//----------------------------------------------

#endif // LUX_MAIN_MENU_LOAD_GAME_H
