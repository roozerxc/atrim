#include "gui/WidgetListBox.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

#include "gui/WidgetSlider.h"

#include <algorithm>

namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cWidgetListBox::cWidgetListBox(cGuiSet *apSet, cGuiSkin *apSkin) : iWidgetListBoxBase(apSet, apSkin)
    {
    }

    //-----------------------------------------------------------------------

    cWidgetListBox::~cWidgetListBox()
    {
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PROTECTED METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    
    void cWidgetListBox::DrawItems(float afTimeStep, cGuiClipRegion* apClipRegion)
    {
        cVector3f vPosition = GetItemStartPos()+cVector3f(3,0,mfBackgroundZ+0.1f);
        for(int i=mlFirstItem; i < (int)mvItems.size(); ++i)
        {
            if(i-mlFirstItem > mlMaxItems) break;
            cWidgetItem* pItem = mvItems[i];
            if(pItem==NULL)
                continue;

            if(pItem->IsSelectable()==false)
            {
                mpSet->DrawGfx( mpGfxBackground, vPosition - cVector3f(3,0,0.01f), cVector2f(mvSize.x, mvDefaultFontSize.y), cColor(0.82f,0.81f,0.79f,1));
            }

            if(pItem->IsSelected())
            {
                mpSet->DrawGfx(    mpGfxSelection,vPosition - cVector3f(3,0,0.01f),
                                cVector2f(mvSize.x,mvDefaultFontSize.y));
                DrawDefaultTextHighlight(pItem->GetText(),vPosition,eFontAlign_Left);
            }
            else
                DrawDefaultText(pItem->GetText(),vPosition,eFontAlign_Left);
            
            vPosition.y += mvDefaultFontSize.y +2;
        }
    }
    
    //-----------------------------------------------------------------------

}
