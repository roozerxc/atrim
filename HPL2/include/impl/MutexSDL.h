#ifndef HPL_MUTEX_SDL_H
#define HPL_MUTEX_SDL_H

#include "system/Mutex.h"

struct SDL_mutex;

namespace hpl {

    class cMutexSDL : public iMutex
    {
    public:
        
        cMutexSDL();
        ~cMutexSDL();

        bool Lock();
        bool Unlock();

    private:
        SDL_mutex* mpMutexHandle;

    };

};
#endif // HPL_MUTEX_SDL_H
