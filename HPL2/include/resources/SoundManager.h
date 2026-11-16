#ifndef HPL_SOUND_MANAGER_H
#define HPL_SOUND_MANAGER_H

#include "resources/ResourceManager.h"

#include <map>
#include <set>

namespace hpl
{

class cSound;
class cResources;
class iSoundData;

typedef std::list<iSoundData *> tSoundDataList;
typedef tSoundDataList::iterator tSoundDataListIt;

class cSoundManager : public iResourceManager
{
public:
    cSoundManager(cSound* apSound, cResources *apResources);
    ~cSoundManager();

    iSoundData *CreateSoundData(const tString& asName, bool abStream, bool abLoopStream = false);

    void Destroy(iResourceBase* apResource);
    void Unload(iResourceBase* apResource);

    void DestroyAll();

    /**
     * True if the sound is not loaded yet and its file is at least the auto stream size:
     * it is then streamed instead of being decoded whole the first time it is played.
     */
    bool ShouldAutoStream(const tString& asName);
    //File size in bytes from which sounds are streamed, 0 = off.
    static void SetAutoStreamMinBytes(unsigned long alX)
    {
        mlAutoStreamMinBytes = alX;
    }

    //Names of the (not streamed) sounds played since the last clear, for preloading.
    const std::set<tString>& GetPlayedSamples()
    {
        return msetPlayedSamples;
    }
    void ClearPlayedSamples()
    {
        msetPlayedSamples.clear();
    }
    void AddPlayedSample(const tString& asName)
    {
        msetPlayedSamples.insert(asName);
    }

private:
    static unsigned long mlAutoStreamMinBytes;
    std::map<tString, bool> m_mapAutoStream;
    std::set<tString> msetPlayedSamples;

    cSound *mpSound;
    cResources *mpResources;

    tStringList mlstFileFormats;

    tSoundDataList mlstStreamData;

    iSoundData *FindSampleData(const tString &asName, tWString &asFilePath);
    void FindStreamPath(const tString &asName, tWString &asFilePath);

};

};
#endif // HPL_SOUND_MANAGER_H
