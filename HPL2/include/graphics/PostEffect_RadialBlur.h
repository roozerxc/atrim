#ifndef HPL_POSTEFFECT_RADIAL_BLUR_H
#define HPL_POSTEFFECT_RADIAL_BLUR_H

#include "graphics/PostEffect.h"

namespace hpl {

    //------------------------------------------
    
    class cPostEffectParams_RadialBlur : public iPostEffectParams
    {
    public:
        cPostEffectParams_RadialBlur() : iPostEffectParams("RadialBlur"),
            mfSize(0.06f),
            mfAlpha(1.0f),
            mfBlurStartDist(0)
        {}
        
        kPostEffectParamsClassInit(cPostEffectParams_RadialBlur)

        float mfSize;
        float mfAlpha;
        float mfBlurStartDist;
    };

    //------------------------------------------
    
    class cPostEffectType_RadialBlur : public iPostEffectType
    {
    friend class cPostEffect_RadialBlur;
    public:
        cPostEffectType_RadialBlur(cGraphics *apGraphics, cResources *apResources);
        virtual ~cPostEffectType_RadialBlur();

        iPostEffect *CreatePostEffect(iPostEffectParams *apParams);
    
    private:
        iGpuProgram *mpProgram;
    };
    
    //------------------------------------------

    class cPostEffect_RadialBlur : public iPostEffect
    {
    public:
        cPostEffect_RadialBlur(cGraphics *apGraphics,cResources *apResources, iPostEffectType *apType);
        ~cPostEffect_RadialBlur();

        void Reset();

    private:
        void OnSetParams();
        iPostEffectParams *GetTypeSpecificParams() { return &mParams; }
        
        iTexture* RenderEffect(iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer);

        void RenderBlur(iTexture *apInputTex);

        cPostEffectType_RadialBlur *mpRadialBlurType;
        cPostEffectParams_RadialBlur mParams;
    };

    //------------------------------------------

};
#endif // HPL_POSTEFFECT_IMAGE_TRAIL_H
