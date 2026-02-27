#ifndef HPL_LOWLEVELSYSTEM_SDL_H
#define HPL_LOWLEVELSYSTEM_SDL_H

#include "system/LowLevelSystem.h"
#include <angelscript.h>
#include <stdio.h>

namespace hpl {

    //------------------------------------------------------

    class cLogWriter
    {
    public:
        cLogWriter(const tWString& asDefaultFile);
        ~cLogWriter();
        
        void Write(const tString& asMessage);
        void Clear();

        void SetFileName(const tWString& asFile);
        
    private:
        void ReopenFile();

        FILE *mpFile;
        tWString msFileName;
    };

    //------------------------------------------------------
    
    class cScriptOutput// : public  asIOutputStream
    {
    public:
        cScriptOutput() : msMessage("") {}
        ~cScriptOutput(){}
        
        void AddMessage(const asSMessageInfo *msg);
        void Display();
        void Clear();

        tString &GetMessage(){ return msMessage;}

    private:
        tString msMessage;
    };

    //------------------------------------------------------


    class cLowLevelSystemSDL : public iLowLevelSystem
    {
    public:
        cLowLevelSystemSDL();
        ~cLowLevelSystemSDL();

        iScript* CreateScript(const tString& asName);

        bool AddScriptFunc(const tString& asFuncDecl, void* pFunc);
        bool AddScriptVar(const tString& asVarDecl, void *pVar);

    private:
        asIScriptEngine *mpScriptEngine;
        cScriptOutput *mpScriptOutput;
        int mlHandleCount;
    };

    //------------------------------------------------------

};
#endif // HPL_LOWLEVELSYSTEM_SDL_H
