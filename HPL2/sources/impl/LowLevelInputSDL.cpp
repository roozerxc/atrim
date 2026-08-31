#include "impl/LowLevelInputSDL.h"

#include "impl/MouseSDL.h"
#include "impl/KeyboardSDL.h"
#if USE_GAMEPAD
#include "impl/GamepadSDL.h"
#endif
#if USE_XINPUT
#pragma comment(lib, "XInput.lib")
#include "impl/GamepadXInput.h"
#endif

#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"

#include "engine/Engine.h"

#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"

#if defined _WIN32
#include <Windows.h>
#include <Dbt.h>
#endif

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLowLevelInputSDL::cLowLevelInputSDL(iLowLevelGraphics *apLowLevelGraphics)
    : mpLowLevelGraphics(apLowLevelGraphics), mbQuitMessagePosted(false)
{
    LockInput(true);
    RelativeMouse(false);
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
}

//-----------------------------------------------------------------------

cLowLevelInputSDL::~cLowLevelInputSDL()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHOD
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLowLevelInputSDL::LockInput(bool abX)
{
    mpLowLevelGraphics->SetWindowGrab(abX);
}

void cLowLevelInputSDL::RelativeMouse(bool abX)
{
    mpLowLevelGraphics->SetRelativeMouse(abX);
}

//-----------------------------------------------------------------------

void cLowLevelInputSDL::BeginInputUpdate()
{
    SDL_Event sdlEvent;

    mlstEvents.clear();
    while(SDL_PollEvent(&sdlEvent)!=0)
    {
#if defined _WIN32
        if(sdlEvent.type==SDL_SYSWMEVENT)
        {
            SDL_SysWMmsg* pMsg = sdlEvent.syswm.msg;

            // This is bad, cos it is actually Windows specific code, should not be here. TODO: move it, obviously
            if(pMsg->msg==WM_DEVICECHANGE)
            {
                if(pMsg->wParam==DBT_DEVICEARRIVAL)
                {
                    cEngine::SetDeviceWasPlugged();
                }
                else if(pMsg->wParam==DBT_DEVICEREMOVECOMPLETE)
                {
                    cEngine::SetDeviceWasRemoved();
                }
            }
        }
        else
#endif //WIN32
#if defined (__APPLE__)
            if (sdlEvent.type==SDL_KEYDOWN)
            {
                if (sdlEvent.key.keysym.sym == SDLK_q && sdlEvent.key.keysym.mod & KMOD_GUI)
                {
                    mbQuitMessagePosted = true;
                }
                else
                {
                    mlstEvents.push_back(sdlEvent);
                }
            }
            else
#endif
                if (sdlEvent.type==SDL_QUIT)
                {
                    mbQuitMessagePosted = true;
                }
                else if(sdlEvent.type == SDL_ACTIVEEVENT)
                {
                    static bool bInputLocked = false;
                    static bool bRelativeMouse = false;
                    static bool bStateSaved = false;

                    if((sdlEvent.active.state & SDL_APPINPUTFOCUS) || (sdlEvent.active.state & SDL_APPACTIVE))
                    {
                        if(sdlEvent.active.gain == 0)
                        {
                            if(!bStateSaved)
                            {
                                bInputLocked = (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_ON);
                                bRelativeMouse = (SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE);
                                bStateSaved = true;
                            }

                            LockInput(false);
                            RelativeMouse(false);
                        }
                        else if(sdlEvent.active.gain == 1)
                        {
                            LockInput(bInputLocked);
                            RelativeMouse(bRelativeMouse);

                            bStateSaved = false;
                        }
                    }
                    mlstEvents.push_back(sdlEvent);
                }
                else
                {
                    mlstEvents.push_back(sdlEvent);
                }
    }
}

//-----------------------------------------------------------------------

void cLowLevelInputSDL::EndInputUpdate()
{

}

//-----------------------------------------------------------------------

#if USE_GAMEPAD
void cLowLevelInputSDL::InitGamepadSupport()
{
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
}

void cLowLevelInputSDL::DropGamepadSupport()
{
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

int cLowLevelInputSDL::GetPluggedGamepadNum()
{
#if USE_XINPUT
    return cGamepadXInput::GetNumConnected();
#else
    return SDL_NumJoysticks();
#endif
}
#endif

//-----------------------------------------------------------------------

iMouse* cLowLevelInputSDL::CreateMouse()
{
    return hplNew( cMouseSDL,(this));
}

//-----------------------------------------------------------------------

iKeyboard* cLowLevelInputSDL::CreateKeyboard()
{
    return hplNew( cKeyboardSDL,(this) );
}

//-----------------------------------------------------------------------

#if USE_GAMEPAD
iGamepad* cLowLevelInputSDL::CreateGamepad(int alIndex)
{
    return hplNew( cGamepadSDL, (this, alIndex) );
}
#endif

//-----------------------------------------------------------------------

bool cLowLevelInputSDL::isQuitMessagePosted()
{
    return mbQuitMessagePosted;
}

void cLowLevelInputSDL::resetQuitMessagePosted()
{
    mbQuitMessagePosted = false;
}

//-----------------------------------------------------------------------

}
