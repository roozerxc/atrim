#include "LuxArea_Examine.h"

#include "LuxMap.h"
#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxEffectHandler.h"
#include "LuxHelpFuncs.h"
#include "LuxCompletionCountHandler.h"
#include "LuxMessageHandler.h"
#include "LuxMapHandler.h"
#include "LuxMap.h"


//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxAreaLoader_Examine::cLuxAreaLoader_Examine(const tString& asName) : iLuxAreaLoader(asName)
{

}

cLuxAreaLoader_Examine::~cLuxAreaLoader_Examine()
{

}

//-----------------------------------------------------------------------

iLuxArea *cLuxAreaLoader_Examine::CreateArea(const tString& asName, int alID, cLuxMap *apMap)
{
    cLuxArea_Examine *pArea = hplNew(cLuxArea_Examine, (asName, alID, apMap));
    return pArea;
}

//-----------------------------------------------------------------------

void cLuxAreaLoader_Examine::LoadVariables(iLuxArea *apArea, cWorld *apWorld)
{
    cLuxArea_Examine *pExamineArea = static_cast<cLuxArea_Examine*>(apArea);

    pExamineArea->msDescCat = GetVarString("DescCat","");
    pExamineArea->msDescEntry = GetVarString("DescEntry","");

    pExamineArea->msDescInsaneCat = GetVarString("DescInsaneCat","");
    pExamineArea->msDescInsaneEntry = GetVarString("DescInsaneEntry","");

    pExamineArea->msSound = GetVarString("Sound","");
    pExamineArea->msInsaneSound = GetVarString("InsaneSound","");
}

void cLuxAreaLoader_Examine::SetupArea(iLuxArea *apArea, cWorld *apWorld)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxArea_Examine::cLuxArea_Examine(const tString &asName, int alID, cLuxMap *apMap)  : iLuxArea(asName,alID,apMap, eLuxAreaType_Examine)
{
    mfMaxFocusDistance = gpBase->mpGameCfg->GetFloat("Player_Interaction","Examine_MaxFocusDist",0);
    mfInsaneLimit = gpBase->mpGameCfg->GetFloat("Insanity","MaxExamineSanity",0);

    mfPlaySoundCount =0;

}

//-----------------------------------------------------------------------

cLuxArea_Examine::~cLuxArea_Examine()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxArea_Examine::SetupAfterLoad(cWorld *apWorld)
{

}

//-----------------------------------------------------------------------

void cLuxArea_Examine::OnUpdate(float afTimeStep)
{
    if(mfPlaySoundCount>0) mfPlaySoundCount -= afTimeStep;
}

//-----------------------------------------------------------------------

bool cLuxArea_Examine::CanInteract(iPhysicsBody *apBody)
{
    return true;
}

//-----------------------------------------------------------------------

bool cLuxArea_Examine::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
    float fSanity = gpBase->mpPlayer->GetSanity();

    ////////////////////////////////
    // Show text
    if(msDescCat != "")
    {
        tString sCat = msDescCat;
        tString sEntry = msDescEntry;
        if(fSanity < mfInsaneLimit && msDescInsaneCat != "" && msDescInsaneEntry != "")
        {
            sCat = msDescInsaneCat;
            sEntry = msDescInsaneEntry;
        }

        gpBase->mpMessageHandler->SetMessage(kTranslate(sCat, sEntry), 0);
    }

    ////////////////////////////////
    // Play sound
    if(mfPlaySoundCount <=0 && msSound != "")
    {
        tString sSound = msSound;
        if(fSanity < mfInsaneLimit && msInsaneSound != "")
            sSound = msInsaneSound;

        if(sSound != "")
            gpBase->mpHelpFuncs->PlayGuiSoundData(sSound, eSoundEntryType_Gui);
    }


    mfPlaySoundCount = 0.5f;
    return true;
}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxArea_Examine::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
    return eLuxFocusCrosshair_Grab;
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

kBeginSerialize(cLuxArea_Examine_SaveData, iLuxArea_SaveData)

kSerializeVar(msDescCat, eSerializeType_String)
kSerializeVar(msDescEntry, eSerializeType_String)

kSerializeVar(msDescInsaneCat, eSerializeType_String)
kSerializeVar(msDescInsaneEntry, eSerializeType_String)

kSerializeVar(msSound, eSerializeType_String)
kSerializeVar(msInsaneSound, eSerializeType_String)

kEndSerialize()

//-----------------------------------------------------------------------

iLuxArea* cLuxArea_Examine_SaveData::CreateArea(cLuxMap *apMap)
{
    return hplNew(cLuxArea_Examine, (msName, mlID, apMap));
}

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxArea_Examine::CreateSaveData()
{
    return hplNew(cLuxArea_Examine_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxArea_Examine::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
    super_class::SaveToSaveData(apSaveData);
    cLuxArea_Examine_SaveData *pData = static_cast<cLuxArea_Examine_SaveData*>(apSaveData);

    kCopyToVar(pData, msDescCat);
    kCopyToVar(pData, msDescEntry);

    kCopyToVar(pData, msDescInsaneCat);
    kCopyToVar(pData, msDescInsaneEntry);

    kCopyToVar(pData, msSound);
    kCopyToVar(pData, msInsaneSound);
}

//-----------------------------------------------------------------------

void cLuxArea_Examine::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
    super_class::LoadFromSaveData(apSaveData);
    cLuxArea_Examine_SaveData *pData = static_cast<cLuxArea_Examine_SaveData*>(apSaveData);

    kCopyFromVar(pData, msDescCat);
    kCopyFromVar(pData, msDescEntry);

    kCopyFromVar(pData, msDescInsaneCat);
    kCopyFromVar(pData, msDescInsaneEntry);

    kCopyFromVar(pData, msSound);
    kCopyFromVar(pData, msInsaneSound);
}

//-----------------------------------------------------------------------

void cLuxArea_Examine::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
    super_class::SetupSaveData(apSaveData);

}

//-----------------------------------------------------------------------

