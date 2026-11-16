#include "graphics/LowLevelGraphics.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// STATIC DATA
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool iLowLevelGraphics::mbForceShaderModel3And4Off = false;
bool iLowLevelGraphics::mbForceGLSL110 = false;
bool iLowLevelGraphics::mbForceNoStencil = false;
bool iLowLevelGraphics::mbForceRectAs2D = false;
bool iLowLevelGraphics::mbLogGLErrors = false;
int iLowLevelGraphics::mlMaxFramesAhead = -1;
int iLowLevelGraphics::mlFullscreenMode = 0;
bool iLowLevelGraphics::mbForceVertexArrays = false;
bool iLowLevelGraphics::mbPostEffectsDisabled = false;
bool iLowLevelGraphics::mbRectAs2D = false;

//-----------------------------------------------------------------------
}
