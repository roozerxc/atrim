#ifndef HPL_MATERIAL_DECAL_H
#define HPL_MATERIAL_DECAL_H

#include "graphics/MaterialType.h"
#include "graphics/Material.h"

namespace hpl {

    //---------------------------------------------------
    // Decal
    //---------------------------------------------------

    class cMaterialType_Decal_Vars : public iMaterialVars
    {
    public:
        cMaterialType_Decal_Vars(){}
        ~cMaterialType_Decal_Vars(){}
       };

    //-----------------------------------------------------

    class cMaterialType_Decal : public iMaterialType
    {
    public:
        cMaterialType_Decal(cGraphics *apGraphics, cResources *apResources);
        ~cMaterialType_Decal();

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
#endif // HPL_MATERIAL_DECAL_H
