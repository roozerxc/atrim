#ifndef HPL_LOWLEVELINPUT_SDL_H
#define HPL_LOWLEVELINPUT_SDL_H

#include <list>
#include "input/LowLevelInput.h"

#if USE_SDL2
#include "SDL2/SDL_events.h"
#else
#include "SDL/SDL_events.h"
#endif

namespace hpl {

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

        void InitGamepadSupport();
        void DropGamepadSupport();

        int GetPluggedGamepadNum();

        iMouse* CreateMouse();
        iKeyboard* CreateKeyboard();
        iGamepad* CreateGamepad(int alIndex);

        iLowLevelGraphics* GetLowLevelGraphics() { return mpLowLevelGraphics; }

        bool isQuitMessagePosted();
        void resetQuitMessagePosted();
    public:
        std::list<SDL_Event> mlstEvents;

    private: 
        iLowLevelGraphics *mpLowLevelGraphics;
        bool mbQuitMessagePosted;
    };
};
#endif // HPL_LOWLEVELINPUT_SDL_H
