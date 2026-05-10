#ifndef HPL_GENERATE_H
#define HPL_GENERATE_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "generate/GenerateTypes.h"

namespace hpl
{

//-------------------------------------

class cGraphics;
class cResources;

class cVoxelMap;

//-------------------------------------

class cGenerate
{
public:
    cGenerate();
    ~cGenerate();

    void Init(cResources *apResources, cGraphics *apGraphics);

    cVoxelMap* CreateVoxelMap(const cVector3l& avSize);
    void DestroyVoxelMap(cVoxelMap* apMap);

private:
    cResources *mpResources;
    cGraphics *mpGraphics;
};

};
#endif // HPL_GENERATE_H
