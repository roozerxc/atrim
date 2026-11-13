#ifndef HPL_GLSL_SHADER_H
#define HPL_GLSL_SHADER_H

//#include <windows.h>
#include <GL/glew.h>

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GPUShader.h"

namespace hpl
{

//----------------------------------------------

class iLowLevelGraphics;

//----------------------------------------------
class cGLSLShader : public iGpuShader
{
public:
    cGLSLShader(const tString& asName, eGpuShaderType aType, iLowLevelGraphics *apLowLevelGraphics);
    ~cGLSLShader();

    bool Reload();
    void Unload();
    void Destroy();

    bool SamplerNeedsTextureUnitSetup()
    {
        return true;
    }

    tString GetProgramName()
    {
        return msName;
    }

    bool CreateFromFile(const tWString& asFile, const tString& asEntry = "main", bool abPrintInfoIfFail = true);
    bool CreateFromString(const char *apStringData, const tString& asEntry = "main", bool abPrintInfoIfFail = true);

    //GLSL Specific
    GLuint GetHandle()
    {
        return mlHandle;
    }

    //Rect samplers that were turned into 2D samplers, each has a "gvRectScale_<name>" uniform.
    const tStringVec& GetRectSamplers()
    {
        return mvRectSamplers;
    }

protected:
    void LogShaderInfoLog();
    void LogShaderCode(const char *apStringData);
    GLenum GetGLShaderType(eGpuShaderType aType);

    iLowLevelGraphics *mpLowLevelGraphics;

    GLuint mlHandle;
    tStringVec mvRectSamplers;
};
};
#endif // HPL_GLSL_SHADER_H
