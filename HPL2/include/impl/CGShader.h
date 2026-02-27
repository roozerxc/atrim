#ifndef HPL_CGSHADER_H
#define HPL_CGSHADER_H

//#include <windows.h>
#include <GL/GLee.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GPUShader.h"

namespace hpl {

    class cCGShader : public iGpuShader
    {
    public:
        cCGShader(tString asName, CGcontext aContext,eGpuShaderType aType);
        ~cCGShader();

        bool Reload();
        void Unload();
        void Destroy();

        tString GetProgramName(){ return msName; }

        bool CreateFromFile(const tString& asFile, const tString& asEntry="main");

        void Bind();
        void UnBind();

        int GetVariableId(const tString& asName);

        bool SetFloat(int alVarId, float afX);
        bool SetVec2f(int alVarId, float afX,float afY);
        bool SetVec3f(int alVarId, float afX,float afY,float afZ);
        bool SetVec4f(int alVarId, float afX,float afY,float afZ, float afW);

        bool SetMatrixf(int alVarId, const cMatrixf& mMtx);
        bool SetMatrixf(int alVarId, eGpuShaderMatrix mType, eGpuShaderMatrixOp mOp);

        bool SetTexture(int alVarId,iTexture* apTexture, bool abAutoDisable=true);
        bool SetTextureToUnit(int alUnit, iTexture* apTexture);

        
        /// CG SPECIFIC //////////////////////

        CGprogram GetProgram(){ return mProgram;}
        CGprofile GetProfile(){ return mProfile;}

        static void SetVProfile(tString asProfile) {
            msForceVP = asProfile;
        }
        static void SetFProfile(tString asProfile) {
            msForceFP = asProfile;
        }
        static tString &GetVProfile(){ return msForceVP; }
        static tString &GetFProfile(){ return msForceFP; }

    protected:
        CGcontext mContext;

        tString msName;
        tString msFile;
        tString msEntry;
        CGprogram mProgram;
        CGprofile mProfile;

        std::vector<CGparameter> mvParameters;

        CGparameter mvTexUnitParam[kMaxTextureUnits];
    

        static tString msForceFP;
        static tString msForceVP;
    };
};
#endif // HPL_CGSHADER_H
