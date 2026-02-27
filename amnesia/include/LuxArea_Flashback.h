#ifndef LUX_AREA_FLASHBACK_H
#define LUX_AREA_FLASHBACK_H

//----------------------------------------------

#include "LuxArea.h"

//----------------------------------------------

class cLuxArea_Flashback_SaveData : public iLuxArea_SaveData
{
    kSerializableClassInit(cLuxArea_Flashback_SaveData)
public:
    iLuxArea* CreateArea(cLuxMap *apMap);

    tString msFlashbackFile;
    tString msCallback;
};

//----------------------------------------------

class cLuxArea_Flashback : public iLuxArea
{
typedef iLuxArea super_class;
friend class cLuxAreaLoader_Flashback;
public:    
    cLuxArea_Flashback(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxArea_Flashback();

    //////////////////////
    //General
    void OnUpdate(float afTimeStep);

    //////////////////////
    //Connection callbacks
    void OnConnectionStateChange(iLuxEntity *apEntity, int alState){}

    //////////////////////
    //Save data stuff
    iLuxEntity_SaveData* CreateSaveData();
    virtual void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
    virtual void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
    virtual void SetupSaveData(iLuxEntity_SaveData *apSaveData);
protected:

private:
    

    /////////////////////////
    // Data
    tString msFlashbackFile;
    tString msCallback;

    /////////////////////////
    // Variables
    float mfCheckCollisionCount;
};

//----------------------------------------------

class cLuxAreaLoader_Flashback : public iLuxAreaLoader
{
public:
    cLuxAreaLoader_Flashback(const tString& asName);
    ~cLuxAreaLoader_Flashback();

    iLuxArea *CreateArea(const tString& asName, int alID, cLuxMap *apMap);
    
    void LoadVariables(iLuxArea *apArea, cWorld *apWorld);
    void SetupArea(iLuxArea *apArea, cWorld *apWorld);
    
};

//----------------------------------------------


#endif // LUX_AREA_Flashback_H
