#ifndef HPL_SOUND_ENTITY_MANAGER_H
#define HPL_SOUND_ENTITY_MANAGER_H

#include "resources/ResourceManager.h"

namespace hpl
{

class cSound;
class cResources;
class cSoundEntityData;

class cSoundEntityManager : public iResourceManager
{
public:
    cSoundEntityManager(cSound* apSound,cResources *apResources);
    ~cSoundEntityManager();

    void Preload(const tString& asFile);

    cSoundEntityData* CreateSoundEntity(const tString& asName);

    void Destroy(iResourceBase* apResource);
    void Unload(iResourceBase* apResource);

private:
    cSound* mpSound;
    cResources* mpResources;
};

};
#endif // HPL_SOUND_ENTITY_MANAGER_H
