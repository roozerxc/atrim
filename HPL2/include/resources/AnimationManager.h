#ifndef HPL_ANIMATION_MANAGER_H
#define HPL_ANIMATION_MANAGER_H

#include "resources/ResourceManager.h"

namespace hpl {

    class cGraphics;
    class cResources;
    class cAnimation;

    class cAnimationManager : public iResourceManager
    {
    public:
        cAnimationManager(cGraphics* apGraphics,cResources *apResources);
        ~cAnimationManager();

        cAnimation* CreateAnimation(const tString& asName);

        void Destroy(iResourceBase* apResource);
        void Unload(iResourceBase* apResource);

    private:
        cGraphics* mpGraphics;
        cResources* mpResources;
    };

};
#endif // HPL_ANIMATION_MANAGER_H
