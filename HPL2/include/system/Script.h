#ifndef HPL_SCRIPT_H
#define HPL_SCRIPT_H

#include "resources/ResourceBase.h"

#ifdef __GNUC__
    #if defined __ppc__ || defined(__LP64__)
        #define __stdcall
    #else
        #define __stdcall __attribute__((stdcall))
    #endif
#endif

namespace hpl {

    class iScript : public iResourceBase
    {
    public:
        iScript(const tString& asName, const tWString& asFullPath) : iResourceBase(asName, asFullPath,0){}
        virtual ~iScript(){}

        bool Reload(){ return false;}
        void Unload(){}
        void Destroy(){}
        
        virtual bool CreateFromFile(const tWString& asFile, tString *apCompileMessages=NULL)=0;
        
        virtual int GetFuncHandle(const tString& asFunc)=0;
        
        virtual void AddArg(const tString& asArg)=0;
        
        /**
         * Runs a func in the script, for example "test(15)"
         * \param asFuncLine the line of code
         * \return true if everything was ok, else false
         */
        virtual bool Run(const tString& asFuncLine)=0;
        virtual bool Run(int alHandle)=0;
        virtual bool RunFuncString(const tString& asFuncName, tString& asStringArg0)=0;
        virtual bool RunFuncFloat(const tString& asFuncName, float asFloatArg0)=0;
    };
};
#endif // HPL_SCRIPT_H
