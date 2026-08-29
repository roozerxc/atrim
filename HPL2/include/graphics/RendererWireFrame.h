#ifndef HPL_RENDERER_WIRE_FRAME_H
#define HPL_RENDERER_WIRE_FRAME_H

#include "graphics/Renderer.h"

namespace hpl
{

//---------------------------------------------

class iFrameBuffer;
class iDepthStencilBuffer;
class iTexture;
class iLight;

//---------------------------------------------

class cRendererWireFrame : public  iRenderer
{
public:
    cRendererWireFrame(cGraphics *apGraphics,cResources* apResources);
    ~cRendererWireFrame();

    bool LoadData();
    void DestroyData();

private:
    void CopyToFrameBuffer();
    void SetupRenderList();
    void RenderObjects();
};

//---------------------------------------------

};
#endif // HPL_RENDERER_WIRE_FRAME_H
