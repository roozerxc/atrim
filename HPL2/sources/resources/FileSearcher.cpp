#include "resources/FileSearcher.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/Platform.h"

#include "resources/LowLevelResources.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cFileSearcherEntry::cFileSearcherEntry(const tWString& asPath)
{
    msPath = asPath;
    mbPreferred = false;
    mbProtected = false;

    tWString sSepp = _W("/\\");
    cString::GetStringVecW(msPath, mvPathDirs, &sSepp);
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cFileSearcher::cFileSearcher()
{
    msNull = _W("");
    mbPreferredBeatsBetterMatch = false;
}

//-----------------------------------------------------------------------

cFileSearcher::~cFileSearcher()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cFileSearcher::AddDirectory(const tWString& asSearchPath, const tString &asMask, bool abAddSubDirectories,
                                 tFileSearcherAddedFileVec *apAddedFiles)
{
    //Make the path with only "/" and lower case.
    tWString sPath = cString::ReplaceCharToW(asSearchPath, _W("\\"), _W("/"));

    ///////////////////////////////
    //Add all files in directory
    tWStringList lstFileNames;

    cPlatform::FindFilesInDir(lstFileNames, sPath, cString::To16Char(asMask));

    for(tWStringListIt it = lstFileNames.begin(); it != lstFileNames.end(); it++)
    {
        tWString& sFile = *it;
        tString sLowFile = cString::ToLowerCase(cString::To8Char(sFile));
        tWString sFilePath = cString::ReplaceCharToW( cPlatform::GetFullFilePath( cString::SetFilePathW(sFile, sPath)), _W("\\"), _W("/"));;

        //Check if file and path already exist (any of the entries with that name)
        bool bExists = false;
        tFilePathMapIt pathIt = m_mapFiles.lower_bound(sLowFile);
        tFilePathMapIt pathEnd = m_mapFiles.upper_bound(sLowFile);
        for(; pathIt != pathEnd; ++pathIt)
        {
            if(pathIt->second.msPath == sFilePath)
            {
                bExists = true;
                break;
            }
        }
        if(bExists)
        {
            continue;
        }

        //Add file
        //Log("Adding lowercase file: '%s' with path: '%s'\n 8bitHash: %u 16bitHash %u\n", sLowFile.c_str(), cString::To8Char(sFilePath).c_str(),
        //    cString::GetHash(cString::To8Char(sFilePath)), cString::GetHashW(sFilePath));
        cFileSearcherEntry entry(sFilePath);
        UpdateEntryFlags(entry);
        m_mapFiles.insert(tFilePathMap::value_type(sLowFile, entry));
        if(apAddedFiles)
        {
            apAddedFiles->push_back(tFileSearcherAddedFile(sLowFile, sFilePath));
        }
    }

    //////////////////////////////////
    //Search sub directories if set.
    if(abAddSubDirectories)
    {
        tWStringList lstDirNames;
        cPlatform::FindFoldersInDir(lstDirNames, sPath, false);

        for(tWStringListIt it = lstDirNames.begin(); it != lstDirNames.end(); it++)
        {
            tWString sNewPath = cString::SetFilePathW(*it, sPath);

            AddDirectory(sNewPath, asMask, true, apAddedFiles);
        }
    }
}

//-----------------------------------------------------------------------

void cFileSearcher::RemoveFiles(const tFileSearcherAddedFileVec& avFiles)
{
    for(size_t i = 0; i < avFiles.size(); ++i)
    {
        tFilePathMapIt it = m_mapFiles.lower_bound(avFiles[i].first);
        tFilePathMapIt itEnd = m_mapFiles.upper_bound(avFiles[i].first);
        for(; it != itEnd; ++it)
        {
            if(it->second.msPath == avFiles[i].second)
            {
                m_mapFiles.erase(it);
                break;
            }
        }
    }
}

//-----------------------------------------------------------------------

void cFileSearcher::ClearDirectories()
{
    m_mapFiles.clear();
    m_setLoadedDirs.clear();
}

//-----------------------------------------------------------------------

void cFileSearcher::SetPreferredDirectory(const tWString& asDir)
{
    if(asDir == msPreferredDir)
    {
        return;
    }

    msPreferredDir = asDir;
    msPreferredDirLow = asDir == _W("") ? tWString(_W("")) : ToLowDirPath(asDir);
    m_setLoggedPreferredFiles.clear();

    UpdateAllEntryFlags();
}

//-----------------------------------------------------------------------

void cFileSearcher::AddProtectedDirectory(const tWString& asDir)
{
    tWString sLowDir = ToLowDirPath(asDir);
    for(size_t i = 0; i < mvProtectedDirsLow.size(); ++i)
    {
        if(mvProtectedDirsLow[i] == sLowDir)
        {
            return;
        }
    }
    mvProtectedDirsLow.push_back(sLowDir);

    UpdateAllEntryFlags();
}

//-----------------------------------------------------------------------

tWString cFileSearcher::ToLowDirPath(const tWString& asDir)
{
    tWString sFull = cString::ReplaceCharToW(cPlatform::GetFullFilePath(asDir), _W("\\"), _W("/"));
    return cString::AddSlashAtEndW(cString::ToLowerCaseW(sFull));
}

//-----------------------------------------------------------------------

bool cFileSearcher::IsInLowDir(const tWString& asPath, const tWString& asLowDir)
{
    if(asLowDir == _W("") || asPath.size() <= asLowDir.size())
    {
        return false;
    }

    //Entry paths are full paths with "/" separators (see AddDirectory)
    return cString::ToLowerCaseW(asPath.substr(0, asLowDir.size())) == asLowDir;
}

//-----------------------------------------------------------------------

void cFileSearcher::UpdateEntryFlags(cFileSearcherEntry& aEntry) const
{
    aEntry.mbPreferred = IsInLowDir(aEntry.msPath, msPreferredDirLow);

    aEntry.mbProtected = false;
    for(size_t i = 0; i < mvProtectedDirsLow.size(); ++i)
    {
        if(IsInLowDir(aEntry.msPath, mvProtectedDirsLow[i]))
        {
            aEntry.mbProtected = true;
            break;
        }
    }
}

//-----------------------------------------------------------------------

void cFileSearcher::UpdateAllEntryFlags()
{
    for(tFilePathMapIt it = m_mapFiles.begin(); it != m_mapFiles.end(); ++it)
    {
        UpdateEntryFlags(it->second);
    }
}

//-----------------------------------------------------------------------

const tWString &cFileSearcher::GetFilePath(const tString& asFileNameAndPath, int *apEqualCount)
{
    tString sFile = cString::GetFileName(asFileNameAndPath);
    tString sLowName = cString::ToLowerCase(sFile);

    //////////////////////
    //Get the iterator to path
    tFilePathMapIt it = m_mapFiles.find(sLowName);
    if(it == m_mapFiles.end())
    {
        if(apEqualCount)
        {
            *apEqualCount = 0;
        }
        return msNull;
    }

    //////////////////////
    //Count the number of files with same name
    //if 1, just return it.
    size_t lCount = m_mapFiles.count(sLowName);
    if(lCount == 1 && apEqualCount == NULL)
    {
        return it->second.msPath;
    }

    //Only matters when there is more than one file with this name, and none of them is protected
    bool bUsePreferred = msPreferredDirLow != _W("") && lCount > 1;
    if(bUsePreferred)
    {
        tFilePathMapIt protIt = it;
        for(size_t itcount = 0; itcount < lCount; ++itcount, ++protIt)
        {
            if(protIt->second.mbProtected)
            {
                bUsePreferred = false;
                break;
            }
        }
    }

    /////////////////////////////
    //Compare paths
    tString string = cString::GetFilePath(asFileNameAndPath);
    tWString sWantedPath = cString::To16Char(string);
    if(string == "" || sWantedPath == _W(""))
    {
        if(bUsePreferred)
        {
            tFilePathMapIt prefIt = it;
            for(size_t itcount = 0; itcount < lCount; ++itcount, ++prefIt)
            {
                if(prefIt->second.mbPreferred)
                {
                    if(m_setLoggedPreferredFiles.insert(prefIt->second.msPath).second)
                    {
                        Log("  Using '%s' from the preferred directory instead of the base game file\n",
                            cString::To8Char(prefIt->second.msPath).c_str());
                    }
                    return prefIt->second.msPath;
                }
            }
        }
        return it->second.msPath;
    }

    tWStringVec vWantedDirs;
    tWString sSepp = _W("/\\");

    int lBestEqualCount = -1;
    bool bBestPreferred = false;
    bool bOutsidePreferred = false; //A file with this name exists outside the preferred directory
    tFilePathMapIt bestEqualIt = it;

    cString::GetStringVecW(sWantedPath, vWantedDirs, &sSepp);

    //Iterate according to count and compare
    for(size_t itcount = 0; itcount < lCount; ++itcount, ++it)
    {
        ///////////////////////////////
        //Compare the wanted path with current, seeing how many directories are in common

        //Start with the wanted path dir
        int lEqualCount1 = 0;
        int j = (int)it->second.mvPathDirs.size() - 1;
        for(int i = (int)vWantedDirs.size() - 1; (i >= 0 && j >= 0); --j)
        {
            //if equal, increase equal count and go to next wanted dir
            if(vWantedDirs[i] == it->second.mvPathDirs[j])
            {
                lEqualCount1++;
                --i;
            }
        }

        //Start with the available path dir
        int lEqualCount2 = 0;
        j = (int)vWantedDirs.size() - 1;
        for(int i = (int)it->second.mvPathDirs.size() - 1; (i >= 0 && j >= 0); --j)
        {
            //if equal, increase equal count and go to next wanted dir
            if(it->second.mvPathDirs[i] == vWantedDirs[j])
            {
                lEqualCount2++;
                --i;
            }
        }

        int lMaxCount = lEqualCount1 > lEqualCount2 ? lEqualCount1 : lEqualCount2;

        /////////////////////
        //A better equal count wins. On a tie, a file in the preferred directory beats one
        //outside it, otherwise the first one wins. With mbPreferredBeatsBetterMatch, a file in
        //the preferred directory beats one outside it whatever the counts.
        bool bPreferred = bUsePreferred && it->second.mbPreferred;
        if(bPreferred == false)
        {
            bOutsidePreferred = true;
        }

        bool bBetter;
        if(lBestEqualCount >= 0 && bPreferred != bBestPreferred && mbPreferredBeatsBetterMatch)
        {
            bBetter = bPreferred;
        }
        else if(lMaxCount != lBestEqualCount)
        {
            bBetter = lMaxCount > lBestEqualCount;
        }
        else
        {
            bBetter = bPreferred && bBestPreferred == false;
        }

        if(bBetter)
        {
            lBestEqualCount = lMaxCount;
            bBestPreferred = bPreferred;
            bestEqualIt = it;
        }
    }

    if(apEqualCount)
    {
        *apEqualCount = lBestEqualCount;
    }

    if(bBestPreferred && bOutsidePreferred && m_setLoggedPreferredFiles.insert(bestEqualIt->second.msPath).second)
    {
        Log("  Using '%s' from the preferred directory instead of the base game file\n",
            cString::To8Char(bestEqualIt->second.msPath).c_str());
    }

    //Return best fit
    return bestEqualIt->second.msPath;
}

//-----------------------------------------------------------------------

}
