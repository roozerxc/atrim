#include "scene/LightBox.h"

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

    cLightBox::cLightBox(tString asName, cResources *apResources) : iLight(asName,apResources)
    {
        mLightType = eLightType_Box;

        mvSize = 1;
        mBlendFunc = eLightBoxBlendFunc_Replace;
        mlBoxLightPrio =0;

        UpdateBoundingVolume();
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    void cLightBox::SetSize(const cVector3f& avSize)
    { 
        mvSize = avSize;

        mbUpdateBoundingVolume = true;

        //This is so that the render container is updated.
        SetTransformUpdated();
    }

    //-----------------------------------------------------------------------

    bool cLightBox::IsVisible()
    {
        if(mDiffuseColor.r <=0 && mDiffuseColor.g <=0 && mDiffuseColor.b <=0 && mDiffuseColor.a <=0) 
            return false;
        
        return mbIsVisible; 
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PRIVATE METHODS
    //////////////////////////////////////////////////////////////////////////
    
    //-----------------------------------------------------------------------
    
    void cLightBox::UpdateBoundingVolume()
    {
        mBoundingVolume.SetSize(mvSize);
        mBoundingVolume.SetPosition(GetWorldPosition());
    }
    //-----------------------------------------------------------------------

}
