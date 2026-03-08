#include "LuxArea_Sign.h"


//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxAreaLoader_Sign::cLuxAreaLoader_Sign(const tString& asName) : iLuxAreaLoader(asName)
{

}

cLuxAreaLoader_Sign::~cLuxAreaLoader_Sign()
{

}

//-----------------------------------------------------------------------

iLuxArea *cLuxAreaLoader_Sign::CreateArea(const tString& asName, int alID, cLuxMap *apMap)
{
    cLuxArea_Sign *pArea = hplNew(cLuxArea_Sign, (asName, alID, apMap));
    return pArea;
}

//-----------------------------------------------------------------------

void cLuxAreaLoader_Sign::LoadVariables(iLuxArea *apArea, cWorld *apWorld)
{
    cLuxArea_Sign *pSignArea = static_cast<cLuxArea_Sign*>(apArea);

    pSignArea->msTextCat = GetVarString("TextCat","");
    pSignArea->msTextEntry = GetVarString("TextEntry","");
}

void cLuxAreaLoader_Sign::SetupArea(iLuxArea *apArea, cWorld *apWorld)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxArea_Sign::cLuxArea_Sign(const tString &asName, int alID, cLuxMap *apMap)  : iLuxArea(asName,alID,apMap, eLuxAreaType_Sign)
{
    mfMaxFocusDistance = gpBase->mpGameCfg->GetFloat("Player_Interaction","Sign_MaxFocusDist",0);
}

//-----------------------------------------------------------------------

cLuxArea_Sign::~cLuxArea_Sign()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxArea_Sign::SetupAfterLoad(cWorld *apWorld)
{

}

//-----------------------------------------------------------------------

void cLuxArea_Sign::OnUpdate(float afTimeStep)
{
}

//-----------------------------------------------------------------------

bool cLuxArea_Sign::CanInteract(iPhysicsBody *apBody)
{
    return true;
}

//-----------------------------------------------------------------------

bool cLuxArea_Sign::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
    return false;
}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxArea_Sign::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
    return eLuxFocusCrosshair_Default;
}

//-----------------------------------------------------------------------

tWString cLuxArea_Sign::GetFocusText()
{
    return kTranslate(msTextCat, msTextEntry);
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

kBeginSerialize(cLuxArea_Sign_SaveData, iLuxArea_SaveData)

kSerializeVar(msTextCat, eSerializeType_String)
kSerializeVar(msTextEntry, eSerializeType_String)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxArea* cLuxArea_Sign_SaveData::CreateArea(cLuxMap *apMap)
{
    return hplNew(cLuxArea_Sign, (msName, mlID, apMap));
}

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxArea_Sign::CreateSaveData()
{
    return hplNew(cLuxArea_Sign_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxArea_Sign::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
    super_class::SaveToSaveData(apSaveData);
    cLuxArea_Sign_SaveData *pData = static_cast<cLuxArea_Sign_SaveData*>(apSaveData);

    kCopyToVar(pData, msTextCat);
    kCopyToVar(pData, msTextEntry);
}

//-----------------------------------------------------------------------

void cLuxArea_Sign::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
    super_class::LoadFromSaveData(apSaveData);
    cLuxArea_Sign_SaveData *pData = static_cast<cLuxArea_Sign_SaveData*>(apSaveData);

    kCopyFromVar(pData, msTextCat);
    kCopyFromVar(pData, msTextEntry);
}

//-----------------------------------------------------------------------

void cLuxArea_Sign::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
    super_class::SetupSaveData(apSaveData);

}

//-----------------------------------------------------------------------

