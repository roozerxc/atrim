#ifndef LUX_AREA_SCRIPT_H
#define LUX_AREA_SCRIPT_H

//----------------------------------------------

#include "LuxArea.h"

//----------------------------------------------

class cLuxArea_Script_SaveData : public iLuxArea_SaveData
{
    kSerializableClassInit(cLuxArea_Script_SaveData)
public:
    iLuxArea* CreateArea(cLuxMap *apMap);

    bool mbItemInteraction;
};

//----------------------------------------------

class cLuxArea_Script : public iLuxArea
{
    typedef iLuxArea super_class;
    friend class cLuxAreaLoader_Script;
public:
    cLuxArea_Script(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxArea_Script();

    //////////////////////
    //General
    bool CanInteract(iPhysicsBody *apBody);
    bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);
    eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

    //////////////////////
    //Connection callbacks
    void OnConnectionStateChange(iLuxEntity *apEntity, int alState) {}

    //////////////////////
    //Save data stuff
    iLuxEntity_SaveData* CreateSaveData();
    virtual void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
    virtual void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
    virtual void SetupSaveData(iLuxEntity_SaveData *apSaveData);

protected:
    bool mbItemInteraction;
};

//----------------------------------------------

class cLuxAreaLoader_Script : public iLuxAreaLoader
{
public:
    cLuxAreaLoader_Script(const tString& asName);
    ~cLuxAreaLoader_Script();

    iLuxArea *CreateArea(const tString& asName, int alID, cLuxMap *apMap);

    void LoadVariables(iLuxArea *apArea, cWorld *apWorld);
    void SetupArea(iLuxArea *apArea, cWorld *apWorld);

};

//----------------------------------------------


#endif // LUX_AREA_SCRIPT_H
