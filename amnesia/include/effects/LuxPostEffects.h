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
    iTexture *RenderEffect(iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer);

    iGpuProgram *mpProgram;
    std::vector<iTexture *> mvAmpMaps;
    iTexture *mpZoomMap;

    float mfT;
    float mfAnimCount;
    float mfWaveAlpha;
    float mfZoomAlpha;
    float mfWaveSpeed;
};


//----------------------------------------------

/**
* Gamma correction for the whole window (replaces SDL1's hardware gamma).
*
* This is NOT a viewport post effect: it is run right before the buffers are
* swapped, so it covers everything drawn in the frame: the 3D view, HUD,
* menus, pre-menu, loading screens, journal, inventory, credits and so on.
* The back buffer is copied to a texture and drawn back with the gamma shader.
* Nothing is done when gamma is 1.0.
*/
class cLuxScreenGamma
{
public:
    cLuxScreenGamma(cGraphics *apGraphics);
    ~cLuxScreenGamma();

    void Render();

private:
    bool CheckScreenTexture(const cVector2l &avScreenSize);

    cGraphics *mpGraphics;
    iGpuProgram *mpProgram;
    iTexture *mpScreenTexture;
    cVector2l mvTextureSize;
    bool mbFailed;
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

    cLuxPostEffect_Insanity *GetInsanity()
    {
        return mpInsanity;
    }

private:
    void LoadMainConfig();
    void SaveMainConfig();

    void AddEffect(iLuxPostEffect *apPostEffect, int alPrio);

    cLuxPostEffect_Insanity *mpInsanity;

    std::vector<iLuxPostEffect *> mvPostEffects;

};

//----------------------------------------------


#endif // LUX_POST_EFFECTS_H
