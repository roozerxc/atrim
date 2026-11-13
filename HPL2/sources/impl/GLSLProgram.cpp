#include "impl/GLSLProgram.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "impl/SDLTexture.h"
#include "impl/GLSLShader.h"
#include "impl/LowLevelGraphicsSDL.h"



namespace hpl
{

//-----------------------------------------------------------------------

int cGLSLProgram::mlCurrentProgram = 0;
cGLSLProgram *cGLSLProgram::mpCurrentProgramPtr = NULL;

//Scale of the rect texture on each texture unit (rect textures backed by 2D textures).
static const int kRectScaleUnits = 32;
static float gvUnitRectScale[kRectScaleUnits][2];
static bool gbUnitRectScaleInit = false;

static void InitUnitRectScales()
{
    if(gbUnitRectScaleInit)
    {
        return;
    }
    gbUnitRectScaleInit = true;
    for(int i = 0; i < kRectScaleUnits; ++i)
    {
        gvUnitRectScale[i][0] = 1.0f;
        gvUnitRectScale[i][1] = 1.0f;
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cGLSLProgram::cGLSLProgram(const tString& asName) : iGpuProgram(asName, eGpuProgramFormat_GLSL)
{
    mlHandle = glCreateProgram();

}

//-----------------------------------------------------------------------

cGLSLProgram::~cGLSLProgram()
{
    if(mpCurrentProgramPtr == this)
    {
        mpCurrentProgramPtr = NULL;
    }

    for(int i = 0; i < 2; ++i)
    {
        cGLSLShader* pGLSLShader = static_cast<cGLSLShader *>(mpShader[i]);
        if(pGLSLShader)
        {
            glDetachShader(mlHandle, pGLSLShader->GetHandle());
        }
    }
    glDeleteProgram(mlHandle);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cGLSLProgram::Link()
{
    ///////////////////////////////////////
    //Attach shaders
    for(int i = 0; i < 2; ++i)
    {
        cGLSLShader* pGLSLShader = static_cast<cGLSLShader *>(mpShader[i]);
        if(pGLSLShader)
        {
            glAttachShader(mlHandle, pGLSLShader->GetHandle());
        }
    }

    ///////////////////////////////////////
    //Link
    glLinkProgram(mlHandle);

    ///////////////////////////////////////
    //Check for errors
    GLint lStatus;
    glGetProgramiv(mlHandle, GL_LINK_STATUS, &lStatus);
    if(lStatus == GL_FALSE)
    {
        Error("Failed to link GLSL program %s\n", msName.c_str());
        LogProgramInfoLog();
        return false;
    }

    for(size_t i = 0; i < mvParameters.size(); ++i)
    {
        mvParameters[i].ClearCache();
    }

    ///////////////////////////////////////
    //Check if the driver fell back to software emulation.
    //Linking still succeeds in that case, the only hint is the info log
    //(e.g. ATI: "Fragment shader will run in software - ...").
    mbRunsInSoftware = false;
    {
        GLint lLogLength = 0;
        glGetProgramiv(mlHandle, GL_INFO_LOG_LENGTH, &lLogLength);
        if(lLogLength > 1)
        {
            char *pLog = (char *)hplMalloc(lLogLength + 1);
            GLsizei lWritten = 0;
            glGetProgramInfoLog(mlHandle, lLogLength, &lWritten, pLog);
            pLog[lWritten] = 0;

            tString sLog = cString::ToLowerCase(tString(pLog));
            if(sLog.find("software") != tString::npos)
            {
                mbRunsInSoftware = true;
                Warning("GLSL program '%s' will run in software according to the driver:\n%s\n", msName.c_str(), pLog);
            }
            hplFree(pLog);
        }
    }

    ///////////////////////////////////////
    //Set up sampler units
    iGpuShader* pFragShader = mpShader[eGpuShaderType_Fragment];
    if(pFragShader && pFragShader->GetSamplerUnitNum() > 0)
    {

        //Log("Setting up samplers in '%s'\n", msName.c_str());
        glUseProgram(mlHandle);

        for(int i = 0; i < pFragShader->GetSamplerUnitNum(); ++i)
        {
            tString &sSamplerName = pFragShader->GetSamplerUnit(i)->msName;
            int lUnit = pFragShader->GetSamplerUnit(i)->mlUnit;

            //Log("Setting %s to %d\n",sSamplerName.c_str(), lUnit);
            GLint lVarHandle = glGetUniformLocation(mlHandle, sSamplerName.c_str());
            if(lVarHandle < 0)
            {
                //Not an error: the sampler is declared but not used by this variant of the
                //shader, and the driver has removed it.
                continue;
            }

            glUniform1i(lVarHandle, lUnit);
        }

        glUseProgram(mlCurrentProgram);
    }

    SetupRectScales();

    return true;
}

//-----------------------------------------------------------------------

void cGLSLProgram::SetupRectScales()
{
    mvRectScales.clear();
    InitUnitRectScales();

    for(int lShader = 0; lShader < 2; ++lShader)
    {
        cGLSLShader* pGLSLShader = static_cast<cGLSLShader *>(mpShader[lShader]);
        if(pGLSLShader == NULL)
        {
            continue;
        }

        const tStringVec &vSamplers = pGLSLShader->GetRectSamplers();
        for(size_t i = 0; i < vSamplers.size(); ++i)
        {
            cRectScale scale;
            scale.msSampler = vSamplers[i];
            scale.mlLocation = glGetUniformLocation(mlHandle, ("gvRectScale_" + vSamplers[i]).c_str());
            if(scale.mlLocation < 0)
            {
                continue;    //Not used by this variant of the shader
            }

            //Samplers that are not given a unit use unit 0
            scale.mlUnit = 0;
            for(int j = 0; j < pGLSLShader->GetSamplerUnitNum(); ++j)
            {
                if(pGLSLShader->GetSamplerUnit(j)->msName == vSamplers[i])
                {
                    scale.mlUnit = pGLSLShader->GetSamplerUnit(j)->mlUnit;
                }
            }

            //Values that are never used, so the first upload always goes through
            scale.mfX = -1.0f;
            scale.mfY = -1.0f;

            mvRectScales.push_back(scale);
        }
    }

    if(mpCurrentProgramPtr == this)
    {
        UploadRectScales();
    }
}

//-----------------------------------------------------------------------

//The program must be the current one.
void cGLSLProgram::UploadRectScales()
{
    for(size_t i = 0; i < mvRectScales.size(); ++i)
    {
        cRectScale &scale = mvRectScales[i];
        if(scale.mlUnit < 0 || scale.mlUnit >= kRectScaleUnits)
        {
            continue;
        }

        float fX = gvUnitRectScale[scale.mlUnit][0];
        float fY = gvUnitRectScale[scale.mlUnit][1];
        if(scale.mfX != fX || scale.mfY != fY)
        {
            scale.mfX = fX;
            scale.mfY = fY;
            glUniform2f(scale.mlLocation, fX, fY);
        }
    }
}

//-----------------------------------------------------------------------

void cGLSLProgram::SetRectScale(int alUnit, float afX, float afY)
{
    if(alUnit < 0 || alUnit >= kRectScaleUnits)
    {
        return;
    }
    InitUnitRectScales();

    if(gvUnitRectScale[alUnit][0] == afX && gvUnitRectScale[alUnit][1] == afY)
    {
        return;
    }
    gvUnitRectScale[alUnit][0] = afX;
    gvUnitRectScale[alUnit][1] = afY;

    //The texture may be set after the program, keep the current program up to date.
    if(mpCurrentProgramPtr && mpCurrentProgramPtr->mvRectScales.empty() == false)
    {
        mpCurrentProgramPtr->UploadRectScales();
    }
}

//-----------------------------------------------------------------------

void cGLSLProgram::Bind()
{
    if(mlCurrentProgram == mlHandle)
    {
        return;
    }

    mlCurrentProgram = mlHandle;
    mpCurrentProgramPtr = this;
    glUseProgram(mlHandle);

    if(mvRectScales.empty() == false)
    {
        UploadRectScales();
    }
}

//-----------------------------------------------------------------------

void cGLSLProgram::UnBind()
{
    if(mlCurrentProgram == 0)
    {
        return;
    }

    mlCurrentProgram = 0;
    mpCurrentProgramPtr = NULL;
    glUseProgram(0);
}


//-----------------------------------------------------------------------

bool cGLSLProgram::SetSamplerToUnit(const tString& asSamplerName, int alUnit)
{
    GLint lVarHandle = glGetUniformLocation(mlHandle, asSamplerName.c_str());
    if(lVarHandle < 0)
    {
        return false;
    }

    //Rect sampler turned into a 2D sampler: its scale follows the unit
    for(size_t i = 0; i < mvRectScales.size(); ++i)
    {
        if(mvRectScales[i].msSampler == asSamplerName)
        {
            mvRectScales[i].mlUnit = alUnit;
            mvRectScales[i].mfX = -1.0f;
            mvRectScales[i].mfY = -1.0f;
        }
    }

    Bind();
    glUniform1i(lVarHandle, alUnit);
    UnBind();

    for(size_t i = 0; i < mvParameters.size(); ++i)
    {
        if(mvParameters[i].mlId == lVarHandle)
        {
            mvParameters[i].ClearCache();
        }
    }

    return true;
}

//-----------------------------------------------------------------------

int cGLSLProgram::GetVariableId(const tString& asName)
{
    for(size_t i = 0; i < mvParameters.size(); ++i)
    {
        if(mvParameters[i].msName == asName)
        {
            return (int)i;
        }
    }

    GLint lId = glGetUniformLocation(mlHandle, asName.c_str());
    if(lId < 0)
    {
        return -1;
    }

    mvParameters.push_back(cGLSLParam(lId, asName));

    return (int)mvParameters.size() - 1;
}

//-----------------------------------------------------------------------

bool cGLSLProgram::GetVariableAsId(const tString& asName, int alId)
{
    if(alId < 0)
    {
        return false;
    }

    ////////////////////////
    // Check if id is already set to variable
    for(size_t i = 0; i < mvParameters.size(); ++i)
    {
        if(mvParameters[i].msName == asName)
        {
            if(i == alId)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    ////////////////////////
    // Check if variable exits
    GLint lHandle = glGetUniformLocation(mlHandle, asName.c_str());
    if(lHandle < 0)
    {
        return false;
    }

    ////////////////////////
    // Check if the id is taken
    if(alId < (int)mvParameters.size())
    {
        if(mvParameters[alId].mlId >= 0)
        {
            if(mvParameters[alId].mlId == lHandle)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    ////////////////////////
    // Resize vector so id fits
    else
    {
        mvParameters.resize(alId + 1);
    }

    ////////////////////////
    // Vector is big enough and id is not taken or already added,
    // add it.
    mvParameters[alId].mlId = lHandle;
    mvParameters[alId].msName = asName;
    mvParameters[alId].ClearCache();

    return true;
}

//-----------------------------------------------------------------------

bool cGLSLProgram::SetInt(int alVarId, int alX)
{
    if(alVarId < 0 || alVarId >= (int)mvParameters.size())
    {
        return false;
    }

    if(mlCurrentProgram != mlHandle)
    {
        Bind();
    }

    cGLSLParam &param = mvParameters[alVarId];
    if(param.mCacheType == eGLSLParamCache_Int && param.mlCacheInt == alX)
    {
        return true;
    }
    param.mCacheType = eGLSLParamCache_Int;
    param.mlCacheInt = alX;

    glUniform1i(param.mlId, alX);

    return true;
}

//-----------------------------------------------------------------------


bool  cGLSLProgram::SetFloat(int alVarId, float afX)
{
    if(alVarId < 0 || alVarId >= (int)mvParameters.size())
    {
        return false;
    }

    if(mlCurrentProgram != mlHandle)
    {
        Bind();
    }

    cGLSLParam &param = mvParameters[alVarId];
    if(param.mCacheType == eGLSLParamCache_Float && param.mfCache[0] == afX)
    {
        return true;
    }
    param.mCacheType = eGLSLParamCache_Float;
    param.mfCache[0] = afX;

    glUniform1f(param.mlId, afX);

    return true;
}

//-----------------------------------------------------------------------

bool  cGLSLProgram::SetVec2f(int alVarId, float afX, float afY)
{
    if(alVarId < 0 || alVarId >= (int)mvParameters.size())
    {
        return false;
    }

    if(mlCurrentProgram != mlHandle)
    {
        Bind();
    }

    cGLSLParam &param = mvParameters[alVarId];
    if(param.mCacheType == eGLSLParamCache_Vec2 && param.mfCache[0] == afX && param.mfCache[1] == afY)
    {
        return true;
    }
    param.mCacheType = eGLSLParamCache_Vec2;
    param.mfCache[0] = afX;
    param.mfCache[1] = afY;

    glUniform2f(param.mlId, afX, afY);

    return true;
}

//-----------------------------------------------------------------------

bool  cGLSLProgram::SetVec3f(int alVarId, float afX, float afY, float afZ)
{
    if(alVarId < 0 || alVarId >= (int)mvParameters.size())
    {
        return false;
    }

    if(mlCurrentProgram != mlHandle)
    {
        Bind();
    }

    cGLSLParam &param = mvParameters[alVarId];
    if(param.mCacheType == eGLSLParamCache_Vec3 &&
       param.mfCache[0] == afX && param.mfCache[1] == afY && param.mfCache[2] == afZ)
    {
        return true;
    }
    param.mCacheType = eGLSLParamCache_Vec3;
    param.mfCache[0] = afX;
    param.mfCache[1] = afY;
    param.mfCache[2] = afZ;

    glUniform3f(param.mlId, afX, afY, afZ);

    return true;
}

//-----------------------------------------------------------------------

bool  cGLSLProgram::SetVec4f(int alVarId, float afX, float afY, float afZ, float afW)
{
    if(alVarId < 0 || alVarId >= (int)mvParameters.size())
    {
        return false;
    }

    if(mlCurrentProgram != mlHandle)
    {
        Bind();
    }

    cGLSLParam &param = mvParameters[alVarId];
    if(param.mCacheType == eGLSLParamCache_Vec4 &&
       param.mfCache[0] == afX && param.mfCache[1] == afY &&
       param.mfCache[2] == afZ && param.mfCache[3] == afW)
    {
        return true;
    }
    param.mCacheType = eGLSLParamCache_Vec4;
    param.mfCache[0] = afX;
    param.mfCache[1] = afY;
    param.mfCache[2] = afZ;
    param.mfCache[3] = afW;

    glUniform4f(param.mlId, afX, afY, afZ, afW);

    return true;
}

//-----------------------------------------------------------------------

bool cGLSLProgram::SetMatrixf(int alVarId, const cMatrixf& aMtx)
{
    if(alVarId < 0 || alVarId >= (int)mvParameters.size())
    {
        return false;
    }

    if(mlCurrentProgram != mlHandle)
    {
        Bind();
    }

    cGLSLParam &param = mvParameters[alVarId];
    if(param.mCacheType == eGLSLParamCache_Matrix && memcmp(param.mfCache, aMtx.v, sizeof(float) * 16) == 0)
    {
        return true;
    }
    param.mCacheType = eGLSLParamCache_Matrix;
    memcpy(param.mfCache, aMtx.v, sizeof(float) * 16);

    glUniformMatrix4fv(param.mlId, 1, true, aMtx.v);

    return true;
}

//-----------------------------------------------------------------------

bool cGLSLProgram::SetMatrixf(int alVarId, eGpuShaderMatrix aType,
                              eGpuShaderMatrixOp aOp)
{
    //Not needed for GLSL
    return false;
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cGLSLProgram::LogProgramInfoLog()
{
    GLint infologLength = 0;
    GLsizei charsWritten  = 0;
    char *infoLog;

    glGetProgramiv(mlHandle, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)hplMalloc(infologLength);
        glGetProgramInfoLog(mlHandle, infologLength, &charsWritten, infoLog);
        Log("-------------\n");
        Log("%s\n", infoLog);
        Log("-------------\n");
        hplFree(infoLog);
    }
}

//-----------------------------------------------------------------------

}
