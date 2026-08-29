#ifndef HPL_LOWLEVELINPUT_SDL_H
#define HPL_LOWLEVELINPUT_SDL_H

#include <list>
#include "input/LowLevelInput.h"

#include "SDL/SDL_events.h"

namespace hpl
{

class iLowLevelGraphics;

class cLowLevelInputSDL : public iLowLevelInput
{
public:
    cLowLevelInputSDL(iLowLevelGraphics *apLowLevelGraphics);
    ~cLowLevelInputSDL();

    void LockInput(bool abX);
    void RelativeMouse(bool abX);

    void BeginInputUpdate();
    void EndInputUpdate();

#if USE_GAMEPAD
    void InitGamepadSupport();
    void DropGamepadSupport();

    bool DirtyGamepads();
    int GetPluggedGamepadNum();
#endif

    iMouse* CreateMouse();
    iKeyboard* CreateKeyboard();
#if USE_GAMEPAD
    iGamepad* CreateGamepad(int alIndex);
#endif

    iLowLevelGraphics* GetLowLevelGraphics()
    {
        return mpLowLevelGraphics;
    }

    bool isQuitMessagePosted();
    void resetQuitMessagePosted();
public:
    std::list<SDL_Event> mlstEvents;
#if USE_GAMEPAD
    int mlConnectedDevices;
    bool mbDirtyGamepads;
#endif

private:
    iLowLevelGraphics *mpLowLevelGraphics;
    bool mbQuitMessagePosted;
};
};
#endif // HPL_LOWLEVELINPUT_SDL_H
