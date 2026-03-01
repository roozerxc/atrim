/*
 *  LusBasePersonal.h
 *  Lux
 *
 *  Created by Edward Rudd on 8/27/10.
 *  Copyright 2010 Frictional Games All rights reserved.
 *
 */

/////////////////////////
// Multi platform personal directory specifics.
#if defined(_WIN32)
#define PERSONAL_RELATIVEROOT _W("")
#define PERSONAL_RELATIVEPIECES
#define PERSONAL_RELATIVEPIECES_COUNT 0
#endif
// roozy: Account for portable backport effort
#define PERSONAL_RELATIVEGAME_PARENT _W("settings/")
#define PERSONAL_RESOURCES _W("local_resources/")
namespace hpl {
inline void SetupBaseDirs(tWStringVec& vDirs, const tWString& asRelativeParent = _W(""), const tWString& asMainFolder = _W(""),
                                        bool userDir = false, const tWString& asCustomStoryPath = _W(""))
{
    vDirs.clear();
#if PERSONAL_RELATIVEPIECES_COUNT > 0
    tWString aDirs[] = { PERSONAL_RELATIVEPIECES };
    for (int i = 0; i < PERSONAL_RELATIVEPIECES_COUNT; ++i) {
        vDirs.push_back(aDirs[i]);
    }
#endif
    if (asRelativeParent.length()) {
        vDirs.push_back(PERSONAL_RELATIVEROOT + asRelativeParent);
        if (asMainFolder.length()) {
            vDirs.push_back(PERSONAL_RELATIVEROOT + asRelativeParent + asMainFolder + _W("/"));
        }
    }
    vDirs.push_back(PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT);
#ifndef HPL_MINIMAL
    // roozy: Account for portable backport effort
    iFileBrowser::msGameDir = cString::AddSlashAtEndW(cPlatform::GetWorkingDir());
    iFileBrowser::msPersonalDir = PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT;
#endif
    if (userDir) {
        vDirs.push_back(PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT PERSONAL_RESOURCES);
        if (asCustomStoryPath.length()) {
            vDirs.push_back(PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT PERSONAL_RESOURCES
                            + asCustomStoryPath + _W("/"));
        }
    }
}

inline void CreateBaseDirs(const tWStringVec& vDirs, const tWString& asRoot)
{
    //Check if directories exist and if not create
    for(tWStringVec::const_iterator it = vDirs.begin(); it != vDirs.end(); ++it)
    {
        tWString sDir = asRoot + (*it);
        if(cPlatform::FolderExists(sDir)) continue;

        cPlatform::CreateFolder(sDir);
    }
}
}
