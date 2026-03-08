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

    mfLightLampMinSanityIncrease = gpBase->mpGameCfg->GetFloat("Player_Sanity", "LightLampMinSanityIncrease",0);
    mfLightLampMaxSanityIncrease = gpBase->mpGameCfg->GetFloat("Player_Sanity", "LightLampMaxSanityIncrease",0);
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

    if(mpScript) gpBase->mpEngine->GetResources()->GetScriptManager()->Destroy(mpScript);
    mpScript = NULL;

    mfEnemyActivateSoundCount =0;
}

//-----------------------------------------------------------------------

void cLuxGlobalDataHandler::Update(float afTimeStep)
{
    if(mfEnemyActivateSoundCount>0)
    {
        mfEnemyActivateSoundCount-=afTimeStep;
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
    if(mpScript==NULL)
    {
        Error("Global script '%s' could not be created!\n", sFile.c_str());
    }
}

bool cLuxGlobalDataHandler::RecompileScript(tString *apOutput)
{
    if(mpScript)
        gpBase->mpEngine->GetResources()->GetScriptManager()->Destroy(mpScript);

    tString sFile = gpBase->mpMapHandler->GetMapFolder() + "global.hps";
    mpScript = gpBase->mpEngine->GetResources()->GetScriptManager()->CreateScript(sFile, apOutput);

    return mpScript != NULL;
}

//-----------------------------------------------------------------------

void cLuxGlobalDataHandler::RunScript(const tString& asCommand)
{
    if(mpScript==NULL) return;

    mpScript->Run(asCommand);
}

//-----------------------------------------------------------------------

cLuxScriptVar* cLuxGlobalDataHandler::GetVar(const tString &asName)
{
    tLuxScriptVarMapIt it = m_mapVars.find(asName);
    if(it != m_mapVars.end()) return &(it->second);

    m_mapVars.insert(tLuxScriptVarMap::value_type(asName, cLuxScriptVar(asName)));
    it = m_mapVars.find(asName);
    return &(it->second);
}

//-----------------------------------------------------------------------

bool cLuxGlobalDataHandler::GetEnemyActivateSoundAllowed()
{
    return mfEnemyActivateSoundCount<=0;
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

