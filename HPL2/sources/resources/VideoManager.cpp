#include "resources/VideoManager.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "resources/Resources.h"
#include "resources/FileSearcher.h"
#include "graphics/VideoStream.h"
#include "resources/VideoLoaderHandler.h"

namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cVideoManager::cVideoManager(cGraphics* apGraphics,cResources *apResources)
        : iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(),
                            apResources->GetLowLevelSystem())
    {
        mpGraphics = apGraphics;
        mpResources = apResources;
    }

    cVideoManager::~cVideoManager()
    {
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    iVideoStream* cVideoManager::CreateVideo(const tString& asName)
    {
        BeginLoad(asName);

        tWString sPath = mpFileSearcher->GetFilePath(asName);
        if(sPath == _W("")) 
        {
            EndLoad();
            Error("Video file '%s' could not be found!\n",asName.c_str());
            return NULL;
        }

        iVideoStream *pVideo = mpResources->GetVideoLoaderHandler()->LoadVideo(sPath);
        pVideo->SetFullPath(sPath);

        if(pVideo == NULL)
        {
            EndLoad();
            Error("Could not load video '%s'\n",asName.c_str());
            return NULL;
        }

        AddResource(pVideo);

        EndLoad();
        return pVideo;
    }

    //-----------------------------------------------------------------------

    void cVideoManager::Unload(iResourceBase* apResource)
    {

    }
    //-----------------------------------------------------------------------

    void cVideoManager::Destroy(iResourceBase* apResource)
    {
        if(apResource)
        {
            RemoveResource(apResource);
            hplDelete(apResource);
        }
    }

    //-----------------------------------------------------------------------

    void cVideoManager::Update(float afTimeStep)
    {
        tResourceBaseMapIt it = m_mapResources.begin();
        for(; it != m_mapResources.end(); ++it)
        {
            iResourceBase *pBase = it->second;
            iVideoStream *pVideo = static_cast<iVideoStream*>(pBase);

            pVideo->Update(afTimeStep);
        }

    }
    
    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PRIVATE METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------
}
