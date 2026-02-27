#ifndef HPL_TEXTURE_CREATOR_H
#define HPL_TEXTURE_CREATOR_H

#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"

namespace hpl {

    class iLowLevelGraphics;
    class cResources;
    class iTexture;

    class cTextureCreator
    {
    public:
        cTextureCreator(iLowLevelGraphics *apLowLevelGraphics, cResources *apResources);
        ~cTextureCreator();
        
        /**
         * Creates a 3d texture with alSize * alSize number of scatter disks with a certain number of samples. The samples are uniformly distribute on the disk.
         * \param *apOffsetTexture A 3d texture where the data will be placed.
         * \param alSize The number of scatter disks per axis (16 creates a texture with height and with 16). Must be power of two.
         * \param alSamples Samples per scatter disk. Number of samples /2 is the depth of the texture.
         * \param abSortSamples If the samples should be sorted, largest -> smallest.
         */
        void GenerateScatterDiskMap3D(iTexture *apOffsetTexture, int alSize, int alSamples, bool abSortSamples);
        void GenerateScatterDiskMap2D(iTexture *apOffsetTexture, int alSize, int alSamples, bool abSortSamples);
        
    private:
        iLowLevelGraphics *mpLowLevelGraphics;
        cResources *mpResources;
    };

};
#endif // HPL_TEXTURE_CREATOR_H


