#ifndef HPL_BITMAP_LOADER_DEVIL_DDS_H
#define HPL_BITMAP_LOADER_DEVIL_DDS_H

#include "impl/BitmapLoaderDevil.h"

namespace hpl {

    class cBitmapLoaderDevilDDS : public iBitmapLoaderDevil
    {
    public:
        cBitmapLoaderDevilDDS();
        ~cBitmapLoaderDevilDDS();

        cBitmap* LoadBitmap(const tWString& asFile, tBitmapLoadFlag aFlags);

        
    protected:
        ePixelFormat GetPixelFormatFromILDXT(int alDxtFormat);
    };

};
#endif // HPL_BITMAP_LOADER_DEVIL_DDS_H
