#include "scene/GuiSetEntity.h"

#include "gui/Gui.h"
#include "gui/GuiSet.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cGuiSetEntity::cGuiSetEntity(const tString asName,cGuiSet *apSet) :
    iRenderable(asName)
{
    mpGuiSet = apSet;

    mBoundingVolume.SetSize(apSet->Get3DSize());
}

//-----------------------------------------------------------------------

cGuiSetEntity::~cGuiSetEntity()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cGuiSetEntity::SetGuiSet(cGuiSet *apSet)
{
    mpGuiSet = apSet;
    mBoundingVolume.SetSize(apSet->Get3DSize());
}

//-----------------------------------------------------------------------

cMatrixf*  cGuiSetEntity::GetModelMatrix(cFrustum *apFrustum)
{
    m_mtxTemp = GetWorldMatrix();
    return &m_mtxTemp;
}

//-----------------------------------------------------------------------

int cGuiSetEntity::GetMatrixUpdateCount()
{
    return GetTransformUpdateCount();
}

//-----------------------------------------------------------------------


}
