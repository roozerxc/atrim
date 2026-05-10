#ifndef HPL_VIDEO_MANAGER_H
#define HPL_VIDEO_MANAGER_H

#include "resources/ResourceManager.h"

namespace hpl
{

class cResources;
class cGraphics;
class iVideoStream;
class iVideoStreamLoader;

//----------------------------------------------------

typedef std::list<iVideoStream*> tVideoStreamList;
typedef tVideoStreamList::iterator tVideoStreamListIt;

//----------------------------------------------------

class cVideoManager : public iResourceManager
{
public:
    cVideoManager(cGraphics* apGraphics,cResources *apResources);
    ~cVideoManager();

    iVideoStream* CreateVideo(const tString& asName);

    void Destroy(iResourceBase* apResource);
    void Unload(iResourceBase* apResource);

    void Update(float afTimeStep);
private:
    cGraphics* mpGraphics;
    cResources *mpResources;
};

};
#endif // HPL_VIDEO_MANAGER_H
