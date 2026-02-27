#ifndef HPL_VOXEL_MAP_H
#define HPL_VOXEL_MAP_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "generate/GenerateTypes.h"

namespace hpl {

    //-------------------------------

    class iLowLevelGraphics;

    //-------------------------------

    class cVoxelMap
    {
    public:
        cVoxelMap(const cVector3l& avSize);
        ~cVoxelMap();

        /////////////////////////////////
        // Action
        void SetVoxel(const cVector3l &avPos, char alVal);

        /////////////////////////////////
        // Properties
        const cVector3l& GetSize(){ return mvSize; }
        void SetSize(const cVector3l& avSize);

        float GetVoxelSize(){ return mfVoxelSize;}
        void SetVoxelSize(float afX){ mfVoxelSize = afX;}

        const cVector3f& GetPosition(){ return mvPosition;}
        void SetPosition(const cVector3f& avPos){ mvPosition = avPos;}

        /////////////////////////////////
        // Debug
        void DebugRender(iLowLevelGraphics *apLowGfx, const cColor &aCol);

    private:
        unsigned char* mpData;

        cVector3l mvSize;

        float mfVoxelSize;
        cVector3f mvPosition;
    };

};
#endif // HPL_VOXEL_MAP_H
