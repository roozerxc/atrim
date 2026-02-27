#ifndef LUX_PRE_MENU_H
#define LUX_PRE_MENU_H

//----------------------------------------------

#include "LuxBase.h"

enum eLuxPreMenuState
{
    eLuxPreMenuState_Initial,
    eLuxPreMenuState_FadeIn,
    eLuxPreMenuState_FadeOut,
    eLuxPreMenuState_FastFadeOut,
    eLuxPreMenuState_ShowPremenuSection,
    eLuxPreMenuState_Final,

    eLuxPreMenuState_LastEnum,
};

//----------------------------------------------

class cLuxPreMenuTextElement
{
public:
    bool Load(cXmlElement* apElement, const cVector2f& avGuiSetSize);

    cWidgetLabel* CreateLabel(cGuiSet* apSet);

private:
    tWString    msText;
    cVector3f    mvPos;
    cVector2f    mvFrameSize;
    cVector2f    mvFontSize;
    cColor        mColor;
    tString        msFontFile;
    eFontAlign    mAlign;
    
    float        mfTime;
};

//----------------------------------------------

typedef std::list<cLuxPreMenuTextElement*>    tPreMenuTextList;
typedef tPreMenuTextList::iterator            tPreMenuTextListIt;

//----------------------------------------------

class cLuxPreMenuSection
{
public:
    cLuxPreMenuSection();
    ~cLuxPreMenuSection();

    bool Load(cXmlElement* apElement, const cVector2f& avGuiSetSize);

    void AddTextElement(cLuxPreMenuTextElement* apText);
    bool HasTextElements();

    const tPreMenuTextList& GetTextElements() { return mlstTextElements; }

    cGuiGfxElement* CreateBackground(cGui* apGui, cTextureManager* apTexMgr);
    float GetTime() { return mfTime; }

    bool ShowFirstStartOnly(){ return mbShowFirstStartOnly;}

    bool HasGammaSettings() { return mbHasGammaSettings; }

public:
    cColor mBackgroundColor;
    tString msBackgroundFile;
    tPreMenuTextList mlstTextElements;
    float mfTime;
    bool mbShowFirstStartOnly;
    bool mbHasGammaSettings;
    tString msMusic;
    float mfMusicVolume;
    float mfMusicFadeTime;
    bool mbAllowSkipping;
};

//----------------------------------------------

class cLuxPreMenu : public iLuxUpdateable
{
public:    
    cLuxPreMenu();
    ~cLuxPreMenu();

    void Update(float afTimeStep);

    void OnEnterContainer(const tString& asOldContainer);
    void OnLeaveContainer(const tString& asNewContainer);

    void OnDraw(float afFrameTime);

    void ButtonPressed();

    void AppLostInputFocus();
    void AppGotInputFocus();
    cGuiSet* GetSet() { return mpGuiSet; }
    eLuxPreMenuState GetState() { return mCurrentState; }
    bool IsContinueButtonVisible() { return mpBContinue->IsVisible(); }

private:
    void UpdateActions(float afTimeStep);
    void UpdateState();

    void LoadPreMenuSections();

    void SetGammaValueToInput(float afGamma, bool abGenCallback=false);

    bool Gamma_ChangeValue(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(Gamma_ChangeValue);
    
    bool Continue_Pressed(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(Continue_Pressed);

    bool Continue_UIPressed(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(Continue_UIPressed);

    bool Gamma_UIArrowPressed(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(Gamma_UIArrowPressed);
    ///////////////////////
    // Settings
    
    ///////////////////////
    // Variables
    cGraphics *mpGraphics;
    
    cGui* mpGui;
    cGuiSet* mpGuiSet;

    cViewport *mpViewport;

    cVector2f mvScreenSize;
    cVector2f mvGuiSetSize;
    cVector2f mvGuiSetCenterSize;
    cVector2f mvGuiSetOffset;
    cVector3f mvGuiSetStartPos;
    
    eLuxPreMenuState mCurrentState;
    int mlCurrentSectionIdx;
    float mfTimer;
    float mfAlphaFade;

    bool mbExitPreMenu;
    
    cGuiGfxElement* mpBlackFade;

    std::vector<cLuxPreMenuSection*> mvSections;

    cLuxPreMenuSection* mpCurrentSection;
    cGuiGfxElement* mpCurrentBackground;
    std::vector<cWidgetLabel*> mvCurrentLabels;
    
    cWidgetButton* mpBContinue;

    cWidgetDummy* mpGGamma;
    cWidgetImage* mpIGammaPreview;
    cWidgetLabel* mpLGamma;
    cWidgetSlider* mpSGamma;
    float mfGammaMinValue;
    float mfGammaMaxValue;
    float mfGammaStep;
};

//----------------------------------------------


#endif // LUX_MAIN_MENU_H
