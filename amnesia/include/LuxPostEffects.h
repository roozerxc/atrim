#ifndef LUX_POST_EFFECTS_H
#define LUX_POST_EFFECTS_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------

class iLuxPostEffect : public iPostEffect
{
public:
    iLuxPostEffect(cGraphics *apGraphics, cResources *apResources) : iPostEffect(apGraphics, apResources, NULL) {}
    virtual ~iLuxPostEffect() {}

    virtual void Update(double adFixedDelta) {}

protected:
    void OnSetParams() {}
    iPostEffectParams *GetTypeSpecificParams()
    {
        return NULL;
    }
};


//----------------------------------------

class cLuxPostEffect_Insanity : public iLuxPostEffect
{
public:
    cLuxPostEffect_Insanity(cGraphics *apGraphics, cResources *apResources);
    ~cLuxPostEffect_Insanity();

    void Update(double adFixedDelta);

    void SetWaveAlpha(float afX)
    {
        mfWaveAlpha = afX;
    }
    void SetZoomAlpha(float afX)
    {
        mfZoomAlpha = afX;
    }
    void SetWaveSpeed(float afX)
    {
        mfWaveSpeed = afX;
    }

private:
    iTexture* RenderEffect(iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer);

    iGpuProgram *mpProgram;
    std::vector<iTexture*> mvAmpMaps;
    iTexture* mpZoomMap;

    float mfT;
    float mfAnimCount;
    float mfWaveAlpha;
    float mfZoomAlpha;
    float mfWaveSpeed;
};


//----------------------------------------------

class cLuxPostEffect_Gamma : public iLuxPostEffect
{
public:
    cLuxPostEffect_Gamma(cGraphics *apGraphics, cResources *apResources);
    ~cLuxPostEffect_Gamma();

private:
    iTexture* RenderEffect(iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer);

    iGpuProgram *mpProgram;
};


//----------------------------------------------


class cLuxPostEffectHandler : public iLuxUpdateable
{
public:
    cLuxPostEffectHandler();
    ~cLuxPostEffectHandler();

    void OnStart();
    void Update(double adFixedDelta);
    void Reset();

    cLuxPostEffect_Insanity* GetInsanity()
    {
        return mpInsanity;
    }

private:
    void LoadMainConfig();
    void SaveMainConfig();

    void AddEffect(iLuxPostEffect *apPostEffect, int alPrio);

    cLuxPostEffect_Insanity *mpInsanity;
    cLuxPostEffect_Gamma *mpGamma;

    std::vector<iLuxPostEffect*> mvPostEffects;

};

//----------------------------------------------


#endif // LUX_POST_EFFECTS_H
