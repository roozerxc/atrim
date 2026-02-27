#ifndef HPL_LANGUAGE_FILE_H
#define HPL_LANGUAGE_FILE_H

#include <map>
#include "system/SystemTypes.h"

namespace hpl {

    class cResources;

    //--------------------------------

    class cLanguageEntry
    {
    public:
        tWString mwsText;        
    };

    typedef std::map<tString, cLanguageEntry*> tLanguageEntryMap;
    typedef tLanguageEntryMap::iterator tLanguageEntryMapIt;

    //--------------------------------

    class cLanguageCategory
    {
    public:
        ~cLanguageCategory(){
            STLMapDeleteAll(m_mapEntries);
        }

        tLanguageEntryMap m_mapEntries;
    };

    typedef std::map<tString, cLanguageCategory*> tLanguageCategoryMap;
    typedef tLanguageCategoryMap::iterator tLanguageCategoryMapIt;

    //--------------------------------

    class cLanguageFile
    {
    public:
        cLanguageFile(cResources *apResources);
        ~cLanguageFile();
        
        bool AddFromFile(const tWString& asFile, bool abAddResourceDirs, const tWString& asAltPath = _W(""));
        
        const tWString& Translate(const tString& asCat, const tString& asName);

        tLanguageCategoryMap* GetCategoryMap(){ return &m_mapCategories;}
        
    private:
        tLanguageCategoryMap m_mapCategories;    
        tWString mwsEmpty;

        cResources *mpResources;
    };

};
#endif // HPL_LANGUAGE_FILE_H
