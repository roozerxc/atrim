#ifndef HPL_THREAD_WIN32_H
#define HPL_THREAD_WIN32_H

#include "system/Thread.h"

#include <windows.h>

namespace hpl
{

class cThreadWin32 : public iThread
{
public:

    cThreadWin32();
    ~cThreadWin32();

    void Start();
    void Stop();
    void Sleep(unsigned int alSleepTime);

    void SetPriority(eThreadPrio aPrio);

private:
    int TranslateEnginePrio(eThreadPrio aPrio);
    HANDLE mpThreadHandle;

};

};
#endif // HPL_THREAD_WIN32_H
