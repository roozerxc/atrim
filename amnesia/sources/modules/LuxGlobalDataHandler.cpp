#include "LuxGlobalDataHandler.h"

#include "LuxMap.h"
#include "LuxMapHandler.h"
#include "LuxPlayer.h"
#include "LuxEnemy.h"


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxGlobalDataHandler::cLuxGlobalDataHandler() : iLuxUpdateable("LuxGlobalDataHandler")
{
    mpScript = NULL;

    mbEnemyActivateSoundEnabled = true;
    mbInsanityRingSoundEnabled = true;
    mbTerrorSoundEnabled = true;

    mfLightLampMinSanityIncrease = gpBase->mpGameCfg->GetFloat("Player_Sanity", "LightLampMinSanityIncrease", 0);
    mfLightLampMaxSanityIncrease = gpBase->mpGameCfg->GetFloat("Player_Sanity", "LightLampMaxSanityIncrease", 0);
}

//-----------------------------------------------------------------------

cLuxGlobalDataHandler::~cLuxGlobalDataHandler()
{
    if(mpScript)
    {
        gpBase->mpEngine->GetResources()->GetScriptManager()->Destroy(mpScript);
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxGlobalDataHandler::LoadAndInitGlobalScript()
{
    LoadScript();

    RunScript("OnGameStart()");
}

//-----------------------------------------------------------------------

void cLuxGlobalDataHandler::OnGameStart()
{

}

//-----------------------------------------------------------------------

void cLuxGlobalDataHandler::OnStart()
{

}

//-----------------------------------------------------------------------

void cLuxGlobalDataHandler::Reset()
{
    m_mapVars.clear();

    if(mpScript)
    {
        gpBase->mpEngine->GetResources()->GetScriptManager()->Destroy(mpScript);
    }
    mpScript = NULL;

    mfEnemyActivateSoundCount = 0;
}

//-----------------------------------------------------------------------

void cLuxGlobalDataHandler::Update(double adFixedDelta)
{
    if(mfEnemyActivateSoundCount > 0)
    {
        mfEnemyActivateSoundCount -= (float)adFixedDelta;
    }
}

//-----------------------------------------------------------------------

void cLuxGlobalDataHandler::OnMapEnter(cLuxMap *apMap)
{
}

//-----------------------------------------------------------------------

void cLuxGlobalDataHandler::OnMapLeave(cLuxMap *apMap)
{
}


//-----------------------------------------------------------------------

void cLuxGlobalDataHandler::LoadScript()
{
    /////////////////////
    // Destroy old
    if(mpScript)
    {
        gpBase->mpEngine->GetResources()->GetScriptManager()->Destroy(mpScript);
        mpScript = NULL;
    }

    /////////////////////
    // Load script
    tString sFile = gpBase->mpMapHandler->GetMapFolder() + "global.hps";
    mpScript  = gpBase->mpEngine->GetResources()->GetScriptManager()->CreateScript(sFile);
    if(mpScript == NULL)
    {
        Error("Global script '%s' could not be created!\n", sFile.c_str());
    }
}

bool cLuxGlobalDataHandler::RecompileScript(tString *apOutput)
{
    if(mpScript)
    {
        gpBase->mpEngine->GetResources()->GetScriptManager()->Destroy(mpScript);
    }

    tString sFile = gpBase->mpMapHandler->GetMapFolder() + "global.hps";
    mpScript = gpBase->mpEngine->GetResources()->GetScriptManager()->CreateScript(sFile, apOutput);

    return mpScript != NULL;
}

//-----------------------------------------------------------------------

void cLuxGlobalDataHandler::RunScript(const tString& asCommand)
{
    if(mpScript == NULL)
    {
        return;
    }

    mpScript->Run(asCommand);
}

//-----------------------------------------------------------------------

cLuxScriptVar *cLuxGlobalDataHandler::GetVar(const tString &asName)
{
    tLuxScriptVarMapIt it = m_mapVars.find(asName);
    if(it != m_mapVars.end())
    {
        return &(it->second);
    }

    m_mapVars.insert(tLuxScriptVarMap::value_type(asName, cLuxScriptVar(asName)));
    it = m_mapVars.find(asName);
    return &(it->second);
}

//-----------------------------------------------------------------------

bool cLuxGlobalDataHandler::GetEnemyActivateSoundAllowed()
{
    return mbEnemyActivateSoundEnabled && mfEnemyActivateSoundCount <= 0;
}

//-----------------------------------------------------------------------

void cLuxGlobalDataHandler::SetInsanityRingSoundEnabled(bool abX)
{
    mbInsanityRingSoundEnabled = abX;

    tString sSound = gpBase->mpGameCfg->GetString("Player_General", "InsanityCollapse_SleepLoopSound", "");
    sSound = cString::SetFileExt(cString::GetFileName(sSound), "");
    if(sSound == "")
    {
        return;
    }

    cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
    if(msInsanityRingSound != "" && msInsanityRingSound != sSound)
    {
        pSoundHandler->SetSoundBlocked(msInsanityRingSound, false);
    }
    pSoundHandler->SetSoundBlocked(sSound, abX == false);

    msInsanityRingSound = abX ? tString("") : sSound;
}

//-----------------------------------------------------------------------

void cLuxGlobalDataHandler::LoadUserConfig()
{
    mbEnemyActivateSoundEnabled = gpBase->mpUserConfig->GetBool("Sound", "EnemyActivateSound", true);
    SetInsanityRingSoundEnabled(gpBase->mpUserConfig->GetBool("Sound", "InsanityRingSound", true));
    mbTerrorSoundEnabled = gpBase->mpUserConfig->GetBool("Sound", "TerrorSound", true);
}

void cLuxGlobalDataHandler::SaveUserConfig()
{
    gpBase->mpUserConfig->SetBool("Sound", "EnemyActivateSound", mbEnemyActivateSoundEnabled);
    gpBase->mpUserConfig->SetBool("Sound", "InsanityRingSound", mbInsanityRingSoundEnabled);
    gpBase->mpUserConfig->SetBool("Sound", "TerrorSound", mbTerrorSoundEnabled);
}

void cLuxGlobalDataHandler::SetEnemyActivateSoundMade()
{
    mfEnemyActivateSoundCount = 5.0f;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------

