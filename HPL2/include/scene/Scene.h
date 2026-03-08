#ifndef HPL_SCENE_H
#define HPL_SCENE_H

#include <list>

#include "system/SystemTypes.h"
#include "engine/EngineTypes.h"

#include "engine/Updateable.h"
#include "scene/Camera.h"

#include "resources/MeshLoader.h"

namespace hpl
{

class cAI;
class cGraphics;
class cResources;
class cSystem;
class cSound;
class cPhysics;
class cGui;

class cCamera;
class cFrustum;
class cUpdater;
class cWorld;
class cViewport;

//--------------------------------------------------------------------

typedef std::list<cViewport*> tViewportList;
typedef tViewportList::iterator tViewportListIt;

typedef std::list<cCamera*> tCameraList;
typedef tCameraList::iterator tCameraListIt;

typedef std::list<cWorld*> tWorldList;
typedef tWorldList::iterator tWorldListIt;

//--------------------------------------------------------------------


#define tSceneRenderFlag_World            0x00000001
#define tSceneRenderFlag_Gui            0x00000002
#define tSceneRenderFlag_PostEffects    0x00000004
#define tSceneRenderFlag_All            0xFFFFFFFF

//--------------------------------------------------------------------

class cScene : public iUpdateable
{
public:
    cScene(cGraphics *apGraphics,cResources *apResources, cSound* apSound, cPhysics *apPhysics,
           cSystem *apSystem, cAI *apAI,cGui *apGui);
    ~cScene();

    void Reset();

    /**
     * Called by cEngine
     */
    void Render(float afFrameTime, tFlag alFlags);

    void PostUpdate(float afTimeStep);

    ///// VIEW PORT METHODS ////////////////////

    cViewport* CreateViewport(cCamera *apCamera=NULL, cWorld *apWorld=NULL, bool abPushFront = false);
    void DestroyViewport(cViewport* apViewPort);
    bool ViewportExists(cViewport* apViewPort);

    void SetCurrentListener(cViewport* apViewPort);

    ///// CAMERA METHODS ////////////////////

    cCamera* CreateCamera(eCameraMoveMode aMoveMode);
    void DestroyCamera(cCamera* apCam);

    ///// WORLD METHODS ////////////////////

    cWorld* LoadWorld(const tString& asFile, tWorldLoadFlag aFlags);
    cWorld* CreateWorld(const tString& asName);
    void DestroyWorld(cWorld* apWorld);
    bool WorldExists(cWorld* apWorld);


private:
    void Render3DGui(cViewport* apViewPort,cFrustum *apFrustum,float afTimeStep);
    void RenderScreenGui(cViewport* apViewPort, float afTimeStep);

    cGraphics *mpGraphics;
    cResources *mpResources;
    cSound *mpSound;
    cPhysics *mpPhysics;
    cSystem *mpSystem;
    cAI *mpAI;
    cGui *mpGui;

    cViewport *mpCurrentListener;

    tViewportList mlstViewports;
    tWorldList mlstWorlds;
    tCameraList mlstCameras;
};

};
#endif // HPL_SCENE_H
