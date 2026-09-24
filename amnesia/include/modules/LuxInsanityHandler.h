#ifndef LUX_INSANITY_HANDLER_H
#define LUX_INSANITY_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class iLuxInsanityEvent
{
public:
    iLuxInsanityEvent();
    virtual ~iLuxInsanityEvent();

    /////////////////
    // General
    virtual void Update(double adFixedDelta) {}
    virtual void OnDraw(double adFrameTime) {}

    void LoadData(cXmlElement * apVarElem);
    virtual void OnLoadData(cXmlElement * apVarElem)=0;

    void Start();
    virtual void OnStart()=0;
    virtual void OnExit()=0;

    /////////////////
    // Properties
    const tString& GetName()
    {
        return msName;
    }
    const tString& GetSet()
    {
        return msSet;
    }
    float GetMaxSanity()
    {
        return mfMaxSanity;
    }

    void SetUsed(bool abX)
    {
        mbUsed = abX;
    }
    bool IsUsed()
    {
        return mbUsed;
    }

    bool IsOver()
    {
        return mbOver;
    }

protected:
    void EventIsDone()
    {
        mbOver = true;
    }

private:
    tString msName;
    tString msSet;
    bool mbUsed;
    bool mbOver;
    float mfMaxSanity;

};

//----------------------------------------------

class cLuxInsanityEvent_Bugs_Bug
{
public:
    bool mbActive;

    cVector2f mvPos;
    cVector2f mvVel;
    float mfSizeMul;
    float mfAngle;

    float mfAlpha;
};

class cLuxInsanityEvent_Bugs : public iLuxInsanityEvent
{
public:
    cLuxInsanityEvent_Bugs();
    ~cLuxInsanityEvent_Bugs();

    void OnLoadData(cXmlElement * apVarElem);

    void OnStart();
    void OnExit();

    void Update(double adFixedDelta);
    void OnDraw(double adFrameTime);

private:
    //////////////////
    //Data
    int mlNumOfBugs;
    tString msLoopSound;
    float mfSoundVolume;
    float mfDuration;

    float mfMinSizeMul;
    float mfMaxSizeMul;

    float mfWanderCircleDist;
    float mfWanderCircleRadius;
    float mfSwarmPointMul;
    float mfMaxSpeed;

    cGuiGfxElement *mpBugImage;

    //////////////////
    //Vars
    std::vector<cLuxInsanityEvent_Bugs_Bug> mvBugs;

    cSoundEntry *mpSoundEntry;
    int mlSoundEntryID;

    float mfTimeCount;
};

//----------------------------------------------

class cLuxInsanityEvent_Particles : public iLuxInsanityEvent
{
public:
    cLuxInsanityEvent_Particles();
    ~cLuxInsanityEvent_Particles();

    void OnLoadData(cXmlElement * apVarElem);

    void OnStart();
    void OnExit();

    void Update(double adFixedDelta);
    void OnDraw(double adFrameTime);

private:
    //////////////////
    //Data
    tString msSoundFile;
    bool mbLoopSound;
    float mfSoundVolume;
    float mfSoundFadeInTime;
    float mfSoundFadeOutTime;

    tString msParticleSystem;

    float mfDuration;

    //////////////////
    //Vars
    float mfTimeCount;

    cSoundEntry *mpSoundEntry;
    int mlSoundEntryID;

    cParticleSystem *mpPS;
};

//----------------------------------------------

class cLuxInsanityEvent_SoundStream : public iLuxInsanityEvent
{
public:
    cLuxInsanityEvent_SoundStream();
    ~cLuxInsanityEvent_SoundStream();

    void OnLoadData(cXmlElement * apVarElem);

    void OnStart();
    void OnExit();

    void Update(double adFixedDelta);
    void OnDraw(double adFrameTime);

private:
    tString msFile;
    float mfVolume;
    float mfSoundDelayTime;
    bool mbFadeScreen;
    cGuiGfxElement *mpWhiteGfx;
    cColor mFadeColor;
    float mfFadeInSpeed;
    float mfFadeOutSpeed;
    bool mbDisablePlayer;
    tString msFadeImageFile;

    tString msSubtitleCat;
    tString msSubtitleEntry;

    std::vector<tWString> mvCurrentTextRows;

    float mfSoundCount;
    float mfFadeAlpha;
    float mfFadeAlphaSpeed;
    cSoundEntry *mpSoundEntry;
    int mlSoundEntryID;
    cGuiGfxElement *mpFadeImage;
};

//----------------------------------------------

class cLuxInsanityEvent_Steps : public iLuxInsanityEvent
{
public:
    cLuxInsanityEvent_Steps();
    ~cLuxInsanityEvent_Steps();

    void OnLoadData(cXmlElement * apVarElem);

    void OnStart();
    void OnExit();

    void Update(double adFixedDelta);
    void OnDraw(double adFrameTime);

private:
    tString msSound;
    float mfStepTime;
    int mlStepNum;
    float mfDistance;

    float mfTimeMulPerStep;
    float mfDistanceMulPerStep;

    float mfSoundCountMax;
    float mfSoundCount;
    int mlCount;
    cVector3f mvPosition;

};

//----------------------------------------------


class cLuxInsanityHandler : public iLuxUpdateable
{
    friend class cLuxInsanityHandler_SaveData;
public:
    cLuxInsanityHandler();
    ~cLuxInsanityHandler();

    void OnStart();
    void Reset();
    void Update(double adFixedDelta);
    void OnDraw(double adFrameTime);

    void StartEvent();
    void StartEvent(int alIdx);
    void StartEvent(const tString &asName);
    void StopCurrentEvent();

    int GetEventNum()
    {
        return (int)mvEvents.size();
    }
    iLuxInsanityEvent *GetEvent(int alIdx)
    {
        return mvEvents[alIdx];
    };

    void ReloadEvents();

    void EnableSet(const tString &asSet);
    void DisableSet(const tString &asSet);

    /////////////////
    // Properties
    bool NewEventIsPossible();
    float GetNewEventCount()
    {
        return mfNewEventCount;
    }
    int GetCurrentEvent()
    {
        return mlCurrentEvent;
    }


private:
    iLuxInsanityEvent* EventTypeToData(const tString& asType);
    void LoadEvents(const tString& asFile);

    bool SetIsDisabled(const tString& asSet);

    //////////////////
    // Data
    float mfMaxSanity_LongWait;
    float mfMaxSanity_MedWait;
    float mfMaxSanity_ShortWait;

    float mfTimeBetween_LongWait;
    float mfTimeBetween_MedWait;
    float mfTimeBetween_ShortWait;



    std::vector<iLuxInsanityEvent*> mvEvents;

    //////////////////
    // Variables
    float mfNewEventCount;
    int mlCurrentEvent;

    tStringSet m_setDisabledSets;
};

//----------------------------------------------


#endif // LUX_Insanity_HANDLER_H
