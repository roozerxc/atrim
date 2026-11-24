#include "impl/SqScript.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "system/Platform.h"
#include "math/Math.h"
#include <stdio.h>
#include "impl/scripthelper.h"
#include "impl/scriptbuilder.h"
#include "impl/scriptstring.h"
#include "resources/BinaryBuffer.h"
#include "resources/Resources.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// PUBLIC DATA
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

#define kEncryptKey 0x4516FFDD

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cSqScript::cSqScript(const tString& asName, asIScriptEngine *apScriptEngine,
                     cScriptOutput *apScriptOutput, int alHandle)
    : iScript(asName, _W(""))
{
    mpScriptEngine = apScriptEngine;
    mpScriptOutput = apScriptOutput;
    mlHandle = alHandle;

    mpContext = mpScriptEngine->CreateContext();
    mpModule = NULL;

    //Create a unique module name
    msModuleName = "Module_" + cString::ToString(cMath::RandRectl(0, 1000000)) +
                   "_" + cString::ToString(mlHandle);

}

cSqScript::~cSqScript()
{
    mpScriptEngine->DiscardModule(msModuleName.c_str());
    mpContext->Release();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cSqScript::CreateFromFile(const tWString& asFileName, tString *apCompileMessages)
{
    SetFullPath(asFileName);

    tWString sExt = cString::ToLowerCaseW(cString::GetFileExtW(asFileName));

    /////////////////////////////////////////
    // Load file
    int lLength = 0;
    char *pCharBuffer = NULL;

    /////////////////////////////////////
    // Normal load
    if(sExt == _W("hps"))
    {
        pCharBuffer = LoadCharBuffer(asFileName, lLength);
        if(pCharBuffer == NULL)
        {
            Error("Couldn't load script '%s'!\n", asFileName.c_str());
            return false;
        }
    }
    /////////////////////////////////////
    // Compressed load
    else if(cResources::GetCreateAndLoadCompressedMaps())
    {
        cBinaryBuffer compBuffer;
        if(compBuffer.Load(asFileName) == false)
        {
            //Log("Could not load compressed map!\n");
            return false;
        }

        int lKey = kEncryptKey;
        compBuffer.XorTransform((char*)&lKey, sizeof(lKey));

        cBinaryBuffer textBuff;
        if(textBuff.DecompressAndAddFromBuffer(&compBuffer, false) == false)
        {
            //Log("Could not decompress map!\n");
            return false;
        }

        textBuff.SetPos(0);
        lLength = (int)textBuff.GetSize();
        pCharBuffer = hplNewArray(char, lLength);
        textBuff.GetCharArray(pCharBuffer, lLength);
    }

    /////////////////////////////////////////
    // Save compressed
    if(sExt == _W("hps") && cResources::GetCreateAndLoadCompressedMaps())
    {
        tWString sCompFile = cString::SetFileExtW(asFileName, _W("chps"));

        //Only recreate if file does not exist or if out of date.
        if(    cPlatform::FileExists(sCompFile) == false ||
                cPlatform::FileModifiedDate(sCompFile) < cPlatform::FileModifiedDate(asFileName))
        {
            cBinaryBuffer textBuff;
            textBuff.AddCharArray(pCharBuffer, lLength);

            cBinaryBuffer compBuff;
            compBuff.CompressAndAdd(textBuff.GetDataPointer(), textBuff.GetSize());

            int lKey = kEncryptKey;
            compBuff.XorTransform((char*)&lKey, sizeof(lKey));

            compBuff.Save(sCompFile);
        }
    }

    /////////////////////////////////////////
    // Create module
    CScriptBuilder builder;
    if(builder.StartNewModule(mpScriptEngine, msModuleName.c_str()) < 0)
    {
        Error("Couldn't start new module '%s'!\n", msModuleName.c_str());
        hplDeleteArray(pCharBuffer);
        return false;
    }

    std::string sFileName = cString::To8Char(asFileName);
    std::string sScriptData(pCharBuffer, lLength);

    if(builder.AddSectionFromMemory(sScriptData.c_str(), sFileName.c_str()) < 0)
    {
        Error("Couldn't add script section '%s'!\n", sFileName.c_str());
        hplDeleteArray(pCharBuffer);
        return false;
    }

    int lBuildOutput = builder.BuildModule();

    // do not pass GM_ALWAYS_CREATE, only if it exists...
    mpModule = mpScriptEngine->GetModule(msModuleName.c_str());

    if(apCompileMessages)
    {
        *apCompileMessages = mpScriptOutput->GetMessage();
    }

    if(lBuildOutput < 0)
    {
        Error("Couldn't build script '%s'!\n", cString::To8Char(asFileName).c_str());
        Log("------- SCRIPT OUTPUT BEGIN --------------------------\n");
        mpScriptOutput->Display();
        mpScriptOutput->Clear();
        Log("------- SCRIPT OUTPUT END ----------------------------\n");

        hplDeleteArray(pCharBuffer);
        return false;
    }
    mpScriptOutput->Clear();

    hplDeleteArray(pCharBuffer);
    return true;
}

//-----------------------------------------------------------------------

int cSqScript::GetFuncHandle(const tString& asFunc)
{
    return mpModule->GetFunctionIdByName(asFunc.c_str());
}

//-----------------------------------------------------------------------

void cSqScript::AddArg(const tString& asArg)
{

}

//-----------------------------------------------------------------------

bool cSqScript::Run(const tString& asFuncLine)
{
    ExecuteString(mpScriptEngine, asFuncLine.c_str(), mpModule);

    return true;
}

//-----------------------------------------------------------------------

bool cSqScript::RunFuncString(const tString& asFuncName, tString& asStringArg0)
{
    if(PrepareSingleArgFunc(asFuncName, mpScriptEngine->GetTypeIdByDecl("string")) == false)
    {
        return false;
    }

    //The script 'string' type is the ref counted CScriptString (see RegisterScriptString), so
    //the argument must be one. Passing a std::string* only worked because 'buffer' happens to
    //be CScriptString's first member; anything touching the ref count read past the object.
    CScriptString *pArg = new CScriptString(asStringArg0);
    mpContext->SetArgObject(0, pArg);
    ExecutePrepared(asFuncName);
    pArg->Release();

    return true;
}

//-----------------------------------------------------------------------

bool cSqScript::RunFuncFloat(const tString& asFuncName, float asFloatArg0)
{
    if(PrepareSingleArgFunc(asFuncName, asTYPEID_FLOAT) == false)
    {
        return false;
    }

    mpContext->SetArgFloat(0, asFloatArg0);
    ExecutePrepared(asFuncName);

    return true;
}

//-----------------------------------------------------------------------

bool cSqScript::Run(int alHandle)
{
    mpContext->Prepare(alHandle);

    /* Set all the args here */

    mpContext->Execute();

    return true;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cSqScript::PrepareSingleArgFunc(const tString& asFuncName, int alArgTypeId)
{
    if(mpModule == NULL || mpContext == NULL || alArgTypeId < 0)
    {
        return false;
    }

    //A script is already running on this context (nested call): let the caller use the text path
    if(mpContext->GetState() == asEXECUTION_ACTIVE)
    {
        return false;
    }

    //Negative for a missing function and for several overloads with this name
    int lFuncId = mpModule->GetFunctionIdByName(asFuncName.c_str());
    if(lFuncId < 0)
    {
        return false;
    }

    //Only call functions with exactly one argument of the expected type, otherwise the
    //SetArg call fails (or writes the wrong type) and Execute runs with a bad argument
    asIScriptFunction *pFunc = mpScriptEngine->GetFunctionDescriptorById(lFuncId);
    if(pFunc == NULL || pFunc->GetParamCount() != 1 || pFunc->GetParamTypeId(0) != alArgTypeId)
    {
        return false;
    }

    return mpContext->Prepare(lFuncId) >= 0;
}

//-----------------------------------------------------------------------

void cSqScript::ExecutePrepared(const tString& asFuncName)
{
    int lRet = mpContext->Execute();
    if(lRet == asEXECUTION_EXCEPTION)
    {
        const char *pException = mpContext->GetExceptionString();
        Error("Script exception in '%s' (%s): %s\n", asFuncName.c_str(), msModuleName.c_str(),
              pException ? pException : "");
    }
}

//-----------------------------------------------------------------------

char *cSqScript::LoadCharBuffer(const tWString& asFileName, int &alLength)
{
    FILE *pFile = cPlatform::OpenFile(asFileName, _W("rb"));
    if(pFile == NULL)
    {
        return NULL;
    }

    fseek(pFile, 0, SEEK_END);
    int lLength = (int)ftell(pFile);
    rewind(pFile);

    alLength = lLength;

    char *pBuffer = hplNewArray(char, lLength);
    fread(pBuffer, lLength, 1, pFile);

    fclose(pFile);

    return pBuffer;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// STATIC PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

}
