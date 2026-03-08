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
    void Update(float afTimeStep);
    void Reset();

    void OnMapEnter(cLuxMap *apMap);
    void OnMapLeave(cLuxMap *apMap);

    void LoadScript();
    bool RecompileScript(tString *apOutput);

    void RunScript(const tString& asCommand);

    cLuxScriptVar* GetVar(const tString &asName);

    /////////////////////////////
    // AI
    bool GetEnemyActivateSoundAllowed();
    void SetEnemyActivateSoundMade();


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

    float mfLightLampMinSanityIncrease;
    float mfLightLampMaxSanityIncrease;
};

//----------------------------------------------


#endif // LUX_GLOBAL_DATA_HANDLER_H
