#ifndef HPL_MATERIAL_WATER_H
#define HPL_MATERIAL_WATER_H

#include "graphics/MaterialType.h"
#include "graphics/Material.h"

namespace hpl {

    //---------------------------------------------------
    // WATER
    //---------------------------------------------------

    class cMaterialType_Water_Vars : public iMaterialVars
    {
    public:
        cMaterialType_Water_Vars() : mbHasReflection(true), mfRefractionScale(0.1f), mfFrenselBias(0.2f), mfFrenselPow(8.0f){}
        ~cMaterialType_Water_Vars(){}

        bool mbHasReflection;
        float mfRefractionScale;
        float mfFrenselBias;
        float mfFrenselPow;
        float mfReflectionFadeStart;
        float mfReflectionFadeEnd;
        float mfWaveSpeed;
        float mfWaveAmplitude;
        float mfWaveFreq;
    };

    //-----------------------------------------------------

    class cMaterialType_Water : public iMaterialType
    {
    public:
        cMaterialType_Water(cGraphics *apGraphics, cResources *apResources);
        ~cMaterialType_Water();

        void DestroyProgram(cMaterial *apMaterial, eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, char alSkeleton);

        bool SupportsHWSkinning(){ return false; }

        iTexture* GetTextureForUnit(cMaterial *apMaterial,eMaterialRenderMode aRenderMode, int alUnit);
        iTexture* GetSpecialTexture(cMaterial *apMaterial, eMaterialRenderMode aRenderMode,iRenderer *apRenderer, int alUnit);

        iGpuProgram* GetGpuProgram(cMaterial *apMaterial, eMaterialRenderMode aRenderMode, char alSkeleton);

        void SetupTypeSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram,iRenderer *apRenderer);
        void SetupMaterialSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, cMaterial *apMaterial,iRenderer *apRenderer);
        void SetupObjectSpecificData(eMaterialRenderMode aRenderMode, iGpuProgram* apProgram, iRenderable *apObject,iRenderer *apRenderer);

        iMaterialVars* CreateSpecificVariables();
        void LoadVariables(cMaterial *apMaterial, cResourceVarsObject *apVars);
        void GetVariableValues(cMaterial* apMaterial, cResourceVarsObject* apVars);

        void CompileMaterialSpecifics(cMaterial *apMaterial);
    
    private:
        void LoadData();
        void DestroyData();
    };

    //---------------------------------------------------

};
#endif // HPL_MATERIAL_WATER_H
