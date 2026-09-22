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

#include "impl/tinyXML/tinyxml.h"

#include "system/String.h"
#include "system/Platform.h"

namespace hpl
{


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cSDLFontData::cSDLFontData(const tString &asName,iLowLevelGraphics* apLowLevelGraphics)
    : iFontData(asName,apLowLevelGraphics)
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
    FILE *pFile = cPlatform::OpenFile(asFileName, _W("rb"));
    if(pFile==NULL)
    {
        return false;
    }

    TiXmlDocument *pXmlDoc = hplNew( TiXmlDocument,() );
    if(pXmlDoc->LoadFile(pFile)==false)
    {
        Error("Couldn't load angle code font file '%s'\n",asFileName.c_str());
        fclose(pFile);
        hplDelete(pXmlDoc);
        return false;
    }

    TiXmlElement *pRootElem = pXmlDoc->RootElement();

    ////////////////////////////////////////////
    // Load Common info
    TiXmlElement *pCommonElem = pRootElem->FirstChildElement("common");

    int lLineHeight = cString::ToInt(pCommonElem->Attribute("lineHeight"),0);
    int lBase = cString::ToInt(pCommonElem->Attribute("base"),0);

    mfHeight = (float)lLineHeight;

    mvSizeRatio.x = (float)lBase / (float)lLineHeight;//I think this is a not correct. Not sure what is done here :S
    mvSizeRatio.y = 1;

    int lLargestGlyphId=-1;

    ////////////////////////////////////////////
    // Check for largest glyph number and resize array.
    TiXmlElement *pCharsRootElem = pRootElem->FirstChildElement("chars");
    TiXmlElement *pCharElem = pCharsRootElem->FirstChildElement("char");
    for(; pCharElem != NULL; pCharElem = pCharElem->NextSiblingElement("char"))
    {
        int lId = cString::ToInt(pCharElem->Attribute("id"),0);
        if(lId >lLargestGlyphId)
        {
            lLargestGlyphId = lId;
        }
    }

    mlFirstChar =0;
    mlLastChar = lLargestGlyphId;
    mvGlyphs.resize(lLargestGlyphId+1, NULL);

    ////////////////////////////////////////////
    // Load bitmaps
    std::vector<cFrameTexture*> vFrameTextures;

    TiXmlElement *pPagesRootElem = pRootElem->FirstChildElement("pages");

    int lCount=0;
    TiXmlElement *pPageElem = pPagesRootElem->FirstChildElement("page");
    for(; pPageElem != NULL; pPageElem = pPageElem->NextSiblingElement("page"), ++lCount)
    {
        tWString sFileName = cString::To16Char(pPageElem->Attribute("file"));
        tWString sFilePath = cString::SetFilePathW(sFileName,sPath);

        //////////////////////////////
        //Load image to bitmap
        cBitmap *pBitmap = mpResources->GetBitmapLoaderHandler()->LoadBitmap(sFilePath, 0);
        if(pBitmap==NULL)
        {
            Error("Couldn't load bitmap %s for FNT file '%s'\n",cString::To8Char(sFilePath).c_str(),cString::To8Char(asFileName).c_str());
            hplDelete(pXmlDoc);
            return false;
        }

        //Make a fix here so we get a white luminence and alpha filled with data.
        if(pBitmap->GetPixelFormat() == ePixelFormat_Luminance)
        {
            cBitmap *pTempBitmap = hplNew( cBitmap, ());
            pTempBitmap->CreateData(pBitmap->GetSize(),ePixelFormat_LuminanceAlpha,0,0);

            pBitmap->SetPixelFormat(ePixelFormat_Alpha);
            pTempBitmap->Blit(pBitmap,0,pBitmap->GetSize(),0);

            hplDelete( pBitmap );

            pBitmap = pTempBitmap;
        }
        //Log("Loaded bitmap %s, bpp: %d pixelformat: %d\n",sFileName.c_str(),pBitmap->GetBytesPerPixel(), pBitmap->GetPixelFormat());

        ///////////////////////
        //Create a texture from bitmap (do not want to load it from texture manager since that would delete the texture on its own).
        tString sName = cString::SetFileExt(cString::To8Char(asFileName),"")+"_"+cString::ToString(lCount);
        iTexture *pTexture = mpLowLevelGraphics->CreateTexture("",eTextureType_2D,eTextureUsage_Normal);

        pTexture->CreateFromBitmap(pBitmap);

        hplDelete( pBitmap ); //Bitmap no longer needed

        ///////////////////////
        //Create Custom Frame for images
        cFrameTexture *pFrameTexture = mpResources->GetImageManager()->CreateCustomFrame(pTexture);

        vFrameTextures.push_back(pFrameTexture);
    }

    ////////////////////////////////////////////
    // Load glyphs
    pCharsRootElem = pRootElem->FirstChildElement("chars");
    pCharElem = pCharsRootElem->FirstChildElement("char");
    for(; pCharElem != NULL; pCharElem = pCharElem->NextSiblingElement("char"))
    {
        //Get the info on the character
        int lId = cString::ToInt(pCharElem->Attribute("id"),0);
        int lX = cString::ToInt(pCharElem->Attribute("x"),0);
        int lY = cString::ToInt(pCharElem->Attribute("y"),0);

        int lW = cString::ToInt(pCharElem->Attribute("width"),0);
        int lH = cString::ToInt(pCharElem->Attribute("height"),0);

        int lXOffset = cString::ToInt(pCharElem->Attribute("xoffset"),0);
        int lYOffset = cString::ToInt(pCharElem->Attribute("yoffset"),0);

        int lAdvance = cString::ToInt(pCharElem->Attribute("xadvance"),0);

        int lPage = cString::ToInt(pCharElem->Attribute("page"),0);

        if(lId<0 || lId>=(int)mvGlyphs.size())
        {
            Warning("Font '%s' contain glyph with invalid id: %d. Skipping loading of it!\n", cString::To8Char(asFileName).c_str(), lId);
            continue;
        }

        //Get the bitmap where the character graphics is
        cFrameTexture* pFrameTexture = vFrameTextures[lPage];

        cFrameSubImage *pImage = pFrameTexture->CreateCustomImage(cVector2l(lX, lY),cVector2l(lW,lH));

        //Create glyph and place it correctly.
        cGlyph *pGlyph = CreateGlyph(pImage,cVector2l(lXOffset,lYOffset),cVector2l(lW,lH),
                                     cVector2l(lBase,lLineHeight),lAdvance);

        mvGlyphs[lId] = pGlyph;

    }

    //Destroy XML
    fclose(pFile);
    hplDelete(pXmlDoc);
    return true;
}

//-----------------------------------------------------------------------

}
