#include "impl/BitmapLoaderDevilDDS.h"

#include "graphics/Bitmap.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "graphics/LowLevelGraphics.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cBitmapLoaderDevilDDS::cBitmapLoaderDevilDDS() : iBitmapLoaderDevil()
{
    AddSupportedExtension("dds");
}

cBitmapLoaderDevilDDS::~cBitmapLoaderDevilDDS()
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cBitmap* cBitmapLoaderDevilDDS::LoadBitmap(const tWString& asFile, tBitmapLoadFlag aFlags)
{
    Initialize();

    //create image id
    unsigned int lImageId;
    ilGenImages(1,&lImageId);

    //Bind image
    ilBindImage(lImageId);

    //Make sure compressed data is kept
    ilSetInteger(IL_KEEP_DXTC_DATA, IL_TRUE);

    //Try and load the file.
    if(LoadDevilImageW(asFile)==false)
    {
        ilDeleteImages(1,&lImageId);
        return NULL;
    }

    cBitmap *pBitmap = hplNew(cBitmap, () );

    ////////////////////////////////////////
    //Get main image properties
    int lNumOfImages = ilGetInteger(IL_NUM_IMAGES) + 1; //Returns number of images - 1, so need to add 1
    int lNumOfMipMaps = ilGetInteger(IL_NUM_MIPMAPS) + 1; //We count first image as mimap too, so add 1
    int lCubeFlags = ilGetInteger(IL_IMAGE_CUBEFLAGS);
    bool useFacesInsteadOfImages = false;

    if (lCubeFlags != 0 && lNumOfImages != 6)
    {
        lNumOfImages = ilGetInteger(IL_NUM_FACES) + 1;
        if (lNumOfImages > 1)
            useFacesInsteadOfImages = true;
        else
            lNumOfImages = 6;
    }

    //No need to setup if only one image and mipmap, data is already setup for that.
    if(lNumOfImages > 1 || lNumOfMipMaps > 1)
    {
        pBitmap->SetUpData(lNumOfImages, lNumOfMipMaps);
    }

    //Fet the size of image,
    cVector3l vSize;
    vSize.x = ilGetInteger(IL_IMAGE_WIDTH);
    vSize.y = ilGetInteger(IL_IMAGE_HEIGHT);
    vSize.z = ilGetInteger(IL_IMAGE_DEPTH);
    pBitmap->SetSize(vSize);

    //Det properties on the pixel data
    int lBytesPerPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
    ePixelFormat pixelFormat = DevilPixelFormatToHPL(ilGetInteger(IL_IMAGE_FORMAT));

    //Get the compression format used (if any)
    int lDXTFormat = ilGetInteger( IL_DXTC_DATA_FORMAT );

    /*Log("Image: %s\n",cString::To8Char(asFile).c_str());
    Log(" Number of mipmaps: %d\n",lNumOfMipMaps);
    Log(" Number of image: %d\n",lNumOfImages);
    Log(" Compression: %d\n",lDXTFormat != IL_DXT_NO_COMP);*/

    //////////////////////////////////////////////////
    // Load compressed image
    if(    lDXTFormat != IL_DXT_NO_COMP && //mpLowLevelGraphics->GetCaps(eGraphicCaps_TextureCompression_DXTC) &&
            (aFlags & eBitmapLoadFlag_ForceNoCompression)==0 )
    {
        ePixelFormat compressedPixelFormat = GetPixelFormatFromILDXT(lDXTFormat);
        pBitmap->SetBytesPerPixel(lBytesPerPixel);
        pBitmap->SetIsCompressed(true);
        pBitmap->SetPixelFormat(compressedPixelFormat);

        for(int image=0; image< lNumOfImages; ++image)
            for(int mip=0; mip< lNumOfMipMaps; ++mip)
            {
                if(lNumOfImages > 1 || lNumOfMipMaps > 1)
                {
                    ilBindImage(lImageId); // For some reason this is needed....
                    if (lNumOfImages > 1)
                    {
                        if (useFacesInsteadOfImages)
                            ilActiveFace(image);
                        else
                            ilActiveImage(image);
                    }
                    if(lNumOfMipMaps > 1)
                        ilActiveMipmap(mip);
                }

                cBitmapData *pImage = pBitmap->GetData(image,mip);

                int lSize = ilGetDXTCData(NULL, 0, lDXTFormat);
                pImage->mlSize = lSize;
                pImage->mpData = hplNewArray(unsigned char,lSize);

                ilGetDXTCData(pImage->mpData, lSize, lDXTFormat);

                //int format = ilGetInteger( IL_DXTC_DATA_FORMAT );
                //Log("  %d bounds: %dx%d size: %d\n",mip,ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT),lSize);

            }
    }
    //////////////////////////////////////////////////
    // Load uncompressed image
    else
    {
        pBitmap->SetBytesPerPixel(lBytesPerPixel);
        pBitmap->SetIsCompressed(false);
        pBitmap->SetPixelFormat(pixelFormat);

        for(int image=0; image< lNumOfImages; ++image)
            for(int mip=0; mip< lNumOfMipMaps; ++mip)
            {
                if(lNumOfImages > 1 || lNumOfMipMaps > 1)
                {
                    ilBindImage(lImageId); // For some reason this is needed....
                    if (lNumOfImages > 1)
                    {
                        if (useFacesInsteadOfImages)
                            ilActiveFace(image);
                        else
                            ilActiveImage(image);
                    }
                    if (lNumOfMipMaps > 1)
                        ilActiveMipmap(mip);
                }

                cBitmapData *pImage = pBitmap->GetData(image,mip);
                int lSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
                pImage->SetData(ilGetData(), lSize);
            }
    }


    ilDeleteImages(1,&lImageId);

    return pBitmap;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

ePixelFormat cBitmapLoaderDevilDDS::GetPixelFormatFromILDXT(int alDxtFormat)
{
    switch(alDxtFormat)
    {
    case IL_DXT1:
        return ePixelFormat_DXT1;
    case IL_DXT3:
        return ePixelFormat_DXT3;
    case IL_DXT5:
        return ePixelFormat_DXT5;
    }

    return ePixelFormat_Unknown;
}

//-----------------------------------------------------------------------
}
