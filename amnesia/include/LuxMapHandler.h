#ifndef LUX_MAP_HANDLER_H
#define LUX_MAP_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxMap;
class cLuxSavedGameMapCollection;
class cLuxModelCache;

typedef std::list<cLuxMap*> tLuxMapList;
typedef tLuxMapList::iterator tLuxMapListIt;

//----------------------------------------------

class cMapHandlerSoundCallback : public iSoundEntityGlobalCallback
{
public:
    cMapHandlerSoundCallback();

    void OnStart(cSoundEntity *apSoundEntity);

private:
    tStringVec mvEnemyHearableSounds;
};

//----------------------------------------------

class cLuxDebugRenderCallback : public iRendererCallback
{
public:
    cLuxDebugRenderCallback();

    void OnPostSolidDraw(cRendererCallbackFunctions* apFunctions);

    void OnPostTranslucentDraw(cRendererCallbackFunctions* apFunctions);

    iPhysicsWorld* mpPhysicsWorld;
    iLowLevelGraphics* mpLowLevelGfx;
};


//----------------------------------------------

class cLuxMapHandler_ChangeMap
{
public:
    cLuxMapHandler_ChangeMap() : mbActive(false) {}

    bool mbActive;
    tString msMapFile;
    tString msStartPos;
    tString msSound;
};

//----------------------------------------------

class cLuxMapHandler : public iLuxUpdateable
{
    friend class cMapHandlerSoundCallback;
public:
    cLuxMapHandler();
    ~cLuxMapHandler();

    void OnStart();
    void Update(float afTimeStep);
    void Reset();
    void OnQuit();

    void LoadUserConfig();
    void SaveUserConfig();

    void CreateDataCache();
    void DestroyDataCache();

    void UpdateViewportRenderProperties();

    void SetUpdateActive(bool abX);

    void RenderSolid(cRendererCallbackFunctions* apFunctions);

    void OnEnterContainer(const tString& asOldContainer);
    void OnLeaveContainer(const tString& asNewContainer);

    void ChangeMap(const tString& asMapName, const tString& asStartPos, const tString& asStartSound, const tString& asEndSound);

    bool MapIsLoaded()
    {
        return mpCurrentMap != NULL;
    }

    cLuxMap* LoadMap(const tString& asName, bool abLoadEntities);
    void DestroyMap(cLuxMap* apMap, bool abRunScript);

    void SetCurrentMap(cLuxMap* apMap, bool abRunScript, bool abFirstTime, const tString& asPlayerPos);
    cLuxMap* GetCurrentMap()
    {
        return mpCurrentMap;
    }

    cViewport* GetViewport()
    {
        return mpViewport;
    }

    const tString& GetMapFolder()
    {
        return msMapFolder;
    }
    void SetMapFolder(const tString& asFolder)
    {
        msMapFolder = asFolder;
    }

    void PauseSoundsAndMusic();
    void ResumeSoundsAndMusic();

    iPostEffect *GetPostEffect_Bloom()
    {
        return mpPostEffect_Bloom;
    }
    iPostEffect *GetPostEffect_ImageTrail()
    {
        return mpPostEffect_ImageTrail;
    }
    iPostEffect *GetPostEffect_Sepia()
    {
        return mpPostEffect_Sepia;
    }
    iPostEffect *GetPostEffect_RadialBlur()
    {
        return mpPostEffect_RadialBlur;
    }

    void ClearSaveMapCollection();
    cLuxSavedGameMapCollection *GetSavedMapCollection()
    {
        return mpSavedGame;
    }
    void SetSavedMapCollection(cLuxSavedGameMapCollection *apMaps);

    tString FileToMapName(const tString& asFile);

    void SetShowCommentary(bool abX);
    bool GetShowCommentary()
    {
        return mbShowCommentary;
    }

    void AppLostInputFocus();
    void AppGotInputFocus();

    //////////////////////////////////
    // Used to lock the SavedMapCollection
    iMutex *mpSavedGameMutex;
private:
    void LoadMainConfig();
    void SaveMainConfig();

    void CheckMapChange(float afTimeStep);

    cLuxDebugRenderCallback mRenderCallback;

    tString msMapFolder;

    cLuxModelCache *mpDataCache;

    cLuxMap* mpCurrentMap;

    tLuxMapList mlstMaps;

    cViewport *mpViewport;
    cMapHandlerSoundCallback* mpSoundCallback;

    bool mbPausedSoundsAndMusic;

    bool mbUpdateActive;

    bool mbShowCommentary;

    iPostEffect *mpPostEffect_Bloom;
    iPostEffect *mpPostEffect_ImageTrail;
    iPostEffect *mpPostEffect_Sepia;
    iPostEffect *mpPostEffect_RadialBlur;

    cLuxMapHandler_ChangeMap mMapChangeData;

    cLuxSavedGameMapCollection *mpSavedGame;
};

//----------------------------------------------


#endif // LUX_MAP_HANDLER_H
