#ifndef LUX_SAVE_HANDLER_H
#define LUX_SAVE_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxSaveGame_SaveData;

//----------------------------------------------

class cLuxSaveHandlerThreadClass : public iThreadClass
{
public:
    cLuxSaveHandlerThreadClass();
    ~cLuxSaveHandlerThreadClass();

    bool IsRunning();

    void SetUpThread();
    void Save(cLuxSaveGame_SaveData* apSaveData, const tWString& asFile);

    void ProcessPendingSaves();

    void UpdateThread();

protected:
    iMutex* mpSaveMutex;
    iThread* mpThread;
    std::vector<cLuxSaveGame_SaveData*> mvSaveData;
    tWStringVec mvSaveFileNames;
};

//----------------------------------------------

class cLuxSaveHandler : public iLuxUpdateable
{
public:
    cLuxSaveHandler();
    ~cLuxSaveHandler();

    void OnStart();
    void Update(float afTimeStep);
    void Reset();

    void SaveGameToFile(const tWString& asFile, bool abSaveSnapshot=false);
    void LoadGameFromFile(const tWString& asFile);

    bool AutoSave();
    bool AutoLoad(bool abResetProgressLogger);

    bool SaveFileExists();

    //////////////////////
    // HARDMODE

    bool HardModeSave();

    //////////////////////

    cLuxSaveGame_SaveData *CreateSaveGameData();
    void LoadSaveGameData(cLuxSaveGame_SaveData *apSave);

    tWString GetProperSaveName(const tWString& asFile);

    cLuxSaveHandlerThreadClass* GetThreadClass()
    {
        return &mSaveHandlerThreadClass;
    }
private:
    tWString GetSaveName(const tWString &asPrefix);
    void DeleteOldestSaveFiles(const tWString &asFolder, int alMax);
    tWString GetNewestSaveFile(const tWString &asFolder);

    bool mbInitialized;
    bool mbStartThread;

    cDate mLatestSaveDate;
    int mlMaxAutoSaves;
    int mlSaveNameCount;

    cLuxSaveHandlerThreadClass mSaveHandlerThreadClass;
};


//----------------------------------------------


#endif // #ifndef LUX_SAVE_HANDLER_H
