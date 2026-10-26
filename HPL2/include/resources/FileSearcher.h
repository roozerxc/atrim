#ifndef HPL_FILESEARCHER_H
#define HPL_FILESEARCHER_H

#include <map>
#include "resources/ResourcesTypes.h"
#include "system/SystemTypes.h"

namespace hpl
{

class iLowLevelResources;

//----------------------------------

class cFileSearcherEntry
{
public:
    cFileSearcherEntry(const tWString& asPath);

    tWString msPath;
    tWStringVec mvPathDirs;
};

//----------------------------------

typedef std::multimap<tString, cFileSearcherEntry> tFilePathMap;
typedef tFilePathMap::iterator tFilePathMapIt;

//File name (lower case) and full path of every entry an AddDirectory call added.
typedef std::pair<tString, tWString> tFileSearcherAddedFile;
typedef std::vector<tFileSearcherAddedFile> tFileSearcherAddedFileVec;

//----------------------------------

class cFileSearcher
{
public:
    cFileSearcher();
    ~cFileSearcher();

    /**
     * Adds a directory that will be searched when looking for files.
     * \param asMask What files that should be searched for, for example: "*.jpeg".
     * \param asPath The path to the directory.
     */
    void AddDirectory(const tWString& asSearchPath, const tString& asMask, bool abAddSubDirectories,
                      tFileSearcherAddedFileVec *apAddedFiles = NULL);

    /**
     * Removes files again that an AddDirectory call added (see apAddedFiles). Entries that
     * were already there before that call (same path) were not listed, so they stay.
     */
    void RemoveFiles(const tFileSearcherAddedFileVec& avFiles);

    /**
     * Clears all directories
     */
    void ClearDirectories();

    /**
     * Gets a file pointer and searches through all added resources.
     * \param asName Name of the file.
     * \return Path to the file. "" if file is not found.
     */
    const tWString &GetFilePath(const tString& asFileNameAndPath, int *apEqualCount = NULL);

private:
    tFilePathMap m_mapFiles;
    tWStringSet m_setLoadedDirs;

    tWString msNull;
};

};
#endif // HPL_FILESEARCHER_H
