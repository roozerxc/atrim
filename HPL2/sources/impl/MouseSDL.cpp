#include "impl/MouseSDL.h"

#include "SDL/SDL.h"

#include "graphics/LowLevelGraphics.h"
#include "impl/LowLevelInputSDL.h"
#include "math/Math.h"

#include "system/LowLevelSystem.h"
#include "system/Platform.h"

namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cMouseSDL::cMouseSDL(cLowLevelInputSDL *apLowLevelInputSDL) : iMouse("SDL Portable Mouse")
    {
        mvMButtonArray.resize(eMouseButton_LastEnum);
        mvMButtonArray.assign(mvMButtonArray.size(),false);

        mpLowLevelInputSDL = apLowLevelInputSDL;

        mvMouseRelPos = cVector2l(0,0);
        mvMouseAbsPos = cVector2l(0,0);

        mbWheelUpMoved = false;
        mbWheelDownMoved = false;

        mbFirstTime = true;
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////
    
    //-----------------------------------------------------------------------

    void cMouseSDL::Update()
    {
        ///////////////////////////////////////
        //On first update make sure to clear all previous events.
        if(mbFirstTime)
        {
            //Clear all relative movement
            for(int i=0; i<10; ++i)
            {
                SDL_PumpEvents();
                int lX,lY;
                SDL_GetRelativeMouseState(&lX, &lY);
            }
            mbFirstTime = false;
        }
        
        iLowLevelGraphics *pLowLevelGfx = mpLowLevelInputSDL->GetLowLevelGraphics();
        //mvMouseRelPos = cVector2f(0,0);
        mbWheelUpMoved = false;
        mbWheelDownMoved = false;

        std::list<SDL_Event>::iterator it = mpLowLevelInputSDL->mlstEvents.begin();
        for(; it != mpLowLevelInputSDL->mlstEvents.end(); ++it)
        {
            SDL_Event *pEvent = &(*it);

            if(    pEvent->type != SDL_MOUSEMOTION && 
                pEvent->type != SDL_MOUSEBUTTONDOWN &&
                pEvent->type != SDL_MOUSEBUTTONUP)
            {
                continue;
            }

            if(pEvent->type == SDL_MOUSEMOTION)
            {
                mvMouseAbsPos = cVector2l(pEvent->motion.x,pEvent->motion.y);
                
                Uint8 buttonState = pEvent->motion.state;

                //Set button here as well just to be sure
                /*if(buttonState & SDL_BUTTON(1)) mvMButtonArray[eMouseButton_Left] = true;
                if(buttonState & SDL_BUTTON(2)) mvMButtonArray[eMouseButton_Middle] = true;
                if(buttonState & SDL_BUTTON(3)) mvMButtonArray[eMouseButton_Right] = true;*/
            }
            else
            {
                bool bButtonIsDown = pEvent->type==SDL_MOUSEBUTTONDOWN;

                //if(pEvent->button.button == SDL_BUTTON_WHEELUP)Log(" Wheel %d!\n",bButtonIsDown);

                switch(pEvent->button.button)
                {
                    case SDL_BUTTON_LEFT: mvMButtonArray[eMouseButton_Left] = bButtonIsDown;break;
                    case SDL_BUTTON_MIDDLE: mvMButtonArray[eMouseButton_Middle] = bButtonIsDown;break;
                    case SDL_BUTTON_RIGHT: mvMButtonArray[eMouseButton_Right] = bButtonIsDown;break;
                    case SDL_BUTTON_X1: mvMButtonArray[eMouseButton_Button6] = bButtonIsDown;break;
                    case SDL_BUTTON_X2: mvMButtonArray[eMouseButton_Button7] = bButtonIsDown;break;
                    case SDL_BUTTON_WHEELUP: 
                        mvMButtonArray[eMouseButton_WheelUp] = bButtonIsDown;
                        if(bButtonIsDown) mbWheelUpMoved = true;
                        break;
                    case SDL_BUTTON_WHEELDOWN: 
                        mvMButtonArray[eMouseButton_WheelDown] = bButtonIsDown;
                        if(bButtonIsDown) mbWheelDownMoved = true;
                        break;
                }
            }
        }

        if(mbWheelDownMoved)    mvMButtonArray[eMouseButton_WheelDown] = true;
        else                    mvMButtonArray[eMouseButton_WheelDown] = false;
        if(mbWheelUpMoved)        mvMButtonArray[eMouseButton_WheelUp] = true;
        else                    mvMButtonArray[eMouseButton_WheelUp] = false;
        
        int lX,lY; 
        SDL_GetRelativeMouseState(&lX, &lY);
        mvMouseRelPos = cVector2l(lX,lY);

        
    }
    
    //-----------------------------------------------------------------------
    
    bool cMouseSDL::ButtonIsDown(eMouseButton mButton)
    {
        return mvMButtonArray[mButton];
    }

    //-----------------------------------------------------------------------

    cVector2l cMouseSDL::GetAbsPosition()
    {
        return mvMouseAbsPos;
    }
    
    //-----------------------------------------------------------------------

    cVector2l cMouseSDL::GetRelPosition()
    {
        cVector2l vPos = mvMouseRelPos;
        mvMouseRelPos = cVector2l(0,0);
        
        return vPos;
    }

    //-----------------------------------------------------------------------
    
    /////////////////////////////////////////////////////////////////////////
    // PRIVATE METHODS
    /////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    
    //-----------------------------------------------------------------------

}
