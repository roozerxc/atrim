#ifndef HPL_LOWLEVELRESOURCES_H
#define HPL_LOWLEVELRESOURCES_H

#include "system/SystemTypes.h"

namespace hpl
{

class cMeshLoaderHandler;
class cBitmapLoaderHandler;
class cVideoLoaderHandler;
class iXmlDocument;

class iLowLevelResources
{
public:
    virtual ~iLowLevelResources() {}

    virtual void AddBitmapLoaders(cBitmapLoaderHandler* apHandler)=0;
    virtual void AddMeshLoaders(cMeshLoaderHandler* apHandler)=0;
    virtual void AddVideoLoaders(cVideoLoaderHandler* apHandler)=0;

    virtual iXmlDocument* CreateXmlDocument(const tString& asName="")=0;
};
};
#endif // HPL_LOWLEVELRESOURCES_H
