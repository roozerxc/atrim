#ifndef LUX_AREA_LADDER_H
#define LUX_AREA_LADDER_H

//----------------------------------------------

#include "LuxArea.h"

//----------------------------------------------

class cLuxArea_Ladder_SaveData : public iLuxArea_SaveData
{
    kSerializableClassInit(cLuxArea_Ladder_SaveData)
public:
    iLuxArea* CreateArea(cLuxMap *apMap);

    tString msMaterial;
};

//----------------------------------------------

class cLuxArea_Ladder : public iLuxArea
{
    typedef iLuxArea super_class;
    friend class cLuxAreaLoader_Ladder;
public:
    cLuxArea_Ladder(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxArea_Ladder();

    //////////////////////
    //General
    void SetupAfterLoad(cWorld *apWorld);

    void OnUpdate(float afTimeStep);

    bool CanInteract(iPhysicsBody *apBody);
    bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);

    eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

    //////////////////////
    //Properties
    cVector3f GetStartRotation();

    const cVector3f& GetForward()
    {
        return mvForward;
    }
    float GetMaxY()
    {
        return mfMaxY;
    }
    float GetMinY()
    {
        return mfMinY;
    }

    const tString& GetMaterial()
    {
        return msMaterial;
    }

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

private:
    cVector3f GetStartPosition();

    /////////////////////////
    // Data
    cVector3f mvForward;
    float mfMaxY;
    float mfMinY;

    tString msMaterial;

    /////////////////////////
    // Variables

};

//----------------------------------------------

class cLuxAreaLoader_Ladder : public iLuxAreaLoader
{
public:
    cLuxAreaLoader_Ladder(const tString& asName);
    ~cLuxAreaLoader_Ladder();

    iLuxArea *CreateArea(const tString& asName, int alID, cLuxMap *apMap);

    void LoadVariables(iLuxArea *apArea, cWorld *apWorld);
    void SetupArea(iLuxArea *apArea, cWorld *apWorld);

};

//----------------------------------------------


#endif // LUX_AREA_LADDER_H
