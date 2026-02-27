#ifndef HPL_RESOURCE_LOADER_HANDLER_H
#define HPL_RESOURCE_LOADER_HANDLER_H

#include "system/SystemTypes.h"

namespace hpl {

    class iResourceLoader;

    typedef std::list<iResourceLoader*> tResourceLoaderList;
    typedef tResourceLoaderList::iterator tResourceLoaderListIt;
    
    //------------------------------------

    class iResourceLoaderHandler
    {
    public:
        virtual ~iResourceLoaderHandler();

        void AddLoader(iResourceLoader *apLoader);

        tStringVec* GetSupportedTypes(){ return &mvSupportedTypes;}
        
        iResourceLoader* GetLoaderForFile(const tString& asFileName);
        iResourceLoader* GetLoaderForFile(const tWString& asFileName);

    protected:
        virtual void SetupLoader(iResourceLoader *apLoader)=0;

        tStringVec mvSupportedTypes;

        tResourceLoaderList mlstLoaders;
    };

};
#endif // HPL_RESOURCE_LOADER_HANDLER_H
