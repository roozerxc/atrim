#include "resources/ScriptManager.h"
#include "system/String.h"
#include "system/System.h"
#include "resources/Resources.h"
#include "system/Script.h"
#include "system/LowLevelSystem.h"



namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cScriptManager::cScriptManager(cSystem* apSystem,cResources *apResources)
    : iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(),
                       apResources->GetLowLevelSystem())
{
    mpSystem = apSystem;
    mpResources = apResources;
}

cScriptManager::~cScriptManager()
{
    DestroyAll();
    Log(" Done with scripts\n");
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iScript* cScriptManager::CreateScript(const tString& asName, tString *apCompileMessages)
{
    tWString sPath;
    iScript* pScript;
    tString asNewName;

    BeginLoad(asName);

    if(cString::GetFileExt(asName) != "chps")
        asNewName = cString::SetFileExt(asName,"hps");
    else
        asNewName = cString::SetFileExt(asName,"chps");

    pScript = static_cast<iScript*>(this->FindLoadedResource(asNewName,sPath));

    if(pScript==NULL && sPath!=_W(""))
    {
        pScript = mpSystem->GetLowLevel()->CreateScript(asNewName);

        if(pScript->CreateFromFile(sPath, apCompileMessages)==false)
        {
            hplDelete(pScript);
            EndLoad();
            return NULL;
        }

        AddResource(pScript);
    }

    if(pScript)pScript->IncUserCount();
    else Error("Couldn't create script '%s'\n",asNewName.c_str());

    EndLoad();
    return pScript;
}

//-----------------------------------------------------------------------

void cScriptManager::Unload(iResourceBase* apResource)
{

}
//-----------------------------------------------------------------------

void cScriptManager::Destroy(iResourceBase* apResource)
{
    apResource->DecUserCount();

    if(apResource->HasUsers()==false)
    {
        RemoveResource(apResource);
        hplDelete(apResource);
    }
}

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
}
