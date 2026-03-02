#ifndef LUX_MAIN_MENU_CUSTOM_STORY_H
#define LUX_MAIN_MENU_CUSTOM_STORY_H

//----------------------------------------------

#include "LuxMainMenu.h"

//----------------------------------------------

class cLuxCustomStorySettings;

//----------------------------------------------

class cLuxMainMenu_CustomStory : public iLuxMainMenuWindow
{
public:
    cLuxMainMenu_CustomStory(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin);
    ~cLuxMainMenu_CustomStory();

    void CreateGui();

    void ExitPressed() {}

    void SetCurrentStory(cLuxCustomStorySettings* apStory);

private:
    void OnSetActive(bool abX);

    bool PressContinue(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(PressContinue);
    
    bool PressStart(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(PressStart);

    bool PressLoadGame(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(PressLoadGame);

    bool PressBack(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(PressBack);

    void RepositionButtons();

    cVector2f mvWindowSize;

    cLuxCustomStorySettings* mpStory;

    cWidgetImage* mpIPicture;

    cWidgetLabel* mpLAuthor;
    cWidgetLabel* mpLDesc;

    std::vector<cWidgetButton*> mvButtons;
};

//----------------------------------------------

class cLuxMainMenu_CustomStoryList : public iLuxMainMenuWindow
{
public:
    cLuxMainMenu_CustomStoryList(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin, cLuxMainMenu_CustomStory* apWindow);
    ~cLuxMainMenu_CustomStoryList();

    void CreateGui();

    void ExitPressed();

private:
    void OnSetActive(bool abX);

    void PopulateStoryList();
    void ClearStoryList();

    ////////////////////////
    // Properties
    cVector2f mvWindowSize;

    cLuxMainMenu_CustomStory* mpStoryWindow;

    ////////////////////////
    // Layout
    cWidgetListBox* mpLBStories;

    ////////////////////////
    // Callbacks
    void LoadStory(int alIdx);

    bool WindowOnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(WindowOnUpdate);

    bool SelectStory(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(SelectStory);

    bool RepopulateStoryList(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(RepopulateStoryList);

    bool PressOK(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(PressOK);

    bool PressCancel(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(PressCancel);

    bool LoadStoryCallback(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(LoadStoryCallback);

    bool ExitCallback(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(ExitCallback);
};

//----------------------------------------------

#endif // LUX_MAIN_MENU_CUSTOM_STORY_H
