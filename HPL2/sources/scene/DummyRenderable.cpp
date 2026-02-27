#include "scene/DummyRenderable.h"

//#include "math/Math.h"

//#include "resources/Resources.h"

namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cDummyRenderable::cDummyRenderable(tString asName) : iRenderable(asName)
    {
        mbApplyTransformToBV = true;

        mBoundingVolume.SetSize(0);
    }

    //-----------------------------------------------------------------------

    cDummyRenderable::~cDummyRenderable()
    {
    }    

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////
    

    //-----------------------------------------------------------------------

    /*
    void cDummyRenderable::SetSize(const cVector3f& avSize)
    {
        mvSize = avSize;

        mBoundingVolume.SetSize(cVector3f(mvSize.x, mvSize.y, mvSize.x));
        
        
        SetTransformUpdated();
    }
    */
    
    //-----------------------------------------------------------------------

    cMatrixf* cDummyRenderable::GetModelMatrix(cFrustum* apFrustum)
    {
        m_mtxModelOutput = GetWorldMatrix();

        return &m_mtxModelOutput;
    }

    //-----------------------------------------------------------------------
    
    //////////////////////////////////////////////////////////////////////////
    // PRIVATE METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------

}
