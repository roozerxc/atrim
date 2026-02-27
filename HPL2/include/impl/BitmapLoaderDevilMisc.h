#ifndef HPL_BITMAP_LOADER_DEVIL_MISC_H
#define HPL_BITMAP_LOADER_DEVIL_MISC_H

#include "impl/BitmapLoaderDevil.h"

namespace hpl {

    class cBitmapLoaderDevilMisc : public iBitmapLoaderDevil
    {
    public:
        cBitmapLoaderDevilMisc();
        ~cBitmapLoaderDevilMisc();

        cBitmap* LoadBitmap(const tWString& asFile, tBitmapLoadFlag aFlags);

    protected:
    };

};
#endif // HPL_BITMAP_LOADER_DEVIL_MISC_H
