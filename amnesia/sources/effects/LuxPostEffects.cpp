#include "LuxPostEffects.h"

#include "LuxMapHandler.h"

//////////////////////////////////////////////////////////////////////////
// VARIABLES
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

#define kVar_afAlpha            0
#define kVar_afT                1
#define kVar_avScreenSize        2
#define kVar_afAmpT                3
#define kVar_afWaveAlpha        4
#define kVar_afZoomAlpha        5

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// INSANITY
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPostEffect_Insanity::cLuxPostEffect_Insanity(cGraphics *apGraphics, cResources *apResources) : iLuxPostEffect(apGraphics, apResources)
{
    //////////////////////////////
    // Create program
    cParserVarContainer vars;
    vars.Add("UseUv");
    mpProgram = mpGraphics->CreateGpuProgramFromShaders("LuxInsanity", "deferred_base_vtx.glsl", "posteffect_insanity_frag.glsl", &vars);
    if(mpProgram)
    {
        mpProgram->GetVariableAsId("afAlpha", kVar_afAlpha);
        mpProgram->GetVariableAsId("afT", kVar_afT);
        mpProgram->GetVariableAsId("avScreenSize", kVar_avScreenSize);
        mpProgram->GetVariableAsId("afAmpT", kVar_afAmpT);
        mpProgram->GetVariableAsId("afWaveAlpha", kVar_afWaveAlpha);
        mpProgram->GetVariableAsId("afZoomAlpha", kVar_afZoomAlpha);
    }


    //////////////////////////////
    // Textures
    mvAmpMaps.resize(3);

    for(size_t i = 0; i < mvAmpMaps.size(); ++i)
    {
        mvAmpMaps[i] = mpResources->GetTextureManager()->Create2D("posteffect_insanity_ampmap" + cString::ToString((int)i), false);
    }

    mpZoomMap = mpResources->GetTextureManager()->Create2D("posteffect_insanity_zoom.jpg", false);

    //////////////////////////////
    // Init vars
    mfT = 0;
    mfAnimCount = 0;
    mfWaveAlpha = 0.0f;
    mfZoomAlpha = 0.0f;
    mfWaveSpeed = 0.0f;
}

//-----------------------------------------------------------------------

cLuxPostEffect_Insanity::~cLuxPostEffect_Insanity()
{

}

//-----------------------------------------------------------------------

void cLuxPostEffect_Insanity::Update(double adFixedDelta)
{
    mfT += (float)adFixedDelta * mfWaveSpeed;

    mfAnimCount += (float)adFixedDelta * 0.15f;

    float fMaxAnim = (float)mvAmpMaps.size();
    if(mfAnimCount >= fMaxAnim)
    {
        mfAnimCount = mfAnimCount - fMaxAnim;
    }
}

//-----------------------------------------------------------------------


iTexture *cLuxPostEffect_Insanity::RenderEffect(iTexture *apInputTexture, iFrameBuffer *apFinalTempBuffer)
{
    /////////////////////////
    // Init render states
    mpCurrentComposite->SetFlatProjection();
    mpCurrentComposite->SetBlendMode(eMaterialBlendMode_None);
    mpCurrentComposite->SetChannelMode(eMaterialChannelMode_RGBA);

    /////////////////////////
    // Render the to final buffer
    // This function sets to frame buffer if post effect is last!
    SetFinalFrameBuffer(apFinalTempBuffer);

    mpCurrentComposite->SetTexture(0, apInputTexture);

    int lAmp0 = (int)mfAnimCount;
    int lAmp1 = (int)(mfAnimCount + 1);
    if(lAmp1 >= (int) mvAmpMaps.size())
    {
        lAmp1 = 0;
    }
    float fAmpT = cMath::GetFraction(mfAnimCount);

    //Log("AnimCount: %f - %d %d - %f\n", mfAnimCount, lAmp0, lAmp1, fAmpT);

    mpCurrentComposite->SetTexture(1, mvAmpMaps[lAmp0]);
    mpCurrentComposite->SetTexture(2, mvAmpMaps[lAmp1]);
    mpCurrentComposite->SetTexture(3, mpZoomMap);

    mpCurrentComposite->SetProgram(mpProgram);
    if(mpProgram)
    {
        mpProgram->SetFloat(kVar_afAlpha, 1.0f);
        mpProgram->SetFloat(kVar_afT, mfT);
        mpProgram->SetVec2f(kVar_avScreenSize, mpLowLevelGraphics->GetScreenSizeFloat());
        mpProgram->SetFloat(kVar_afAmpT, fAmpT);
        mpProgram->SetFloat(kVar_afWaveAlpha, mfWaveAlpha);
        mpProgram->SetFloat(kVar_afZoomAlpha, mfZoomAlpha);
    }


    DrawQuad(0, 1, apInputTexture, true);

    mpCurrentComposite->SetTextureRange(NULL, 1);

    return apFinalTempBuffer->GetColorBuffer(0)->ToTexture();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// GAMMA
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

#define kVar_afGamma 0

//-----------------------------------------------------------------------

cLuxScreenGamma::cLuxScreenGamma(cGraphics *apGraphics)
{
    mpGraphics = apGraphics;
    mpScreenTexture = NULL;
    mvTextureSize = cVector2l(0, 0);
    mbFailed = false;

    //////////////////////////////
    // Create program
    cParserVarContainer vars;
    vars.Add("UseUv");
    mpProgram = mpGraphics->CreateGpuProgramFromShaders("ScreenGammaCorrection", "deferred_base_vtx.glsl", "posteffect_gamma_frag.glsl", &vars);
    if(mpProgram)
    {
        mpProgram->GetVariableAsId("afGamma", kVar_afGamma);
    }
    else
    {
        Error("Could not create the gamma correction program, gamma setting will have no effect!\n");
        mbFailed = true;
    }
}

//-----------------------------------------------------------------------

cLuxScreenGamma::~cLuxScreenGamma()
{
    if(mpScreenTexture)
    {
        mpGraphics->DestroyTexture(mpScreenTexture);
    }
    if(mpProgram)
    {
        mpGraphics->DestroyGpuProgram(mpProgram);
    }
}

//-----------------------------------------------------------------------

bool cLuxScreenGamma::CheckScreenTexture(const cVector2l &avScreenSize)
{
    if(mpScreenTexture && mvTextureSize == avScreenSize)
    {
        return true;
    }

    //Screen size changed (or first use), (re)create the copy texture
    if(mpScreenTexture)
    {
        mpGraphics->DestroyTexture(mpScreenTexture);
        mpScreenTexture = NULL;
    }

    mpScreenTexture = mpGraphics->CreateTexture("ScreenGamma", eTextureType_Rect, eTextureUsage_RenderTarget);
    if(mpScreenTexture == NULL ||
       mpScreenTexture->CreateFromRawData(cVector3l(avScreenSize.x, avScreenSize.y, 0), ePixelFormat_RGBA, NULL) == false)
    {
        Error("Could not create the gamma correction screen texture (%dx%d), gamma setting will have no effect!\n", avScreenSize.x, avScreenSize.y);
        if(mpScreenTexture)
        {
            mpGraphics->DestroyTexture(mpScreenTexture);
            mpScreenTexture = NULL;
        }
        mbFailed = true;
        return false;
    }
    mpScreenTexture->SetWrapSTR(eTextureWrap_ClampToEdge);
    mpScreenTexture->SetFilter(eTextureFilter_Nearest);    //Copied 1:1, no need to filter

    mvTextureSize = avScreenSize;
    return true;
}

//-----------------------------------------------------------------------

void cLuxScreenGamma::Render()
{
    if(mbFailed)
    {
        return;
    }

    iLowLevelGraphics *pLowGfx = mpGraphics->GetLowLevel();

    //Do nothing (no copy, no extra pass) at default gamma
    float fGamma = pLowGfx->GetGammaCorrection();
    if(cMath::Abs(fGamma - 1.0f) <= 0.001f)
    {
        return;
    }

    cVector2l vScreenSize = pLowGfx->GetScreenSizeInt();
    if(vScreenSize.x <= 0 || vScreenSize.y <= 0)
    {
        return;
    }
    if(CheckScreenTexture(vScreenSize) == false)
    {
        return;
    }
    cVector2f vScreenSizeF((float)vScreenSize.x, (float)vScreenSize.y);

    /////////////////////////
    // Set states. Whatever drew last (gui, menus, loading screens) can leave
    // anything set, so make sure nothing clips or blends the full screen quad.
    pLowGfx->SetCurrentFrameBuffer(NULL);

    pLowGfx->SetBlendActive(false);
    pLowGfx->SetAlphaTestActive(false);
    pLowGfx->SetDepthTestActive(false);
    pLowGfx->SetDepthWriteActive(false);
    pLowGfx->SetStencilActive(false);
    pLowGfx->SetScissorActive(false);
    pLowGfx->SetCullActive(false);
    pLowGfx->SetColorWriteActive(true, true, true, true);
    for(int i = 0; i < 4; ++i)
    {
        pLowGfx->SetClipPlaneActive(i, false);
    }

    pLowGfx->SetOrthoProjection(vScreenSizeF, -1000, 1000);
    pLowGfx->SetIdentityMatrix(eMatrix_ModelView);

    /////////////////////////
    // Copy back buffer to texture
    pLowGfx->CopyFrameBufferToTexture(mpScreenTexture, 0, vScreenSize, 0);

    /////////////////////////
    // Draw it back with gamma
    mpProgram->Bind();
    mpProgram->SetFloat(kVar_afGamma, fGamma);

    pLowGfx->SetTexture(0, mpScreenTexture);
    pLowGfx->DrawQuad(0, vScreenSizeF, cVector2f(0, vScreenSizeF.y), cVector2f(vScreenSizeF.x, 0), cColor(1, 1));

    mpProgram->UnBind();

    /////////////////////////
    // Exit
    pLowGfx->SetTexture(0, NULL);
    pLowGfx->SetDepthTestActive(true);
    pLowGfx->SetDepthWriteActive(true);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// POST EFFECT HANDLER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPostEffectHandler::cLuxPostEffectHandler() : iLuxUpdateable("LuxPostEffectHandler")
{
    cGraphics *pGraphics = gpBase->mpEngine->GetGraphics();
    cResources *pResources = gpBase->mpEngine->GetResources();

    ///////////////////////
    // Create post effects
    mpInsanity = hplNew(cLuxPostEffect_Insanity, (pGraphics, pResources) );
    AddEffect(mpInsanity, 25);
    mpInsanity->SetActive(true);
}

//-----------------------------------------------------------------------

cLuxPostEffectHandler::~cLuxPostEffectHandler()
{
    STLDeleteAll(mvPostEffects);
}

//-----------------------------------------------------------------------

void cLuxPostEffectHandler::OnStart()
{

}

//-----------------------------------------------------------------------

void cLuxPostEffectHandler::Update(double adFixedDelta)
{
    for(size_t i = 0; i < mvPostEffects.size(); ++i)
    {
        iLuxPostEffect *pPostEffect = mvPostEffects[i];

        if(pPostEffect->IsActive())
        {
            pPostEffect->Update(adFixedDelta);
        }
    }
}

//-----------------------------------------------------------------------

void cLuxPostEffectHandler::Reset()
{

}

//-----------------------------------------------------------------------

void cLuxPostEffectHandler::LoadMainConfig()
{
    cConfigFile *pMainCfg = gpBase->mpMainConfig;

    mpInsanity->SetDisabled(pMainCfg->GetBool("Graphics", "PostEffectInsanity", true) == false);
    //Fullbright G-buffer mode: albedo only, no post effects (the options menu hides them).
    //SaveMainConfig does not write it in this mode, so the user's setting stays in the config.
    if(cRendererDeferred::GetFullbrightActive())
    {
        mpInsanity->SetDisabled(true);
    }

}

//-----------------------------------------------------------------------

void cLuxPostEffectHandler::SaveMainConfig()
{
    cConfigFile *pMainCfg = gpBase->mpMainConfig;

    if(cRendererDeferred::GetFullbrightActive() == false)
    {
        pMainCfg->SetBool("Graphics", "PostEffectInsanity", mpInsanity->IsDisabled() == false);
    }
}

//-----------------------------------------------------------------------

void cLuxPostEffectHandler::AddEffect(iLuxPostEffect *apPostEffect, int alPrio)
{
    mvPostEffects.push_back(apPostEffect);
    apPostEffect->SetActive(false);
    gpBase->mpMapHandler->GetViewport()->GetPostEffectComposite()->AddPostEffect(apPostEffect, alPrio);
}

//-----------------------------------------------------------------------
