#ifndef LUX_PROP_ITEM_H
#define LUX_PROP_ITEM_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_Item_SaveData : public iLuxProp_SaveData
{
    kSerializableClassInit(cLuxProp_Item_SaveData)
public:
    tString msVal;
    tString msExtraVal;
    int mlSpawnContainerID;
    float mfAmount;
    tString msSubItemTypeName;
};


//----------------------------------------------

class cLuxProp_Item : public iLuxProp
{
typedef iLuxProp super_class;
friend class cLuxPropLoader_Item;
public:    
    cLuxProp_Item(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxProp_Item();

    //////////////////////
    //General
    bool CanInteract(iPhysicsBody *apBody);
    bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);

    void OnSetupAfterLoad(cWorld *apWorld);

    void OnResetProperties();

    void UpdatePropSpecific(float afTimeStep);
    
    void BeforePropDestruction();

    eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

    //////////////////////
    //Properties
    const tString& GetStringVal(){ return msVal; }

    eLuxItemType GetItemType(){ return mItemType;}

    const tString& GetImageFile(){ return msImageFile;}
    const tString& GetPickSound(){ return msPickSound;}
    const tString& GetSubItemTypeName(){ return msSubItemTypeName;}

    float GetAmount(){ return mfAmount;}

    int GetSpawnContainerID() { return mlSpawnContainerID;}
    void SetSpawnContainerID(int alX) { mlSpawnContainerID = alX;}
    
    //////////////////////
    //Connection callbacks
    void OnConnectionStateChange(iLuxEntity *apEntity, int alState);

    //////////////////////
    //Save data stuff
    iLuxEntity_SaveData* CreateSaveData();
    void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
    void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
    void SetupSaveData(iLuxEntity_SaveData *apSaveData);


private:
    ////////////////
    //Variables
    tString msVal;
    tString msExtraVal;

    int mlSpawnContainerID;

    float mfFlashAlpha;

    ////////////////
    //Data
    eLuxItemType mItemType;

    tString msImageFile;
    tString msPickSound;
    tString msSubItemTypeName;

    float mfAmount;
};

//----------------------------------------------

class cLuxPropLoader_Item : public iLuxPropLoader
{
public:
    cLuxPropLoader_Item(const tString& asName);
    virtual ~cLuxPropLoader_Item(){}

    iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
    void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
    void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_PROP_ITEM_H
