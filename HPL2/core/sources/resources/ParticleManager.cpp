#include "resources/ParticleManager.h"

#include "resources/Resources.h"
#include "resources/FileSearcher.h"
#include "graphics/Graphics.h"
#include "scene/ParticleSystem.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "impl/tinyXML/tinyxml.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cParticleManager::cParticleManager(cGraphics* apGraphics,cResources *apResources)
    : iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(),
                       apResources->GetLowLevelSystem())
{
    mpGraphics = apGraphics;
    mpResources = apResources;
}

cParticleManager::~cParticleManager()
{
    tResourceBaseMapIt it = m_mapResources.begin();
    for(; it != m_mapResources.end(); ++it)
    {
        iResourceBase* pResource = it->second;
        while(pResource->HasUsers()) pResource->DecUserCount();
    }

    DestroyUnused(0);

    Log(" Done with particles\n");


}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cParticleSystem* cParticleManager::CreatePS(const tString& asName,const tString& asType, cVector3f avSize)
{
    cParticleSystemData *pData = NULL;

    tString sFile = cString::SetFileExt(asType,"ps");
    tWString sPath = mpFileSearcher->GetFilePath(sFile);

    BeginLoad(asName);

    pData = static_cast<cParticleSystemData*>(GetResource(sPath));
    if(pData == NULL)
    {
        if(sPath == _W(""))
        {
            Error("Couldn't find particle system file '%s'\n",sFile.c_str());
            EndLoad();
            return NULL;
        }

        cParticleSystemData *pPSData = hplNew( cParticleSystemData, (sFile,    mpResources,mpGraphics) );

        if(pPSData->LoadFromFile(sPath)==false)
        {
            Error("Can't load data from particle system file '%s'\n",cString::To8Char(sPath).c_str());
            hplDelete(pPSData);
            EndLoad();
            return NULL;
        }

        AddData(pPSData);

        pData = pPSData;
    }


    pData->IncUserCount();
    cParticleSystem* pPS = pData->Create(asName,avSize);
    pPS->SetDataName(asType);
    pPS->SetDataSize(avSize);
    pPS->SetParticleManager(this);

    EndLoad();

    return pPS;
}

//-----------------------------------------------------------------------

cParticleSystem* cParticleManager::CreatePS(const tString& asName, const tString& asDataName, cXmlElement* apElement, cVector3f avSize)
{
    cParticleSystemData *pPSData = NULL;

    BeginLoad(asName);

    pPSData = static_cast<cParticleSystemData*>(GetResource(cString::To16Char(asDataName)));

    if(pPSData==NULL)
    {
        pPSData = hplNew( cParticleSystemData,(asDataName,mpResources,mpGraphics) );
        if(pPSData->LoadFromElement(apElement)==false)
        {
            Error("Can't load particle system data '%s'\n",asDataName.c_str());
            hplDelete(pPSData);
            EndLoad();
            return NULL;
        }

        AddData(pPSData);
    }

    pPSData->IncUserCount();
    cParticleSystem* pPS = pPSData->Create(asName,avSize);
    pPS->SetDataName(asDataName);
    pPS->SetDataSize(avSize);
    pPS->SetParticleManager(this);

    EndLoad();

    return pPS;
}

//-----------------------------------------------------------------------

void cParticleManager::AddData(cParticleSystemData *apData)
{
    AddResource(apData);
}

//-----------------------------------------------------------------------

void cParticleManager::Preload(const tString& asFile)
{
    tString sFile = cString::SetFileExt(asFile,"ps");
    tWString sPath = mpFileSearcher->GetFilePath(sFile);

    cParticleSystemData *pData = static_cast<cParticleSystemData*>(GetResource(sPath));
    if(pData == NULL)
    {
        if(sPath == _W(""))
        {
            Error("Couldn't find particle system file '%s'\n",sFile.c_str());
            return;
        }

        cParticleSystemData *pPSData = hplNew( cParticleSystemData, (sFile, mpResources,mpGraphics) );

        if(pPSData->LoadFromFile(sPath)==false)
        {
            Error("Can't load data from particle system file '%s'\n",cString::To8Char(sPath).c_str());
            hplDelete(pPSData);
            return;
        }

        AddData(pPSData);
    }
}

//-----------------------------------------------------------------------

void cParticleManager::Unload(iResourceBase* apResource)
{

}
//-----------------------------------------------------------------------

void cParticleManager::Destroy(iResourceBase* apResource)
{
    if(apResource->HasUsers())
    {
        apResource->DecUserCount();
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
}
