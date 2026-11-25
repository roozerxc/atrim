//#include <vld.h>
//Use this to check for memory leaks!

#ifdef _WIN32
#pragma comment(lib, "angelscript.lib")
#define UNICODE
#include <windows.h>
#include <shlobj.h>
#endif

#define _UNICODE

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <map>

#include "impl/LowLevelSystemSDL.h"
#include "impl/SqScript.h"
#include "system/Platform.h"

#include "SDL/SDL.h"

#include "impl/scriptstring.h"

#include "system/String.h"

#include "impl/PlatformBuildID.h"

#ifndef IGNORE_HPL_MAIN
extern int hplMain(const hpl::tString &asCommandLine);

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return hplMain(lpCmdLine);
}
#else
int main(int argc, char *argv[])
{
#ifdef __linux__
    if(!std::setlocale(LC_CTYPE, ""))
    {
        fprintf(stderr, "Can't set the specified locale! Check LANG, LC_CTYPE, LC_ALL.\n");
        return 1;
    }

    char *charset = nl_langinfo(CODESET);
    bool utf8_mode = (strcasecmp(charset, "UTF-8") == 0);

    if (!utf8_mode)
    {
        fprintf(stderr, "UTF-8 Charset %s available.\nCurrent LANG is %s\nCharset: %s\n",
                utf8_mode ? "is" : "not", getenv("LANG"), charset);
    }
#endif
    bool cwd = false;
    hpl::tString cmdline = "";
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-cwd") == 0)
        {
            cwd = true;
        }
        else if (strncmp(argv[i], "-psn", 4) == 0)
        {
            // skip "finder" process number
        }
        else
        {
            if (cmdline.length() > 0)
            {
                cmdline.append(" ").append(argv[i]);
            }
            else
            {
                cmdline.append(argv[i]);
            }
        }
    }
    if (!cwd)
    {
        hpl::tString dataDir = hpl::cPlatform::GetDataDir();

        chdir(dataDir.c_str());
    }

    return hplMain(cmdline);
}
#endif
#endif

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// LOG WRITER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

static cLogWriter gLogWriter(_W("hpl.log"));
static cLogWriter gUpdateLogWriter(_W("hpl_update.log"));

//-----------------------------------------------------------------------

cLogWriter::cLogWriter(const tWString& asFileName)
{
    msFileName = asFileName;
    mpFile = NULL;
}

cLogWriter::~cLogWriter()
{
    if(mpFile)
    {
        fclose(mpFile);
    }
}

void cLogWriter::Write(const tString& asMessage)
{
#ifdef _WIN32
    OutputDebugStringA(asMessage.c_str());
#endif

    if(!mpFile)
    {
        ReopenFile();
    }

    if(mpFile)
    {
        fprintf(mpFile, "%s", asMessage.c_str());
        fflush(mpFile);
    }
}

void cLogWriter::Clear()
{
    ReopenFile();
    if(mpFile)
    {
        fflush(mpFile);
    }
}

//-----------------------------------------------------------------------


void cLogWriter::SetFileName(const tWString& asFile)
{
    if(msFileName == asFile)
    {
        return;
    }

    msFileName = asFile;
    if(mpFile)
    {
        ReopenFile();
    }
}

//-----------------------------------------------------------------------


void cLogWriter::ReopenFile()
{
    if(mpFile)
    {
        fclose(mpFile);
    }

#ifdef _WIN32
    mpFile = _wfopen(msFileName.c_str(), _W("w"));
#else
    mpFile = fopen(cString::To8Char(msFileName).c_str(), "w");
#endif
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// LOG FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

static tLogMessageCallbackFunc gpLogMessageCallbackFunc = NULL;

//-----------------------------------------------------------------------

void SetLogFile(const tWString &asFile)
{
    gLogWriter.SetFileName(asFile);
}

//-----------------------------------------------------------------------

void FatalError(const char* fmt, ... )
{
    char text[4096];
    va_list ap;
    if (fmt == NULL)
    {
        return;
    }
    va_start(ap, fmt);
    vsnprintf(text, sizeof(text), fmt, ap);
    va_end(ap);

    tString sMess = "FATAL ERROR: ";
    sMess += text;
    gLogWriter.Write(sMess);

    if(gpLogMessageCallbackFunc)
    {
        gpLogMessageCallbackFunc(eLogOutputType_FatalError, sMess.c_str());
    }

#if defined(__APPLE__) || defined(__linux__)
    SDL_Quit();
#endif

    cPlatform::CreateMessageBox(eMsgBoxType_Error, _W("FATAL ERROR"), _W("%ls"), cString::To16Char(sMess).c_str());

    exit(1);
}

//-----------------------------------------------------------------------

//Errors and warnings repeated over and over (a map script asking every few seconds
//for lights that do not exist, say) are written a few times only. Each written line
//flushes the file, and hundreds of them bury the lines that matter.
static const int kMaxRepeatedLogLines = 5;
//Bounds the memory used, messages past this many different ones are always written.
static const size_t kMaxTrackedLogLines = 4096;
//Made on first use and never freed: errors can still be logged by other static
//destructors at exit, after a static map here would already be gone.
static std::map<tString, int> *gpRepeatedLogLines = NULL;
static SDL_mutex *gpRepeatedLogMutex = NULL;

//False if the message was written enough times already. The last time it is written,
//a note about the repeats is added to asMess.
static bool AllowRepeatedLogLine(tString &asMess)
{
    //Created by the first error or warning. The first log lines are written by the main
    //thread during start up, before any other thread runs.
    if(gpRepeatedLogLines == NULL)
    {
        gpRepeatedLogMutex = SDL_CreateMutex();
        gpRepeatedLogLines = new std::map<tString, int>();
    }
    if(gpRepeatedLogMutex)
    {
        SDL_LockMutex(gpRepeatedLogMutex);
    }

    bool bAllow = true;
    std::map<tString, int> &mapLines = *gpRepeatedLogLines;
    std::map<tString, int>::iterator it = mapLines.find(asMess);
    if(it == mapLines.end())
    {
        if(mapLines.size() < kMaxTrackedLogLines)
        {
            mapLines.insert(std::map<tString, int>::value_type(asMess, 1));
        }
    }
    else
    {
        ++it->second;
        if(it->second == kMaxRepeatedLogLines)
        {
            size_t lEnd = asMess.size();
            while(lEnd > 0 && (asMess[lEnd - 1] == '\n' || asMess[lEnd - 1] == '\r'))
            {
                --lEnd;
            }
            tString sEnding = asMess.substr(lEnd);
            asMess.resize(lEnd);
            asMess += " (logged " + cString::ToString(kMaxRepeatedLogLines) + " times, further repeats are not logged)";
            asMess += sEnding;
        }
        else if(it->second > kMaxRepeatedLogLines)
        {
            bAllow = false;
        }
    }

    if(gpRepeatedLogMutex)
    {
        SDL_UnlockMutex(gpRepeatedLogMutex);
    }
    return bAllow;
}

//-----------------------------------------------------------------------

void Error(const char* fmt, ...)
{
    char text[2048];
    va_list ap;
    if (fmt == NULL)
    {
        return;
    }
    va_start(ap, fmt);
    vsnprintf(text, sizeof(text), fmt, ap);
    va_end(ap);

    tString sMess = "ERROR: ";
    sMess += text;
    if(AllowRepeatedLogLine(sMess))
    {
        gLogWriter.Write(sMess);
    }

    if(gpLogMessageCallbackFunc)
    {
        gpLogMessageCallbackFunc(eLogOutputType_Error, sMess.c_str());
    }
}

//-----------------------------------------------------------------------


void Warning(const char* fmt, ...)
{
    char text[2048];
    va_list ap;
    if (fmt == NULL)
    {
        return;
    }
    va_start(ap, fmt);
    vsnprintf(text, sizeof(text), fmt, ap);
    va_end(ap);

    tString sMess = "WARNING: ";
    sMess += text;
    if(AllowRepeatedLogLine(sMess))
    {
        gLogWriter.Write(sMess);
    }

    if(gpLogMessageCallbackFunc)
    {
        gpLogMessageCallbackFunc(eLogOutputType_Warning, sMess.c_str());
    }
}

//-----------------------------------------------------------------------


void Log(const char* fmt, ...)
{
    char text[4096];
    va_list ap;
    if (fmt == NULL)
    {
        return;
    }
    va_start(ap, fmt);
    vsnprintf(text, sizeof(text), fmt, ap);
    va_end(ap);

    tString sMess = "";
    sMess += text;
    gLogWriter.Write(sMess);

    if(gpLogMessageCallbackFunc)
    {
        gpLogMessageCallbackFunc(eLogOutputType_Normal, sMess.c_str());
    }
}

//-----------------------------------------------------------------------

static bool gbUpdateLogIsActive;
void SetUpdateLogFile(const tWString &asFile)
{
    gUpdateLogWriter.SetFileName(asFile);
}

void ClearUpdateLogFile()
{
    if(!gbUpdateLogIsActive)
    {
        return;
    }

    gUpdateLogWriter.Clear();
}

void SetUpdateLogActive(bool abX)
{
    gbUpdateLogIsActive = abX;
}

bool GetUpdateLogActive()
{
    return gbUpdateLogIsActive;
}

//-----------------------------------------------------------------------

void LogUpdate(const char* fmt, ...)
{
    if(!gbUpdateLogIsActive)
    {
        return;
    }

    char text[2048];
    va_list ap;
    if (fmt == NULL)
    {
        return;
    }
    va_start(ap, fmt);
    vsnprintf(text, sizeof(text), fmt, ap);
    va_end(ap);

    tString sMess = "";
    sMess += text;
    gUpdateLogWriter.Write(sMess);
}

//-----------------------------------------------------------------------

extern void SetLogMessageCallback(tLogMessageCallbackFunc apFunc)
{
    gpLogMessageCallbackFunc = apFunc;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SCRIPT OUTPUT
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cScriptOutput::AddMessage(const asSMessageInfo *msg)
{
    char sMess[1024];

    tString type = "ERR ";
    if( msg->type == asMSGTYPE_WARNING )
    {
        type = "WARN";
    }
    else if( msg->type == asMSGTYPE_INFORMATION )
    {
        type = "INFO";
    }

    snprintf(sMess, sizeof(sMess), "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type.c_str(), msg->message);

    msMessage += sMess;
}
//-----------------------------------------------------------------------

void cScriptOutput::Display()
{
    if(msMessage.size() > 500)
    {
        while(msMessage.size() > 500)
        {
            tString sSub = msMessage.substr(0, 500);
            msMessage = msMessage.substr(500);
            Log(sSub.c_str());
        }
        Log(msMessage.c_str());
    }
    else
    {
        Log(msMessage.c_str());
    }
}
//-----------------------------------------------------------------------

void cScriptOutput::Clear()
{
    msMessage = "";
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLowLevelSystemSDL::cLowLevelSystemSDL()
{
    mpScriptEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
    if(mpScriptEngine == NULL)
    {
        Error("Failed to start AngelScript!\n");
    }

    mpScriptOutput = hplNew( cScriptOutput, () );
    mpScriptEngine->SetMessageCallback(asMETHOD(cScriptOutput, AddMessage), mpScriptOutput, asCALL_THISCALL);

    RegisterScriptString(mpScriptEngine);

    mlHandleCount = 0;

    Log("------------------ THE HPL ENGINE LOG ------------------\n");
    Log("Engine Build ID: %s", GetBuildID());
    Log("\n\n");

    //const char* pASLibOptions = asGetLibraryOptions();
    //Log(" AngelScript options: '%s'\n", pASLibOptions);
}

//-----------------------------------------------------------------------

cLowLevelSystemSDL::~cLowLevelSystemSDL()
{
    /*Release all runnings contexts */

    mpScriptEngine->Release();
    hplDelete(mpScriptOutput);

    //perhaps not the best thing to skip :)
    //if(gpLogWriter)    hplDelete(gpLogWriter);
    //gpLogWriter = NULL;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


iScript *cLowLevelSystemSDL::CreateScript(const tString& asName)
{
    return hplNew( cSqScript, (asName, mpScriptEngine, mpScriptOutput, mlHandleCount++) );
}

//-----------------------------------------------------------------------

bool cLowLevelSystemSDL::AddScriptFunc(const tString& asFuncDecl, void* pFunc)
{
    if(mpScriptEngine->RegisterGlobalFunction(asFuncDecl.c_str(), asFUNCTION(pFunc), asCALL_STDCALL) < 0)
    {
        Error("Couldn't add func '%s'\n", asFuncDecl.c_str());
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------

bool cLowLevelSystemSDL::AddScriptVar(const tString& asVarDecl, void *pVar)
{
    if(mpScriptEngine->RegisterGlobalProperty(asVarDecl.c_str(), pVar) < 0)
    {
        Error("Couldn't add var '%s'\n", asVarDecl.c_str());
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
}
