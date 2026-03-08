#ifndef LUX_AREA_INSANITY_H
#define LUX_AREA_INSANITY_H

//----------------------------------------------

#include "LuxArea.h"

//----------------------------------------------

class cLuxArea_Insanity_SaveData : public iLuxArea_SaveData
{
    kSerializableClassInit(cLuxArea_Insanity_SaveData)
public:
    iLuxArea* CreateArea(cLuxMap *apMap);

    bool mbAutoDisable;
};

//----------------------------------------------

class cLuxArea_Insanity : public iLuxArea
{
    typedef iLuxArea super_class;
    friend class cLuxAreaLoader_Insanity;
public:
    cLuxArea_Insanity(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxArea_Insanity();

    //////////////////////
    //General
    void OnUpdate(float afTimeStep);

    //////////////////////
    //Connection callbacks
    void OnConnectionStateChange(iLuxEntity *apEntity, int alState) {}

    //////////////////////
    //Save data stuff
    iLuxEntity_SaveData* CreateSaveData();
    virtual void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
    virtual void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
    virtual void SetupSaveData(iLuxEntity_SaveData *apSaveData);
private:

    /////////////////////////
    // Data
    float mfCheckTimeMin;
    float mfCheckTimeMax;

    /////////////////////////
    // Variables
    float mfCheckCollisionCount;
    bool mbAutoDisable;
};

//----------------------------------------------

class cLuxAreaLoader_Insanity : public iLuxAreaLoader
{
public:
    cLuxAreaLoader_Insanity(const tString& asName);
    ~cLuxAreaLoader_Insanity();

    iLuxArea *CreateArea(const tString& asName, int alID, cLuxMap *apMap);

    void LoadVariables(iLuxArea *apArea, cWorld *apWorld);
    void SetupArea(iLuxArea *apArea, cWorld *apWorld);

};

//----------------------------------------------


#endif // LUX_AREA_Insanity_H
