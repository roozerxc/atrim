#include "generate/VoxelMap.h"

#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"

#include <cstring>

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cVoxelMap::cVoxelMap(const cVector3l& avSize)
{
    mpData = NULL;
    SetSize(avSize);

    mfVoxelSize = 0.1f;
    mvPosition = 0;
}

//-----------------------------------------------------------------------

cVoxelMap::~cVoxelMap()
{
    hplDeleteArray(mpData);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cVoxelMap::SetVoxel(const cVector3l &avPos, char alVal)
{
    mpData[avPos.z * mvSize.x*mvSize.y + avPos.y * mvSize.x + avPos.x] = alVal;
}

//-----------------------------------------------------------------------

void cVoxelMap::SetSize(const cVector3l& avSize)
{
    if(mpData) hplDeleteArray(mpData);

    mvSize = avSize;

    mpData = hplNewArray(unsigned char, mvSize.x*mvSize.y*mvSize.z*sizeof(unsigned char));
    memset(mpData, 0, mvSize.x*mvSize.y*mvSize.z*sizeof(unsigned char));
}

//-----------------------------------------------------------------------

void cVoxelMap::DebugRender(iLowLevelGraphics *apLowGfx, const cColor &aCol)
{
    for(int z=0; z<mvSize.z; ++z)
        for(int y=0; y<mvSize.y; ++y)
            for(int x=0; x<mvSize.x; ++x)
            {
                if(mpData[z * mvSize.x*mvSize.y + y * mvSize.x + x]==1)
                {
                    apLowGfx->DrawSphere(mvPosition + cVector3f((float)x, (float)y, (float)z)*mfVoxelSize + cVector3f(mfVoxelSize/2), mfVoxelSize/2, aCol);
                }
            }
}

//-----------------------------------------------------------------------

}
