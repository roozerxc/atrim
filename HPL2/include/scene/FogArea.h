#ifndef HPL_FOG_AREA_H
#define HPL_FOG_AREA_H

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

class cFogArea : public iRenderable
{
public:
    cFogArea(tString asName, cResources *apResources);
    virtual ~cFogArea();

    //////////////////////////////
    //Properties
    const cColor& GetColor()
    {
        return mColor;
    }
    void SetColor(const cColor& aCol)
    {
        mColor = aCol;
    }

    const cVector3f& GetSize()
    {
        return mvSize;
    }
    void SetSize(const cVector3f& avSize);

    void SetStart(float afX)
    {
        mfStart = afX;
    }
    void SetEnd(float afX)
    {
        mfEnd = afX;
    }

    float GetStart()
    {
        return mfStart;
    }
    float GetEnd()
    {
        return mfEnd;
    }

    void SetFalloffExp(float afX)
    {
        mfFalloffExp = afX;
    }
    float GetFalloffExp()
    {
        return mfFalloffExp;
    }

    void SetShowBacksideWhenOutside(bool abX)
    {
        mbShowBacksideWhenOutside=abX;
    }
    void SetShowBacksideWhenInside(bool abX)
    {
        mbShowBacksideWhenInside=abX;
    }
    bool GetShowBacksideWhenOutside()
    {
        return mbShowBacksideWhenOutside;
    }
    bool GetShowBacksideWhenInside()
    {
        return mbShowBacksideWhenInside;
    }

    //////////////////////////////
    //iEntity implementation
    tString GetEntityType()
    {
        return "cFogArea";
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
        return eRenderableType_FogArea;
    }

    int GetMatrixUpdateCount()
    {
        return GetTransformUpdateCount();
    }
    cMatrixf* GetModelMatrix(cFrustum* apFrustum);

private:
    cColor mColor;
    cVector3f mvSize;
    float mfStart;
    float mfEnd;
    float mfFalloffExp;

    bool mbShowBacksideWhenOutside;
    bool mbShowBacksideWhenInside;

    cMatrixf m_mtxModelOutput;
};

};
#endif // HPL_FOG_AREA_H
