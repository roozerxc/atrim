#ifndef HPL_RESOURCE_LOADER_H
#define HPL_RESOURCE_LOADER_H

#include "system/SystemTypes.h"
#include "resources/ResourcesTypes.h"

namespace hpl
{

class iResourceLoader
{
    friend class iResourceLoaderHandler;
public:
    virtual ~iResourceLoader() {}

    bool IsSupported(const tString& asFileExt);

    void AddSupportedExtension(const tString& asExt);

protected:
    tStringList mlstExtensions;
};

};
#endif // HPL_RESOURCE_LOADER_H
