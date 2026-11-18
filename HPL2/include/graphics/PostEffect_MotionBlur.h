#ifndef HPL_POSTEFFECT_MOTION_BLUR_H
#define HPL_POSTEFFECT_MOTION_BLUR_H

#include "graphics/PostEffect.h"
#include "math/MathTypes.h"

namespace hpl
{

class cFrustum;

//------------------------------------------

/**
 * Camera motion blur, a port of the HPL1 (Penumbra) "Motion" program.
 *
 * HPL1 drew every object a second time with its current and previous matrices to get a
 * velocity per pixel. HPL2 already has the linear depth of every pixel in the G-buffer,
 * so the velocity is found by projecting the pixel with the previous camera instead
 * (one full screen pass, no extra geometry). The sampling along the velocity is the same
 * as in HPL1.
 *
 * Unlike HPL1 the blur length does not depend on the frame rate: it is the motion of the
 * camera during a fixed exposure time (mfShutter seconds), so 30, 60 and 240 fps look the same.
 */
class cPostEffectParams_MotionBlur : public iPostEffectParams
{
public:
    cPostEffectParams_MotionBlur() : iPostEffectParams("MotionBlur"),
        mfShutter(1.0f / 120.0f),
        mfMaxLength(0.04f),
        mfSmoothTime(0.03f),
        mbHighQuality(true)
    {}

    kPostEffectParamsClassInit(cPostEffectParams_MotionBlur)

    /** Exposure time in seconds. 1/120 is a 180 degree shutter at 60 fps. */
    float mfShutter;
    /** Blur length (in screen heights) that is approached but never reached. */
    float mfMaxLength;
    /** Camera velocity is smoothed over about this many seconds (mouse input is uneven at high frame rates). */
    float mfSmoothTime;
    /** 16 samples instead of 8. Falls back to 8 if the card cannot run it. */
    bool mbHighQuality;
};

//------------------------------------------

enum eMotionBlurProgram
{
    eMotionBlurProgram_Low,
    eMotionBlurProgram_High,
    eMotionBlurProgram_LastEnum
};

class cPostEffectType_MotionBlur : public iPostEffectType
{
    friend class cPostEffect_MotionBlur;
public:
    cPostEffectType_MotionBlur(cGraphics *apGraphics, cResources *apResources);
    virtual ~cPostEffectType_MotionBlur();

    iPostEffect *CreatePostEffect(iPostEffectParams *apParams);

private:
    iGpuProgram *CreateProgram(eMotionBlurProgram aType);
    iGpuProgram *GetProgram(bool abHighQuality);

    iGpuProgram *mpProgram[eMotionBlurProgram_LastEnum];
    bool mbProgramTested[eMotionBlurProgram_LastEnum];
};

//------------------------------------------

class cPostEffect_MotionBlur : public iPostEffect
{
public:
    cPostEffect_MotionBlur(cGraphics *apGraphics, cResources *apResources, iPostEffectType *apType);
    ~cPostEffect_MotionBlur();

    void Reset();

private:
    void OnSetActive(bool abX);
    void OnSetDisabled(bool abX);
    void OnSetParams();
    iPostEffectParams *GetTypeSpecificParams()
    {
        return &mParams;
    }

    iTexture *RenderEffect(iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer);

    void UpdateRendererFlag();
    void UpdateCameraMotion(cFrustum *apFrustum);

    cPostEffectType_MotionBlur *mpMotionBlurType;
    cPostEffectParams_MotionBlur mParams;

    //Camera history
    bool mbHistoryValid;
    cMatrixf m_mtxPrevView;
    cFrustum *mpPrevFrustum;
    double mdPrevTime;

    //Smoothed camera motion in view space, per second. Maps the current view to the earlier one.
    cVector3f mvAngularVelocity;
    cVector3f mvLinearVelocity;
};

//------------------------------------------

};
#endif // HPL_POSTEFFECT_MOTION_BLUR_H
