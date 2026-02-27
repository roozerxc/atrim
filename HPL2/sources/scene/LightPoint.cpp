#include "scene/LightPoint.h"

#include "graphics/LowLevelGraphics.h"
#include "scene/Camera.h"
#include "math/Math.h"

#include "scene/World.h"
#include "scene/Scene.h"
#include "engine/Engine.h"


namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cLightPoint::cLightPoint(tString asName, cResources *apResources) : iLight(asName,apResources)
    {
        mLightType = eLightType_Point;

        UpdateBoundingVolume();
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PRIVATE METHODS
    //////////////////////////////////////////////////////////////////////////
    
    //-----------------------------------------------------------------------
    
    void cLightPoint::UpdateBoundingVolume()
    {
        mBoundingVolume.SetSize(mfRadius*2);
        mBoundingVolume.SetPosition(GetWorldPosition());
    }
    //-----------------------------------------------------------------------

}
