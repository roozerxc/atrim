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

    bool mbPreferred; //Inside the preferred directory (see cFileSearcher::SetPreferredDirectory)
    bool mbProtected; //Inside a protected directory (see cFileSearcher::AddProtectedDirectory)
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

    /**
     * While set, files inside this directory (and its sub directories) win over files with the
     * same name elsewhere when both match the wanted path equally well, or when no path is given.
     * This is what putting the directory first in resources.cfg used to do. Used for the current
     * custom story, so it can replace base game files.
     * \param asDir The directory, relative or full. "" turns it off.
     */
    void SetPreferredDirectory(const tWString& asDir);
    const tWString& GetPreferredDirectory() const { return msPreferredDir; }

    /**
     * When true, a file in the preferred directory wins even over a file that matches the wanted
     * path better. Used only while player hands and hand objects load, where custom stories keep
     * their replacement files in any folder.
     */
    void SetPreferredDirectoryBeatsBetterMatch(bool abX) { mbPreferredBeatsBetterMatch = abX; }
    bool GetPreferredDirectoryBeatsBetterMatch() const { return mbPreferredBeatsBetterMatch; }

    /**
     * Files with a name that also exists inside this directory (and its sub directories) are
     * never replaced through the preferred directory. Used for the game's core folder (shaders,
     * core models and textures), which custom stories must not replace with old copies.
     * \param asDir The directory, relative or full.
     */
    void AddProtectedDirectory(const tWString& asDir);

private:
    static tWString ToLowDirPath(const tWString& asDir);
    static bool IsInLowDir(const tWString& asPath, const tWString& asLowDir);
    void UpdateEntryFlags(cFileSearcherEntry& aEntry) const;
    void UpdateAllEntryFlags();

    tFilePathMap m_mapFiles;
    tWStringSet m_setLoadedDirs;

    tWString msPreferredDir;    //As passed to SetPreferredDirectory (for GetPreferredDirectory)
    tWString msPreferredDirLow; //Full path, "/" separators, lower case, ending with "/". "" when off.
    bool mbPreferredBeatsBetterMatch;
    tWStringVec mvProtectedDirsLow; //Same form as msPreferredDirLow
    tWStringSet m_setLoggedPreferredFiles; //Logged once each, cleared when the directory changes

    tWString msNull;
};

};
#endif // HPL_FILESEARCHER_H
