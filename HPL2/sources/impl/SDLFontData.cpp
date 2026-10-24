#include "impl/SDLFontData.h"

#include "resources/Resources.h"
#include "graphics/LowLevelGraphics.h"
#include "system/LowLevelSystem.h"
#include "resources/BitmapLoaderHandler.h"
#include "graphics/Bitmap.h"
#include "resources/ImageManager.h"
#include "graphics/FrameTexture.h"
#include "graphics/FrameSubImage.h"
#include "graphics/Texture.h"

#include "impl/XmlPugi.h"

#include "system/String.h"
#include "system/Platform.h"

namespace hpl
{


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cSDLFontData::cSDLFontData(const tString &asName, iLowLevelGraphics* apLowLevelGraphics)
    : iFontData(asName, apLowLevelGraphics)
{


}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cSDLFontData::CreateFromBitmapFile(const tWString &asFileName)
{
    SetFullPath(asFileName);

    tWString sPath = cString::GetFilePathW(asFileName);

    ////////////////////////////////////////////
    // Load xml file
    if(cPlatform::FileExists(asFileName) == false)
    {
        return false;
    }

    pugi::xml_document xmlDoc;
    if(Xml::LoadFile(xmlDoc, asFileName) == false)
    {
        Error("Couldn't load angle code font file '%s', file does not exist or is not in XML format\n", cString::To8Char(asFileName).c_str());
        return false;
    }

    pugi::xml_node pRootElem = xmlDoc.document_element();

    ////////////////////////////////////////////
    // Load Common info
    pugi::xml_node pCommonElem = pRootElem.child("common");

    int lLineHeight = cString::ToInt(Xml::Attr(pCommonElem, "lineHeight"), 0);
    int lBase = cString::ToInt(Xml::Attr(pCommonElem, "base"), 0);

    //////////////////////////////////////////
    // Load info
    pugi::xml_node pInfoElem = pRootElem.child("info");
    int lOutline = cString::ToInt(Xml::Attr(pInfoElem, "outline"), 0) * 2;

    /////////////
    // Apply outline to the scale
    mfHeight = (float)(lLineHeight + lOutline);

    mvSizeRatio.x = (float)(lBase + lOutline) / (float)(lLineHeight + lOutline);//I think this is a not correct. Not sure what is done here :S
    mvSizeRatio.y = 1;

    int lLargestGlyphId = -1;

    ////////////////////////////////////////////
    // Check for largest glyph number and resize array.
    pugi::xml_node pCharsRootElem = pRootElem.child("chars");
    pugi::xml_node pCharElem = pCharsRootElem.child("char");
    for(; pCharElem; pCharElem = pCharElem.next_sibling("char"))
    {
        int lId = cString::ToInt(Xml::Attr(pCharElem, "id"), 0);
        if(lId > lLargestGlyphId)
        {
            lLargestGlyphId = lId;
        }
    }

    mlFirstChar = 0;
    mlLastChar = lLargestGlyphId;
    mvGlyphs.resize(lLargestGlyphId + 1, NULL);

    ////////////////////////////////////////////
    // Load bitmaps
    std::vector<cFrameTexture *> vFrameTextures;

    pugi::xml_node pPagesRootElem = pRootElem.child("pages");

    int lCount = 0;
    pugi::xml_node pPageElem = pPagesRootElem.child("page");
    for(; pPageElem; pPageElem = pPageElem.next_sibling("page"), ++lCount)
    {
        tWString sFileName = cString::To16Char(pPageElem.attribute("file").value());
        tWString sFilePath = cString::SetFilePathW(sFileName, sPath);

        //////////////////////////////
        //Load image to bitmap
        cBitmap *pBitmap = mpResources->GetBitmapLoaderHandler()->LoadBitmap(sFilePath, 0);
        if(pBitmap == NULL)
        {
            Error("Couldn't load bitmap %s for FNT file '%s'\n", cString::To8Char(sFilePath).c_str(), cString::To8Char(asFileName).c_str());
            return false;
        }

        //Make a fix here so we get a white luminence and alpha filled with data.
        if(pBitmap->GetPixelFormat() == ePixelFormat_Luminance)
        {
            cBitmap *pTempBitmap = hplNew( cBitmap, ());
            pTempBitmap->CreateData(pBitmap->GetSize(), ePixelFormat_LuminanceAlpha, 0, 0);

            pBitmap->SetPixelFormat(ePixelFormat_Alpha);
            pTempBitmap->Blit(pBitmap, 0, pBitmap->GetSize(), 0);

            hplDelete( pBitmap );

            pBitmap = pTempBitmap;
        }
        //Log("Loaded bitmap %s, bpp: %d pixelformat: %d\n",sFileName.c_str(),pBitmap->GetBytesPerPixel(), pBitmap->GetPixelFormat());

        ///////////////////////
        //Create a texture from bitmap (do not want to load it from texture manager since that would delete the texture on its own).
        tString sName = cString::SetFileExt(cString::To8Char(asFileName), "") + "_" + cString::ToString(lCount);
        iTexture *pTexture = mpLowLevelGraphics->CreateTexture("", eTextureType_2D, eTextureUsage_Normal);

        pTexture->CreateFromBitmap(pBitmap);

        hplDelete( pBitmap ); //Bitmap no longer needed

        ///////////////////////
        //Create Custom Frame for images
        cFrameTexture *pFrameTexture = mpResources->GetImageManager()->CreateCustomFrame(pTexture);

        vFrameTextures.push_back(pFrameTexture);
    }

    ////////////////////////////////////////////
    // Load glyphs
    pCharsRootElem = pRootElem.child("chars");
    pCharElem = pCharsRootElem.child("char");
    for(; pCharElem; pCharElem = pCharElem.next_sibling("char"))
    {
        //Get the info on the character
        int lId = cString::ToInt(Xml::Attr(pCharElem, "id"), 0);
        int lX = cString::ToInt(Xml::Attr(pCharElem, "x"), 0);
        int lY = cString::ToInt(Xml::Attr(pCharElem, "y"), 0);

        int lW = cString::ToInt(Xml::Attr(pCharElem, "width"), 0);
        int lH = cString::ToInt(Xml::Attr(pCharElem, "height"), 0);

        int lXOffset = cString::ToInt(Xml::Attr(pCharElem, "xoffset"), 0);
        int lYOffset = cString::ToInt(Xml::Attr(pCharElem, "yoffset"), 0);

        int lAdvance = cString::ToInt(Xml::Attr(pCharElem, "xadvance"), 0);

        int lPage = cString::ToInt(Xml::Attr(pCharElem, "page"), 0);

        if(lId < 0 || lId >= (int)mvGlyphs.size())
        {
            Warning("Font '%s' contain glyph with invalid id: %d. Skipping loading of it!\n", cString::To8Char(asFileName).c_str(), lId);
            continue;
        }

        //Get the bitmap where the character graphics is
        cFrameTexture* pFrameTexture = vFrameTextures[lPage];

        cFrameSubImage *pImage = pFrameTexture->CreateCustomImage(cVector2l(lX, lY), cVector2l(lW, lH));

        //Create glyph and place it correctly.
        cGlyph *pGlyph = CreateGlyph(pImage, cVector2l(lXOffset, lYOffset), cVector2l(lW, lH),
                                     cVector2l(lBase + lOutline, lLineHeight + lOutline), (lOutline + lAdvance * 2) / 2);

        mvGlyphs[lId] = pGlyph;

    }

    //Destroy XML
    return true;
}

//-----------------------------------------------------------------------

}
