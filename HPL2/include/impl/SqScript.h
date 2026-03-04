#ifndef HPL_SQ_SCRIPT_H
#define HPL_SQ_SCRIPT_H

#include "system/Script.h"
#include "impl/LowLevelSystemSDL.h"
#include <angelscript.h>


namespace hpl {

    class cSqScript : public iScript
    {
    public:
        cSqScript(const tString& asName, asIScriptEngine *apScriptEngine,cScriptOutput *apScriptOutput, int alHandle); 
        ~cSqScript();

        bool CreateFromFile(const tWString& asFileName, tString *apCompileMessages=NULL);

        int GetFuncHandle(const tString& asFunc);
        void AddArg(const tString& asArg);

        bool Run(const tString& asFuncLine);
        bool Run(int alHandle);
        bool RunFuncString(const tString& asFuncName, tString& asStringArg0);
        bool RunFuncFloat(const tString& asFuncName, float asFloatArg0);
    private:
        asIScriptEngine *mpScriptEngine;
        cScriptOutput *mpScriptOutput;
        
        asIScriptContext *mpContext;
        asIScriptModule *mpModule;
        
        int mlHandle;
        tString msModuleName;

        char* LoadCharBuffer(const tWString& asFileName, int& alLength);
    };
};
#endif // HPL_SCRIPT_H
