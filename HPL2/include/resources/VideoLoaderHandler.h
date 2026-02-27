#ifndef HPL_VIDEO_LOADER_HANDLER_H
#define HPL_VIDEO_LOADER_HANDLER_H

#include "resources/ResourceLoaderHandler.h"

#include "system/SystemTypes.h"

namespace hpl {
    
    class cResources;
    class cGraphics;
    class iVideoStream;

    //--------------------------------

    class cVideoLoaderHandler : public iResourceLoaderHandler
    {
    public:
        cVideoLoaderHandler(cResources* apResources,cGraphics *apGraphics);
        ~cVideoLoaderHandler();

        iVideoStream* LoadVideo(const tWString& asFile);
    
    private:
        void SetupLoader(iResourceLoader *apLoader);

        cGraphics *mpGraphics;
        cResources* mpResources;
    };

};
#endif // HPL_VIDEO_LOADER_HANDLER_H
