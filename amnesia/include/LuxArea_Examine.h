#ifndef LUX_AREA_EXAMINE_H
#define LUX_AREA_EXAMINE_H

//----------------------------------------------

#include "LuxArea.h"

//----------------------------------------------

class cLuxArea_Examine_SaveData : public iLuxArea_SaveData
{
    kSerializableClassInit(cLuxArea_Examine_SaveData)
public:
    iLuxArea* CreateArea(cLuxMap *apMap);

    tString msDescCat;
    tString msDescEntry;

    tString msDescInsaneCat;
    tString msDescInsaneEntry;

    tString msSound;
    tString msInsaneSound;
};

//----------------------------------------------

class cLuxArea_Examine : public iLuxArea
{
typedef iLuxArea super_class;
friend class cLuxAreaLoader_Examine;
public:    
    cLuxArea_Examine(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxArea_Examine();

    //////////////////////
    //General
    void SetupAfterLoad(cWorld *apWorld);

    void OnUpdate(float afTimeStep);

    bool CanInteract(iPhysicsBody *apBody);
    bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);

    eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

    //////////////////////
    //Properties
    
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
    cVector3f GetStartPosition();

    /////////////////////////
    // Data
    float mfInsaneLimit;

    tString msDescCat;
    tString msDescEntry;

    tString msDescInsaneCat;
    tString msDescInsaneEntry;

    tString msSound;
    tString msInsaneSound;
    
    /////////////////////////
    // Variables
    float mfPlaySoundCount;
};

//----------------------------------------------

class cLuxAreaLoader_Examine : public iLuxAreaLoader
{
public:
    cLuxAreaLoader_Examine(const tString& asName);
    ~cLuxAreaLoader_Examine();

    iLuxArea *CreateArea(const tString& asName, int alID, cLuxMap *apMap);
    
    void LoadVariables(iLuxArea *apArea, cWorld *apWorld);
    void SetupArea(iLuxArea *apArea, cWorld *apWorld);
    
};

//----------------------------------------------


#endif // LUX_AREA_EXAMINE_H
