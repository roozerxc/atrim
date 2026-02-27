#include "scene/FogArea.h"

#include "math/Math.h"

#include "resources/Resources.h"

namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cFogArea::cFogArea(tString asName, cResources *apResources) : iRenderable(asName)
    {
        mColor = cColor(1,1);
        mvSize = 1;

        mfStart = 0;
        mfEnd = 10;
        mfFalloffExp = 1.0f;

        mbShowBacksideWhenOutside = true;
        mbShowBacksideWhenInside = true;

        mbApplyTransformToBV = true;

        mBoundingVolume.SetSize(1);
    }

    //-----------------------------------------------------------------------

    cFogArea::~cFogArea()
    {
    }    

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////
    

    //-----------------------------------------------------------------------

    void cFogArea::SetSize(const cVector3f& avSize)
    {
        mvSize = avSize;

        mBoundingVolume.SetSize(cVector3f(mvSize.x, mvSize.y, mvSize.x));
        
        
        SetTransformUpdated();
    }
    
    //-----------------------------------------------------------------------

    cMatrixf* cFogArea::GetModelMatrix(cFrustum* apFrustum)
    {
        m_mtxModelOutput = cMath::MatrixMul(GetWorldMatrix(), cMath::MatrixScale(mvSize));

        return &m_mtxModelOutput;
    }
    //-----------------------------------------------------------------------
    
    //////////////////////////////////////////////////////////////////////////
    // PRIVATE METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------

}
