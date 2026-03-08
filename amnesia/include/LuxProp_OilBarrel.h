#ifndef LUX_PROP_OIL_BARELL_H
#define LUX_PROP_OIL_BARELL_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_OilBarrel_SaveData : public iLuxProp_SaveData
{
    kSerializableClassInit(cLuxProp_OilBarrel_SaveData)
public:
    bool mbAmountCalculated;
    float mfOilAmount;
};

//----------------------------------------------


class cLuxProp_OilBarrel : public iLuxProp
{
    typedef iLuxProp super_class;
    friend class cLuxPropLoader_OilBarrel;
public:
    cLuxProp_OilBarrel(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxProp_OilBarrel();

    //////////////////////
    //General
    bool CanInteract(iPhysicsBody *apBody);
    bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);

    void OnSetupAfterLoad(cWorld *apWorld);

    void OnResetProperties();

    void UpdatePropSpecific(float afTimeStep);

    void BeforePropDestruction();

    eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);
    tWString GetFocusText();

    //////////////////////
    //Properties

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
    void CalculateOilAmount();

    //////////////////////
    // Data
    tString msFillSound;
    tString msEmptySound;

    //////////////////////
    // Variables
    bool mbAmountCalculated;
    float mfOilAmount;
};

//----------------------------------------------

class cLuxPropLoader_OilBarrel : public iLuxPropLoader
{
public:
    cLuxPropLoader_OilBarrel(const tString& asName);
    virtual ~cLuxPropLoader_OilBarrel() {}

    iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
    void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
    void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_PROP_OIL_BARELL_H
