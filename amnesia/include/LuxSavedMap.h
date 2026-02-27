#ifndef LUX_SAVED_MAP_H
#define LUX_SAVED_MAP_H

//----------------------------------------------

#include "LuxBase.h"
#include "LuxSavedEngineTypes.h"

//----------------------------------------------

class iLuxEntity_SaveData;
class cLuxMap;

//----------------------------------------------

class cLuxSavedMap : public iSerializable
{
    kSerializableClassInit(cLuxSavedMap)
public:    
    cLuxSavedMap();
    ~cLuxSavedMap();

    void FromMap(cLuxMap *apMap);
    void ToMap(cLuxMap *apMap);

    tString msFileName;

    tString msDisplayNameEntry;

    tString msLanternLitCallback;

    int mlNumberOfQuests;
    int mlTotalCompletionAmount;
    int mlCurrentCompletionAmount;

    bool mbSkyBoxActive;
    tString msSkyboxTexture;
    cColor mSkyBoxColor;

    bool mbFogActive;
    bool mbFogCulling;
    float mfFogStart;
    float mfFogEnd;
    float mfFogFalloffExp;
    cColor mFogColor;

    tString msCheckPointName;
    tString msCheckPointStartPos;
    tString msCheckPointCallback;
    int mlCheckPointCount;
    tString msCheckPointMusic;
    int mlCheckPointMusicPrio;
    bool mbCheckPointMusicResume;
    float mfCheckPointMusicVolume;
    
    cContainerList<cEngineRope_SaveData> mlstRopes;
    cContainerList<cEngineLight_SaveData> mlstLights;
    cContainerList<cEngineSound_SaveData> mlstSounds;
    cContainerList<cEnginePS_SaveData> mlstPS;

    cContainerList<iLuxEntity_SaveData*> mlstEntities;

    cContainerList<cLuxEventTimer> mlstTimers;

    cContainerList<cLuxScriptVar> mlstVars;

    cContainerVec<cLuxUseItemCallback> mvUseItemCallbacks;

private:
};

//----------------------------------------------


#endif // LUX_SAVED_MAP_H
