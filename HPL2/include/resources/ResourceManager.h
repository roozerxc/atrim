#ifndef HPL_RESOURCEMANAGER_H
#define HPL_RESOURCEMANAGER_H

#include <map>
#include "system/SystemTypes.h"

namespace hpl {

    class iLowLevelResources;
    class iLowLevelSystem;
    class cFileSearcher;
    class iResourceBase;

    typedef std::multimap<unsigned int, iResourceBase*> tResourceBaseMap;
    typedef tResourceBaseMap::iterator tResourceBaseMapIt;

    typedef std::list<iResourceBase*> tResourceBaseList;
    typedef tResourceBaseList::iterator tResourceBaseListIt;

    typedef cSTLMapIterator<iResourceBase*, tResourceBaseMap, tResourceBaseMapIt> cResourceBaseIterator;
    
    //------------------------------------------

    class iResourceManager
    {
    public:
        iResourceManager(cFileSearcher *apFileSearcher, iLowLevelResources *apLowLevelResources,
                        iLowLevelSystem *apLowLevelSystem);
        virtual ~iResourceManager(){}

        iResourceBase* GetResource(const tWString& asFullPath);
        
        cResourceBaseIterator GetResourceBaseIterator();

        void DestroyUnused(int alMaxToKeep);

        virtual void Destroy(iResourceBase* apResource)=0;
        virtual void DestroyAll();

        virtual void Unload(iResourceBase* apResource)=0;
        
        virtual void Update(float afTimeStep){}
        
    protected:
        tResourceBaseMap m_mapResources;
        
        cFileSearcher *mpFileSearcher;
        iLowLevelResources *mpLowLevelResources;
        iLowLevelSystem *mpLowLevelSystem;

        void BeginLoad(const tString& asFile);
        void EndLoad();
        
        unsigned long mlTimeStart;

        /**
         * Checks if a resource alllready is in the manager, else searches the resources.
         * \param &asName Name of the resource.
         * \param &asFilePath If the file is not in the manager, the path is put here. "" if there is no such file.
         * \return A pointer to the resource. NULL if not in manager.
         */
        iResourceBase* FindLoadedResource(const tString &asName, tWString &asFilePath, int *apEqualCount=NULL);
        void AddResource(iResourceBase* apResource, bool abLog=true, bool abAddToSet=true);
        void RemoveResource(iResourceBase* apResource);

        tString GetTabs();
        static int mlTabCount;

    };

};
#endif // HPL_RESOURCEMANAGER_H
