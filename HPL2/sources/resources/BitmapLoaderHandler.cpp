#include "resources/BitmapLoaderHandler.h"

#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "graphics/Graphics.h"

#include "graphics/Bitmap.h"
#include "resources/BitmapLoader.h"


namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cBitmapLoaderHandler::cBitmapLoaderHandler(cResources* apResources, cGraphics* apGraphics)
{
    mpResources = apResources;
    mpGraphics = apGraphics;
}

//-----------------------------------------------------------------------

cBitmapLoaderHandler::~cBitmapLoaderHandler()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cBitmap* cBitmapLoaderHandler::LoadBitmap(const tWString& asFile, tBitmapLoadFlag aFlags)
{
    iBitmapLoader *pBitmapLoader = static_cast<iBitmapLoader*>(GetLoaderForFile(asFile));

    if(pBitmapLoader)
    {
        cBitmap* pBitmap = pBitmapLoader->LoadBitmap(asFile, aFlags);

        //Set name of the file loaded.
        if(pBitmap) pBitmap->SetFileName(cString::GetFileNameW(asFile));

        return pBitmap;
    }
    else
    {
        return NULL;
    }
}

//-----------------------------------------------------------------------

bool cBitmapLoaderHandler::SaveBitmap(cBitmap* apBitmap, const tWString& asFile, tBitmapSaveFlag aFlags)
{
    iBitmapLoader *pBitmapLoader = static_cast<iBitmapLoader*>(GetLoaderForFile(asFile));

    if(pBitmapLoader)
    {
        return pBitmapLoader->SaveBitmap(apBitmap,asFile,aFlags);
    }
    return false;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cBitmapLoaderHandler::SetupLoader(iResourceLoader *apLoader)
{
    iBitmapLoader *pBitmapLoader = static_cast<iBitmapLoader*>(apLoader);

    pBitmapLoader->mpLowLevelGraphics = mpGraphics->GetLowLevel();
}

//-----------------------------------------------------------------------
}
