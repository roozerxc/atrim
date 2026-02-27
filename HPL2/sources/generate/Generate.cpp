#include "generate/Generate.h"

#include "system/LowLevelSystem.h"

#include "generate/VoxelMap.h"


namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cGenerate::cGenerate()
    {
    }

    //-----------------------------------------------------------------------

    cGenerate::~cGenerate()
    {
        Log("Exiting Generate Module\n");
        Log("--------------------------------------------------------\n");

        Log("--------------------------------------------------------\n\n");
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    
    void cGenerate::Init(cResources *apResources, cGraphics *apGraphics)
    {
        mpResources = apResources;
        mpGraphics = apGraphics;
    }

    //-----------------------------------------------------------------------

    cVoxelMap* cGenerate::CreateVoxelMap(const cVector3l& avSize)
    {
        return hplNew(cVoxelMap, (avSize));
    }

    void cGenerate::DestroyVoxelMap(cVoxelMap* apMap)
    {
        hplDelete(apMap);
    }

    //-----------------------------------------------------------------------

}
