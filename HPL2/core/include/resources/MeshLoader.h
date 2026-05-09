#ifndef HPL_MESH_LOADER_H
#define HPL_MESH_LOADER_H

#include "resources/ResourceLoader.h"

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "resources/ResourcesTypes.h"

namespace hpl
{

class cMesh;

class cResources;
class cMaterialManager;
class cMeshManager;
class cAnimationManager;
class cMeshLoaderHandler;
class iLowLevelGraphics;
class cWorld;
class cScene;
class cAnimation;
class cSystem;

//----------------------------------------

class iMeshLoader : public iResourceLoader
{
    friend class cMeshLoaderHandler;
public:
    iMeshLoader(iLowLevelGraphics *apLowLevelGraphics): mpLowLevelGraphics(apLowLevelGraphics) {}

    virtual cMesh* LoadMesh(const tWString& asFile, tMeshLoadFlag aFlags)=0;
    virtual bool SaveMesh(cMesh* apMesh,const tWString& asFile)=0;

    virtual cAnimation* LoadAnimation(const tWString& asFile)=0;
    virtual bool SaveAnimation(cAnimation* apAnimation, const tWString& asFile)=0;

protected:
    cMaterialManager *mpMaterialManager;
    cMeshManager *mpMeshManager;
    cAnimationManager *mpAnimationManager;
    iLowLevelGraphics *mpLowLevelGraphics;
};

};
#endif // HPL_MESH_LOADER_H
