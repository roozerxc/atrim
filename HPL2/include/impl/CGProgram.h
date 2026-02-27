#ifndef HPL_CGPROGRAM_H
#define HPL_CGPROGRAM_H

//#include <windows.h>

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GPUShader.h"

#include "graphics/GPUProgram.h"

namespace hpl {

    class cCGProgram : public iGpuProgram
    {
    public:
        cCGProgram();

        bool Link();

        void Bind();
        void UnBind();
        
    protected:
    };
};
#endif // HPL_CGPROGRAM_H
