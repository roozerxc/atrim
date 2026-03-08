#include "graphics/GPUShader.h"

namespace hpl
{

bool iGpuShader::mbDebugInfo=false;

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iGpuShader::AddSamplerUnit(const tString& asSamplerName, int alUnit)
{
    cGpuShader_SamplerUnit samplerUnit;
    samplerUnit.msName = asSamplerName;
    samplerUnit.mlUnit = alUnit;

    mvSamplerUnits.push_back(samplerUnit);
}


//-----------------------------------------------------------------------

}
