#include "LuxArea_Insanity.h"

#include "LuxInsanityHandler.h"



//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxAreaLoader_Insanity::cLuxAreaLoader_Insanity(const tString& asName) : iLuxAreaLoader(asName)
{

}

cLuxAreaLoader_Insanity::~cLuxAreaLoader_Insanity()
{

}

//-----------------------------------------------------------------------

iLuxArea *cLuxAreaLoader_Insanity::CreateArea(const tString& asName, int alID, cLuxMap *apMap)
{
    cLuxArea_Insanity *pArea = hplNew(cLuxArea_Insanity, (asName, alID, apMap));
    return pArea;
}

//-----------------------------------------------------------------------

void cLuxAreaLoader_Insanity::LoadVariables(iLuxArea *apArea, cWorld *apWorld)
{
    cLuxArea_Insanity *pFlashArea = static_cast<cLuxArea_Insanity*>(apArea);

    pFlashArea->mbAutoDisable = GetVarBool("AutoDisable",false);
}

void cLuxAreaLoader_Insanity::SetupArea(iLuxArea *apArea, cWorld *apWorld)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxArea_Insanity::cLuxArea_Insanity(const tString &asName, int alID, cLuxMap *apMap)  : iLuxArea(asName,alID,apMap, eLuxAreaType_Insanity)
{
    mfCheckTimeMin = gpBase->mpGameCfg->GetFloat("Insanity", "InsanityArea_CheckTimeMin",0);
    mfCheckTimeMax = gpBase->mpGameCfg->GetFloat("Insanity", "IsanityArea_CheckTimeMax",0);

    mfCheckCollisionCount = cMath::RandRectf(mfCheckTimeMin, mfCheckTimeMax);
}

//-----------------------------------------------------------------------

cLuxArea_Insanity::~cLuxArea_Insanity()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxArea_Insanity::OnUpdate(float afTimeStep)
{
    //////////////////////////
    // Check update count
    mfCheckCollisionCount-=afTimeStep;
    if(mfCheckCollisionCount>0) return;
    mfCheckCollisionCount = cMath::RandRectf(mfCheckTimeMin, mfCheckTimeMax);

    //////////////////////////
    // Check collision
    if(CollidesWithPlayer())
    {
        if(gpBase->mpInsanityHandler->NewEventIsPossible())
        {
            if(mbAutoDisable) SetActive(false);
            gpBase->mpInsanityHandler->StartEvent();
        }
    }
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxArea_Insanity_SaveData, iLuxArea_SaveData)

kSerializeVar(mbAutoDisable, eSerializeType_Bool)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxArea* cLuxArea_Insanity_SaveData::CreateArea(cLuxMap *apMap)
{
    return hplNew(cLuxArea_Insanity, (msName, mlID, apMap));
}

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxArea_Insanity::CreateSaveData()
{
    return hplNew(cLuxArea_Insanity_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxArea_Insanity::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
    super_class::SaveToSaveData(apSaveData);
    cLuxArea_Insanity_SaveData *pData = static_cast<cLuxArea_Insanity_SaveData*>(apSaveData);

    kCopyToVar(pData, mbAutoDisable);
}

//-----------------------------------------------------------------------

void cLuxArea_Insanity::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
    super_class::LoadFromSaveData(apSaveData);
    cLuxArea_Insanity_SaveData *pData = static_cast<cLuxArea_Insanity_SaveData*>(apSaveData);

    kCopyFromVar(pData, mbAutoDisable);
}

//-----------------------------------------------------------------------

void cLuxArea_Insanity::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
    super_class::SetupSaveData(apSaveData);

}

//-----------------------------------------------------------------------

