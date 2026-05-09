#include "impl/ThreadSDL.h"

#if USE_SDL2
#include "SDL2/SDL.h"
#include <typeinfo>
#else
#include "SDL/SDL.h"
#include <pthread.h>
#endif

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cThreadSDL::cThreadSDL()
{
    mpThreadHandle = NULL;
}

//-----------------------------------------------------------------------

cThreadSDL::~cThreadSDL()
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cThreadSDL::Start()
{
    mbThreadActive = true;
#if SDL_VERSION_ATLEAST(2, 0, 0)
    mpThreadHandle = SDL_CreateThread(iThread::MainThreadFunc, typeid(this).name(), this);
#else
    mpThreadHandle = SDL_CreateThread(iThread::MainThreadFunc, this);
#endif
}

//-----------------------------------------------------------------------

void cThreadSDL::Stop()
{
    mbThreadActive = false;
    SDL_WaitThread(mpThreadHandle,0);
    mpThreadHandle = NULL;
}

//-----------------------------------------------------------------------

void cThreadSDL::Sleep(unsigned int alSleepTime)
{
    SDL_Delay(alSleepTime);
}

//-----------------------------------------------------------------------

void cThreadSDL::SetPriority(eThreadPrio aPrio)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
    int prio = TranslateEnginePrio(aPrio);
    SDL_SetThreadPriority((SDL_ThreadPriority)prio);
#else
    /** investigate further on other linux distros and mac os x
     * As right now Priority range seems to be 0 .. 0 in linux */
#   ifdef __linux__
    struct sched_param param;
    param.sched_priority = TranslateEnginePrio(aPrio);
    int ret = pthread_setschedparam(SDL_GetThreadID(mpThreadHandle),SCHED_OTHER,&param);
#   endif
#endif
    iThread::SetPriority(aPrio);
}

//-----------------------------------------------------------------------

int cThreadSDL::TranslateEnginePrio(eThreadPrio aPrio)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
    switch (aPrio)
    {
    case eThreadPrio_Low:
        return SDL_THREAD_PRIORITY_LOW;
    case eThreadPrio_High:
        return SDL_THREAD_PRIORITY_HIGH;
    case eThreadPrio_Normal:
    default:
        return SDL_THREAD_PRIORITY_NORMAL;
    }
#else
    switch (aPrio)
    {
    case eThreadPrio_Low:
    case eThreadPrio_Normal:
    case eThreadPrio_High:
    default:
        return 0;
    }
#endif
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


//-----------------------------------------------------------------------

}

