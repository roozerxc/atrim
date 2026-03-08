#include "resources/MeshLoaderHandler.h"

#include "resources/MeshLoader.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "scene/Scene.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cMeshLoaderHandler::cMeshLoaderHandler(cResources* apResources, cScene *apScene)
{
    mpResources = apResources;
    mpScene = apScene;
}

//-----------------------------------------------------------------------

cMeshLoaderHandler::~cMeshLoaderHandler()
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cMesh* cMeshLoaderHandler::LoadMesh(const tWString& asFile,tMeshLoadFlag aFlags)
{
    iMeshLoader *pMeshLoader = static_cast<iMeshLoader*>(GetLoaderForFile(asFile));

    if(pMeshLoader)
    {
        return pMeshLoader->LoadMesh(asFile,aFlags);
    }
    return NULL;
}

//-----------------------------------------------------------------------

bool cMeshLoaderHandler::SaveMesh(cMesh* apMesh,const tWString& asFile)
{
    iMeshLoader *pMeshLoader = static_cast<iMeshLoader*>(GetLoaderForFile(asFile));

    if(pMeshLoader)
    {
        return pMeshLoader->SaveMesh(apMesh,asFile);
    }
    return false;
}

//-----------------------------------------------------------------------
cAnimation* cMeshLoaderHandler::LoadAnimation(const tWString& asFile)
{
    iMeshLoader *pMeshLoader = static_cast<iMeshLoader*>(GetLoaderForFile(asFile));

    if(pMeshLoader)
    {
        return pMeshLoader->LoadAnimation(asFile);
    }
    else
    {
        return NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


void cMeshLoaderHandler::SetupLoader(iResourceLoader *apLoader)
{
    iMeshLoader *pMeshLoader = static_cast<iMeshLoader*>(apLoader);

    pMeshLoader->mpMaterialManager = mpResources->GetMaterialManager();
    pMeshLoader->mpMeshManager = mpResources->GetMeshManager();
    pMeshLoader->mpAnimationManager = mpResources->GetAnimationManager();
}

//-----------------------------------------------------------------------
}
