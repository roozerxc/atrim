#include "impl/LowLevelResourcesSDL.h"
#include "impl/MeshLoaderMSH.h"
#include "impl/MeshLoaderFBX.h"
#include "impl/MeshLoaderCollada.h"
#include "impl/VideoStreamTheora.h"
#include "impl/XmlDocumentTiny.h"
#include "impl/BitmapLoaderDevilDDS.h"
#include "impl/BitmapLoaderDevilMisc.h"

#include "system/String.h"

#include "resources/MeshLoaderHandler.h"
#include "resources/VideoLoaderHandler.h"
#include "resources/BitmapLoaderHandler.h"

namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cLowLevelResourcesSDL::cLowLevelResourcesSDL(iLowLevelGraphics *apLowLevelGraphics)
    {
        mpLowLevelGraphics = apLowLevelGraphics;
    }

    //-----------------------------------------------------------------------

    cLowLevelResourcesSDL::~cLowLevelResourcesSDL()
    {

    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHOD
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    void cLowLevelResourcesSDL::AddBitmapLoaders(cBitmapLoaderHandler* apHandler)
    {
        apHandler->AddLoader(hplNew( cBitmapLoaderDevilDDS,()));
        apHandler->AddLoader(hplNew( cBitmapLoaderDevilMisc,()));
    }

    //-----------------------------------------------------------------------

    void cLowLevelResourcesSDL::AddMeshLoaders(cMeshLoaderHandler* apHandler)
    {
        cMeshLoaderMSH *pLoaderMSH = hplNew( cMeshLoaderMSH,(mpLowLevelGraphics));
        apHandler->AddLoader(pLoaderMSH);
        apHandler->AddLoader(hplNew( cMeshLoaderCollada,(mpLowLevelGraphics, pLoaderMSH, true)));
        //apHandler->AddLoader(hplNew( cMeshLoaderFBX,(mpLowLevelGraphics, pLoaderMSH, true)));
    }

    //-----------------------------------------------------------------------

    void cLowLevelResourcesSDL::AddVideoLoaders(cVideoLoaderHandler* apHandler)
    {
        apHandler->AddLoader(hplNew( cVideoStreamTheora_Loader,()));
    }

    //-----------------------------------------------------------------------
    
    iXmlDocument* cLowLevelResourcesSDL::CreateXmlDocument(const tString& asName)
    {
        return hplNew( cXmlDocumentTiny,(asName) );
    }

    //-----------------------------------------------------------------------

}
