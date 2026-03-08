#ifndef HPL_THREAD_SDL_H
#define HPL_THREAD_SDL_H

#include "system/Thread.h"

struct SDL_Thread;

namespace hpl
{

class cThreadSDL : public iThread
{
public:

    cThreadSDL();
    ~cThreadSDL();

    void Start();
    void Stop();
    void Sleep(unsigned int alSleepTime);
    void SetPriority(eThreadPrio aPrio);

protected:
    int TranslateEnginePrio(eThreadPrio aPrio);

private:
    SDL_Thread* mpThreadHandle;

};

};
#endif // HPL_THREAD_SDL_H
