#include "gui/GuiSkin.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/Platform.h"

#include "impl/XmlPugi.h"

#include "gui/Gui.h"
#include "gui/GuiGfxElement.h"

#include "resources/Resources.h"
#include "resources/FontManager.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// FONT
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cGuiSkinFont::cGuiSkinFont(cGui *apGui)
{
    mpGui = apGui;

    mpMaterial = NULL;
}
cGuiSkinFont::~cGuiSkinFont()
{
    //Skip deleting font...
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cGuiSkin::cGuiSkin(const tString & asName, cGui *apGui)
{
    mpGui = apGui;
    msName = asName;

    mvGfxElements.resize(eGuiSkinGfx_LastEnum, NULL);
    mvFonts.resize(eGuiSkinFont_LastEnum, NULL);
    mvAttributes.resize(eGuiSkinAttribute_LastEnum, cVector3f(0) );
}

//-----------------------------------------------------------------------

cGuiSkin::~cGuiSkin()
{
    for(size_t i = 0; i < mvGfxElements.size(); ++i)
    {
        if(mvGfxElements[i])
        {
            mpGui->DestroyGfx(mvGfxElements[i]);
        }
    }

    STLDeleteAll(mvFonts);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

static eGuiGfxAnimationType ToAnimType(const char* apString)
{
    if(apString == NULL)
    {
        return eGuiGfxAnimationType_Loop;
    }

    tString sLow = cString::ToLowerCase(apString);

    if(sLow == "loop")
    {
        return eGuiGfxAnimationType_Loop;
    }
    if(sLow == "oscillate")
    {
        return eGuiGfxAnimationType_Oscillate;
    }
    if(sLow == "random")
    {
        return eGuiGfxAnimationType_Random;
    }
    if(sLow == "stop_at_end")
    {
        return eGuiGfxAnimationType_StopAtEnd;
    }

    Warning("Animation type '%s' does not exist!\n", sLow.c_str());

    return eGuiGfxAnimationType_Loop;
}

static eGuiMaterial ToMaterial(const char* apString)
{
    if(apString == NULL)
    {
        return eGuiMaterial_Alpha;
    }

    tString sLow = cString::ToLowerCase(apString);

    if(sLow == "alpha")
    {
        return eGuiMaterial_Alpha;
    }
    if(sLow == "diffuse")
    {
        return eGuiMaterial_Diffuse;
    }
    if(sLow == "font_normal")
    {
        return eGuiMaterial_FontNormal;
    }
    if(sLow == "additive")
    {
        return eGuiMaterial_Additive;
    }
    if(sLow == "modulative")
    {
        return eGuiMaterial_Modulative;
    }
    if(sLow == "premulalpha")
    {
        return eGuiMaterial_PremulAlpha;
    }

    Warning("Material type '%s' does not exist!\n", sLow.c_str());

    return eGuiMaterial_Alpha;
}

bool cGuiSkin::LoadFromFile(const tWString &asFile)
{
    //////////////////////////////////
    //Open document
    pugi::xml_document xmlDoc;
    if(Xml::LoadFile(xmlDoc, asFile) == false)
    {
        Error("Couldn't load skin file '%s'!\n", cString::To8Char(asFile).c_str());
        return false;
    }

    /////////////////////////
    //Get the root.
    pugi::xml_node pRootElem = xmlDoc.document_element();


    //////////////////////////////////////
    // ATTRIBUTES
    pugi::xml_node pAttributesElement = pRootElem.child("Attributes");

    tBoolVec vAttribLoaded;
    vAttribLoaded.resize(eGuiSkinAttribute_LastEnum, false);

    //Iterate font elements
    pugi::xml_node pAttributeElem = Xml::FirstElem(pAttributesElement);
    for(; pAttributeElem; pAttributeElem = Xml::NextElem(pAttributeElem))
    {
        tString sType = cString::ToString(Xml::Attr(pAttributeElem, "type"), "");

        eGuiSkinAttribute type = mpGui->GetSkinAttributeFromString(sType);
        if(type == eGuiSkinAttribute_LastEnum)
        {
            continue;
        }

        tString sValue = cString::ToString(Xml::Attr(pAttributeElem, "value"), "");

        cVector3f vVal(0);
        tFloatVec vValues;
        tString sSepp = " ";
        cString::GetFloatVec(sValue, vValues, &sSepp);

        if(vValues.size() > 0)
        {
            vVal.x = vValues[0];
        }
        if(vValues.size() > 1)
        {
            vVal.y = vValues[1];
        }
        if(vValues.size() > 2)
        {
            vVal.z = vValues[2];
        }

        mvAttributes[type] = vVal;
        vAttribLoaded[type] = true;
    }

    // Error checking, see so all elements are there
    for(int i = 0; i < eGuiSkinAttribute_LastEnum; ++i)
    {
        if(vAttribLoaded[i] == false)
        {
            Error("Could not find gui skin '%s' attribute %s!\n", msName.c_str(), mpGui->GetSkinAttributeString((eGuiSkinAttribute)i).c_str());
        }
    }



    //////////////////////////////////////
    // FONTS
    pugi::xml_node pFontsElement = pRootElem.child("Fonts");

    //Iterate font elements
    pugi::xml_node pFontElem = Xml::FirstElem(pFontsElement);
    for(; pFontElem; pFontElem = Xml::NextElem(pFontElem))
    {
        tString sType = cString::ToString(Xml::Attr(pFontElem, "type"), "");

        eGuiSkinFont type = mpGui->GetSkinFontFromString(sType);
        if(type == eGuiSkinFont_LastEnum)
        {
            continue;
        }

        tString sFontFile = cString::ToString(Xml::Attr(pFontElem, "file"), "");
        cVector2f vSize = cString::ToVector2f(Xml::Attr(pFontElem, "size"), 1);
        cColor color = cString::ToColor(Xml::Attr(pFontElem, "color"), cColor(1, 1));

        cGuiSkinFont *pFont = hplNew( cGuiSkinFont, (mpGui) );

        pFont->mpFont = mpGui->GetResources()->GetFontManager()->CreateFontData(sFontFile);
        pFont->mvSize = vSize;
        pFont->mColor = color;


        mvFonts[type] = pFont;
    }

    // Error checking, see so all elements are there
    for(int i = 0; i < eGuiSkinFont_LastEnum; ++i)
    {
        if(mvFonts[i] == NULL)
        {
            Error("Could not find gui skin '%s' font %s!\n", msName.c_str(), mpGui->GetSkinFontString((eGuiSkinFont)i).c_str());
        }
    }


    //////////////////////////////////////
    // GFX ELEMENTS

    //Get gfx element
    pugi::xml_node pGfxElementsElement = pRootElem.child("GfxElements");

    //Iterate gfx elements
    pugi::xml_node pGfxElem = Xml::FirstElem(pGfxElementsElement);
    for(; pGfxElem; pGfxElem = Xml::NextElem(pGfxElem))
    {
        tString sType = cString::ToString(Xml::Attr(pGfxElem, "type"), "");

        eGuiSkinGfx type = mpGui->GetSkinGfxFromString(sType);
        if(type == eGuiSkinGfx_LastEnum)
        {
            continue;
        }

        tString sFile = cString::ToString(Xml::Attr(pGfxElem, "file"), "");
        cVector2f vOffset = cString::ToVector2f(Xml::Attr(pGfxElem, "offset"), 0);
        cVector2f vSize = cString::ToVector2f(Xml::Attr(pGfxElem, "active_size"), -1);
        cColor color = cString::ToColor(Xml::Attr(pGfxElem, "color"), cColor(1, 1));
        eGuiMaterial material = ToMaterial(Xml::Attr(pGfxElem, "material"));

        bool bAnimated = cString::ToBool(Xml::Attr(pGfxElem, "animated"), false);
        float fAnimFrameTime = cString::ToFloat(Xml::Attr(pGfxElem, "anim_frame_time"), 1);
        eGuiGfxAnimationType animType = ToAnimType(Xml::Attr(pGfxElem, "anim_mode"));

        cGuiGfxElement *pGfx = NULL;
        if(sFile != "")
        {
            if(bAnimated)
            {
                pGfx = mpGui->CreateGfxImageBuffer(sFile, material, true, color);
            }
            else
            {
                pGfx = mpGui->CreateGfxImage(sFile, material, color);
            }
        }
        else
        {
            pGfx = mpGui->CreateGfxFilledRect(color, material);
        }

        if(pGfx)
        {
            if(bAnimated)
            {
                cGuiGfxAnimation *pAnim = pGfx->GetAnimation(0);
                pAnim->SetFrameLength(fAnimFrameTime);
                pAnim->SetType(animType);
            }

            if(vSize.x >= 0)
            {
                pGfx->SetActiveSize(vSize);
            }
            pGfx->SetOffset(cVector3f(vOffset.x, vOffset.y, 0));

            mvGfxElements[type] = pGfx;
        }
    }

    // Error checking, see so all elements are there
    for(int i = 0; i < eGuiSkinGfx_LastEnum; ++i)
    {
        if(mvGfxElements[i] == NULL)
        {
            Error("Could not find gui skin '%s' gfx %s!\n", msName.c_str(), mpGui->GetSkinGfxString((eGuiSkinGfx)i).c_str());
        }
    }


    return true;
}

//-----------------------------------------------------------------------

cGuiGfxElement *cGuiSkin::GetGfx(eGuiSkinGfx aType)
{
    return mvGfxElements[aType];
}

cGuiSkinFont *cGuiSkin::GetFont(eGuiSkinFont aType)
{
    return mvFonts[aType];
}

const cVector3f &cGuiSkin::GetAttribute(eGuiSkinAttribute aType)
{
    return mvAttributes[aType];
}
//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

}
