#include "impl/GLSLShader.h"
#include "impl/SDLTexture.h"
#include "impl/LowLevelGraphicsSDL.h"
#include "system/LowLevelSystem.h"

#include "system/Platform.h"
#include "system/String.h"

#ifdef _WIN32
#include <io.h>
#endif

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

//RECT-REWRITE-BEGIN
static inline bool IsIdentChar(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

/**
 * Turns rect samplers into 2D samplers, for drivers whose GLSL has no sampler2DRect:
 *   uniform sampler2DRect aMap;   ->  uniform sampler2D aMap; uniform vec2 gvRectScale_aMap;
 *   texture2DRect(aMap, coord)    ->  texture2D(aMap, (coord) * gvRectScale_aMap)
 * Line count is kept so compile errors still point at the right line.
 */
static void RewriteRectSamplers(tString &asCode, tStringVec &avSamplers)
{
    const tString sSamplerType = "sampler2DRect";
    const tString sLookup = "texture2DRect";
    const tString sExtension = "GL_ARB_texture_rectangle";

    ///////////////////////////
    // Extension line is of no use any more (and gives a warning)
    size_t lPos = asCode.find(sExtension);
    while(lPos != tString::npos)
    {
        size_t lLineStart = asCode.rfind('\n', lPos);
        lLineStart = lLineStart == tString::npos ? 0 : lLineStart + 1;
        size_t lLineEnd = asCode.find('\n', lPos);
        if(lLineEnd == tString::npos)
        {
            lLineEnd = asCode.size();
        }

        size_t lHash = asCode.find('#', lLineStart);
        if(lHash != tString::npos && lHash < lPos && asCode.compare(lHash + 1, 9, "extension") == 0)
        {
            bool bCarriageReturn = lLineEnd > lLineStart && asCode[lLineEnd - 1] == '\r';
            asCode.replace(lLineStart, lLineEnd - lLineStart, bCarriageReturn ? "\r" : "");
            lPos = asCode.find(sExtension, lLineStart);
        }
        else
        {
            lPos = asCode.find(sExtension, lPos + sExtension.size());
        }
    }

    ///////////////////////////
    // Declarations
    lPos = asCode.find(sSamplerType);
    while(lPos != tString::npos)
    {
        size_t lEnd = lPos + sSamplerType.size();
        if((lPos > 0 && IsIdentChar(asCode[lPos - 1])) || (lEnd < asCode.size() && IsIdentChar(asCode[lEnd])))
        {
            lPos = asCode.find(sSamplerType, lEnd);
            continue;
        }

        asCode.replace(lPos, sSamplerType.size(), "sampler2D");
        lEnd = lPos + 9;

        //Name of the sampler
        size_t lNameStart = lEnd;
        while(lNameStart < asCode.size() && (asCode[lNameStart] == ' ' || asCode[lNameStart] == '\t'))
        {
            ++lNameStart;
        }
        size_t lNameEnd = lNameStart;
        while(lNameEnd < asCode.size() && IsIdentChar(asCode[lNameEnd]))
        {
            ++lNameEnd;
        }
        size_t lSemiColon = lNameEnd;
        while(lSemiColon < asCode.size() && (asCode[lSemiColon] == ' ' || asCode[lSemiColon] == '\t'))
        {
            ++lSemiColon;
        }

        //Only plain "uniform sampler2DRect name;" gets a scale (the only form the shaders use)
        if(lNameEnd > lNameStart && lSemiColon < asCode.size() && asCode[lSemiColon] == ';')
        {
            tString sName = asCode.substr(lNameStart, lNameEnd - lNameStart);
            bool bExists = false;
            for(size_t i = 0; i < avSamplers.size(); ++i)
            {
                if(avSamplers[i] == sName)
                {
                    bExists = true;
                }
            }
            if(bExists == false)
            {
                avSamplers.push_back(sName);
                asCode.insert(lSemiColon + 1, " uniform vec2 gvRectScale_" + sName + ";");
            }
        }

        lPos = asCode.find(sSamplerType, lEnd);
    }

    ///////////////////////////
    // Lookups
    lPos = asCode.find(sLookup);
    while(lPos != tString::npos)
    {
        size_t lEnd = lPos + sLookup.size();
        size_t lParen = lEnd;
        while(lParen < asCode.size() && (asCode[lParen] == ' ' || asCode[lParen] == '\t'))
        {
            ++lParen;
        }
        if((lPos > 0 && IsIdentChar(asCode[lPos - 1])) || lParen >= asCode.size() || asCode[lParen] != '(')
        {
            lPos = asCode.find(sLookup, lEnd);
            continue;
        }

        //First argument: the sampler
        size_t lNameStart = lParen + 1;
        while(lNameStart < asCode.size() && (asCode[lNameStart] == ' ' || asCode[lNameStart] == '\t'))
        {
            ++lNameStart;
        }
        size_t lNameEnd = lNameStart;
        while(lNameEnd < asCode.size() && IsIdentChar(asCode[lNameEnd]))
        {
            ++lNameEnd;
        }
        size_t lComma = lNameEnd;
        while(lComma < asCode.size() && (asCode[lComma] == ' ' || asCode[lComma] == '\t'))
        {
            ++lComma;
        }

        tString sName = asCode.substr(lNameStart, lNameEnd - lNameStart);
        bool bKnown = false;
        for(size_t i = 0; i < avSamplers.size(); ++i)
        {
            if(avSamplers[i] == sName)
            {
                bKnown = true;
            }
        }

        //Second argument: everything up to the closing parenthesis
        size_t lClose = tString::npos;
        if(bKnown && lComma < asCode.size() && asCode[lComma] == ',')
        {
            int lDepth = 1;
            for(size_t i = lComma + 1; i < asCode.size(); ++i)
            {
                if(asCode[i] == '(')
                {
                    ++lDepth;
                }
                else if(asCode[i] == ')')
                {
                    if(--lDepth == 0)
                    {
                        lClose = i;
                        break;
                    }
                }
                else if(asCode[i] == ';')
                {
                    break;
                }
            }
        }

        if(lClose == tString::npos)
        {
            lPos = asCode.find(sLookup, lEnd);
            continue;
        }

        //Close first so the positions before it stay valid
        asCode.insert(lClose, ") * gvRectScale_" + sName);
        asCode.insert(lComma + 1, " (");
        asCode.replace(lPos, sLookup.size(), "texture2D");

        //Go on right after the name, nested lookups in the coordinate are handled too.
        lPos = asCode.find(sLookup, lPos + 9);
    }
}
//RECT-REWRITE-END

//-----------------------------------------------------------------------

cGLSLShader::cGLSLShader(const tString& asName, eGpuShaderType aType, iLowLevelGraphics *apLowLevelGraphics)
    : iGpuShader(asName, _W(""), aType, eGpuProgramFormat_GLSL)
{
    mpLowLevelGraphics = apLowLevelGraphics;

    mlHandle = glCreateShader(GetGLShaderType(aType));
}

cGLSLShader::~cGLSLShader()
{
    glDeleteShader(mlHandle);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cGLSLShader::Reload()
{
    return false;
}
void cGLSLShader::Unload() {}
void cGLSLShader::Destroy() {}

//-----------------------------------------------------------------------

bool cGLSLShader::CreateFromFile(const tWString &asFile, const tString &asEntry, bool abPrintInfoIfFail)
{
    ///////////////////////////////////////
    //Load file
    FILE *pFile = cPlatform::OpenFile(asFile, _W("rb"));
    if(pFile == NULL)
    {
        Error("Could not open file %ls\n", asFile.c_str());
        return false;
    }
    SetFullPath(asFile);

    fseek(pFile, 0, SEEK_END);
    int lFileSize = ftell(pFile);
    rewind(pFile);

    char *pBuffer = (char*) hplMalloc(sizeof(char) * lFileSize + 1);

    fread(pBuffer, sizeof(GLchar), lFileSize, pFile);
    pBuffer[lFileSize] = 0; //Zero at end so it is a proper string.

    fclose(pFile);

    bool bRet = CreateFromString(pBuffer, asEntry, abPrintInfoIfFail);

    hplFree(pBuffer);

    return bRet;
}

//-----------------------------------------------------------------------


bool cGLSLShader::CreateFromString(const char *apStringData, const tString& asEntry, bool abPrintInfoIfFail)
{
    ///////////////////////////////////////
    //Compile

    //Drivers with GLSL 1.10 only (OpenGL 2.0) reject "#version 120" so lower the version directive.
    tString sDowngraded;
    if(mpLowLevelGraphics->GetCaps(eGraphicCaps_GLSLVersion) < 120)
    {
        sDowngraded = apStringData;
        size_t lPos = sDowngraded.find("#version 120");
        if(lPos != tString::npos)
        {
            sDowngraded.replace(lPos, 12, "#version 110");
            apStringData = sDowngraded.c_str();
        }
    }

    //Drivers without sampler2DRect in GLSL: rect textures are 2D textures, change the samplers to fit.
    tString sRewritten;
    mvRectSamplers.clear();
    if(iLowLevelGraphics::GetRectAs2D())
    {
        sRewritten = apStringData;
        RewriteRectSamplers(sRewritten, mvRectSamplers);
        apStringData = sRewritten.c_str();
    }

    glShaderSource(mlHandle, 1, &apStringData, NULL);

    glCompileShader(mlHandle);

    ///////////////////////////////////////
    //Check for errors.
    GLint lStatus;
    glGetShaderiv(mlHandle, GL_COMPILE_STATUS, &lStatus);
    if(lStatus == GL_FALSE)
    {
        if(abPrintInfoIfFail)
        {
            Error("Failed to compile GLSL shader '%s'!\n", cString::To8Char(GetFullPath()).c_str());
            Log("Shader code:\n-------------------\n");

            LogShaderCode(apStringData);


            Log("---------------------\n");
            Log("Compile log:\n");
            LogShaderInfoLog();
        }
        return false;
    }

    /////////////////////////////
    //If debug is on, then show code + compile log
    if(mbDebugInfo)
    {
        Log("Shader '%s' code:\n-------------------\n", msName.c_str());

        //LogShaderCode(apStringData);

        //Log("---------------------\n");
        Log("Compile log:\n");
        LogShaderInfoLog();
    }


    return true;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cGLSLShader::LogShaderInfoLog()
{
    GLint infologLength = 0;
    GLsizei charsWritten  = 0;
    char *infoLog;

    glGetShaderiv(mlHandle, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)hplMalloc(infologLength);
        glGetShaderInfoLog(mlHandle, infologLength, &charsWritten, infoLog);
        Log("---------------------\n");
        Log("%s\n", infoLog);
        Log("---------------------\n");
        hplFree(infoLog);
    }
}

//-----------------------------------------------------------------------

void cGLSLShader::LogShaderCode(const char *apStringData)
{
    int lRow = 1;
    tString sRowCode = "";
    for(int i = 0; apStringData[i] != 0; ++i)
    {
        char lChar = apStringData[i];
        if(lChar == '\r')
        {
            continue;    //Can skip this sign.
        }

        if(lChar == '\n')
        {
            Log("[%04d] %s\n", lRow, sRowCode.c_str());
            sRowCode.resize(0);
            lRow++;
        }
        else
        {
            sRowCode += lChar;
        }
    }
}

//-----------------------------------------------------------------------

GLenum cGLSLShader::GetGLShaderType(eGpuShaderType aType)
{
    switch(aType)
    {
    case eGpuShaderType_Fragment:
        return GL_FRAGMENT_SHADER;
    case eGpuShaderType_Vertex:
        return GL_VERTEX_SHADER;
    }

    return GL_VERTEX_SHADER;
}

//-----------------------------------------------------------------------

}
