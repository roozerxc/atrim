#include "gui/GuiMaterialBasicTypes.h"

#include "graphics/LowLevelGraphics.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// DIFFUSE
//////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------

void cGuiMaterial_Diffuse::BeforeRender()
{
    mpLowLevelGraphics->SetBlendActive(true);
    mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_Zero);
}

//-----------------------------------------------------------------------

void cGuiMaterial_Diffuse::AfterRender()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// ALPHA
//////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------

void cGuiMaterial_Alpha::BeforeRender()
{
    mpLowLevelGraphics->SetBlendActive(true);
    mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
}

//-----------------------------------------------------------------------

void cGuiMaterial_Alpha::AfterRender()
{
    //Not needed right?
    //mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_OneMinusSrcAlpha);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// FONT NORMAL
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cGuiMaterial_FontNormal::BeforeRender()
{
    mpLowLevelGraphics->SetBlendActive(true);
    mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
}

//-----------------------------------------------------------------------

void cGuiMaterial_FontNormal::AfterRender()
{
    //Not needed right?
    //mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_OneMinusSrcAlpha);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// ADDITIVE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cGuiMaterial_Additive::BeforeRender()
{
    mpLowLevelGraphics->SetBlendActive(true);
    mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_One);
}

//-----------------------------------------------------------------------

void cGuiMaterial_Additive::AfterRender()
{
    //Not needed right?
    //mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_OneMinusSrcAlpha);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// MODULATIVE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cGuiMaterial_Modulative::BeforeRender()
{
    mpLowLevelGraphics->SetBlendActive(true);
    mpLowLevelGraphics->SetBlendFunc(eBlendFunc_DestColor, eBlendFunc_Zero);
}

//-----------------------------------------------------------------------

void cGuiMaterial_Modulative::AfterRender()
{
    //Not needed right?
    //mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_OneMinusSrcAlpha);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PREMUL ALPHA
//////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------

void cGuiMaterial_PremulAlpha::BeforeRender()
{
    mpLowLevelGraphics->SetBlendActive(true);
    mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_OneMinusSrcAlpha);
}

//-----------------------------------------------------------------------

void cGuiMaterial_PremulAlpha::AfterRender()
{
    //Not needed right?
    //mpLowLevelGraphics->SetBlendFunc(eBlendFunc_One, eBlendFunc_OneMinusSrcAlpha);
}

//-----------------------------------------------------------------------

}
