#ifndef HPL_POSTEFFECT_IMAGE_TRAIL_H
#define HPL_POSTEFFECT_IMAGE_TRAIL_H

#include "graphics/PostEffect.h"

namespace hpl
{

//------------------------------------------

class cPostEffectParams_ImageTrail : public iPostEffectParams
{
public:
    cPostEffectParams_ImageTrail() : iPostEffectParams("ImageTrail"),

        mfAmount(0.3f)
    {}

    kPostEffectParamsClassInit(cPostEffectParams_ImageTrail)

    float mfAmount;

};

//------------------------------------------

class cPostEffectType_ImageTrail : public iPostEffectType
{
    friend class cPostEffect_ImageTrail;
public:
    cPostEffectType_ImageTrail(cGraphics *apGraphics, cResources *apResources);
    virtual ~cPostEffectType_ImageTrail();

    iPostEffect *CreatePostEffect(iPostEffectParams *apParams);

private:
    iGpuProgram *mpProgram;
};

//------------------------------------------

class cPostEffect_ImageTrail : public iPostEffect
{
public:
    cPostEffect_ImageTrail(cGraphics *apGraphics,cResources *apResources, iPostEffectType *apType);
    ~cPostEffect_ImageTrail();

    void Reset();

private:
    void OnSetActive(bool abX);
    void OnSetParams();
    iPostEffectParams *GetTypeSpecificParams()
    {
        return &mParams;
    }

    iTexture* RenderEffect(iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer);

    iFrameBuffer *mpAccumBuffer;
    iTexture *mpAccumTexture;

    cPostEffectType_ImageTrail *mpImageTrailType;
    cPostEffectParams_ImageTrail mParams;

    bool mbClearFrameBuffer;
};

//------------------------------------------

};
#endif // HPL_POSTEFFECT_IMAGE_TRAIL_H
