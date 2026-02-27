#ifndef HPL_OCCLUSION_QUERY_OGL_H
#define HPL_OCCLUSION_QUERY_OGL_H

#include "graphics/OcclusionQuery.h"

namespace hpl {

    class cOcclusionQueryOGL : public iOcclusionQuery
    {
    public:
        cOcclusionQueryOGL();
        ~cOcclusionQueryOGL();

        void Begin();
        void End();
        bool FetchResults();
        unsigned int GetSampleCount();

    public:
        bool mbResultsAreFetched;
        int mlLastSampleCount;
        unsigned int mlQueryId;
    };

};
#endif // HPL_OCCLUSION_QUERY_H
