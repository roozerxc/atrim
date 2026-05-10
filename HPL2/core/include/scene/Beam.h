#ifndef HPL_BEAM_H
#define HPL_BEAM_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "scene/Entity3D.h"
#include "graphics/Renderable.h"

namespace hpl
{

class cMaterialManager;
class cResources;
class cGraphics;
class cFileSearcher;
class iLowLevelGraphics;
class cMaterial;
class iVertexBuffer;

//------------------------------------------

class cBeam;
class cBeamEnd : public iEntity3D
{
    friend class cBeam;
    friend class cBeamEnd_UpdateCallback;
public:
    cBeamEnd(const tString asName, cBeam *apBeam) : iEntity3D(asName),
        mColor(1,1),mpBeam(apBeam) {}

    void SetColor(const cColor &aColor);
    const cColor& GetColor()
    {
        return mColor;
    }

    /////////////////////////////////
    //Entity implementation
    tString GetEntityType()
    {
        return "BeamEnd";
    }
private:
    cColor mColor;
    cBeam *mpBeam;
};

//------------------------------------------

class cBeamEnd_UpdateCallback : public iEntityCallback
{
public:
    void OnTransformUpdate(iEntity3D * apEntity);
};

//------------------------------------------

class cBeam : public iRenderable
{
#ifdef __GNUC__
    typedef iRenderable __super;
#endif
    friend class cBeamEnd;
public:
    cBeam(const tString asName, cResources *apResources,cGraphics *apGraphics);
    ~cBeam();

    void SetMaterial(cMaterial * apMaterial);

    const tString& GetFileName()
    {
        return msFileName;
    }

    /**
     * Set the size. X= the thickness of the line, width of texture used. Y = the length that one texture height takes.
     * \param avSize
     */
    void SetSize(const cVector2f& avSize);
    cVector2f GetSize()
    {
        return mvSize;
    }

    void SetColor(const cColor &aColor);
    const cColor& GetColor()
    {
        return mColor;
    }

    void SetTileHeight(bool abX);
    bool GetTileHeight()
    {
        return mbTileHeight;
    }

    void SetMultiplyAlphaWithColor(bool abX);
    bool GetMultiplyAlphaWithColor()
    {
        return mbMultiplyAlphaWithColor;
    }

    cBeamEnd* GetEnd()
    {
        return mpEnd;
    }

    bool LoadXMLProperties(const tString asFile);

    cVector3f GetAxis()
    {
        return mvAxis;
    }
    cVector3f GetMidPosition()
    {
        return mvMidPosition;
    }

    /////////////////////////////////
    //Entity implementation
    tString GetEntityType()
    {
        return "Beam";
    }

    bool IsVisible();

    //Renderable implementations
    cMaterial *GetMaterial()
    {
        return mpMaterial;
    }
    iVertexBuffer* GetVertexBuffer()
    {
        return mpVtxBuffer;
    }

    void UpdateGraphicsForFrame(float afFrameTime);

    cBoundingVolume* GetBoundingVolume();

    cMatrixf* GetModelMatrix(cFrustum *apFrustum);

    int GetMatrixUpdateCount();


    eRenderableType GetRenderType()
    {
        return eRenderableType_Beam;
    }
private:
    cMaterialManager* mpMaterialManager;
    cFileSearcher *mpFileSearcher;
    iLowLevelGraphics* mpLowLevelGraphics;

    cMaterial *mpMaterial;
    iVertexBuffer* mpVtxBuffer;

    cBeamEnd *mpEnd;

    tString msFileName;

    int mlStartTransformCount;
    int mlEndTransformCount;

    cMatrixf m_mtxTempTransform;

    int mlLastRenderCount;

    cBeamEnd_UpdateCallback mEndCallback;

    cVector2f mvSize;

    cVector3f mvAxis;
    cVector3f mvMidPosition;

    bool mbTileHeight;
    bool mbMultiplyAlphaWithColor;

    cColor mColor;
};

};
#endif // HPL_BEAM_H
