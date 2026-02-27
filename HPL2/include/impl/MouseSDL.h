#ifndef HPL_MOUSE_SDL_H
#define HPL_MOUSE_SDL_H

#include <vector>
#include "input/Mouse.h"


namespace hpl {
    
    class iLowLevelGraphics;
    class cLowLevelInputSDL;

    class cMouseSDL : public iMouse
    {
    public:
        cMouseSDL(cLowLevelInputSDL *apLowLevelInputSDL);

        bool ButtonIsDown(eMouseButton);
        
        void Update();
        
        /**
        * \todo Fix so it works and handles screen size
        * \return 
        */
        cVector2l GetAbsPosition();
        
        /**
         * \todo Fix so it works and handles screen size
         * \return 
         */
        cVector2l GetRelPosition();

        void Reset();
        
    private:
        bool mbFirstTime;

        cVector2l mvMouseAbsPos;
        cVector2l mvMouseRelPos;

        std::vector<bool> mvMButtonArray;

        cLowLevelInputSDL *mpLowLevelInputSDL;
        
        bool mbWheelUpMoved;
        bool mbWheelDownMoved;
    };

};

#endif // HPL_MOUSE_SDL_H
