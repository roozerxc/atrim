#include "graphics/PostEffect_MotionBlur.h"

#include "graphics/Graphics.h"

#include "graphics/LowLevelGraphics.h"
#include "graphics/PostEffectComposite.h"
#include "graphics/FrameBuffer.h"
#include "graphics/Texture.h"
#include "graphics/GPUProgram.h"
#include "graphics/GPUShader.h"
#include "graphics/RendererDeferred.h"

#include "resources/Resources.h"
#include "resources/GpuShaderManager.h"

#include "system/LowLevelSystem.h"
#include "system/Platform.h"
#include "system/PreprocessParser.h"

#include "math/Math.h"
#include "math/Frustum.h"

#include <math.h>

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// PROGRAM VARS
//////////////////////////////////////////////////////////////////////////

#define kVar_avReprojX      0
#define kVar_avReprojY      1
#define kVar_avReprojW      2
#define kVar_afInvMaxLengthSqr  3

//////////////////////////////////////////////////////////////////////////
// CAMERA MATH HELPERS
//////////////////////////////////////////////////////////////////////////

//Larger camera changes than these in a single frame are cuts/teleports, not motion.
static const float kfMotionBlurCutAngle = 0.5f;     //radians (~29 degrees)
static const float kfMotionBlurCutDistance = 1.5f;  //meters
//Frames further apart than this (loading, menus, effect turned off) start a new history.
static const double kdMotionBlurMaxFrameGap = 0.2;
//Below this many pixels of motion the pass is skipped.
static const float kfMotionBlurMinPixels = 0.25f;

//-----------------------------------------------------------------------

//Rotation vector (axis * angle) of the rotation part of a matrix.
static cVector3f RotationMatrixToVector(const cMatrixf &a_mtxRot)
{
    cVector3f vAxis(a_mtxRot.m[2][1] - a_mtxRot.m[1][2],
                    a_mtxRot.m[0][2] - a_mtxRot.m[2][0],
                    a_mtxRot.m[1][0] - a_mtxRot.m[0][1]);

    float fCos = (a_mtxRot.m[0][0] + a_mtxRot.m[1][1] + a_mtxRot.m[2][2] - 1.0f) * 0.5f;
    if(fCos > 1.0f)
    {
        fCos = 1.0f;
    }
    if(fCos < -1.0f)
    {
        fCos = -1.0f;
    }
    float fAngle = acosf(fCos);
    float fSin = sinf(fAngle);

    //angle / (2 sin(angle)) -> 0.5 for small angles
    float fMul = fSin > 0.0001f ? fAngle / (2.0f * fSin) : 0.5f;

    return vAxis * fMul;
}

//-----------------------------------------------------------------------

//Rotation matrix from a rotation vector (Rodrigues)
static cMatrixf RotationVectorToMatrix(const cVector3f &avRot)
{
    float fAngle = avRot.Length();
    if(fAngle < 0.000001f)
    {
        return cMatrixf::Identity;
    }

    cVector3f vA = avRot / fAngle;
    float fC = cosf(fAngle);
    float fS = sinf(fAngle);
    float fT = 1.0f - fC;

    return cMatrixf(fT * vA.x * vA.x + fC,           fT * vA.x * vA.y - fS * vA.z,   fT * vA.x * vA.z + fS * vA.y,   0,
                    fT * vA.x * vA.y + fS * vA.z,    fT * vA.y * vA.y + fC,          fT * vA.y * vA.z - fS * vA.x,   0,
                    fT * vA.x * vA.z - fS * vA.y,    fT * vA.y * vA.z + fS * vA.x,   fT * vA.z * vA.z + fC,          0,
                    0,                               0,                              0,                              1);
}

//////////////////////////////////////////////////////////////////////////
// POST EFFECT BASE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cPostEffectType_MotionBlur::cPostEffectType_MotionBlur(cGraphics *apGraphics, cResources *apResources) : iPostEffectType("MotionBlur", apGraphics, apResources)
{
    for(int i = 0; i < eMotionBlurProgram_LastEnum; ++i)
    {
        mpProgram[i] = NULL;
        mbProgramTested[i] = false;
    }
}

//-----------------------------------------------------------------------

cPostEffectType_MotionBlur::~cPostEffectType_MotionBlur()
{

}

//-----------------------------------------------------------------------

iPostEffect *cPostEffectType_MotionBlur::CreatePostEffect(iPostEffectParams *apParams)
{
    cPostEffect_MotionBlur *pEffect = hplNew(cPostEffect_MotionBlur, (mpGraphics, mpResources, this));

    return pEffect;
}

//-----------------------------------------------------------------------

iGpuProgram *cPostEffectType_MotionBlur::CreateProgram(eMotionBlurProgram aType)
{
    const char *sTypeName = aType == eMotionBlurProgram_High ? "high (16 samples)" : "low (8 samples)";

    cParserVarContainer vars;
    if(cRendererDeferred::GetGBufferType() == eDeferredGBuffer_32Bit)
    {
        vars.Add("PackedDepth");
    }
    if(aType == eMotionBlurProgram_High)
    {
        vars.Add("HighQuality");
    }

    cGpuShaderManager *pShaderManager = mpResources->GetGpuShaderManager();
    iGpuShader *pVtxShader = pShaderManager->CreateShader("deferred_base_vtx.glsl", eGpuShaderType_Vertex, &vars);
    if(pVtxShader == NULL)
    {
        Log(" Motion blur %s: vertex shader does not compile\n", sTypeName);
        return NULL;
    }
    iGpuShader *pFragShader = pShaderManager->CreateShader("posteffect_motion_blur_frag.glsl", eGpuShaderType_Fragment, &vars);
    if(pFragShader == NULL)
    {
        pShaderManager->Destroy(pVtxShader);
        Log(" Motion blur %s: fragment shader does not compile\n", sTypeName);
        return NULL;
    }

    tString sName = aType == eMotionBlurProgram_High ? "MotionBlurHigh" : "MotionBlurLow";
    iGpuProgram *pProgram = mpGraphics->CreateGpuProgram(sName);
    pProgram->SetShader(eGpuShaderType_Vertex, pVtxShader);
    pProgram->SetShader(eGpuShaderType_Fragment, pFragShader);

    //Test what the card actually does with it: it has to link and run in hardware.
    if(pProgram->Link() == false)
    {
        Log(" Motion blur %s: program does not link\n", sTypeName);
        mpGraphics->DestroyGpuProgram(pProgram);
        return NULL;
    }
    if(pProgram->GetRunsInSoftware())
    {
        Log(" Motion blur %s: driver runs the program in software\n", sTypeName);
        mpGraphics->DestroyGpuProgram(pProgram);
        return NULL;
    }

    pProgram->GetVariableAsId("avReprojX", kVar_avReprojX);
    pProgram->GetVariableAsId("avReprojY", kVar_avReprojY);
    pProgram->GetVariableAsId("avReprojW", kVar_avReprojW);
    pProgram->GetVariableAsId("afInvMaxLengthSqr", kVar_afInvMaxLengthSqr);

    Log(" Motion blur %s: OK\n", sTypeName);
    return pProgram;
}

//-----------------------------------------------------------------------

iGpuProgram *cPostEffectType_MotionBlur::GetProgram(bool abHighQuality)
{
    //Programs are made on first use, so nothing is compiled if motion blur is never turned on.
    if(abHighQuality)
    {
        if(mbProgramTested[eMotionBlurProgram_High] == false)
        {
            mbProgramTested[eMotionBlurProgram_High] = true;
            mpProgram[eMotionBlurProgram_High] = CreateProgram(eMotionBlurProgram_High);
            if(mpProgram[eMotionBlurProgram_High] == NULL)
            {
                Log(" Motion blur: using the 8 sample program instead\n");
            }
        }
        if(mpProgram[eMotionBlurProgram_High])
        {
            return mpProgram[eMotionBlurProgram_High];
        }
    }

    if(mbProgramTested[eMotionBlurProgram_Low] == false)
    {
        mbProgramTested[eMotionBlurProgram_Low] = true;
        mpProgram[eMotionBlurProgram_Low] = CreateProgram(eMotionBlurProgram_Low);
        if(mpProgram[eMotionBlurProgram_Low] == NULL)
        {
            Warning("Motion blur is not supported by this card/driver, it is turned off.\n");
        }
    }
    return mpProgram[eMotionBlurProgram_Low];
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// POST EFFECT
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cPostEffect_MotionBlur::cPostEffect_MotionBlur(cGraphics *apGraphics, cResources *apResources, iPostEffectType *apType) : iPostEffect(apGraphics, apResources, apType)
{
    mpMotionBlurType = static_cast<cPostEffectType_MotionBlur *>(mpType);

    mpPrevFrustum = NULL;
    mdPrevTime = 0;
    Reset();
}

//-----------------------------------------------------------------------

cPostEffect_MotionBlur::~cPostEffect_MotionBlur()
{
    cRendererDeferred::SetMotionBlurActive(false);
}

//-----------------------------------------------------------------------

void cPostEffect_MotionBlur::Reset()
{
    mbHistoryValid = false;
    mvAngularVelocity = 0;
    mvLinearVelocity = 0;
}

//-----------------------------------------------------------------------

void cPostEffect_MotionBlur::OnSetParams()
{
    UpdateRendererFlag();
}

//-----------------------------------------------------------------------

void cPostEffect_MotionBlur::OnSetActive(bool abX)
{
    Reset();
    UpdateRendererFlag();
}

void cPostEffect_MotionBlur::OnSetDisabled(bool abX)
{
    Reset();
    UpdateRendererFlag();
}

//-----------------------------------------------------------------------

void cPostEffect_MotionBlur::UpdateRendererFlag()
{
    bool bActive = IsActive();

    //Compile (and test) the program now, not on the first camera move (would be a hitch on old cards).
    if(bActive && mpMotionBlurType->GetProgram(mParams.mbHighQuality) == NULL)
    {
        bActive = false;
    }

    //The renderer only writes the view model mask and clears the linear depth when this is on.
    cRendererDeferred::SetMotionBlurActive(bActive);
}

//-----------------------------------------------------------------------

void cPostEffect_MotionBlur::UpdateCameraMotion(cFrustum *apFrustum)
{
    double dTime = cPlatform::GetApplicationTimeX();
    double dFrameTime = dTime - mdPrevTime;
    mdPrevTime = dTime;

    if(apFrustum == NULL || apFrustum->GetProjectionType() != eProjectionType_Perspective)
    {
        Reset();
        mpPrevFrustum = NULL;
        return;
    }

    const cMatrixf &mtxView = apFrustum->GetViewMatrix();

    //New history: other camera, first frame, or too long since the last frame.
    if(mbHistoryValid == false || apFrustum != mpPrevFrustum || dFrameTime <= 0.0 || dFrameTime > kdMotionBlurMaxFrameGap)
    {
        Reset();
        mbHistoryValid = true;
        mpPrevFrustum = apFrustum;
        m_mtxPrevView = mtxView;
        return;
    }

    ////////////////////////////
    // Camera motion this frame: maps current view space to the previous view space
    cMatrixf mtxDelta = cMath::MatrixMul(m_mtxPrevView, cMath::MatrixInverse(mtxView));
    m_mtxPrevView = mtxView;

    cVector3f vRot = RotationMatrixToVector(mtxDelta);
    cVector3f vTrans = mtxDelta.GetTranslation();

    if(vRot.Length() > kfMotionBlurCutAngle || vTrans.Length() > kfMotionBlurCutDistance)
    {
        //Camera cut or teleport: never blur across it.
        mvAngularVelocity = 0;
        mvLinearVelocity = 0;
        return;
    }

    ////////////////////////////
    // Smooth the velocity. At high frame rates the mouse does not move every frame,
    // without this the blur length would flicker from frame to frame.
    float fInvFrameTime = (float)(1.0 / dFrameTime);
    float fAlpha = 1.0f;
    if(mParams.mfSmoothTime > 0.0f)
    {
        fAlpha = 1.0f - (float)exp(-dFrameTime / (double)mParams.mfSmoothTime);
    }

    mvAngularVelocity += (vRot * fInvFrameTime - mvAngularVelocity) * fAlpha;
    mvLinearVelocity += (vTrans * fInvFrameTime - mvLinearVelocity) * fAlpha;
}

//-----------------------------------------------------------------------

iTexture *cPostEffect_MotionBlur::RenderEffect(iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer)
{
    cFrustum *pFrustum = mpCurrentComposite->GetCurrentFrustum();
    UpdateCameraMotion(pFrustum);

    if(mbHistoryValid == false)
    {
        return apInputTexture;
    }

    cVector2l vRenderTargetSize = mpCurrentComposite->GetRenderTargetSize();
    cVector2f vSize((float)vRenderTargetSize.x, (float)vRenderTargetSize.y);

    const cMatrixf &mtxProj = pFrustum->GetProjectionMatrix();

    ////////////////////////////
    // Skip the pass when there is hardly any motion (standing still costs nothing)
    {
        float fPixelsPerRadian = vSize.y * 0.5f * mtxProj.m[1][1];
        //Translation is checked against something half a meter away.
        float fMotion = (mvAngularVelocity.Length() + mvLinearVelocity.Length() * 2.0f) * mParams.mfShutter;
        if(fMotion * fPixelsPerRadian < kfMotionBlurMinPixels)
        {
            return apInputTexture;
        }
    }

    iGpuProgram *pProgram = mpMotionBlurType->GetProgram(mParams.mbHighQuality);
    cRendererDeferred *pRenderer = static_cast<cRendererDeferred *>(mpGraphics->GetRenderer(eRenderer_Main));
    iTexture *pDepthTexture = pRenderer ? pRenderer->GetMainLinearDepthTexture() : NULL;
    if(pProgram == NULL || pDepthTexture == NULL)
    {
        return apInputTexture;
    }

    ////////////////////////////
    // Camera motion during the exposure time
    cMatrixf mtxShutter = RotationVectorToMatrix(mvAngularVelocity * mParams.mfShutter);
    mtxShutter.SetTranslation(mvLinearVelocity * mParams.mfShutter);

    ////////////////////////////
    // Reprojection matrix.
    // The shader gives (x*d, y*d, d, 1): x,y = window position, d = depth as stored in the G-buffer.
    // Result is (x', y', w), previous window position = (x', y') / w.

    //Depth to view space units: 32 bit stores depth / far plane, 64 bit stores the view position.
    float fDepthMul = cRendererDeferred::GetGBufferType() == eDeferredGBuffer_32Bit ? pFrustum->GetFarPlane() : 1.0f;

    //Window position and depth -> view space position (works for off-center projections too)
    cMatrixf mtxWindowToView(
        2.0f * fDepthMul / (vSize.x * mtxProj.m[0][0]), 0, fDepthMul * (mtxProj.m[0][2] - 1.0f) / mtxProj.m[0][0], 0,
        0, 2.0f * fDepthMul / (vSize.y * mtxProj.m[1][1]), fDepthMul * (mtxProj.m[1][2] - 1.0f) / mtxProj.m[1][1], 0,
        0, 0, -fDepthMul, 0,
        0, 0, 0, 1);

    //Clip space -> window position (before divide)
    cMatrixf mtxClipToWindow(
        vSize.x * 0.5f, 0, 0, vSize.x * 0.5f,
        0, vSize.y * 0.5f, 0, vSize.y * 0.5f,
        0, 0, 1, 0,
        0, 0, 0, 1);

    //The current projection is used for both, so FOV changes (zoom, insanity) do not blur.
    cMatrixf mtxReproj = cMath::MatrixMul(mtxClipToWindow, cMath::MatrixMul(mtxProj, cMath::MatrixMul(mtxShutter, mtxWindowToView)));

    float fMaxLength = mParams.mfMaxLength * vSize.y;
    if(fMaxLength < 1.0f)
    {
        fMaxLength = 1.0f;
    }

    /////////////////////////
    // Init render states
    mpCurrentComposite->SetFlatProjection();
    mpCurrentComposite->SetBlendMode(eMaterialBlendMode_None);
    mpCurrentComposite->SetChannelMode(eMaterialChannelMode_RGBA);
    mpCurrentComposite->SetTextureRange(NULL, 2);

    /////////////////////////
    // Render
    // This function sets to frame buffer is post effect is last!
    SetFinalFrameBuffer(apFinalTempBuffer);

    mpCurrentComposite->SetProgram(pProgram);

    pProgram->SetVec4f(kVar_avReprojX, mtxReproj.m[0][0], mtxReproj.m[0][1], mtxReproj.m[0][2], mtxReproj.m[0][3]);
    pProgram->SetVec4f(kVar_avReprojY, mtxReproj.m[1][0], mtxReproj.m[1][1], mtxReproj.m[1][2], mtxReproj.m[1][3]);
    pProgram->SetVec4f(kVar_avReprojW, mtxReproj.m[3][0], mtxReproj.m[3][1], mtxReproj.m[3][2], mtxReproj.m[3][3]);
    pProgram->SetFloat(kVar_afInvMaxLengthSqr, 1.0f / (fMaxLength * fMaxLength));

    mpCurrentComposite->SetTexture(0, apInputTexture);
    mpCurrentComposite->SetTexture(1, pDepthTexture);

    DrawQuad(0, 1, apInputTexture, true);

    mpCurrentComposite->SetTexture(1, NULL);
    mpCurrentComposite->SetProgram(NULL);
    mpCurrentComposite->SetBlendMode(eMaterialBlendMode_None);

    return apFinalTempBuffer->GetColorBuffer(0)->ToTexture();
}

//-----------------------------------------------------------------------

}
