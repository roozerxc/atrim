#ifndef LUX_GLOBAL_DATA_HANDLER_H
#define LUX_GLOBAL_DATA_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxGlobalDataHandler : public iLuxUpdateable
{
    friend class cLuxGlobalDataHandler_SaveData;
public:
    cLuxGlobalDataHandler();
    ~cLuxGlobalDataHandler();

    void LoadAndInitGlobalScript();
    void OnGameStart();
    void OnStart();
    void Update(double adFixedDelta);
    void Reset();

    void OnMapEnter(cLuxMap *apMap);
    void OnMapLeave(cLuxMap *apMap);

    void LoadScript();
    bool RecompileScript(tString *apOutput);

    void RunScript(const tString& asCommand);

    void LoadUserConfig();
    void SaveUserConfig();

    cLuxScriptVar *GetVar(const tString &asName);

    /////////////////////////////
    // AI
    bool GetEnemyActivateSoundAllowed();
    void SetEnemyActivateSoundMade();

    /////////////////////////////
    // Extra sound settings
    void SetEnemyActivateSoundEnabled(bool abX)
    {
        mbEnemyActivateSoundEnabled = abX;
    }
    bool GetEnemyActivateSoundEnabled()
    {
        return mbEnemyActivateSoundEnabled;
    }
    void SetInsanityRingSoundEnabled(bool abX);
    bool GetInsanityRingSoundEnabled()
    {
        return mbInsanityRingSoundEnabled;
    }
    void SetTerrorSoundEnabled(bool abX)
    {
        mbTerrorSoundEnabled = abX;
    }
    bool GetTerrorSoundEnabled()
    {
        return mbTerrorSoundEnabled;
    }


    /////////////////////////////
    // Misc settings
    float GetLightLampMinSanityIncrease()
    {
        return mfLightLampMinSanityIncrease;
    }
    float GetLightLampMaxSanityIncrease()
    {
        return mfLightLampMaxSanityIncrease;
    }

private:
    tLuxScriptVarMap m_mapVars;
    iScript *mpScript;

    float mfEnemyActivateSoundCount;

    bool mbEnemyActivateSoundEnabled;
    bool mbInsanityRingSoundEnabled;
    bool mbTerrorSoundEnabled;

    tString msInsanityRingSound;

    float mfLightLampMinSanityIncrease;
    float mfLightLampMaxSanityIncrease;
};

//----------------------------------------------


#endif // LUX_GLOBAL_DATA_HANDLER_H
