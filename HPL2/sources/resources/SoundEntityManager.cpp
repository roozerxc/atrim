#include "resources/SoundEntityManager.h"

#include "system/String.h"
#include "sound/Sound.h"
#include "resources/Resources.h"
#include "sound/SoundEntityData.h"
#include "system/LowLevelSystem.h"
#include "sound/SoundHandler.h"
#include "sound/SoundChannel.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cSoundEntityManager::cSoundEntityManager(cSound* apSound,cResources *apResources)
    : iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(),
                       apResources->GetLowLevelSystem())
{
    mpSound = apSound;
    mpResources = apResources;
}

cSoundEntityManager::~cSoundEntityManager()
{
    DestroyAll();

    Log(" Done with sound entities\n");
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cSoundEntityManager::Preload(const tString& asFile)
{
    cSoundEntityData *pData = CreateSoundEntity(asFile);
    if(pData == NULL)
    {
        Warning("Couldn't preload sound entity '%s'\n",asFile.c_str());
        return;
    }

    pData->PreloadSounds();
}

//-----------------------------------------------------------------------

cSoundEntityData* cSoundEntityManager::CreateSoundEntity(const tString& asName)
{
    tWString sPath;
    cSoundEntityData* pSoundEntity;
    tString asNewName;

    BeginLoad(asName);

    asNewName = cString::SetFileExt(asName,"snt");

    pSoundEntity = static_cast<cSoundEntityData*>(this->FindLoadedResource(asNewName,sPath));

    if(pSoundEntity==NULL && sPath!=_W(""))
    {
        pSoundEntity = hplNew( cSoundEntityData, (asNewName, mpResources,mpSound) );

        if(pSoundEntity->CreateFromFile(sPath))
        {
            AddResource(pSoundEntity);
        }
        else
        {
            hplDelete(pSoundEntity);
            pSoundEntity =NULL;
        }
    }

    if(pSoundEntity)pSoundEntity->IncUserCount();
    else Error("Couldn't create SoundEntity '%s'\n",asNewName.c_str());

    EndLoad();
    return pSoundEntity;
}

//-----------------------------------------------------------------------

void cSoundEntityManager::Unload(iResourceBase* apResource)
{

}
//-----------------------------------------------------------------------

void cSoundEntityManager::Destroy(iResourceBase* apResource)
{
    apResource->DecUserCount();

    if(apResource->HasUsers()==false)
    {
        RemoveResource(apResource);
        hplDelete(apResource);
    }
}

//-----------------------------------------------------------------------
}
