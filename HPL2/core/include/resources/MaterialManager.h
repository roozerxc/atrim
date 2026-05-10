#ifndef HPL_MATERIAL_MANAGER_H
#define HPL_MATERIAL_MANAGER_H

#include "resources/ResourceManager.h"
#include "graphics/Texture.h"
#include "graphics/Material.h"

namespace hpl
{

class cGraphics;
class cResources;
class cMaterial;
class iMaterialType;

class cMaterialManager : public iResourceManager
{
public:
    cMaterialManager(cGraphics* apGraphics,cResources *apResources);
    ~cMaterialManager();

    cMaterial* CreateMaterial(const tString& asName);

    void Update(float afTimeStep);

    void Destroy(iResourceBase* apResource);
    void Unload(iResourceBase* apResource);

    void SetTextureSizeDownScaleLevel(unsigned int alLevel)
    {
        mlTextureSizeDownScaleLevel = alLevel;
    }
    int GetTextureSizeDownScaleLevel()
    {
        return mlTextureSizeDownScaleLevel;
    }

    void SetTextureFilter(eTextureFilter aFilter);
    eTextureFilter GetTextureFilter()
    {
        return mTextureFilter;
    }

    void SetTextureAnisotropy(float afX);
    float GetTextureAnisotropy()
    {
        return mfTextureAnisotropy;
    }

    tString GetPhysicsMaterialName(const tString& asName);

    cMaterial* CreateCustomMaterial(const tString& asName, iMaterialType *apMaterialType);

    tString GetTextureString(eMaterialTexture aType);

    void SetDisableRenderDataLoading(bool abX)
    {
        mbDisableRenderDataLoading = abX;
    }

    // Useful stuff if public
    eTextureType GetType(const tString& asType);
    eTextureWrap GetWrap(const tString& asType);
    eTextureAnimMode GetAnimMode(const tString& asType);
    eMaterialBlendMode GetBlendMode(const tString& asType);

    eMaterialUvAnimation GetUvAnimType(const char* apString);
    eMaterialAnimationAxis GetAnimAxis(const char* apString);

private:
    cMaterial* LoadFromFile(const tString& asName,const tWString& asPath);

    unsigned int mlTextureSizeDownScaleLevel;
    eTextureFilter mTextureFilter;
    float mfTextureAnisotropy;

    tStringList mlstFileFormats;

    tStringVec mvCubeSideSuffixes;

    cGraphics* mpGraphics;
    cResources* mpResources;

    bool mbDisableRenderDataLoading;

    int mlIdCounter;
};

};
#endif // HPL_MATERIAL_MANAGER_H
