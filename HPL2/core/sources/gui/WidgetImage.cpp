#include "gui/WidgetImage.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cWidgetImage::cWidgetImage(cGuiSet *apSet, cGuiSkin *apSkin) : iWidget(eWidgetType_Image,apSet, apSkin)
{
    mpGfxImage = NULL;
}

//-----------------------------------------------------------------------

cWidgetImage::~cWidgetImage()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cWidgetImage::SetImage(cGuiGfxElement *apGfx)
{
    if(mpGfxImage == apGfx) return;

    mpGfxImage = apGfx;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cWidgetImage::OnLoadGraphics()
{
}

//-----------------------------------------------------------------------

void cWidgetImage::OnChangeSize()
{
}

//-----------------------------------------------------------------------

void cWidgetImage::OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion)
{
    ////////////////////////////////
    // Background
    if(mpGfxImage)
    {
        mpSet->DrawGfx(    mpGfxImage,    GetGlobalPosition(),mvSize,mColorMul);
    }
}

//-----------------------------------------------------------------------

bool cWidgetImage::OnMouseMove(const cGuiMessageData& aData)
{
    return true;
}

//-----------------------------------------------------------------------

bool cWidgetImage::OnMouseDown(const cGuiMessageData& aData)
{
    if(aData.mlVal==eGuiMouseButton_WheelUp || aData.mlVal==eGuiMouseButton_WheelDown)
        return false;

    return true;
}

//-----------------------------------------------------------------------

bool cWidgetImage::OnMouseUp(const cGuiMessageData& aData)
{
    if(aData.mlVal==eGuiMouseButton_WheelUp || aData.mlVal==eGuiMouseButton_WheelDown)
        return false;

    return true;
}

//-----------------------------------------------------------------------

bool cWidgetImage::OnMouseEnter(const cGuiMessageData& aData)
{
    return false;
}

//-----------------------------------------------------------------------

bool cWidgetImage::OnMouseLeave(const cGuiMessageData& aData)
{
    return false;
}

//-----------------------------------------------------------------------


}
