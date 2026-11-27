#include "LuxMainMenu_Options.h"

#include "LuxMap.h"
#include "LuxMapHandler.h"
#include "LuxEffectHandler.h"
#include "LuxInputHandler.h"
#include "LuxHintHandler.h"
#include "LuxConfigHandler.h"
#include "LuxPostEffects.h"
#include "LuxMessageHandler.h"
#include "LuxPlayerHelpers.h"
#include "LuxPlayer.h"
#include "LuxHelpFuncs.h"
#include "LuxGlobalDataHandler.h"

//////////////////////////////////////////////////////////////////////////
// HELPERS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

static int GetIndexFromAnisotropy(float afX)
{
    if(afX <= 1)
    {
        return 0;
    }
    if(afX <= 2)
    {
        return 1;
    }
    if(afX <= 4)
    {
        return 2;
    }
    if(afX <= 8)
    {
        return 3;
    }
    if(afX <= 16)
    {
        return 4;
    }
    return 0;
}

static float GetAnisotropyFromIndex(int alX)
{
    switch(alX)
    {
    case 0:
        return 1.0f;
    case 1:
        return 2.0f;
    case 2:
        return 4.0f;
    case 3:
        return 8.0f;
    case 4:
        return 16.0f;
    }
    return 1.0f;
}

//-----------------------------------------------------------------------

static int GetIndexFromSSAOSamples(int alX)
{
    if(alX <= 4)
    {
        return 0;
    }
    if(alX <= 8)
    {
        return 1;
    }
    if(alX <= 16)
    {
        return 2;
    }
    if(alX <= 32)
    {
        return 3;
    }
    //if(alX <= 64) return 4;
    //if(alX <= 128) return 5;
    return 0;
}

//-----------------------------------------------------------------------

//Reduce input lag combo box: 0 = Off (driver queues frames, MaxFramesAhead -1),
//1 = On (at most one frame queued), 2 = Strict (glFinish every frame, MaxFramesAhead 0)
static int GetMaxFramesAheadFromIndex(int alIdx)
{
    if(alIdx == 1)
    {
        return 1;
    }
    if(alIdx == 2)
    {
        return 0;
    }
    return -1;
}

static int GetIndexFromMaxFramesAhead(int alX)
{
    if(alX < 0)
    {
        return 0;
    }
    if(alX == 0)
    {
        return 2;
    }
    //1, and 2 or 3 set by hand in the config
    return 1;
}

//The value to use for the selected item: a hand set value (2 or 3) is kept as long as
//the selection still shows it.
static int GetMaxFramesAheadForSelection(int alIdx, int alCurrent)
{
    if(alIdx == GetIndexFromMaxFramesAhead(alCurrent))
    {
        return alCurrent;
    }
    return GetMaxFramesAheadFromIndex(alIdx);
}

static int GetSSAOSamplesFromIndex(int alX)
{
    switch(alX)
    {
    case 0:
        return 4;
    case 1:
        return 8;
    case 2:
        return 16;
    case 3:
        return 32;
        //case 4: return 64;
        //case 5: return 128;
    }
    return 8;
}

//-----------------------------------------------------------------------

cResourceVarsObject cLuxMainMenu_Options::mInitialValues = cResourceVarsObject();
cResourceVarsObject cLuxMainMenu_Options::mCurrentValues = cResourceVarsObject();

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxMainMenu_Options::cLuxMainMenu_Options(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin) : iLuxMainMenuWindow(apGuiSet, apGuiSkin)
{
    mvWindowSize = cVector2f(620, 460);

    mbTipFadeRestart = false;
    mbTipWidgetUpdated = true;
    mbTipTextReset = false;
    mpCurrentTipWidget = NULL;

    mfFlashMin = 0.0f;
    mfFlashStep = 0.05f;
    mfFlashMax = 1.0f;

    mfFOVMin = cLuxPlayer::kfMinFOVDegrees;
    mfFOVStep = 1.0f;
    mfFOVMax = cLuxPlayer::kfMaxFOVDegrees;

    mfGammaMin = 0.3f;
    mfGammaStep = 0.05f;
    mfGammaMax = 2.0f;

    mfMouseSensitivityMin = 0.2f;
    mfMouseSensitivityStep = 0.1f;
    mfMouseSensitivityMax = 5.0f;

#if USE_GAMEPAD
    mfGamepadLookSensitivityMin = 0.5f;
    mfGamepadLookSensitivityStep = 0.05f;
    mfGamepadLookSensitivityMax = 3.0f;
#endif

    mfVolumeMin = 0.0f;
    mfVolumeStep = 0.01f;
    mfVolumeMax = 1.0f;

    mbShowCommentary = gpBase->mpMenuCfg->GetBool("Options", "ShowCommentary", false);
    mbShaderModel3 = gpBase->mpEngine->GetGraphics()->GetLowLevel()->GetCaps(eGraphicCaps_ShaderModel_3) != 0;
    mbFullbright = cRendererDeferred::GetFullbrightActive();

    mbSettingInitialValues = false;

    mbKeyConfigOpen = false;
}

//-----------------------------------------------------------------------

cLuxMainMenu_Options::~cLuxMainMenu_Options()
{
    STLDeleteAll(mvOptionData);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------

void cLuxMainMenu_Options::CreateGui()
{
    mInitialValues = cResourceVarsObject();
    mCurrentValues = cResourceVarsObject();

    CreateMainGui();
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::ExitPressed()
{
    if(mpGuiSet->PopUpIsActive())
    {
        return;
    }

    gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_LastEnum);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::CreateMainGui()
{
    float fLeftBorderSize = 30;
    float fUpperBorderSize = 10;

    cVector3f vPos(fLeftBorderSize, 35 + fUpperBorderSize, 1);

    //////////////////////////
    //Window
    mpWindow = mpGuiSet->CreateWidgetWindow(eWidgetWindowButtonFlag_None, cVector3f(0, 0, 5), mvWindowSize, kTranslate("MainMenu", "Options"));
    mpWindow->AddCallback(eGuiMessage_OnUpdate, this, kGuiCallback(Window_OnUpdate));
    mpWindow->SetStatic(true);

    //////////////////////////
    //Buttons
    float fButtonWidth = 80;
    float fButtonSepp = 3;
    vPos.x = mpWindow->GetSize().x - fButtonWidth * 2 - fButtonSepp - 5;
    vPos.y = mpWindow->GetSize().y - 25 - 10;

    mpBOK = mpGuiSet->CreateWidgetButton(vPos, cVector2f(fButtonWidth, 30), kTranslate("MainMenu", "OK"), mpWindow);
    mpBOK->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(PressOK));

    vPos.x += fButtonWidth + fButtonSepp;
    mpBCancel = mpGuiSet->CreateWidgetButton(vPos, cVector2f(fButtonWidth, 30), kTranslate("MainMenu", "Cancel"), mpWindow);
    mpBCancel->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(PressCancel));
    mpBCancel->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(UIPressCancel));
    mpBCancel->SetGlobalUIInputListener(true);

    mpBOK->SetFocusNavigation(eUIArrow_Right, mpBCancel);
    mpBCancel->SetFocusNavigation(eUIArrow_Left, mpBOK);

    vPos = cVector3f(fLeftBorderSize, 35 + fUpperBorderSize, 1);

    //////////////////////////
    //Tabs
    cVector2f vTabFrameSize(mvWindowSize.x - fLeftBorderSize * 2, mvWindowSize.y - fUpperBorderSize * 3 - 30 - 25 - 20);
    cWidgetTabFrame *pTabFrame = mpGuiSet->CreateWidgetTabFrame(vPos, vTabFrameSize, _W(""), mpWindow, false, true);
    pTabFrame->SetGlobalUIInputListener(true);


    pTabFrame->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(TabFrame_OnPageChange));

    mpTabGame = pTabFrame->AddTab(kTranslate("MainMenu", "OptionsGame"));
    mpTabGraphics = pTabFrame->AddTab(kTranslate("MainMenu", "OptionsGraphics"));
    mpTabInput = pTabFrame->AddTab(kTranslate("MainMenu", "OptionsInput"));
    mpTabSound = pTabFrame->AddTab(kTranslate("MainMenu", "OptionsSound"));

#if USE_GAMEPAD
    ///////////////////
    // Add help icons for tabs
    cVector2f vImageSize = fUpperBorderSize * 2;
    vImageSize.x *= 1.5f;
    vPos.y -= 1;
    vPos.x -= fUpperBorderSize * 1.3f;

    mpShoulderHint[0] = mpGuiSet->CreateWidgetImage("gamepad_lb.tga", vPos, vImageSize, eGuiMaterial_Alpha, false, mpWindow, "LB Tip");

    vPos.x = mpWindow->GetSize().x - fUpperBorderSize * 5.0f;

    mpShoulderHint[1] = mpGuiSet->CreateWidgetImage("gamepad_rb.tga", vPos, vImageSize, eGuiMaterial_Alpha, false, mpWindow, "RB Tip");
#endif

    for(int i = 0; i < pTabFrame->GetTabNum(); ++i)
    {
        cWidgetTab* pTab = pTabFrame->GetTab(i);
        int lPrev = i - 1;
        int lNext = i + 1;

        if(lPrev >= 0)
        {
            pTab->GetTabLabel()->SetFocusNavigation(eUIArrow_Left, pTabFrame->GetTab(lPrev)->GetTabLabel());
        }
        if(lNext < pTabFrame->GetTabNum())
        {
            pTab->GetTabLabel()->SetFocusNavigation(eUIArrow_Right, pTabFrame->GetTab(lNext)->GetTabLabel());
        }
    }

    AddGraphicsOptions(mpTabGraphics);
    AddSoundOptions(mpTabSound);
    AddGameOptions(mpTabGame);
    AddInputOptions(mpTabInput);

    //////////////////////////
    //Tip Label
    vPos.x = fLeftBorderSize;
    vPos.y = mpWindow->GetSize().y - 25 - 10;
    mpLTip = mpGuiSet->CreateWidgetLabel(vPos + cVector3f(10, 0, 0), cVector2f(400, 30), _W(""), mpWindow);
    mpLTip->SetDefaultFontColor(cColor(1, 1));
    mpLTip->SetWordWrap(true);
    mpLTip->SetClipActive(true);
    mpLTip->SetScrollSpeedMul(4.0f);
    mpLTip->SetDrawBackGround(true);
    mpLTip->SetBackGroundColor(cColor(0, 0.5f));

    mpLastFocusedWidget = mpCBLanguage;
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::AddGameOptions(cWidgetTab* apTab)
{
    float fBorderSize = 15;
    cVector3f vPos(fBorderSize, 6 + fBorderSize + 10, 0.1f);

    ///////////////////////////////////////////////
    // Language Combobox
    cWidgetLabel* pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "Language"), apTab);
    mpCBLanguage = mpGuiSet->CreateWidgetComboBox(vPos + cVector3f(pLabel->GetSize().x + 5, -2, 0), cVector2f(150, 25), _W(""), apTab);
    //Language is only loaded at startup, so a restart is needed for it to apply everywhere.
    SetUpInput(pLabel, mpCBLanguage, true, kTranslate("OptionsMenu", "LanguageTip"));
    mpCBLanguage->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(ChangeLanguage));

    vPos.y += mpCBLanguage->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Subtitles Checkbox
    mpChBShowSubtitles = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "ShowSubtitles"), apTab);
    SetUpInput(NULL, mpChBShowSubtitles, false, kTranslate("OptionsMenu", "ShowSubtitlesTip"));

    ///////////////////////////////////////////////
    // Effect subtitles Checkbox
    mpChBShowEffectSubtitles = mpGuiSet->CreateWidgetCheckBox(vPos + cVector3f(mpChBShowSubtitles->GetSize().x + 15, 0, 0), 0, kTranslate("OptionsMenu", "ShowEffectSubtitles"), apTab);
    SetUpInput(NULL, mpChBShowEffectSubtitles, false, kTranslate("OptionsMenu", "ShowEffectSubtitlesTip"));

    vPos.y += mpChBShowSubtitles->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Hints Checkbox
    mpChBShowHints = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "ShowHints"), apTab);
    SetUpInput(NULL, mpChBShowHints, false, kTranslate("OptionsMenu", "ShowHintsTip"));

    vPos.y += mpChBShowHints->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Death Hints subtitles Checkbox
    mpChBShowDeathHints = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "ShowDeathHints"), apTab);
    SetUpInput(NULL, mpChBShowDeathHints, false, kTranslate("OptionsMenu", "ShowDeathHintsTip"));

    vPos.y += mpChBShowDeathHints->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Crosshair Checkbox
    mpChBShowCrosshair = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "ShowCrosshair"), apTab);
    SetUpInput(NULL, mpChBShowCrosshair, false, kTranslate("OptionsMenu", "ShowCrosshairTip"));
    vPos.y += mpChBShowCrosshair->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Focus Icon style Combobox
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "FocusIconStyle"), apTab);
    mpCBFocusIconStyle = mpGuiSet->CreateWidgetComboBox(vPos + cVector3f(pLabel->GetSize().x + 5, -2, 0), cVector2f(150, 25), _W(""), apTab);
    SetUpInput(pLabel, mpCBFocusIconStyle, false, kTranslate("OptionsMenu", "FocusIconStyleTip"));
    mpCBFocusIconStyle->AddItem(kTranslate("OptionsMenu", "FocusIconStyleDefault"));
    mpCBFocusIconStyle->AddItem(kTranslate("OptionsMenu", "FocusIconStyleSimple"));
    vPos.y += pLabel->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Hand orientation Combobox
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "HandOrientation"), apTab);
    mpCBHandOrientation = mpGuiSet->CreateWidgetComboBox(vPos + cVector3f(pLabel->GetSize().x + 5, -2, 0), cVector2f(150, 25), _W(""), apTab);
    SetUpInput(pLabel, mpCBHandOrientation, false, kTranslate("OptionsMenu", "HandOrientationTip"));
    mpCBHandOrientation->AddItem(kTranslate("ButtonNames", "Left"));
    mpCBHandOrientation->AddItem(kTranslate("ButtonNames", "Right"));
    vPos.y += pLabel->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Field of view (per profile, in game only; the menu cameras keep theirs)
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "FieldOfView"), apTab);
    mpSFOV = mpGuiSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal, cVector3f(pLabel->GetSize().x + 5, -1, 0), cVector2f(150, 20), 0, pLabel);
    SetUpInput(pLabel, mpSFOV, false, kTranslate("OptionsMenu", "FieldOfViewTip"));
    SetUpSlider(mpSFOV, mfFOVMin, mfFOVMax, mfFOVStep, kGuiCallback(FOVSlider_OnMove), &mpLFOV);
    vPos.y += pLabel->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Flashback Intensity
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "FlashIntensity"), apTab);
    mpSFlash = mpGuiSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal, cVector3f(pLabel->GetSize().x + 5, -1, 0), cVector2f(150, 20), 0, pLabel);
    SetUpInput(pLabel, mpSFlash, false, kTranslate("OptionsMenu", "FlashIntensityTip"));
    SetUpSlider(mpSFlash, mfFlashMin, mfFlashMax, mfFlashStep, kGuiCallback(FlashSlider_OnMove), &mpLFlash);
    vPos.y += pLabel->GetSize().y + 15;

    //////////////////////////////////
    // Commentary
    if(mbShowCommentary)
    {
        mpChBShowCommentary = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "Commentary"), apTab);
        SetUpInput(NULL, mpChBShowCommentary, false, kTranslate("OptionsMenu", "CommentaryTip"));
        vPos.y += mpChBShowCommentary->GetSize().y + 15;
    }

    // Populate languages
    PopulateLanguageList();

    //////////////////////////////////////////////////////////////////////////////////
    // Set up navigation
    iWidget* pLastWidget = NULL;

    // Down
    mpCBLanguage->SetFocusNavigation(eUIArrow_Down, mpChBShowSubtitles);
    mpChBShowSubtitles->SetFocusNavigation(eUIArrow_Down, mpChBShowHints);
    mpChBShowEffectSubtitles->SetFocusNavigation(eUIArrow_Down, mpChBShowHints);
    mpChBShowHints->SetFocusNavigation(eUIArrow_Down, mpChBShowDeathHints);
    mpChBShowDeathHints->SetFocusNavigation(eUIArrow_Down, mpChBShowCrosshair);
    mpChBShowCrosshair->SetFocusNavigation(eUIArrow_Down, mpCBFocusIconStyle);
    mpCBFocusIconStyle->SetFocusNavigation(eUIArrow_Down, mpCBHandOrientation);
    mpCBHandOrientation->SetFocusNavigation(eUIArrow_Down, mpSFOV);
    mpSFOV->SetFocusNavigation(eUIArrow_Down, mpSFlash);
    if(mbShowCommentary)
    {
        mpSFlash->SetFocusNavigation(eUIArrow_Down, mpChBShowCommentary);
        mpChBShowCommentary->SetFocusNavigation(eUIArrow_Down, mpBOK);

        pLastWidget = mpChBShowCommentary;
    }
    else
    {
        mpSFlash->SetFocusNavigation(eUIArrow_Down, mpBOK);

        pLastWidget = mpSFlash;
    }

    apTab->SetUserData(pLastWidget);
    apTab->GetTabLabel()->SetUserData(mpCBLanguage);

    // Up
    mpChBShowSubtitles->SetFocusNavigation(eUIArrow_Up, mpCBLanguage);
    mpChBShowEffectSubtitles->SetFocusNavigation(eUIArrow_Up, mpCBLanguage);
    mpChBShowHints->SetFocusNavigation(eUIArrow_Up, mpChBShowSubtitles);
    mpChBShowDeathHints->SetFocusNavigation(eUIArrow_Up, mpChBShowHints);
    mpChBShowCrosshair->SetFocusNavigation(eUIArrow_Up, mpChBShowDeathHints);
    mpCBFocusIconStyle->SetFocusNavigation(eUIArrow_Up, mpChBShowCrosshair);
    mpCBHandOrientation->SetFocusNavigation(eUIArrow_Up, mpCBFocusIconStyle);
    mpSFOV->SetFocusNavigation(eUIArrow_Up, mpCBHandOrientation);
    mpSFlash->SetFocusNavigation(eUIArrow_Up, mpSFOV);
    if(mbShowCommentary)
    {
        mpChBShowCommentary->SetFocusNavigation(eUIArrow_Up, mpSFlash);
    }

    // Left/Right
    mpChBShowSubtitles->SetFocusNavigation(eUIArrow_Right, mpChBShowEffectSubtitles);
    mpChBShowEffectSubtitles->SetFocusNavigation(eUIArrow_Left, mpChBShowSubtitles);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::AddGraphicsOptions(cWidgetTab* apTab)
{
    float fBorderSize = 15;
    cVector3f vPos(fBorderSize - 5, 6 + fBorderSize, 0.1f);

    /////////////////////////////////
    // Basic options
    mpDBasicGfxOptions = mpGuiSet->CreateWidgetDummy(vPos, apTab);
    mpDBasicGfxOptions->SetVisible(true);
    mpDBasicGfxOptions->SetEnabled(true);
    AddBasicGfxOptions(mpDBasicGfxOptions);

    /////////////////////////////////
    // Avanced options
    mpDAdvancedGfxOptions = mpGuiSet->CreateWidgetDummy(vPos, apTab);
    mpDAdvancedGfxOptions->SetVisible(false);
    mpDAdvancedGfxOptions->SetEnabled(false);
    AddAdvancedGfxOptions(mpDAdvancedGfxOptions);

    // Basic/Advanced toggle button
    tWStringVec vToggleButtonStrings;
    vToggleButtonStrings.push_back(kTranslate("OptionsMenu", "AdvancedOptions"));
    vToggleButtonStrings.push_back(kTranslate("OptionsMenu", "BasicOptions"));

    float fButtonWidth = 0;
    mpBToggleShowGfxOptions = mpGuiSet->CreateWidgetButton(0,
                              cVector2f(0, 25),
                              vToggleButtonStrings[0],
                              apTab);
    mpBToggleShowGfxOptions->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(PressToggleShowGfxOptions));
    for(int i = 0; i < (int)vToggleButtonStrings.size(); ++i)
    {
        float fStringLength = mpBToggleShowGfxOptions->GetDefaultFontType()->GetLength(mpBToggleShowGfxOptions->GetDefaultFontSize(),
                              mpBToggleShowGfxOptions->GetText().c_str()) + 10;

        if(fButtonWidth < fStringLength)
        {
            fButtonWidth = fStringLength;
        }
    }


    mpBToggleShowGfxOptions->SetSize(cVector2f(fButtonWidth, mpBToggleShowGfxOptions->GetSize().y));
    mpBToggleShowGfxOptions->SetPosition(cVector3f(apTab->GetSize()) - cVector3f(fButtonWidth, 50, -2));

    SetUpInput(NULL, mpBToggleShowGfxOptions, true, _W(""));

    mpBToggleShowGfxOptions->SetFocusNavigation(eUIArrow_Down, mpBOK);
    mpBToggleShowGfxOptions->SetFocusNavigation(eUIArrow_Up, mpSGamma);

    mpSGamma->SetFocusNavigation(eUIArrow_Down, mpBToggleShowGfxOptions);
    mpChBPotatoMode->SetFocusNavigation(eUIArrow_Down, mpBToggleShowGfxOptions);
    //Bottom row of the Misc group
    mpChBShaderWarming->SetFocusNavigation(eUIArrow_Down, mpBToggleShowGfxOptions);
    mpCBMaxFramesAhead->SetFocusNavigation(eUIArrow_Down, mpBToggleShowGfxOptions);

    apTab->SetUserData(mpBToggleShowGfxOptions);
    apTab->GetTabLabel()->SetUserData(mpCBResolution);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::AddBasicGfxOptions(cWidgetDummy* apDummy)
{
    float fBorderSize = 0;
    cVector3f vPos(fBorderSize, 6 + fBorderSize, 0.1f);

    cWidgetLabel* pLabel = NULL;

    /////////////////////////////////
    // Screen group
    cWidgetGroup *pGroup = mpGuiSet->CreateWidgetGroup(vPos, 0, kTranslate("OptionsMenu", "Screen"), apDummy);
    {
        float fBorderSize = 15;
        pGroup->SetSize(cVector2f(apDummy->GetParent()->GetSize().x - fBorderSize - fBorderSize, 70));
        cVector3f vPosInGroup = cVector3f(fBorderSize, fBorderSize, 0.1f);

        /////////////////////////////////
        // Resolution
        pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "Resolution"), pGroup);
        mpCBResolution = mpGuiSet->CreateWidgetComboBox(pLabel->GetLocalPosition() + cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(175, 25), _W(""), pGroup);
        SetUpInput(pLabel, mpCBResolution, true, kTranslate("OptionsMenu", "ResolutionTip"));

        vPosInGroup.x += mpCBResolution->GetSize().x + 100;

        /////////////////////////////////
        // Full screen and Vsync
        mpChBFullScreen = mpGuiSet->CreateWidgetCheckBox(vPosInGroup + cVector3f(0, 2, 0), -1, kTranslate("OptionsMenu", "FullScreen"), pGroup);
        SetUpInput(NULL, mpChBFullScreen, true, kTranslate("OptionsMenu", "FullScreenTip"));

        mpChBVSync = mpGuiSet->CreateWidgetCheckBox(vPosInGroup + cVector3f(0, mpChBFullScreen->GetSize().y + 10, 0), 0, kTranslate("OptionsMenu", "VSync"), pGroup);
        SetUpInput(NULL, mpChBVSync, false, kTranslate("OptionsMenu", "VSyncTip"));
    }

    vPos.y += pGroup->GetSize().y + 15;

    /////////////////////////////////
    // Texture Quality
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "TexQuality"), apDummy);
    mpCBTextureSizeLevel = mpGuiSet->CreateWidgetComboBox(pLabel->GetLocalPosition() + cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(100, 25), _W(""), apDummy);
    SetUpInput(pLabel, mpCBTextureSizeLevel, true, kTranslate("OptionsMenu", "TexQualityTip"));

    cMaterialManager* pMatMgr = gpBase->mpEngine->GetResources()->GetMaterialManager();
    tWStringVec vTexQualityStrings;
    vTexQualityStrings.push_back(kTranslate("OptionsMenu", "UltraLow"));
    vTexQualityStrings.push_back(kTranslate("OptionsMenu", "Low"));
    vTexQualityStrings.push_back(kTranslate("OptionsMenu", "Medium"));
    vTexQualityStrings.push_back(kTranslate("OptionsMenu", "High"));

    mpCBTextureSizeLevel->ClearItems();
    for(int i = 0; i < (int)vTexQualityStrings.size(); ++i)
    {
        mpCBTextureSizeLevel->AddItem(vTexQualityStrings[i]);
    }

    /////////////////////////////////
    // Potato mode (fullbright G-buffer mode, GBufferMode=1), below texture quality
    {
        cVector3f vPotatoPos = mpCBTextureSizeLevel->GetLocalPosition() + cVector3f(0, mpCBTextureSizeLevel->GetSize().y + 15, 0);
        mpChBPotatoMode = mpGuiSet->CreateWidgetCheckBox(vPotatoPos, 0, kTranslate("OptionsMenu", "PotatoMode"), apDummy);
        SetUpInput(NULL, mpChBPotatoMode, true, kTranslate("OptionsMenu", "PotatoModeTip"));
    }

    /////////////////////////////////
    // Gamma
    vPos.x += 140;

    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "Gamma"), apDummy);
    {
        cVector3f vLabelPos(0, pLabel->GetSize().y + 5, 0);

        cWidgetImage *pImg = mpGuiSet->CreateWidgetImage("menu_gamma.tga", vLabelPos, -1, eGuiMaterial_Alpha, false, pLabel);
        SetUpInput(NULL, pImg, false, kTranslate("OptionsMenu", "GammaInstructions"));
        vLabelPos.y += pImg->GetSize().y + 5.0f;

        mpSGamma = mpGuiSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal, vLabelPos, cVector2f(pImg->GetSize().x, 20), 0, pLabel);
        SetUpInput(pLabel, mpSGamma, false, kTranslate("OptionsMenu", "GammaInstructions"));
        SetUpSlider(mpSGamma, mfGammaMin, mfGammaMax, mfGammaStep, kGuiCallback(GammaSlider_OnMove), &mpLGamma);
        mpSGamma->SetBarValueSize(4);

        vLabelPos.y += mpSGamma->GetSize().y + 4.0f;
    }

    mpCBResolution->SetFocusNavigation(eUIArrow_Down, mpCBTextureSizeLevel);
    mpCBResolution->SetFocusNavigation(eUIArrow_Right, mpChBFullScreen);

    mpChBFullScreen->SetFocusNavigation(eUIArrow_Left, mpCBResolution);
    mpChBFullScreen->SetFocusNavigation(eUIArrow_Down, mpChBVSync);

    mpChBVSync->SetFocusNavigation(eUIArrow_Left, mpCBResolution);
    mpChBVSync->SetFocusNavigation(eUIArrow_Up, mpChBFullScreen);
    mpChBVSync->SetFocusNavigation(eUIArrow_Down, mpCBTextureSizeLevel);

    mpCBTextureSizeLevel->SetFocusNavigation(eUIArrow_Up, mpCBResolution);
    mpCBTextureSizeLevel->SetFocusNavigation(eUIArrow_Down, mpChBPotatoMode);
    mpCBTextureSizeLevel->SetFocusNavigation(eUIArrow_Right, mpSGamma);

    mpChBPotatoMode->SetFocusNavigation(eUIArrow_Up, mpCBTextureSizeLevel);
    mpChBPotatoMode->SetFocusNavigation(eUIArrow_Right, mpSGamma);

    mpSGamma->SetFocusNavigation(eUIArrow_Up, mpCBTextureSizeLevel);
    mpSGamma->SetFocusNavigation(eUIArrow_Left, mpChBPotatoMode);

}

//-----------------------------------------------------------------------

// Sets up/down/left/right links for widgets laid out in a grid (row major,
// alNumCols per row, NULL for empty cells). Left/right go to the nearest
// widget in the row. Up/down go to the next row that has a widget in the
// same column, or failing that to the nearest widget in the next row that
// has any. Links off the edges are left alone.
void cLuxMainMenu_Options::SetUpGridNavigation(iWidget** apGrid, int alNumRows, int alNumCols)
{
    for(int lRow = 0; lRow < alNumRows; ++lRow)
    {
        for(int lCol = 0; lCol < alNumCols; ++lCol)
        {
            iWidget* pWidget = apGrid[lRow * alNumCols + lCol];
            if(pWidget == NULL)
            {
                continue;
            }

            //////////////////
            // Left / right
            for(int lDir = -1; lDir <= 1; lDir += 2)
            {
                iWidget* pNext = NULL;
                for(int c = lCol + lDir; c >= 0 && c < alNumCols && pNext == NULL; c += lDir)
                {
                    pNext = apGrid[lRow * alNumCols + c];
                }
                pWidget->SetFocusNavigation(lDir < 0 ? eUIArrow_Left : eUIArrow_Right, pNext);
            }

            //////////////////
            // Up / down
            for(int lDir = -1; lDir <= 1; lDir += 2)
            {
                iWidget* pNext = NULL;

                // Same column
                for(int r = lRow + lDir; r >= 0 && r < alNumRows && pNext == NULL; r += lDir)
                {
                    pNext = apGrid[r * alNumCols + lCol];
                }

                // Nearest widget in the next row that has any
                for(int r = lRow + lDir; r >= 0 && r < alNumRows && pNext == NULL; r += lDir)
                {
                    for(int lOffset = 1; lOffset < alNumCols && pNext == NULL; ++lOffset)
                    {
                        if(lCol - lOffset >= 0)
                        {
                            pNext = apGrid[r * alNumCols + lCol - lOffset];
                        }
                        if(pNext == NULL && lCol + lOffset < alNumCols)
                        {
                            pNext = apGrid[r * alNumCols + lCol + lOffset];
                        }
                    }
                }

                pWidget->SetFocusNavigation(lDir < 0 ? eUIArrow_Up : eUIArrow_Down, pNext);
            }
        }
    }
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::AddAdvancedGfxOptions(cWidgetDummy* apDummy)
{
    float fBorderSize = 0;
    cVector3f vPos(fBorderSize, 6 + fBorderSize, 0.1f);
    float fItemSep = 180;

    cWidgetFrame* pMainFrame = mpGuiSet->CreateWidgetFrame(cVector3f(0, 0, 1), cVector2f(550, 275), false, apDummy, false, true);
    pMainFrame->SetDrawBackground(false);

    cWidgetLabel* pLabel = NULL;

    cVector2f vGroupSize = cVector2f(520, 70);

    ////////////////////////////
    // Texture
    cWidgetGroup *pGroup = mpGuiSet->CreateWidgetGroup(vPos, vGroupSize, kTranslate("OptionsMenu", "Material"), pMainFrame);
    {
        float fBorderSize = 15;
        cVector3f vPosInGroup = cVector3f(fBorderSize, fBorderSize, 0.1f);

        ////////////////////////////
        // Texture filter
        pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "TexFilter"), pGroup);
        mpCBTextureFilter = mpGuiSet->CreateWidgetComboBox(cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(150, 25), _W(""), pLabel);
        SetUpInput(pLabel, mpCBTextureFilter, false, kTranslate("OptionsMenu", "TexFilterTip"));

        //vPosInGroup.x += mpCBTextureFilter->GetSize().x + 20;

        vPosInGroup.x += fItemSep;

        /////////////////////////////
        // Anisotropy
        pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "Anisotropy"), pGroup);
        mpCBAnisotropy = mpGuiSet->CreateWidgetComboBox(cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(100, 25), _W(""), pLabel);
        SetUpInput(pLabel, mpCBAnisotropy, false, kTranslate("OptionsMenu", "AnisotropyTip"));

        //vPosInGroup.x += mpCBAnisotropy->GetSize().x + 20;

        vPosInGroup.x += fItemSep;

        /////////////////////////////
        // Parallax Quality
        mpChBParallax = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "Parallax"), pGroup);
        SetUpInput(NULL, mpChBParallax, true, kTranslate("OptionsMenu", "ParallaxEnabledTip"));

        mpCBParallaxQuality = mpGuiSet->CreateWidgetComboBox(cVector3f(mpCBAnisotropy->GetSize().x + 80, mpChBParallax->GetSize().y + 5, 0), cVector2f(100, 25), _W(""), pLabel);
        SetUpInput(NULL, mpCBParallaxQuality, true, kTranslate("OptionsMenu", "ParallaxQualityTipNew"));

        mpCBParallaxQuality->ClearItems();
        mpCBParallaxQuality->AddItem(kTranslate("OptionsMenu", "ParallaxSimple"));
        mpCBParallaxQuality->AddItem(kTranslate("OptionsMenu", "ParallaxRelief"));

        //Relief parallax requires SM3+, also position checkbox so that its center
        if(mbShaderModel3 == false)
        {
            mpCBParallaxQuality->SetVisible(false);
            mpCBParallaxQuality->SetEnabled(false);

            float fRowHeight = pLabel->GetSize().y + 5 + mpCBAnisotropy->GetSize().y;
            float fCheckBoxY = vPosInGroup.y + (fRowHeight - mpChBParallax->GetSize().y) * 0.5f;
            mpChBParallax->SetPosition(cVector3f(vPosInGroup.x, fCheckBoxY, vPosInGroup.z));
        }

        //Fullbright only draws the albedo, no parallax at all
        if(mbFullbright)
        {
            mpChBParallax->SetVisible(false);
            mpChBParallax->SetEnabled(false);
            mpCBParallaxQuality->SetVisible(false);
            mpCBParallaxQuality->SetEnabled(false);
        }
    }

    vPos.y += pGroup->GetSize().y + 10;

    /////////////////////////////
    // Shadows Group
    pGroup = mpGuiSet->CreateWidgetGroup(vPos, vGroupSize, kTranslate("OptionsMenu", "Shadows"), pMainFrame);
    {
        float fBorderSize = 15;
        cVector3f vPosInGroup = cVector3f(fBorderSize, fBorderSize, 0.1f);

        /////////////////////////////
        // Shadows Active
        mpChBShadows = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "Shadows"), pGroup);
        SetUpInput(NULL, mpChBShadows, false, kTranslate("OptionsMenu", "ShadowsTip"));

        //vPosInGroup.x += mpChBShadows->GetSize().x + 15;
        vPosInGroup.x += fItemSep;

        /////////////////////////////
        // Shadow Quality
        pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "ShadowQuality"), pGroup);
        mpCBShadowQuality = mpGuiSet->CreateWidgetComboBox(cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(100, 25), _W(""), pLabel);
        SetUpInput(pLabel, mpCBShadowQuality, true, kTranslate("OptionsMenu", "ShadowQualityTip"));

        //vPosInGroup.x += mpCBShadowQuality->GetSize().x + 15;
        vPosInGroup.x += fItemSep;

        /////////////////////////////
        // Shadow Resolution
        pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "ShadowRes"), pGroup);
        mpCBShadowRes = mpGuiSet->CreateWidgetComboBox(cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(100, 25), _W(""), pLabel);
        SetUpInput(pLabel, mpCBShadowRes, true, kTranslate("OptionsMenu", "ShadowResTip"));

        // Set up values
        tWStringVec vOptionStrings;
        vOptionStrings.push_back(kTranslate("OptionsMenu", "Low"));
        vOptionStrings.push_back(kTranslate("OptionsMenu", "Medium"));
        vOptionStrings.push_back(kTranslate("OptionsMenu", "High"));
        vOptionStrings.push_back(kTranslate("OptionsMenu", "Ultra"));

        mpCBShadowQuality->ClearItems();
        mpCBShadowRes->ClearItems();
        for(int i = 0; i < (int)vOptionStrings.size(); ++i)
        {
            mpCBShadowQuality->AddItem(vOptionStrings[i]);
            mpCBShadowRes->AddItem(vOptionStrings[i]);
        }
    }

    //Fullbright has no lights, so no shadows
    if(mbFullbright)
    {
        pGroup->SetVisible(false);
        pGroup->SetEnabled(false);
    }
    else
    {
        vPos.y += pGroup->GetSize().y + 10;
    }

    ////////////////////////////
    // Post Effects
    pGroup = mpGuiSet->CreateWidgetGroup(vPos, vGroupSize, kTranslate("OptionsMenu", "PostEffects"), pMainFrame);
    {
        float fBorderSize = 15;
        float fInputSep = 10;
        cVector3f vPosInGroup = cVector3f(fBorderSize, fBorderSize, 0.1f);
        float fMaxWidth = 0;

        // Bloom
        mpChBBloom = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "Bloom"), pGroup);
        SetUpInput(NULL, mpChBBloom, false, kTranslate("OptionsMenu", "BloomTip"));

        if(fMaxWidth < mpChBBloom->GetSize().x)
        {
            fMaxWidth = mpChBBloom->GetSize().x;
        }

        vPosInGroup.y += mpChBBloom->GetSize().y + fInputSep;

        // Motion blur
        mpChBMotionBlur = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "MotionBlur"), pGroup);
        SetUpInput(NULL, mpChBMotionBlur, false, kTranslate("OptionsMenu", "MotionBlurTip"));

        if(fMaxWidth < mpChBMotionBlur->GetSize().x)
        {
            fMaxWidth = mpChBMotionBlur->GetSize().x;
        }

        vPosInGroup.y = fBorderSize;
        //vPosInGroup.x += fMaxWidth + fInputSep;
        //fMaxWidth = 0;

        vPosInGroup.x += fItemSep;

        // Sepia
        mpChBSepia = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "Sepia"), pGroup);
        SetUpInput(NULL, mpChBSepia, false, kTranslate("OptionsMenu", "SepiaTip"));

        if(fMaxWidth < mpChBBloom->GetSize().x)
        {
            fMaxWidth = mpChBBloom->GetSize().x;
        }

        vPosInGroup.y += mpChBSepia->GetSize().y + fInputSep;

        // RadialBlur
        mpChBRadialBlur = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "RadialBlur"), pGroup);
        SetUpInput(NULL, mpChBRadialBlur, false, kTranslate("OptionsMenu", "RadialBlurTip"));

        if(fMaxWidth < mpChBRadialBlur->GetSize().x)
        {
            fMaxWidth = mpChBRadialBlur->GetSize().x;
        }

        vPosInGroup.y = fBorderSize;
        //vPosInGroup.x += fMaxWidth + fInputSep;
        vPosInGroup.x += fItemSep;

        // Insanity
        mpChBInsanity = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "Insanity"), pGroup);
        SetUpInput(NULL, mpChBInsanity, false, kTranslate("OptionsMenu", "InsanityTip"));


        // Color Grading (LightColorGradeMode, the combo box index is the mode). It does not fit
        // next to Insanity's column, so it gets its own row under Motion Blur, starting at the
        // first column. Label with the combo box to its right, like Reduce Input Lag in Misc.
        {
            vPosInGroup.x = fBorderSize;
            vPosInGroup.y = mpChBMotionBlur->GetLocalPosition().y + mpChBMotionBlur->GetSize().y + fInputSep;

            pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "ColorGrading"), pGroup);
            mpCBLightColorGrade = mpGuiSet->CreateWidgetComboBox(cVector3f(pLabel->GetSize().x + 5, -2, 0), cVector2f(100, 25), _W(""), pLabel);
            SetUpInput(pLabel, mpCBLightColorGrade, true, kTranslate("OptionsMenu", "ColorGradingTip"));

            mpCBLightColorGrade->AddItem(kTranslate("OptionsMenu", "ColorGradingVanilla"));
            mpCBLightColorGrade->AddItem(kTranslate("OptionsMenu", "ColorGradingVibrant"));
            mpCBLightColorGrade->AddItem(kTranslate("OptionsMenu", "ColorGradingNatural"));

            //The combo box is a bit taller than a check box
            float fGroupHeight = vPosInGroup.y - 2 + mpCBLightColorGrade->GetSize().y + fBorderSize;
            if(fGroupHeight > vGroupSize.y)
            {
                pGroup->SetSize(cVector2f(vGroupSize.x, fGroupHeight));
            }
        }
    }

    //Fullbright: no post effects
    if(mbFullbright)
    {
        pGroup->SetVisible(false);
        pGroup->SetEnabled(false);
    }
    else
    {
        vPos.y += pGroup->GetSize().y + 10;
    }
    ////////////////////////////
    // SSAO Group
    pGroup = mpGuiSet->CreateWidgetGroup(vPos, vGroupSize, kTranslate("OptionsMenu", "SSAO"), pMainFrame);
    {
        float fBorderSize = 15;
        cVector3f vPosInGroup = cVector3f(fBorderSize, fBorderSize, 0.1f);

        ///////////////////////////
        // SSAO Active
        mpChBSSAO = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "SSAO"), pGroup);
        SetUpInput(NULL, mpChBSSAO, true, kTranslate("OptionsMenu", "SSAOTip"));

        //vPosInGroup.x += mpChBSSAO->GetSize().x + 20;
        vPosInGroup.x += fItemSep;

        ///////////////////////////
        // SSAO Samples
        pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "SSAOSamples"), pGroup);
        mpCBSSAOSamples = mpGuiSet->CreateWidgetComboBox(cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(60, 25), _W(""), pLabel);
        SetUpInput(pLabel, mpCBSSAOSamples, true, kTranslate("OptionsMenu", "SSAOSamplesTip"));

        for(int i = 0; i < 4; ++i)
        {
            int lSamples = GetSSAOSamplesFromIndex(i);
            mpCBSSAOSamples->AddItem(cString::ToStringW(lSamples));
        }

        //vPosInGroup.x += mpCBSSAOSamples->GetSize().x + 20;
        vPosInGroup.x += fItemSep;

        ///////////////////////////
        // SSAO Resolution
        pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "SSAOResolution"), pGroup);
        mpCBSSAOResolution = mpGuiSet->CreateWidgetComboBox(cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(80, 25), _W(""), pLabel);
        SetUpInput(pLabel, mpCBSSAOResolution, true, kTranslate("OptionsMenu", "SSAOResolutionTip"));

        mpCBSSAOResolution->AddItem(kTranslate("OptionsMenu", "Medium"));
        mpCBSSAOResolution->AddItem(kTranslate("OptionsMenu", "High"));
    }

    //SSAO requires SM3+ so hide the whole group if unsupported (and fullbright has no SSAO)
    if(mbShaderModel3 && mbFullbright == false)
    {
        vPos.y += pGroup->GetSize().y + 10;
    }
    else
    {
        pGroup->SetVisible(false);
        pGroup->SetEnabled(false);
    }

    ////////////////////////////
    // Water Group
    //pGroup = mpGuiSet->CreateWidgetGroup(vPos, vGroupSize, _W("Water"), pMainFrame);
    //{
    //    float fBorderSize = 15;
    //    cVector3f vPosInGroup = cVector3f(fBorderSize, fBorderSize, 0.1f);
    //}

    //vPos.x = fBorderSize;
    //vPos.y += pGroup->GetSize().y + 10;

    ////////////////////////////
    // Misc
    // Two columns, like the other groups:
    //   Refraction       World Reflection (Water)
    //   Shader Warming   Reduce Input Lag [combo box]
    // Fullbright hides refraction and reflection, so the second row moves up.
    pGroup = mpGuiSet->CreateWidgetGroup(vPos, vGroupSize, kTranslate("KeyConfig", "Misc"), pMainFrame);
    {
        float fBorderSize = 15;
        float fInputSep = 10;
        cVector3f vPosInGroup = cVector3f(fBorderSize, fBorderSize, 0.1f);

        ///////////////////////////
        // Refraction
        mpChBRefraction = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "Refraction"), pGroup);
        SetUpInput(NULL, mpChBRefraction, true, kTranslate("OptionsMenu", "RefractionTip"));

        ///////////////////////////
        // Shader Warming (only used at start up)
        cVector3f vRow2Pos = vPosInGroup;
        if(mbFullbright == false)
        {
            vRow2Pos.y += mpChBRefraction->GetSize().y + fInputSep;
        }
        mpChBShaderWarming = mpGuiSet->CreateWidgetCheckBox(vRow2Pos, 0, kTranslate("OptionsMenu", "ShaderWarming"), pGroup);
        SetUpInput(NULL, mpChBShaderWarming, true, kTranslate("OptionsMenu", "ShaderWarmingTip"));

        //Second column lines up with the groups above, unless the first column is wider (long translations)
        float fColumnWidth = mpChBShaderWarming->GetSize().x;
        if(mbFullbright == false)
        {
            fColumnWidth = cMath::Max(fColumnWidth, mpChBRefraction->GetSize().x);
        }
        float fColumn2X = fBorderSize + cMath::Max(fItemSep, fColumnWidth + 20);

        ///////////////////////////
        // World Reflection Active
        mpChBWorldReflection = mpGuiSet->CreateWidgetCheckBox(cVector3f(fColumn2X, vPosInGroup.y, vPosInGroup.z), 0, kTranslate("OptionsMenu", "WorldReflection") + _W(" (") + kTranslate("OptionsMenu", "Water") + _W(")"), pGroup);
        SetUpInput(NULL, mpChBWorldReflection, false, kTranslate("OptionsMenu", "WorldReflectionTip"));

        ///////////////////////////
        // Reduce Input Lag (MaxFramesAhead, applied right away), label with the combo box to its right
        pLabel = mpGuiSet->CreateWidgetLabel(cVector3f(fColumn2X, vRow2Pos.y, vRow2Pos.z), -1, kTranslate("OptionsMenu", "ReduceInputLag"), pGroup);
        mpCBMaxFramesAhead = mpGuiSet->CreateWidgetComboBox(cVector3f(pLabel->GetSize().x + 5, -2, 0), cVector2f(100, 25), _W(""), pLabel);
        SetUpInput(pLabel, mpCBMaxFramesAhead, false, kTranslate("OptionsMenu", "ReduceInputLagTip"));

        mpCBMaxFramesAhead->AddItem(kTranslate("OptionsMenu", "ReduceInputLagOff"));
        mpCBMaxFramesAhead->AddItem(kTranslate("OptionsMenu", "ReduceInputLagOn"));
        mpCBMaxFramesAhead->AddItem(kTranslate("OptionsMenu", "ReduceInputLagStrict"));

        //The combo box is a bit taller than a check box
        float fGroupHeight = vRow2Pos.y - 2 + mpCBMaxFramesAhead->GetSize().y + fBorderSize;
        if(fGroupHeight > vGroupSize.y)
        {
            pGroup->SetSize(cVector2f(vGroupSize.x, fGroupHeight));
        }
    }

    //Fullbright: no refraction or reflection (shader warming and input lag still apply)
    if(mbFullbright)
    {
        mpChBRefraction->SetVisible(false);
        mpChBRefraction->SetEnabled(false);
        mpChBWorldReflection->SetVisible(false);
        mpChBWorldReflection->SetEnabled(false);
    }


    //////////////
    // Setup gamepad navigation
    mpCBTextureFilter;
    mpCBAnisotropy;
    mpCBParallaxQuality;

    mpChBShadows;
    mpCBShadowQuality;
    mpCBShadowRes;

    mpChBBloom;
    mpChBSepia;
    mpChBInsanity;
    mpChBMotionBlur;
    mpChBRadialBlur;

    mpChBSSAO;
    mpCBSSAOSamples;
    mpCBSSAOResolution;

    mpChBWorldReflection;
    mpChBRefraction;
    mpChBShaderWarming;
    mpCBMaxFramesAhead;
    mpCBLightColorGrade;

    // Laid out as a grid, same as the window: columns left to right, rows top
    // to bottom. The relief parallax combo box sits under the parallax check
    // box, so it gets a row of its own, and so does the color grading combo box
    // under the post effect check boxes.
    // Options that are hidden (SM3-only ones on cards without SM3, everything
    // but the texture, shader warming and input lag options in fullbright)
    // are left out.
    //
    // This used to be written out link by link and had gaps: the parallax
    // check box could not be reached, and Up from Refraction (the first
    // option reached from the Basic/Advanced button) pointed at itself, so
    // nothing above it could be reached.
    {
        iWidget* vGrid[][3] =
        {
            { mpCBTextureFilter,    mpCBAnisotropy,     mpChBParallax },
            { NULL,                 NULL,               mpCBParallaxQuality },
            { mpChBShadows,         mpCBShadowQuality,  mpCBShadowRes },
            { mpChBBloom,           mpChBSepia,         mpChBInsanity },
            { mpChBMotionBlur,      mpChBRadialBlur,    NULL },
            { mpCBLightColorGrade,  NULL,               NULL },
            { mpChBSSAO,            mpCBSSAOSamples,    mpCBSSAOResolution },
            { mpChBRefraction,      mpChBWorldReflection, NULL },
            { mpChBShaderWarming,   mpCBMaxFramesAhead, NULL },
        };
        const int lRows = sizeof(vGrid) / sizeof(vGrid[0]);
        const int lMiscRow = lRows - 2;

        //Relief parallax and SSAO need SM3
        if(mbShaderModel3 == false)
        {
            vGrid[1][2] = NULL;
            const int lSSAORow = 6;
            vGrid[lSSAORow][0] = vGrid[lSSAORow][1] = vGrid[lSSAORow][2] = NULL;
        }

        //Fullbright: texture filter, anisotropy, and the second Misc row (shader warming
        //and input lag, moved up where refraction and reflection were)
        if(mbFullbright)
        {
            vGrid[0][2] = NULL;
            for(int lRow = 1; lRow <= lMiscRow; ++lRow)
            {
                vGrid[lRow][0] = vGrid[lRow][1] = vGrid[lRow][2] = NULL;
            }
        }

        //(down from the bottom row goes to the Basic/Advanced button, set in AddGraphicsOptions)
        SetUpGridNavigation(&vGrid[0][0], lRows, 3);
    }
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::AddInputOptions(cWidgetTab* apTab)
{
    float fBorderSize = 15;
    cVector3f vPos(fBorderSize, 6 + fBorderSize + 10, 0.1f);

    /////////////////////////////
    // Invert Mouse
    mpChBInvertMouse = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "InvertMouse"), apTab);
    SetUpInput(NULL, mpChBInvertMouse, false, kTranslate("OptionsMenu", "InvertMouseTip"));

    vPos.y += mpChBInvertMouse->GetSize().y + 15;

    /////////////////////////////
    // Smooth Mouse
    mpChBSmoothMouse = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "SmoothMouse"), apTab);
    SetUpInput(NULL, mpChBSmoothMouse, false, kTranslate("OptionsMenu", "SmoothMouseTip"));

    vPos.y += mpChBSmoothMouse->GetSize().y + 15;

    /////////////////////////////
    // Mouse Sensitivity
    cWidgetLabel* pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "MouseSensitivity"), apTab);
    mpSMouseSensitivity = mpGuiSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal, cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(100, 20), 0, pLabel);
    SetUpInput(pLabel, mpSMouseSensitivity, false, kTranslate("OptionsMenu", "MouseSensitivityTip"));
    SetUpSlider(mpSMouseSensitivity, mfMouseSensitivityMin, mfMouseSensitivityMax, mfMouseSensitivityStep, kGuiCallback(MouseSensitivitySlider_OnMove), &mpLMouseSensitivity);

    vPos.y += mpSMouseSensitivity->GetLocalPosition().y + mpSMouseSensitivity->GetSize().y + 15;

#if USE_GAMEPAD
    //////////////////////////////////////////////////
    // Gamepad stuff
    mpChBGamepadInvertLook = NULL;
    mpLGamepadLookSensitivity = NULL;
    mpSGamepadLookSensitivity = NULL;

    /////////////////////////////
    // Invert Gamepad look
    mpChBGamepadInvertLook = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "InvertGamepadLook"), apTab);
    SetUpInput(NULL, mpChBGamepadInvertLook, false, kTranslate("OptionsMenu", "InvertGamepadLookTip"));

    vPos.y += mpChBGamepadInvertLook->GetSize().y + 15;

    /////////////////////////////
    // Gamepad Sensitivity
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "GamepadLookSensitivity"), apTab);
    mpSGamepadLookSensitivity = mpGuiSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal, cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(100, 20), 0, pLabel);
    SetUpInput(pLabel, mpSMouseSensitivity, false, kTranslate("OptionsMenu", "GamepadLookSensitivityTip"));
    SetUpSlider(mpSGamepadLookSensitivity, mfGamepadLookSensitivityMin, mfGamepadLookSensitivityMax, mfGamepadLookSensitivityStep, kGuiCallback(GamepadLookSensitivitySlider_OnMove), &mpLGamepadLookSensitivity);

    vPos.y += mpSGamepadLookSensitivity->GetLocalPosition().y + mpSGamepadLookSensitivity->GetSize().y + 15;
#endif

    //////////////////////////////
    // Key Config Button
    mpBKeyConfig = mpGuiSet->CreateWidgetButton(vPos, cVector2f(0, 25), kTranslate("OptionsMenu", "KeyConfigButton"), apTab);
    float fButtonWidth = mpBKeyConfig->GetDefaultFontType()->GetLength(mpBKeyConfig->GetDefaultFontSize(), mpBKeyConfig->GetText().c_str());
    mpBKeyConfig->SetSize(cVector2f(fButtonWidth + 20, mpBKeyConfig->GetSize().y));
    SetUpInput(NULL, mpBKeyConfig, false, kTranslate("OptionsMenu", "KeyConfigButtonTip"));
    mpBKeyConfig->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(PressKeyConfig));



    //////////////////////////////////////////////////////////////////////////////////
    // Set up navigation

    apTab->SetUserData(mpBKeyConfig);
    apTab->GetTabLabel()->SetUserData(mpChBInvertMouse);

    // Down
    mpChBInvertMouse->SetFocusNavigation(eUIArrow_Down, mpChBSmoothMouse);
    mpChBSmoothMouse->SetFocusNavigation(eUIArrow_Down, mpSMouseSensitivity);
    mpSMouseSensitivity->SetFocusNavigation(eUIArrow_Down, mpBKeyConfig);
#if USE_GAMEPAD
    mpSMouseSensitivity->SetFocusNavigation(eUIArrow_Down, mpChBGamepadInvertLook);
    mpChBGamepadInvertLook->SetFocusNavigation(eUIArrow_Down, mpSGamepadLookSensitivity);
    mpSGamepadLookSensitivity->SetFocusNavigation(eUIArrow_Down, mpBKeyConfig);
#endif
    mpBKeyConfig->SetFocusNavigation(eUIArrow_Down, mpBOK);

    // Up
    mpChBSmoothMouse->SetFocusNavigation(eUIArrow_Up, mpChBInvertMouse);
    mpSMouseSensitivity->SetFocusNavigation(eUIArrow_Up, mpChBSmoothMouse);
    mpBKeyConfig->SetFocusNavigation(eUIArrow_Up, mpSMouseSensitivity);
#if USE_GAMEPAD
    mpChBGamepadInvertLook->SetFocusNavigation(eUIArrow_Up, mpSMouseSensitivity);
    mpSGamepadLookSensitivity->SetFocusNavigation(eUIArrow_Up, mpChBGamepadInvertLook);
    mpBKeyConfig->SetFocusNavigation(eUIArrow_Up, mpSGamepadLookSensitivity);
#endif

}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::AddSoundOptions(cWidgetTab* apTab)
{
    float fBorderSize = 15;
    cVector3f vPos(fBorderSize, 6 + fBorderSize + 10, 0.1f);

    /////////////////////////////
    // Sound Device selector
    cWidgetLabel* pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "SoundDevice"), apTab);
    vPos.y += pLabel->GetSize().y + 5;

    mpCBSndDevice = mpGuiSet->CreateWidgetComboBox(vPos, cVector2f(400, 25), _W(""), apTab);
    SetUpInput(pLabel, mpCBSndDevice, true, kTranslate("OptionsMenu", "SoundDeviceTip"));

    vPos.y += mpCBSndDevice->GetSize().y + 15;

    /////////////////////////////
    // Volume sliders, next to their labels (like Field of view / Flashback intensity)

    // Master (everything, OpenAL listener gain)
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "MasterVolume"), apTab);
    mpSVolume = mpGuiSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal, cVector3f(pLabel->GetSize().x + 5, -1, 0), cVector2f(150, 20), 0, pLabel);
    SetUpInput(pLabel, mpSVolume, false, kTranslate("OptionsMenu", "MasterVolumeTip"));
    SetUpSlider(mpSVolume, mfVolumeMin, mfVolumeMax, mfVolumeStep, kGuiCallback(SoundSlider_OnMove), &mpLVolume);
    vPos.y += pLabel->GetSize().y + 15;

    // Sounds (effects, ambience, voices, ui)
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "SoundVolume"), apTab);
    mpSSoundVolume = mpGuiSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal, cVector3f(pLabel->GetSize().x + 5, -1, 0), cVector2f(150, 20), 0, pLabel);
    SetUpInput(pLabel, mpSSoundVolume, false, kTranslate("OptionsMenu", "SoundVolumeTip"));
    SetUpSlider(mpSSoundVolume, mfVolumeMin, mfVolumeMax, mfVolumeStep, kGuiCallback(SoundVolumeSlider_OnMove), &mpLSoundVolume);
    vPos.y += pLabel->GetSize().y + 15;

    // Music
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "MusicVolume"), apTab);
    mpSMusicVolume = mpGuiSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal, cVector3f(pLabel->GetSize().x + 5, -1, 0), cVector2f(150, 20), 0, pLabel);
    SetUpInput(pLabel, mpSMusicVolume, false, kTranslate("OptionsMenu", "MusicVolumeTip"));
    SetUpSlider(mpSMusicVolume, mfVolumeMin, mfVolumeMax, mfVolumeStep, kGuiCallback(MusicVolumeSlider_OnMove), &mpLMusicVolume);
    vPos.y += pLabel->GetSize().y + 15;

    /////////////////////////////
    // Extra settings
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "ExtraSoundSettings"), apTab);
    vPos.y += pLabel->GetSize().y + 8;

    vPos.x += 10;
    mpChBEnemyActivateSound = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "EnemyActivateSound"), apTab);
    SetUpInput(NULL, mpChBEnemyActivateSound, false, kTranslate("OptionsMenu", "EnemyActivateSoundTip"));
    vPos.y += mpChBEnemyActivateSound->GetSize().y + 10;

    mpChBInsanityRingSound = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "InsanityRingSound"), apTab);
    SetUpInput(NULL, mpChBInsanityRingSound, false, kTranslate("OptionsMenu", "InsanityRingSoundTip"));
    vPos.y += mpChBInsanityRingSound->GetSize().y + 10;

    mpChBTerrorSound = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "TerrorSound"), apTab);
    SetUpInput(NULL, mpChBTerrorSound, false, kTranslate("OptionsMenu", "TerrorSoundTip"));
    vPos.y += mpChBTerrorSound->GetSize().y + 10;
    vPos.x -= 10;

    mpCBSndDevice->SetFocusNavigation(eUIArrow_Down, mpSVolume);

    mpSVolume->SetFocusNavigation(eUIArrow_Up, mpCBSndDevice);
    mpSVolume->SetFocusNavigation(eUIArrow_Down, mpSSoundVolume);
    mpSSoundVolume->SetFocusNavigation(eUIArrow_Up, mpSVolume);
    mpSSoundVolume->SetFocusNavigation(eUIArrow_Down, mpSMusicVolume);
    mpSMusicVolume->SetFocusNavigation(eUIArrow_Up, mpSSoundVolume);
    mpSMusicVolume->SetFocusNavigation(eUIArrow_Down, mpChBEnemyActivateSound);
    mpChBEnemyActivateSound->SetFocusNavigation(eUIArrow_Up, mpSMusicVolume);
    mpChBEnemyActivateSound->SetFocusNavigation(eUIArrow_Down, mpChBInsanityRingSound);
    mpChBInsanityRingSound->SetFocusNavigation(eUIArrow_Up, mpChBEnemyActivateSound);
    mpChBInsanityRingSound->SetFocusNavigation(eUIArrow_Down, mpChBTerrorSound);
    mpChBTerrorSound->SetFocusNavigation(eUIArrow_Up, mpChBInsanityRingSound);
    mpChBTerrorSound->SetFocusNavigation(eUIArrow_Down, mpBOK);

    apTab->SetUserData(mpChBTerrorSound);
    apTab->GetTabLabel()->SetUserData(mpCBSndDevice);

    PopulateSoundDevices();
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetInputValues(cResourceVarsObject& aObj)
{
    mbSettingInitialValues = true;
    ////////////////////////////////
    // Game options
    {
        // Show hints
        mpChBShowSubtitles->SetChecked(aObj.GetVarBool("ShowSubtitles"), false);
        mpChBShowEffectSubtitles->SetChecked(aObj.GetVarBool("ShowEffectSubtitles"), false);
        mpChBShowHints->SetChecked(aObj.GetVarBool("ShowHints"), false);
        mpChBShowDeathHints->SetChecked(aObj.GetVarBool("ShowDeathHints"), false);

        mpChBShowCrosshair->SetChecked(aObj.GetVarBool("ShowCrosshair"), false);

        mpCBFocusIconStyle->SetSelectedItem(aObj.GetVarInt("FocusIconStyle"), false, false);
        if(mpCBFocusIconStyle->GetSelectedItem() == -1)
        {
            mpCBFocusIconStyle->SetSelectedItem(0, false, true);
        }

        mpCBHandOrientation->SetSelectedItem(aObj.GetVarInt("HandOrientation"), false, false);
        if(mpCBHandOrientation->GetSelectedItem() == -1)
        {
            mpCBHandOrientation->SetSelectedItem(0, false, true);
        }

        if(mbShowCommentary)
        {
            mpChBShowCommentary->SetChecked(aObj.GetVarBool("ShowCommentary"), false);
        }

        float mfFlashIntensity = aObj.GetVarFloat("FlashIntensity");
        SetSliderValue(mpSFlash, mfFlashIntensity, false, mfFlashMin, mfFlashMax);
        SetFlashLabelString(mfFlashIntensity);

        float fFOV = aObj.GetVarFloat("FOV", cLuxPlayer::kfDefaultFOVDegrees);
        SetSliderValue(mpSFOV, fFOV, false, mfFOVMin, mfFOVMax);
        SetFOVLabelString(fFOV);

        // Language
        {
            tString sLang = aObj.GetVarString("Language", gpBase->msDefaultGameLanguage);
            int lLangIndex = 0;
            for(int i = 0; i < (int)mvLangFiles.size(); ++i)
            {
                if(sLang == cString::To8Char(mvLangFiles[i]))
                {
                    lLangIndex = i;
                    break;
                }
            }
            mpCBLanguage->SetSelectedItem(lLangIndex, false, false);
        }
    }
    ////////////////////////////////
    // Graphics options
    {

        /////////////////////////
        // Resolution
        {
            int lCurrentMode = -1;
            bool bGenerateCallback = false;
            tVideoModeVec vVidModes;
            cPlatform::GetAvailableVideoModes(vVidModes, 32);

            cVector2f vCurrentResf = aObj.GetVarVector2f("Resolution");
            cVideoMode vCurrentRes = cVideoMode(cVector2l((int)vCurrentResf.x, (int)vCurrentResf.y), -1, -1);

            /////////////////
            // Remove duplicates
            for(size_t i = 0; i < vVidModes.size(); ++i)
            {
                int lRemove = 0;
                int lRefreshRate = vVidModes[i].mlRefreshRate;

                //////////////
                // Move forward until there are no more matches
                for(size_t j = i + 1; j < vVidModes.size(); ++j)
                {
                    if(vVidModes[i].mvScreenSize == vVidModes[j].mvScreenSize)
                    {
                        lRemove++;
                        lRefreshRate = cMath::Max(lRefreshRate, vVidModes[i].mlRefreshRate);
                    }
                    else
                    {
                        break;
                    }
                }

                //////////////
                // Remove dupilcates and get the one with the highest refreshrate
                for(size_t j = i + lRemove; j < vVidModes.size(); j++)
                {
                    vVidModes[j - lRemove] = vVidModes[j];
                }
                vVidModes[i].mlRefreshRate = lRefreshRate;

                while(lRemove--)
                {
                    vVidModes.pop_back();
                }
            }

            mpCBResolution->ClearItems();
            for(size_t i = 0; i < vVidModes.size(); ++i)
            {
                const cVideoMode& mode = vVidModes[i];
                tWString sRes;

                sRes = cString::ToStringW(mode.mvScreenSize.x) + _W("x") + cString::ToStringW(mode.mvScreenSize.y);

                mpCBResolution->AddItem(sRes);
                mvScreenSizes.push_back(mode);

                if(mode == vCurrentRes)
                {
                    lCurrentMode = mpCBResolution->GetItemNum() - 1;
                }
            }
            if(lCurrentMode == -1)
            {
                tWString sRes = cString::ToStringW(vCurrentRes.mvScreenSize.x) + _W("x") + cString::ToStringW(vCurrentRes.mvScreenSize.y) + _W(" (Custom)");

                mpCBResolution->AddItem(sRes);
                mvScreenSizes.push_back(vCurrentRes);
                lCurrentMode = mpCBResolution->GetItemNum() - 1;

                //lCurrentMode=0;
                //bGenerateCallback = true;
            }
            mpCBResolution->SetSelectedItem(lCurrentMode, true, false);
        }

        /////////////////////////
        // Fullscreen & vsync
        mpChBFullScreen->SetChecked(aObj.GetVarBool("FullScreen"), false);
        mpChBVSync->SetChecked(aObj.GetVarBool("VSync"), false);
        mpChBPotatoMode->SetChecked(aObj.GetVarBool("PotatoMode"), false);

        /////////////////////////
        // Texture quality and filtering
        {
            /////////////////////////////////
            // Texture Quality
            mpCBTextureSizeLevel->SetSelectedItem((mpCBTextureSizeLevel->GetItemNum() - 1) - aObj.GetVarInt("TextureQuality"), true, false);

            /////////////////////////////////
            // Texture filtering
            tWStringVec vTexFilterStrings;
            vTexFilterStrings.push_back(kTranslate("OptionsMenu", "Nearest"));
            vTexFilterStrings.push_back(kTranslate("OptionsMenu", "Bilinear"));
            vTexFilterStrings.push_back(kTranslate("OptionsMenu", "Trilinear"));

            mpCBTextureFilter->ClearItems();
            for(int i = 0; i < (int)vTexFilterStrings.size(); ++i)
            {
                mpCBTextureFilter->AddItem(vTexFilterStrings[i]);
            }

            mpCBTextureFilter->SetSelectedItem(aObj.GetVarInt("TextureFilter"), true, false);

            /////////////////////////////////
            // Anisotropy
            mpCBAnisotropy->ClearItems();
            mpCBAnisotropy->AddItem(kTranslate("OptionsMenu", "Off"));
            int lAnisoPow = 2;
            for(int i = 0; i < 4; ++i)
            {
                cWidgetItem *pItem = mpCBAnisotropy->AddItem(cString::ToStringW(lAnisoPow) + _W("X"));
                lAnisoPow *= 2;
            }
            mpCBAnisotropy->SetSelectedItem(GetIndexFromAnisotropy(aObj.GetVarFloat("TextureAnisotropy")));
        }

        /////////////////////////
        // Shadows & Parallax
        {
            mpChBShadows->SetChecked(aObj.GetVarBool("ShadowsActive"), false);

            tWStringVec vOptionStrings;
            vOptionStrings.push_back(kTranslate("OptionsMenu", "Low"));
            vOptionStrings.push_back(kTranslate("OptionsMenu", "Medium"));
            vOptionStrings.push_back(kTranslate("OptionsMenu", "High"));
            vOptionStrings.push_back(kTranslate("OptionsMenu", "Ultra"));

            mpCBShadowQuality->ClearItems();
            mpCBShadowRes->ClearItems();
            mpCBParallaxQuality->ClearItems();
            for(int i = 0; i < (int)vOptionStrings.size(); ++i)
            {
                mpCBShadowQuality->AddItem(vOptionStrings[i]);
                mpCBShadowRes->AddItem(vOptionStrings[i]);
            }

            mpCBShadowQuality->SetSelectedItem(aObj.GetVarInt("ShadowQuality"), true, false);
            mpCBShadowRes->SetSelectedItem(aObj.GetVarInt("ShadowResolution"), true, false);

            mpChBParallax->SetChecked(aObj.GetVarBool("ParallaxEnabled"), false);

            mpCBParallaxQuality->AddItem(kTranslate("OptionsMenu", "ParallaxSimple"));
            mpCBParallaxQuality->AddItem(kTranslate("OptionsMenu", "ParallaxRelief"));

            mpCBParallaxQuality->SetSelectedItem(aObj.GetVarInt("ParallaxQuality"), true, false);
        }

        /////////////////////////
        // Water
        {
            mpChBWorldReflection->SetChecked(aObj.GetVarBool("WorldReflection"), false);
            mpChBRefraction->SetChecked(aObj.GetVarBool("Refraction"), false);
        }

        /////////////////////////
        // Misc
        {
            mpChBShaderWarming->SetChecked(aObj.GetVarBool("ShaderWarming"), false);
            mpCBMaxFramesAhead->SetSelectedItem(GetIndexFromMaxFramesAhead(aObj.GetVarInt("MaxFramesAhead", -1)), true, false);
            mpCBLightColorGrade->SetSelectedItem(aObj.GetVarInt("LightColorGradeMode", 2), true, false);
        }

        /////////////////////////
        // SSAO
        {

            /////////////////////////////
            // Active
            mpChBSSAO->SetChecked(aObj.GetVarBool("SSAOActive"), false);

            /////////////////////////////
            // Samples
            int lCurrentSamples = aObj.GetVarInt("SSAONumOfSamples");
            mpCBSSAOSamples->SetSelectedItem(GetIndexFromSSAOSamples(lCurrentSamples), true, false);
            /*
            for(int i=0;i<(int)mpCBSSAOSamples->GetItemNum();++i)
            {
                cWidgetItem* pItem = mpCBSSAOSamples->GetItem(i);
                int lSamples = cString::ToInt(cString::To8Char(pItem->GetText()).c_str(), 0);
                if(lCurrentSamples == lSamples)
                {
                    lCurrentSamplesIdx = i;
                    break;
                }
            }
            */

            /////////////////////////////
            // Resolution
            mpCBSSAOResolution->SetSelectedItem(aObj.GetVarInt("SSAOResolution"));
        }

        /////////////////
        // PostEffects
        {
            cLuxMapHandler* pMapHdlr = gpBase->mpMapHandler;
            cLuxPostEffectHandler *pPostEffects = gpBase->mpPostEffectHandler;
            // Bloom
            mpChBBloom->SetChecked(aObj.GetVarBool("BloomActive"), false);
            // Motion blur
            mpChBMotionBlur->SetChecked(aObj.GetVarBool("MotionBlurActive"), false);
            // Sepia
            mpChBSepia->SetChecked(aObj.GetVarBool("SepiaActive"), false);
            // RadialBlur
            mpChBRadialBlur->SetChecked(aObj.GetVarBool("RadialBlurActive"), false);
            //Insanity
            mpChBInsanity->SetChecked(aObj.GetVarBool("InsanityActive"), false);
        }

        // Gamma
        float fGamma = aObj.GetVarFloat("Gamma");
        SetSliderValue(mpSGamma, fGamma, false, mfGammaMin, mfGammaMax);
        SetGammaLabelString(fGamma);
    }
    ////////////////////////////////
    // Input
    mpChBInvertMouse->SetChecked(aObj.GetVarBool("InvertMouse"), false);
    mpChBSmoothMouse->SetChecked(aObj.GetVarBool("SmoothMouse"), false);

    float fSensitivity = aObj.GetVarFloat("MouseSensitivity");
    SetSliderValue(mpSMouseSensitivity, fSensitivity, false, mfMouseSensitivityMin, mfMouseSensitivityMax);
    SetSensitivityLabelString(fSensitivity);

#if USE_GAMEPAD
    mpChBGamepadInvertLook->SetChecked(aObj.GetVarBool("GamepadInvertLook"), false);

    fSensitivity = aObj.GetVarFloat("GamepadLookSensitivity");
    SetSliderValue(mpSGamepadLookSensitivity, fSensitivity, false, mfGamepadLookSensitivityMin, mfGamepadLookSensitivityMax);
    SetGamepadLookSensitivityLabelString(fSensitivity);
#endif

    ////////////////////////////////
    // Sound
    float fVolume = aObj.GetVarFloat("SoundVolume");
    SetSliderValue(mpSVolume, fVolume, false, mfVolumeMin, mfVolumeMax);
    SetVolumeLabelString(fVolume);

    float fEffectsVolume = aObj.GetVarFloat("EffectsVolume", 1.0f);
    SetSliderValue(mpSSoundVolume, fEffectsVolume, false, mfVolumeMin, mfVolumeMax);
    SetVolumeLabelString(mpLSoundVolume, fEffectsVolume);

    float fMusicVolume = aObj.GetVarFloat("MusicVolume", 1.0f);
    SetSliderValue(mpSMusicVolume, fMusicVolume, false, mfVolumeMin, mfVolumeMax);
    SetVolumeLabelString(mpLMusicVolume, fMusicVolume);

    mpChBEnemyActivateSound->SetChecked(aObj.GetVarBool("EnemyActivateSound", true), false);
    mpChBInsanityRingSound->SetChecked(aObj.GetVarBool("InsanityRingSound", true), false);
    mpChBTerrorSound->SetChecked(aObj.GetVarBool("TerrorSound", true), false);

    int lSndDev = aObj.GetVarInt("SoundDeviceID");
    int lSndDevIdx = -1;
    for(size_t i = 0; i < mvSoundDevices.size(); ++i)
    {
        if(lSndDev == mvSoundDevices[i]->GetID())
        {
            lSndDevIdx = (int)i;
            break;
        }
    }

    if(lSndDevIdx == -1)
    {
        mpCBSndDevice->SetSelectedItem(mpCBSndDevice->GetItemNum() - 1, true, false);
    }
    else
    {
        mpCBSndDevice->SetSelectedItem(lSndDevIdx, true, false);
    }

    mbSettingInitialValues = false;
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetUpInput(cWidgetLabel* apLabel, iWidget* apInput, bool abNeedsRestart, const tWString& asMessage)
{
    cLuxOption_ExtData* pData = AddOptionData(abNeedsRestart, asMessage);
    if(apLabel)
    {
        apLabel->SetUserData(pData);
        apLabel->AddCallback(eGuiMessage_MouseMove, this, kGuiCallback(Option_OnMouseOver));
        apLabel->AddCallback(eGuiMessage_GetUINavFocus, this, kGuiCallback(Option_OnMouseOver));
    }
    apInput->SetUserData(pData);
    apInput->AddCallback(eGuiMessage_MouseMove, this, kGuiCallback(Option_OnMouseOver));
    apInput->AddCallback(eGuiMessage_GetUINavFocus, this, kGuiCallback(Option_OnMouseOver));

    eGuiMessage message = eGuiMessage_LastEnum;
    switch(apInput->GetType())
    {
    case eWidgetType_CheckBox:
        message = eGuiMessage_CheckChange;
        break;
    case eWidgetType_ComboBox:
        message = eGuiMessage_SelectionChange;
        break;
    case eWidgetType_Slider:
        message = eGuiMessage_SliderMove;
        break;
    case eWidgetType_Button:
        message = eGuiMessage_ButtonPressed;
        break;
    }
    if(message != eGuiMessage_LastEnum)
    {
        apInput->AddCallback(message, this, kGuiCallback(Option_OnChangeValue));
    }
}

//-----------------------------------------------------------------------

cLuxOption_ExtData *cLuxMainMenu_Options::AddOptionData(bool abNeedsRestart, const tWString& asMessage)
{
    mvOptionData.push_back(hplNew(cLuxOption_ExtData, (abNeedsRestart, asMessage)));
    return mvOptionData.back();
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetCurrentTipWidget(iWidget* apWidget)
{
    if(mpCurrentTipWidget == apWidget)
    {
        mbTipFadeRestart = true;
        return;
    }

    mbTipWidgetUpdated = true;
    mpCurrentTipWidget = apWidget;
}

//-----------------------------------------------------------------------


void cLuxMainMenu_Options::ApplyChanges()
{
    // Reset initial values on next enter
    mInitialValues.AddVarBool("InitialValuesSet", false);

    cLuxConfigHandler* pCfgHdr = gpBase->mpConfigHandler;
    ///////////////////////////
    // Game
    {
        pCfgHdr->msLangFile = cString::To8Char(mvLangFiles[mpCBLanguage->GetSelectedItem()]);
        gpBase->mpMessageHandler->SetShowSubtitles(mpChBShowSubtitles->IsChecked());
        gpBase->mpMessageHandler->SetShowEffectSubtitles(mpChBShowEffectSubtitles->IsChecked());
        gpBase->mpHintHandler->SetActive(mpChBShowHints->IsChecked());
        gpBase->mpPlayer->GetHelperDeath()->SetShowHint(mpChBShowDeathHints->IsChecked());

        if(mbShowCommentary)
        {
            gpBase->mpMapHandler->SetShowCommentary(mpChBShowCommentary->IsChecked());
        }

        gpBase->mpPlayer->SetShowCrosshair(mpChBShowCrosshair->IsChecked());

        gpBase->mpPlayer->SetFocusIconStyle((eLuxFocusIconStyle)mpCBFocusIconStyle->GetSelectedItem());

        gpBase->mpPlayer->SetHandOrientation((eLuxHandOrientation)mpCBHandOrientation->GetSelectedItem());

        gpBase->mpEffectHandler->SetFlashIntensity(GetFlash());

        gpBase->mpPlayer->SetFOVDegrees(GetFOV());
    }


    ///////////////////////////
    // Graphics
    {
        cGraphics* pGfx = gpBase->mpEngine->GetGraphics();
        cRenderSettings* pRenderSettings = gpBase->mpMapHandler->GetViewport()->GetRenderSettings();
        cMaterialManager* pMatMgr = gpBase->mpEngine->GetResources()->GetMaterialManager();

        const cVideoMode vidMode = mvScreenSizes[mpCBResolution->GetSelectedItem()];
        pCfgHdr->mvScreenSize = vidMode.mvScreenSize;
        pCfgHdr->mbFullscreen = mpChBFullScreen->IsChecked();
        pCfgHdr->mbVSync = mpChBVSync->IsChecked();

        // Potato mode: fullbright G-buffer mode from the next start. The mode it replaces is kept
        // in the config, so turning it off goes back to it (normal or hardware depth).
        {
            eDeferredGBufferMode currentMode = cRendererDeferred::GetGBufferMode();
            bool bPotato = mpChBPotatoMode->IsChecked();
            if(bPotato && currentMode != eDeferredGBufferMode_Fullbright)
            {
                gpBase->mpMainConfig->SetInt("GraphicsAdvanced", "GBufferModeBeforePotato", currentMode);
                cRendererDeferred::SetGBufferMode(eDeferredGBufferMode_Fullbright);
            }
            else if(bPotato == false && currentMode == eDeferredGBufferMode_Fullbright)
            {
                int lPrevMode = gpBase->mpMainConfig->GetInt("GraphicsAdvanced", "GBufferModeBeforePotato", eDeferredGBufferMode_Normal);
                if(lPrevMode < 0 || lPrevMode >= eDeferredGBufferMode_LastEnum || lPrevMode == eDeferredGBufferMode_Fullbright)
                {
                    lPrevMode = eDeferredGBufferMode_Normal;
                }
                cRendererDeferred::SetGBufferMode((eDeferredGBufferMode)lPrevMode);
            }
        }
        pGfx->GetLowLevel()->SetVsyncActive(pCfgHdr->mbVSync);
        pGfx->GetLowLevel()->SetGammaCorrection(GetGamma());

        // Texture
        pCfgHdr->mlTextureQuality = (mpCBTextureSizeLevel->GetItemNum() - 1) - mpCBTextureSizeLevel->GetSelectedItem();
        pCfgHdr->mlTextureFilter = mpCBTextureFilter->GetSelectedItem();
        pCfgHdr->mfTextureAnisotropy = GetAnisotropyFromIndex(mpCBAnisotropy->GetSelectedItem());

        pMatMgr->SetTextureAnisotropy(pCfgHdr->mfTextureAnisotropy);
        pMatMgr->SetTextureFilter((eTextureFilter)pCfgHdr->mlTextureFilter);

        // Parallax
        pCfgHdr->mbParallaxEnabled = mpChBParallax->IsChecked();
        pCfgHdr->mlParallaxQuality = mpCBParallaxQuality->GetSelectedItem();

        // Shadows
        pCfgHdr->mbShadowsActive = mpChBShadows->IsChecked();
        pCfgHdr->mlShadowQuality = mpCBShadowQuality->GetSelectedItem();
        pCfgHdr->mlShadowRes = mpCBShadowRes->GetSelectedItem();

        // Water
        pCfgHdr->mbWorldReflection = mpChBWorldReflection->IsChecked();
        pCfgHdr->mbRefraction = mpChBRefraction->IsChecked();

        // Misc
        pCfgHdr->mbShaderWarming = mpChBShaderWarming->IsChecked();
        pCfgHdr->mlMaxFramesAhead = GetMaxFramesAheadForSelection(mpCBMaxFramesAhead->GetSelectedItem(), pCfgHdr->mlMaxFramesAhead);
        iLowLevelGraphics::SetMaxFramesAhead(pCfgHdr->mlMaxFramesAhead);
        //Used from the next start (mode 2 is built into the light shaders)
        pCfgHdr->mlLightColorGradeMode = mpCBLightColorGrade->GetSelectedItem();

        //Update the viewport stuff
        gpBase->mpMapHandler->UpdateViewportRenderProperties();

        /////////////////////
        // PostEffects (hidden and forced off in fullbright mode)
        if(mbFullbright == false)
        {
            cLuxMapHandler* pMapHdlr = gpBase->mpMapHandler;
            cLuxPostEffectHandler *pPostEffects = gpBase->mpPostEffectHandler;

            // Bloom
            pMapHdlr->GetPostEffect_Bloom()->SetDisabled(mpChBBloom->IsChecked() == false);
            // Motion blur
            pMapHdlr->GetPostEffect_MotionBlur()->SetDisabled(mpChBMotionBlur->IsChecked() == false);
            // Sepia
            pMapHdlr->GetPostEffect_Sepia()->SetDisabled(mpChBSepia->IsChecked() == false);
            // RadialBlur
            pMapHdlr->GetPostEffect_RadialBlur()->SetDisabled(mpChBRadialBlur->IsChecked() == false);
            //Insanity
            pPostEffects->GetInsanity()->SetDisabled(mpChBInsanity->IsChecked() == false);
        }

        // SSAO
        pCfgHdr->mbSSAOActive = mpChBSSAO->IsChecked();
        pCfgHdr->mlSSAOSamples = GetSSAOSamplesFromIndex(mpCBSSAOSamples->GetSelectedItem());
        pCfgHdr->mlSSAOResolution = mpCBSSAOResolution->GetSelectedItem();
    }


    //////////////////////////////
    // Input
    gpBase->mpInputHandler->SetInvertMouse(mpChBInvertMouse->IsChecked());
    gpBase->mpInputHandler->SetSmoothMouse(mpChBSmoothMouse->IsChecked());

    gpBase->mpInputHandler->SetMouseSensitivity(GetSensitivity());

#if USE_GAMEPAD
    gpBase->mpInputHandler->SetGamepadLookSensitivity(GetGamepadLookSensitivity());
    gpBase->mpInputHandler->SetInvertGamepadLook(mpChBGamepadInvertLook->IsChecked());
#endif

    //////////////////////////////
    // Extra sound settings
    gpBase->mpGlobalDataHandler->SetEnemyActivateSoundEnabled(mpChBEnemyActivateSound->IsChecked());
    gpBase->mpGlobalDataHandler->SetInsanityRingSoundEnabled(mpChBInsanityRingSound->IsChecked());
    gpBase->mpGlobalDataHandler->SetTerrorSoundEnabled(mpChBTerrorSound->IsChecked());
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetFlashLabelString(float afX)
{
    SetSliderLabelString(mpLFlash, afX, mfFlashMin, mfFlashMax);
}

void cLuxMainMenu_Options::SetFOVLabelString(float afX)
{
    SetSliderLabelString(mpLFOV, (float)cMath::RoundToInt(afX), mfFOVMin, mfFOVMax);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetGammaLabelString(float afX)
{
    SetSliderLabelString(mpLGamma, afX, mfGammaMin, mfGammaMax);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetSensitivityLabelString(float afX)
{
    SetSliderLabelString(mpLMouseSensitivity, afX, mfMouseSensitivityMin, mfMouseSensitivityMax);
}

//-----------------------------------------------------------------------

#if USE_GAMEPAD
void cLuxMainMenu_Options::SetGamepadLookSensitivityLabelString(float afX)
{
    SetSliderLabelString(mpLGamepadLookSensitivity, afX, mfGamepadLookSensitivityMin, mfGamepadLookSensitivityMax);
}
#endif

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetVolumeLabelString(cWidgetLabel *apLabel, float afX)
{
    SetSliderLabelString(apLabel, afX * 10.0f, mfVolumeMin * 10.0f, mfVolumeMax * 10.0f);
}

void cLuxMainMenu_Options::SetVolumeLabelString(float afX)
{
    SetSliderLabelString(mpLVolume, afX * 10.0f, mfVolumeMin * 10.0f, mfVolumeMax * 10.0f);
}

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::ChangeLanguage(iWidget* apWidget, const cGuiMessageData& aData)
{
    ///////////////////////////////////////////////////////
    // --- REMOVED - Language is now loaded only at startup

    //mInitialValues.AddVarBool("SettingLanguage", true);

    //DumpCurrentValues(mCurrentValues);

    //gpBase->LoadLanguage(cString::To8Char(mvLangFiles[mpCBLanguage->GetSelectedItem()]));
    //gpBase->mpMainMenu->RecreateGui();

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, ChangeLanguage);

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::PopulateLanguageList()
{
    mpCBLanguage->ClearItems();
    mvLangFiles.clear();

    tWStringList lstLangs;
    tWString sPath = cString::To16Char(gpBase->msGameLanguageFolder);
    cPlatform::FindFilesInDir(lstLangs, sPath, _W("*.lang"));

    tWStringListIt it = lstLangs.begin();
    for(; it != lstLangs.end(); ++it)
    {
        tWString sLang = *it;

        tWString sLangEntry = cString::ToLowerCaseW(cString::SubW(sLang, 0,
                              cString::GetLastStringPosW(sLang, _W("."))));

        mpCBLanguage->AddItem(kTranslate("Languages", cString::To8Char(sLangEntry)));
        mvLangFiles.push_back(sLang);
    }
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::PopulateSoundDevices()
{
    iLowLevelSound* pLowLevelSound = gpBase->mpEngine->GetSound()->GetLowLevel();
    iSoundDeviceIdentifier* pCurSndDev = pLowLevelSound->GetCurrentSoundDevice();

    mpCBSndDevice->ClearItems();
    mvSoundDevices.clear();

    mvSoundDevices = pLowLevelSound->GetFilteredSoundDevices();
    if(mvSoundDevices.empty())
    {
        cWidgetItem* pItem = mpCBSndDevice->AddItem(pCurSndDev->GetName());
        pItem->SetUserData(pCurSndDev);
    }
    else
    {
        bool bCurrentDevFound = false;
        for(int i = 0; i < (int)mvSoundDevices.size(); ++i)
        {
            iSoundDeviceIdentifier* pSndDev = mvSoundDevices[i];
            if(bCurrentDevFound == false && pSndDev == pCurSndDev)
            {
                bCurrentDevFound = true;
            }

            cWidgetItem* pItem = mpCBSndDevice->AddItem(pSndDev->GetName());
            pItem->SetUserData(pSndDev);
        }
        if(bCurrentDevFound == false)
        {
            cWidgetItem* pItem = mpCBSndDevice->AddItem("(Unsupported) " + pCurSndDev->GetName());
            pItem->SetUserData(pCurSndDev);
        }
    }
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetUpSlider(cWidgetSlider* apSlider, float afMinValue, float afMaxValue, float afStepValue, tGuiCallbackFunc apCallback, cWidgetLabel** apValueDisplay)
{
    int lMaxValue = cMath::RoundToInt((afMaxValue - afMinValue) / afStepValue);
    apSlider->SetMaxValue(lMaxValue);
    apSlider->SetBarValueSize(cMath::RoundToInt(0.25f * (float)lMaxValue));

    if(apCallback)
    {
        apSlider->AddCallback(eGuiMessage_SliderMove, this, apCallback);
    }

    if(apValueDisplay)
    {
        cWidgetLabel* pLabel = mpGuiSet->CreateWidgetLabel(cVector3f(apSlider->GetSize().x * 0.5f, 2, 1), -1, _W(""), apSlider);
        pLabel->SetTextAlign(eFontAlign_Center);

        *apValueDisplay = pLabel;
    }
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetSliderValue(cWidgetSlider* apSlider, float afValue, bool abGenCallback, float afMinValue, float afMaxValue)
{
    afValue = cMath::Clamp(afValue, afMinValue, afMaxValue);

    float fMaxSliderValue = (float) apSlider->GetMaxValue();
    float fRange = afMaxValue - afMinValue;

    int lValue = cMath::RoundToInt((afValue - afMinValue) * fMaxSliderValue / fRange);

    apSlider->SetValue(lValue, abGenCallback);
}

//-----------------------------------------------------------------------

float cLuxMainMenu_Options::GetSliderValue(cWidgetSlider* apSlider, float afMinValue, float afMaxValue)
{
    float fSliderRelValue = ((float)apSlider->GetValue()) / (float)apSlider->GetMaxValue();
    float fRange = afMaxValue - afMinValue;

    return afMinValue + fRange * fSliderRelValue;
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetSliderLabelString(cWidgetLabel* apLabel, float afValue,
        float afMinValue, float afMaxValue,
        const tWString& asMin, const tWString& asMax)
{
    tWString sText;

    if(afValue <= afMinValue)
    {
        if(asMin.empty())
        {
            afValue = afMinValue;
        }
        else
        {
            sText = asMin;
        }
    }
    else if(afValue >= afMaxValue)
    {
        if(asMax.empty())
        {
            afValue = afMaxValue;
        }
        else
        {
            sText = asMax;
        }
    }

    if(sText.empty())
    {
        sText = cString::ToStringW(afValue, 3, true);
    }

    apLabel->SetText(sText);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::DumpInitialValues(cResourceVarsObject &aObj)
{
    ////////////////////////////////
    // Game options
    {
        // Show hints
        aObj.AddVarBool("ShowHints", gpBase->mpHintHandler->IsActive());
        aObj.AddVarBool("ShowDeathHints", gpBase->mpPlayer->GetHelperDeath()->ShowHint());
        aObj.AddVarBool("ShowSubtitles", gpBase->mpMessageHandler->ShowSubtitles());
        aObj.AddVarBool("ShowEffectSubtitles", gpBase->mpMessageHandler->ShowEffectSubtitles());

        aObj.AddVarBool("ShowCrosshair", gpBase->mpPlayer->GetShowCrosshair());
        aObj.AddVarInt("FocusIconStyle", gpBase->mpPlayer->GetFocusIconStyle());
        aObj.AddVarInt("HandOrientation", gpBase->mpPlayer->GetHandOrientation());
        aObj.AddVarBool("ShowCommentary", gpBase->mpMapHandler->GetShowCommentary());
        aObj.AddVarFloat("FlashIntensity", gpBase->mpEffectHandler->GetFlashIntensity());
        aObj.AddVarFloat("FOV", (float)cMath::RoundToInt(gpBase->mpPlayer->GetFOVDegrees()));

        // Language
        aObj.AddVarString("Language", gpBase->mpConfigHandler->msLangFile);
    }
    ////////////////////////////////
    // Graphics options
    {
        /////////////////////////
        // Resolution
        const cVector2l& vResolution = gpBase->mpConfigHandler->mvScreenSize;
        cVector2f vResolutionf = cVector2f((float)vResolution.x, (float)vResolution.y);
        aObj.AddVarVector2f("Resolution", vResolutionf);

        /////////////////////////
        // Fullscreen & vsync
        aObj.AddVarBool("FullScreen", gpBase->mpConfigHandler->mbFullscreen);
        aObj.AddVarBool("VSync", gpBase->mpConfigHandler->mbVSync);
        //The mode for the next start (can differ from the running one after applying)
        aObj.AddVarBool("PotatoMode", cRendererDeferred::GetGBufferMode() == eDeferredGBufferMode_Fullbright);

        /////////////////////////
        // Texture quality and filtering
        aObj.AddVarInt("TextureQuality", gpBase->mpConfigHandler->mlTextureQuality);
        aObj.AddVarInt("TextureFilter", gpBase->mpConfigHandler->mlTextureFilter);
        aObj.AddVarFloat("TextureAnisotropy", gpBase->mpConfigHandler->mfTextureAnisotropy);

        /////////////////////////
        // Shadows & Parallax
        aObj.AddVarBool("ShadowsActive", gpBase->mpConfigHandler->mbShadowsActive);
        aObj.AddVarInt("ShadowQuality", gpBase->mpConfigHandler->mlShadowQuality);
        aObj.AddVarInt("ShadowResolution", gpBase->mpConfigHandler->mlShadowRes);
        aObj.AddVarBool("ParallaxEnabled", gpBase->mpConfigHandler->mbParallaxEnabled);
        aObj.AddVarInt("ParallaxQuality", gpBase->mpConfigHandler->mlParallaxQuality);

        /////////////////////////
        // Water
        aObj.AddVarBool("WorldReflection", gpBase->mpConfigHandler->mbWorldReflection);
        aObj.AddVarBool("Refraction", gpBase->mpConfigHandler->mbRefraction);

        /////////////////////////
        // Misc
        aObj.AddVarBool("ShaderWarming", gpBase->mpConfigHandler->mbShaderWarming);
        aObj.AddVarInt("MaxFramesAhead", gpBase->mpConfigHandler->mlMaxFramesAhead);
        aObj.AddVarInt("LightColorGradeMode", gpBase->mpConfigHandler->mlLightColorGradeMode);

        /////////////////////////
        // SSAO
        aObj.AddVarBool("SSAOActive", gpBase->mpConfigHandler->mbSSAOActive);
        aObj.AddVarInt("SSAONumOfSamples", gpBase->mpConfigHandler->mlSSAOSamples);
        aObj.AddVarInt("SSAOResolution", gpBase->mpConfigHandler->mlSSAOResolution);

        /////////////////
        // PostEffects
        cLuxMapHandler* pMapHdlr = gpBase->mpMapHandler;
        cLuxPostEffectHandler *pPostEffects = gpBase->mpPostEffectHandler;
        aObj.AddVarBool("BloomActive", pMapHdlr->GetPostEffect_Bloom()->IsDisabled() == false);
        aObj.AddVarBool("MotionBlurActive", pMapHdlr->GetPostEffect_MotionBlur()->IsDisabled() == false);
        aObj.AddVarBool("SepiaActive", pMapHdlr->GetPostEffect_Sepia()->IsDisabled() == false);
        aObj.AddVarBool("RadialBlurActive", pMapHdlr->GetPostEffect_RadialBlur()->IsDisabled() == false);
        aObj.AddVarBool("InsanityActive", pPostEffects->GetInsanity()->IsDisabled() == false);


        ///////////////////
        // Gamma
        aObj.AddVarFloat("Gamma", gpBase->mpEngine->GetGraphics()->GetLowLevel()->GetGammaCorrection());
    }

    ////////////////////////////////
    // Input
    aObj.AddVarBool("InvertMouse", gpBase->mpInputHandler->GetInvertMouse());
    aObj.AddVarBool("SmoothMouse", gpBase->mpInputHandler->GetSmoothMouse());
    aObj.AddVarFloat("MouseSensitivity", gpBase->mpInputHandler->GetMouseSensitivity());

#if USE_GAMEPAD
    aObj.AddVarBool("GamepadInvertLook", gpBase->mpInputHandler->GetInvertGamepadLook());
    aObj.AddVarFloat("GamepadLookSensitivity", gpBase->mpInputHandler->GetGamepadLookSensitivity());
#endif

    ////////////////////////////////
    // Sound
    aObj.AddVarFloat("SoundVolume", gpBase->mpEngine->GetSound()->GetLowLevel()->GetVolume());
    aObj.AddVarFloat("EffectsVolume", gpBase->mpEngine->GetSound()->GetSoundHandler()->GetUserVolume());
    aObj.AddVarFloat("MusicVolume", gpBase->mpEngine->GetSound()->GetMusicHandler()->GetUserVolume());
    aObj.AddVarInt("SoundDeviceID", gpBase->mpConfigHandler->mlSoundDevID);
    aObj.AddVarBool("EnemyActivateSound", gpBase->mpGlobalDataHandler->GetEnemyActivateSoundEnabled());
    aObj.AddVarBool("InsanityRingSound", gpBase->mpGlobalDataHandler->GetInsanityRingSoundEnabled());
    aObj.AddVarBool("TerrorSound", gpBase->mpGlobalDataHandler->GetTerrorSoundEnabled());
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::DumpCurrentValues(cResourceVarsObject &aObj)
{
    ////////////////////////////////
    // Game options
    {
        aObj.AddVarBool("ShowHints",        mpChBShowHints->IsChecked());
        aObj.AddVarBool("ShowDeathHints",    mpChBShowDeathHints->IsChecked());
        aObj.AddVarBool("ShowSubtitles",    mpChBShowSubtitles->IsChecked());
        aObj.AddVarBool("ShowEffectSubtitles", mpChBShowEffectSubtitles->IsChecked());

        aObj.AddVarBool("ShowCrosshair",    mpChBShowCrosshair->IsChecked());
        aObj.AddVarInt("FocusIconStyle",    mpCBFocusIconStyle->GetSelectedItem());
        aObj.AddVarInt("HandOrientation",    mpCBHandOrientation->GetSelectedItem());
        aObj.AddVarBool("ShowCommentary",   mpChBShowCommentary->IsChecked());
        aObj.AddVarFloat("FlashIntensity",  GetFlash());
        aObj.AddVarFloat("FOV",             GetFOV());

        aObj.AddVarString("Language",        cString::To8Char(mvLangFiles[mpCBLanguage->GetSelectedItem()]));
    }

    ////////////////////////////////
    // Graphics options
    {
        /////////////////////////
        // Resolution
        const cVideoMode& vResolution = mvScreenSizes[mpCBResolution->GetSelectedItem()];
        cVector2f vResolutionf = cVector2f((float)vResolution.mvScreenSize.x, (float)vResolution.mvScreenSize.y);
        aObj.AddVarVector2f("Resolution", vResolutionf);

        /////////////////////////
        // Fullscreen & vsync
        aObj.AddVarBool("FullScreen",    mpChBFullScreen->IsChecked());
        aObj.AddVarBool("VSync",        mpChBVSync->IsChecked());
        aObj.AddVarBool("PotatoMode",   mpChBPotatoMode->IsChecked());

        /////////////////////////
        // Texture quality and filtering
        aObj.AddVarInt("TextureQuality", (mpCBTextureSizeLevel->GetItemNum() - 1) - mpCBTextureSizeLevel->GetSelectedItem());
        aObj.AddVarInt("TextureFilter", mpCBTextureFilter->GetSelectedItem());
        aObj.AddVarFloat("TextureAnisotropy", GetAnisotropyFromIndex(mpCBAnisotropy->GetSelectedItem()));

        /////////////////////////
        // Shadows & Parallax
        aObj.AddVarBool("ShadowsActive", mpChBShadows->IsChecked());
        aObj.AddVarInt("ShadowQuality",  mpCBShadowQuality->GetSelectedItem());
        aObj.AddVarInt("ShadowResolution", mpCBShadowRes->GetSelectedItem());

        aObj.AddVarBool("ParallaxEnabled", mpChBParallax->IsChecked());
        aObj.AddVarInt("ParallaxQuality",  mpCBParallaxQuality->GetSelectedItem());

        /////////////////////////
        // Water
        aObj.AddVarBool("WorldReflection", mpChBWorldReflection->IsChecked());
        aObj.AddVarBool("Refraction", mpChBRefraction->IsChecked());

        /////////////////////////
        // Misc
        aObj.AddVarBool("ShaderWarming", mpChBShaderWarming->IsChecked());
        aObj.AddVarInt("MaxFramesAhead", GetMaxFramesAheadForSelection(mpCBMaxFramesAhead->GetSelectedItem(), gpBase->mpConfigHandler->mlMaxFramesAhead));
        aObj.AddVarInt("LightColorGradeMode", mpCBLightColorGrade->GetSelectedItem());

        /////////////////////////
        // SSAO
        aObj.AddVarBool("SSAOActive", mpChBSSAO->IsChecked());
        aObj.AddVarInt("SSAONumOfSamples", GetSSAOSamplesFromIndex(mpCBSSAOSamples->GetSelectedItem()));
        aObj.AddVarInt("SSAOResolution", mpCBSSAOResolution->GetSelectedItem());

        /////////////////
        // PostEffects
        aObj.AddVarBool("BloomActive", mpChBBloom->IsChecked());
        aObj.AddVarBool("MotionBlurActive", mpChBMotionBlur->IsChecked());
        aObj.AddVarBool("SepiaActive", mpChBSepia->IsChecked());
        aObj.AddVarBool("RadialBlurActive", mpChBRadialBlur->IsChecked());
        aObj.AddVarBool("InsanityActive", mpChBInsanity->IsChecked());

        ///////////////////
        // Gamma
        aObj.AddVarFloat("Gamma", GetGamma());
    }

    ////////////////////////////////
    // Input
    aObj.AddVarBool("InvertMouse", mpChBInvertMouse->IsChecked());
    aObj.AddVarBool("SmoothMouse", mpChBSmoothMouse->IsChecked());
    aObj.AddVarFloat("MouseSensitivity", GetSensitivity());

#if USE_GAMEPAD
    aObj.AddVarBool("GamepadInvertLook", mpChBGamepadInvertLook->IsChecked());
    aObj.AddVarFloat("GamepadLookSensitivity", GetGamepadLookSensitivity());
#endif

    ////////////////////////////////
    // Sound
    aObj.AddVarFloat("SoundVolume", GetVolume());
    aObj.AddVarFloat("EffectsVolume", GetSoundVolume());
    aObj.AddVarFloat("MusicVolume", GetMusicVolume());
    cWidgetItem* pItem = mpCBSndDevice->GetItem(mpCBSndDevice->GetSelectedItem());
    int lDevID = -1;
    if(pItem)
    {
        iSoundDeviceIdentifier* pSndDev = (iSoundDeviceIdentifier*)pItem->GetUserData();
        lDevID = pSndDev->GetID();
    }

    aObj.AddVarInt("SoundDeviceID", lDevID);

    aObj.AddVarBool("EnemyActivateSound", mpChBEnemyActivateSound->IsChecked());
    aObj.AddVarBool("InsanityRingSound", mpChBInsanityRingSound->IsChecked());
    aObj.AddVarBool("TerrorSound", mpChBTerrorSound->IsChecked());
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CALLBACKS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::Window_OnUpdate(iWidget* apWidget, const cGuiMessageData& aData)
{
    ///////////////////////////////////////////////////
    // If there is a popup active, dont update tips
    if(mpGuiSet->PopUpIsActive())
    {
        return true;
    }

    ////////////////////////////////////////////////////
    // Update Tip label fade
    cColor labelCol = mpLTip->GetDefaultFontColor();
    if(mbTipFadeRestart)
    {
        if(mbTipTextReset)
        {
            mbTipTextReset = false;
            mpLTip->SetScrollOffset(0);
        }
        labelCol = labelCol + cColor(0, aData.mfVal * 3);
        if(labelCol.a >= 1.0f)
        {
            labelCol.a = 1.0f;
            mbTipFadeRestart = false;
        }
        mpLTip->SetDefaultFontColor(labelCol);
    }
    else
    {
        if(labelCol.a > 0.0f)
        {
            mpLTip->SetDefaultFontColor(labelCol - cColor(0, aData.mfVal * 0.8f));
        }
        else if(mbTipTextReset == false)
        {
            mbTipTextReset = true;
        }
    }

    ///////////////////////////////////////////////////
    // Update Tip label text

    if(mbTipWidgetUpdated)
    {
        mbTipWidgetUpdated = false;

        if(mpCurrentTipWidget && mpCurrentTipWidget->GetUserData())
        {
            cLuxOption_ExtData* pData = (cLuxOption_ExtData*)mpCurrentTipWidget->GetUserData();
            tWString sRestart = pData->mbNeedsRestart ? _W(" (") + kTranslate("OptionsMenu", "ReqRestart") + _W(")") : _W("");

            mpLTip->SetText(pData->msMessage + sRestart);
        }
    }

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, Window_OnUpdate);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::Option_OnMouseOver(iWidget* apWidget, const cGuiMessageData& aData)
{
    if(apWidget->GetUserData() == NULL)
    {
        return true;
    }

    SetCurrentTipWidget(apWidget);

    /////////////
    // Make sure the focused widget is visible
    if(aData.mMessage == eGuiMessage_GetUINavFocus)
    {
        iWidget* pParent = apWidget->GetParent();

        if(pParent)
        {
            iWidget* pParent2 = pParent->GetParent();
            cWidgetFrame* pFrame = dynamic_cast<cWidgetFrame *>(pParent2);

            if(pFrame == NULL
                    && pParent2 != NULL)
            {
                pFrame = dynamic_cast<cWidgetFrame *>(pParent2->GetParent());
            }

            if(pFrame)
            {
                cVector3f vFramePos = pFrame->GetGlobalPosition();
                cVector3f vWidgedPos = apWidget->GetGlobalPosition();
                cVector3f vScrollAmount = pFrame->GetScrollAmount();
                cVector3f vScrollPos = vWidgedPos + vScrollAmount - vFramePos;
                vScrollPos.y -= 150;
                pFrame->ScrollToPosition(cVector2f(vScrollPos.x, vScrollPos.y));
            }
        }
    }
    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, Option_OnMouseOver);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::Option_OnChangeValue(iWidget* apWidget, const cGuiMessageData& aData)
{
    if(mbSettingInitialValues)
    {
        return true;
    }

    if(apWidget->GetUserData())
    {
        cLuxOption_ExtData* pData = (cLuxOption_ExtData*)apWidget->GetUserData();
        if(pData->mbNeedsRestart)
        {
            gpBase->mpConfigHandler->SetGameNeedsRestart();
        }
    }
    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, Option_OnChangeValue);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::FlashSlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData)
{
    float mfFlashIntensity = GetFlash();
    SetSliderLabelString(mpLFlash, mfFlashIntensity, mfFlashMin, mfFlashMax);

    gpBase->mpEffectHandler->SetFlashIntensity(mfFlashIntensity);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, FlashSlider_OnMove);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::FOVSlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData)
{
    SetFOVLabelString(GetFOV());

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, FOVSlider_OnMove);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::GammaSlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData)
{
    float fGamma = GetGamma();
    SetGammaLabelString(fGamma);

    gpBase->mpEngine->GetGraphics()->GetLowLevel()->SetGammaCorrection(fGamma);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, GammaSlider_OnMove);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::MouseSensitivitySlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData)
{
    float fSensitivity = GetSensitivity();
    SetSensitivityLabelString(fSensitivity);

    gpBase->mpInputHandler->SetMouseSensitivity(fSensitivity);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, MouseSensitivitySlider_OnMove);

//-----------------------------------------------------------------------

#if USE_GAMEPAD
bool cLuxMainMenu_Options::GamepadLookSensitivitySlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData)
{
    float fSensitivity = GetGamepadLookSensitivity();
    SetGamepadLookSensitivityLabelString(fSensitivity);

    gpBase->mpInputHandler->SetGamepadLookSensitivity(fSensitivity);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, GamepadLookSensitivitySlider_OnMove);
#endif

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::SoundSlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData)
{
    float fSndVol = GetVolume();
    SetVolumeLabelString(fSndVol);

    cSound *pSound = gpBase->mpEngine->GetSound();

    pSound->GetLowLevel()->SetVolume(fSndVol);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, SoundSlider_OnMove);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::SoundVolumeSlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData)
{
    float fVol = GetSoundVolume();
    SetVolumeLabelString(mpLSoundVolume, fVol);

    //Live, like the master volume (menu sounds let you hear it)
    gpBase->mpEngine->GetSound()->GetSoundHandler()->SetUserVolume(fVol);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, SoundVolumeSlider_OnMove);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::MusicVolumeSlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData)
{
    float fVol = GetMusicVolume();
    SetVolumeLabelString(mpLMusicVolume, fVol);

    gpBase->mpEngine->GetSound()->GetMusicHandler()->SetUserVolume(fVol);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, MusicVolumeSlider_OnMove);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::PressToggleShowGfxOptions(iWidget* apWidget, const cGuiMessageData& aData)
{
    bool bBasicVisible = mpDBasicGfxOptions->IsVisible();
    tWString sText[] = { kTranslate("OptionsMenu", "BasicOptions"), kTranslate("OptionsMenu", "AdvancedOptions") };

    mpDBasicGfxOptions->SetVisible(bBasicVisible == false);
    mpDBasicGfxOptions->SetEnabled(bBasicVisible == false);

    mpDAdvancedGfxOptions->SetVisible(bBasicVisible);
    mpDAdvancedGfxOptions->SetEnabled(bBasicVisible);

    cWidgetTab* pTab = mpTabGraphics;

    if(bBasicVisible)
    {
        //Reduce Input Lag is the bottom right option of the Misc group (also in fullbright,
        //which shows the Misc group with only shader warming and input lag)
        mpBToggleShowGfxOptions->SetFocusNavigation(eUIArrow_Up, mpCBMaxFramesAhead);

        pTab->GetTabLabel()->SetUserData(mpBToggleShowGfxOptions);
    }
    else
    {
        mpBToggleShowGfxOptions->SetFocusNavigation(eUIArrow_Up, mpSGamma);

        pTab->GetTabLabel()->SetUserData(mpCBResolution);
    }

    apWidget->SetText(sText[bBasicVisible == false]);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, PressToggleShowGfxOptions);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::PressKeyConfig(iWidget* apWidget, const cGuiMessageData& aData)
{
    mbKeyConfigOpen = true;
    gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_KeyConfig);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, PressKeyConfig);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::PressOK(iWidget* apWidget, const cGuiMessageData& aData)
{
    ApplyChanges();
    gpBase->SaveConfig();
    //If some major stuff (that needs restart) have been changed. Then say so!
    if(gpBase->mpConfigHandler->ShowRestartWarning(mpGuiSet, this, kGuiCallback(MessageBoxCallback)) == false)
    {
        gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_LastEnum);
    }

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, PressOK);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::PressCancel(iWidget* apWidget, const cGuiMessageData& aData)
{
    //Cancel any changes made.

    /////////////////////////////////////////////////////////////
    // --- REMOVED - Language is now loaded only at startup
    //if(gpBase->msCurrentLanguage!=mInitialValues.GetVarString("Language"))
    //{
    //    gpBase->LoadLanguage(mInitialValues.GetVarString("Language"));
    //    gpBase->mpMainMenu->RecreateGui();
    //}
    gpBase->mpEngine->GetGraphics()->GetLowLevel()->SetGammaCorrection(mInitialValues.GetVarFloat("Gamma"));
    gpBase->mpInputHandler->SetMouseSensitivity(mInitialValues.GetVarFloat("MouseSensitivity"));
#if USE_GAMEPAD
    gpBase->mpInputHandler->SetGamepadLookSensitivity(mInitialValues.GetVarFloat("GamepadLookSensitivity"));
#endif
    gpBase->mpEngine->GetSound()->GetLowLevel()->SetVolume(mInitialValues.GetVarFloat("SoundVolume"));
    gpBase->mpEngine->GetSound()->GetSoundHandler()->SetUserVolume(mInitialValues.GetVarFloat("EffectsVolume", 1.0f));
    gpBase->mpEngine->GetSound()->GetMusicHandler()->SetUserVolume(mInitialValues.GetVarFloat("MusicVolume", 1.0f));

    gpBase->mpEffectHandler->SetFlashIntensity(mInitialValues.GetVarFloat("FlashIntensity", 1.0f));

    MessageBoxCallback(apWidget, aData);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, PressCancel);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::UIPressCancel(iWidget* apWidget, const cGuiMessageData& aData)
{
    if(aData.mlVal == eUIButton_Secondary)
    {
        return PressCancel(apWidget, aData);
    }

    return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, UIPressCancel);


//-----------------------------------------------------------------------

void cLuxMainMenu_Options::OnSetActive(bool abX)
{
    // If the key config menu is open, dont touch anything. Else, set up initial values.
    if(abX)
    {
        SetTabNavigation(mpTabGame->GetParentTabFrame()->GetTabOnTop(), true);

        if(mbKeyConfigOpen == false)
        {
            if(mInitialValues.GetVarBool("SettingLanguage", false))
            {
                SetInputValues(mCurrentValues);
                mInitialValues.AddVarBool("SettingLanguage", false);
            }
            else
            {
                if(mInitialValues.GetVarBool("InitialValuesSet") == false)
                {
                    DumpInitialValues(mInitialValues);
                }

                SetInputValues(mInitialValues);
                mInitialValues.AddVarBool("InitialValuesSet", true);
            }
        }
        else
        {
            mbKeyConfigOpen = false;
        }
    }

#if USE_GAMEPAD
    if(mpShoulderHint[0])
    {
        mpShoulderHint[0]->SetVisible(gpBase->mpInputHandler->IsGamepadPresent());
        mpShoulderHint[1]->SetVisible(gpBase->mpInputHandler->IsGamepadPresent());
    }
#endif
}

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::MessageBoxCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
    gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_LastEnum);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, MessageBoxCallback);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::TabFrame_OnPageChange(iWidget* apWidget, const cGuiMessageData& aData)
{
    cWidgetTabFrame* pTabFrame = static_cast<cWidgetTabFrame *>(apWidget);
    cWidgetTab* pTab = pTabFrame->GetTab(aData.mlVal);

    SetTabNavigation(pTab, true);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, TabFrame_OnPageChange);

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetTabNavigation(cWidgetTab* apTab, bool abSetFocus)
{
    iWidget* pFirstWidget = static_cast<iWidget *>(apTab->GetTabLabel()->GetUserData());
    iWidget* pLastWidget = static_cast<iWidget *>(apTab->GetUserData());

    mpGuiSet->SetDefaultFocusNavWidget(pFirstWidget);
    if(abSetFocus)
    {
        mpGuiSet->SetFocusedWidget(pFirstWidget);
    }

    mpBOK->SetFocusNavigation(eUIArrow_Up, pLastWidget);
    mpBCancel->SetFocusNavigation(eUIArrow_Up, pLastWidget);
}

//-----------------------------------------------------------------------
