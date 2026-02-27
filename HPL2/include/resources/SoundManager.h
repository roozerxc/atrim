#ifndef HPL_SOUND_MANAGER_H
#define HPL_SOUND_MANAGER_H

#include "resources/ResourceManager.h"

namespace hpl {

    class cSound;
    class cResources;
    class iSoundData;

    typedef std::list<iSoundData*> tSoundDataList;
    typedef tSoundDataList::iterator tSoundDataListIt;

    class cSoundManager : public iResourceManager
    {
    public:
        cSoundManager(cSound* apSound,cResources *apResources);
        ~cSoundManager();

        iSoundData* CreateSoundData(const tString& asName, bool abStream, bool abLoopStream=false);

        void Destroy(iResourceBase* apResource);
        void Unload(iResourceBase* apResource);

        void DestroyAll();

    private:
        cSound* mpSound;
        cResources *mpResources;

        tStringList mlstFileFormats;

        tSoundDataList mlstStreamData;

        iSoundData *FindSampleData(const tString &asName, tWString &asFilePath);
        void FindStreamPath(const tString &asName, tWString &asFilePath);

    };

};
#endif // HPL_SOUND_MANAGER_H
