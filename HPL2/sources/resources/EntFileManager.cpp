#include "resources/EntFileManager.h"

#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "resources/LowLevelResources.h"
#include "resources/XmlDocument.h"


namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // ENT FILE
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    
    cEntFile::cEntFile(const tString& asName, const tWString& asFullPath, cResources *apResources) : iResourceBase(asName, asFullPath, 0)
    {
        mpXmlDoc = apResources->GetLowLevel()->CreateXmlDocument(asName);
    }
    cEntFile::~cEntFile()
    {
        hplDelete(mpXmlDoc);
    }

    bool cEntFile::CreateFromFile()
    {
        return mpXmlDoc->CreateFromFile(GetFullPath());
    }

    //-----------------------------------------------------------------------


    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cEntFileManager::cEntFileManager(cResources *apResources)
        : iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(), apResources->GetLowLevelSystem())
    {
        mpResources = apResources;
    }

    cEntFileManager::~cEntFileManager()
    {
        DestroyAll();
        Log(" Done with ent files\n");
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cEntFile* cEntFileManager::CreateEntFile(const tString& asName)
    {
        tWString sPath;
        cEntFile* pEntFile;
        tString asNewName = cString::SetFileExt(cString::ToLowerCase(asName), "ent");

        BeginLoad(asName);
        
        pEntFile = static_cast<cEntFile*>(this->FindLoadedResource(asNewName,sPath));

        if(pEntFile==NULL && sPath!=_W(""))
        {
            pEntFile = hplNew(cEntFile, (asNewName, sPath.c_str(), mpResources));
            if(pEntFile->CreateFromFile()==false)
            {
                Error("Couldn't load entity file '%s'!\n",cString::To8Char(sPath).c_str());
                hplDelete(pEntFile);

                EndLoad();
                return NULL;
            }
            
            AddResource(pEntFile);
        }
        
        if(pEntFile)
            pEntFile->IncUserCount();
        else
            Error("Couldn't create ent file '%s'\n",asNewName.c_str());
        
        EndLoad();
        return pEntFile;
    }

    //-----------------------------------------------------------------------

    void cEntFileManager::Unload(iResourceBase* apResource)
    {

    }
    //-----------------------------------------------------------------------

    void cEntFileManager::Destroy(iResourceBase* apResource)
    {
        apResource->DecUserCount();

        if(apResource->HasUsers()==false){
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
