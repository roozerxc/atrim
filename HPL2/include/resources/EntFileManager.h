#ifndef HPL_ENT_FILE_MANAGER_H
#define HPL_ENT_FILE_MANAGER_H

#include "resources/ResourceManager.h"
#include "resources/ResourceBase.h"

namespace hpl {
    
    class cResources;
    class iXmlDocument;

    //------------------------------------
    
    class cEntFile : public iResourceBase 
    {
    public:
        cEntFile(const tString& asName, const tWString& asFullPath, cResources *apResources);
        ~cEntFile();

        bool CreateFromFile();

        iXmlDocument *GetXmlDoc(){ return mpXmlDoc;}

        //resources stuff.
        bool Reload(){ return false;}
        void Unload(){}
        void Destroy(){}
        
    private:
        iXmlDocument *mpXmlDoc;
    };


    //------------------------------------
    
    class cEntFileManager : public iResourceManager
    {
    public:
        cEntFileManager(cResources *apResources);
        ~cEntFileManager();

        cEntFile* CreateEntFile(const tString& asName);

        void Destroy(iResourceBase* apResource);
        void Unload(iResourceBase* apResource);

    private:
        cResources *mpResources;
    };

};
#endif // HPL_ENT_FILE_MANAGER_H
