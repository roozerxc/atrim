#ifndef HPL_MUTEX_WIN32_H
#define HPL_MUTEX_WIN32_H

#include "system/Mutex.h"

#include <windows.h>

namespace hpl
{

class cMutexWin32 : public iMutex
{
public:

    cMutexWin32();
    ~cMutexWin32();

    bool Lock();
    bool Unlock();

private:
    HANDLE mpMutexHandle;

};

};
#endif // HPL_MUTEX_WIN32_H
