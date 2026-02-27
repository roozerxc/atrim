#ifndef HPL_TIMER_SDL_H
#define HPL_TIMER_SDL_H

#include "system/Timer.h"

#ifdef _WIN32   // Windows system specific
    #include <windows.h>
#else          // Unix based system specific
    #include <sys/time.h>
#endif

namespace hpl {

    class cTimerSDL : public iTimer
    {
    public:
        
        cTimerSDL();
        ~cTimerSDL();
        
        void Start();
        void Stop();

        double GetTimeInMicroSec(); 
    private:
        double mfStartTimeInMicroSec;
        double mfEndTimeInMicroSec;
        bool   mbStopped;
        
        #ifdef _WIN32
            LARGE_INTEGER mFrequency; 
            LARGE_INTEGER mStartCount;
            LARGE_INTEGER mEndCount;  
        #else
            double mFrequency;
            timeval mStartCount;
            timeval mEndCount;
        #endif
    };

};
#endif // HPL_TIMER_SDL_H
