#ifndef HPL_RENDERER_SIMPLE_H
#define HPL_RENDERER_SIMPLE_H

#include "graphics/Renderer.h"

namespace hpl {

    //---------------------------------------------
    
    class iFrameBuffer;
    class iDepthStencilBuffer;
    class iTexture;
    class iLight;
    
    //---------------------------------------------
    
    class cRendererSimple : public  iRenderer
    {
    public:
        cRendererSimple(cGraphics *apGraphics,cResources* apResources);
        ~cRendererSimple();
        
        bool LoadData();
        void DestroyData();

        static void SetUseShaders(bool abX){mbUseShaders = abX; }
        static bool GetUseShaders(){return mbUseShaders; }

    private:
        void CopyToFrameBuffer();
        void SetupRenderList();
        void RenderObjects();

        static bool mbUseShaders;

        iGpuProgram *mpFlatProgram;
        iGpuProgram *mpDiffuseProgram;
    };

    //---------------------------------------------

};
#endif // HPL_RENDERER_WIRE_FRAME_H
