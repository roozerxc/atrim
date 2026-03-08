#ifndef HPL_VIDEO_LOADER_H
#define HPL_VIDEO_LOADER_H

#include "resources/ResourceLoader.h"

namespace hpl
{

class iVideoStream;
class iLowLevelGraphics;

//----------------------------------------

class iVideoLoader : public iResourceLoader
{
    friend class cVideoLoaderHandler;
public:

    virtual iVideoStream* LoadVideo(const tWString& asFile)=0;

protected:
    iLowLevelGraphics *mpLowLevelGraphics;
};

};
#endif // HPL_VIDEO_LOADER_H
