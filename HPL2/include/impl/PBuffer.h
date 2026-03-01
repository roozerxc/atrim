#ifndef HPL_P_BUFFER_H
#define HPL_P_BUFFER_H

#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#endif
#include <vector>


#include "graphics/GraphicsTypes.h"
namespace hpl {
    class iLowLevelGraphics;

    class cPBuffer
    {
    public:
        cPBuffer(iLowLevelGraphics* apLowLevelGraphics,bool abShareObjects,bool abUseMipMaps=false,bool abUseDepth=true,bool abUseStencil=true);
        ~cPBuffer();

        bool Init(unsigned int alWidth,unsigned int alHeight, cColor aCol);

        int MakeCurrentContext();

        void Bind();
        void UnBind();

    private:
        #ifdef _WIN32
        HDC         mDeviceContext;
        HGLRC       mGLContext;
        HPBUFFERARB mPBuffer;
        #endif

        int mlWidth;
        int mlHeight;

        bool mbShareObjects;
        iLowLevelGraphics* mpLowLevelGraphics;

        std::vector<int> mvAttribBuffer;
        std::vector<int> mvAttribFormat;
    };

};
#endif // HPL_P_BUFFER_H
