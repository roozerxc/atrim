#ifndef HPL_DUMMY_RENDERABLE_H
#define HPL_DUMMY_RENDERABLE_H

#include "graphics/GraphicsTypes.h"
#include "graphics/Renderable.h"

namespace hpl
{

//------------------------------------------

class cCamera;
class cFrustum;
class iTexture;
class cResources;

//------------------------------------------

class cDummyRenderable : public iRenderable
{
public:
    cDummyRenderable(tString asName);
    virtual ~cDummyRenderable();

    //////////////////////////////
    //Properties

    //////////////////////////////
    //iEntity implementation
    tString GetEntityType()
    {
        return "cDummy";
    }

    ///////////////////////////////
    //Renderable implementation:
    cMaterial *GetMaterial()
    {
        return NULL;
    }
    iVertexBuffer* GetVertexBuffer()
    {
        return NULL;
    }

    eRenderableType GetRenderType()
    {
        return eRenderableType_Dummy;
    }

    int GetMatrixUpdateCount()
    {
        return GetTransformUpdateCount();
    }
    cMatrixf* GetModelMatrix(cFrustum* apFrustum);

private:
    cMatrixf m_mtxModelOutput;
};

};
#endif // HPL_DUMMY_RENDERABLE_H
