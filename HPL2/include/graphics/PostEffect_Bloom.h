#ifndef HPL_POSTEFFECT_BLOOM_H
#define HPL_POSTEFFECT_BLOOM_H

#include "graphics/PostEffect.h"

namespace hpl {

    //------------------------------------------
    
    class cPostEffectParams_Bloom : public iPostEffectParams
    {
    public:
        cPostEffectParams_Bloom() : iPostEffectParams("Bloom"),  
            mvRgbToIntensity(0.3f, 0.58f, 0.12f),
            mlBlurIterations(2),
            mfBlurSize(1.0f)
        {}
        
        kPostEffectParamsClassInit(cPostEffectParams_Bloom)

        cVector3f mvRgbToIntensity;

        float mfBlurSize;
        int mlBlurIterations;
    };

    //------------------------------------------
    
    class cPostEffectType_Bloom : public iPostEffectType
    {
    friend class cPostEffect_Bloom;
    public:
        cPostEffectType_Bloom(cGraphics *apGraphics, cResources *apResources);
        virtual ~cPostEffectType_Bloom();

        iPostEffect *CreatePostEffect(iPostEffectParams *apParams);
    
    private:
        iGpuProgram *mpBlurProgram[2];
        iGpuProgram *mpBloomProgram;
    };
    
    //------------------------------------------

    class cPostEffect_Bloom : public iPostEffect
    {
    public:
        cPostEffect_Bloom(cGraphics *apGraphics,cResources *apResources, iPostEffectType *apType);
        ~cPostEffect_Bloom();

    private:
        void OnSetParams();
        iPostEffectParams *GetTypeSpecificParams() { return &mParams; }
        
        iTexture* RenderEffect(iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer);

        void RenderBlur(iTexture *apInputTex);

        iFrameBuffer *mpBlurBuffer[2];
        iTexture *mpBlurTexture[2];
        
        cPostEffectType_Bloom *mpBloomType;

        cPostEffectParams_Bloom mParams;
    };

    //------------------------------------------

};
#endif // HPL_POSTEFFECT_BLOOM_H
