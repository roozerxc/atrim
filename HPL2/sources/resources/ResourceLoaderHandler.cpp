#include "resources/ResourceLoaderHandler.h"

#include "resources/ResourceLoader.h"
#include "system/String.h"
#include "system/LowLevelSystem.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iResourceLoaderHandler::~iResourceLoaderHandler()
{
    STLDeleteAll(mlstLoaders);
}
//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iResourceLoaderHandler::AddLoader(iResourceLoader *apLoader)
{
    mlstLoaders.push_back(apLoader);

    //call implemented method to set it up.
    SetupLoader(apLoader);

    //Add all supported types from the loader
    tStringListIt it = apLoader->mlstExtensions.begin();
    for(; it != apLoader->mlstExtensions.end(); ++it)
    {
        mvSupportedTypes.push_back(*it);
    }
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iResourceLoader* iResourceLoaderHandler::GetLoaderForFile(const tWString& asFileName)
{
    return GetLoaderForFile(cString::To8Char(asFileName));
}

//-----------------------------------------------------------------------

iResourceLoader* iResourceLoaderHandler::GetLoaderForFile(const tString& asFileName)
{
    tString sLowExt = cString::ToLowerCase(cString::GetFileExt(asFileName));

    tResourceLoaderListIt it = mlstLoaders.begin();
    for(; it != mlstLoaders.end(); it++)
    {
        iResourceLoader *pLoader = *it;

        if(pLoader->IsSupported(sLowExt))
        {
            return pLoader;
        }
    }
    Error("No loader for file extension '%s' found!\n",sLowExt.c_str());
    return NULL;
}

//-----------------------------------------------------------------------

}
