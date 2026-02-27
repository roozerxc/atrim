#include "graphics/GPUProgram.h"

#include "resources/Resources.h"
#include "resources/GpuShaderManager.h"

namespace hpl{

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    iGpuProgram::iGpuProgram(const tString& asName,eGpuProgramFormat aProgramFormat) 
    {
        msName = asName;

        mpResources = NULL;

        mProgramFormat = aProgramFormat;

        mbAutoDestroyShaders = true;

        for(int i=0; i<2; ++i) mpShader[i] = NULL;

        mlUserId = 0;
    }

    iGpuProgram::~iGpuProgram()
    {
        if(mbAutoDestroyShaders && mpResources)
        {
            for(int i=0; i<2; ++i) 
            {
                if(mpShader[i]) mpResources->GetGpuShaderManager()->Destroy(mpShader[i]); 
            }
        }
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    void iGpuProgram::SetShader(eGpuShaderType aType, iGpuShader *apShader)
    {
        mpShader[aType] = apShader;
    }

    //-----------------------------------------------------------------------

}
