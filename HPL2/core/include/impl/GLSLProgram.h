#ifndef HPL_GLSL_PROGRAM_H
#define HPL_GLSL_PROGRAM_H

//#include <windows.h>

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GPUShader.h"

#include "graphics/GPUProgram.h"

#include <GL/glew.h>

namespace hpl
{

//-----------------------------------------------------

class cGLSLParam
{
public:
    cGLSLParam() : mlId(-1), msName("") {}
    cGLSLParam(GLint alId, const tString& asName) : mlId(alId), msName(asName) {}

    tString msName;
    GLint mlId;
};

//-----------------------------------------------------

class cGLSLProgram : public iGpuProgram
{
public:
    cGLSLProgram(const tString& asName);
    ~cGLSLProgram();

    bool Link();

    void Bind();
    void UnBind();

    bool CanAccessAPIMatrix()
    {
        return true;
    }
    bool SamplerNeedsTextureUnitSetup()
    {
        return true;
    }

    /**
     * Binds and unbinds program so must not be done during rendering.
     */
    bool SetSamplerToUnit(const tString& asSamplerName, int alUnit);

    int GetVariableId(const tString& asName);
    bool GetVariableAsId(const tString& asName, int alId);

    bool SetInt(int alVarId, int alX);
    bool SetFloat(int alVarId, float afX);
    bool SetVec2f(int alVarId, float afX,float afY);
    bool SetVec3f(int alVarId, float afX,float afY,float afZ);
    bool SetVec4f(int alVarId, float afX,float afY,float afZ, float afW);

    bool SetMatrixf(int alVarId, const cMatrixf& aMtx);
    bool SetMatrixf(int alVarId, eGpuShaderMatrix aType, eGpuShaderMatrixOp aOp);

private:
    void LogProgramInfoLog();

    GLuint mlHandle;
    static int mlCurrentProgram;

    std::vector<cGLSLParam> mvParameters;
};
};
#endif // HPL_GLSL_PROGRAM_H
