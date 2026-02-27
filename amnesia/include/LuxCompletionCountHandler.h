#ifndef LUX_COMPLETION_COUNT_HANDLER_H
#define LUX_COMPLETION_COUNT_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------

class cLuxCompletionCountHandler : public iLuxUpdateable
{
friend class cLuxMusicHandler_SaveData;
public:    
    cLuxCompletionCountHandler();
    ~cLuxCompletionCountHandler();
    
    void LoadFonts();
    void OnStart();
    void Update(float afTimeStep);
    void Reset();
    void OnDraw(float afFrameTime);

    void OnMapExit(cLuxMap *apMap);

    void ShowCompletionIncrease(float afPrevius, float afNew, float afDelay);

public:
    int mlQuestCompletionValue;
    int mlItemCompletionValue;
    int mlNoteCompletionValue;
    int mlDiaryCompletionValue;
    int mlFlashbackCompletionValue;
    int mlChestCompletionValue;

private:
    //////////////////
    // Data
    iFontData *mpFont;
    
    //////////////////
    // Variables
    bool mbActive;
    float mfAlpha;

    float mfDelay;
    float mfNewAmount;
    float mfDisplayedAmount;
    float mfDisableDelay;
};

//----------------------------------------------


#endif // LUX_COMPLETION_COUNT_HANDLER_H
