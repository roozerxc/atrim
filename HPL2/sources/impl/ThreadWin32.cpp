#include "impl/ThreadWin32.h"

namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    
    cThreadWin32::cThreadWin32()
    {
        mpThreadHandle = NULL;
    }

    //-----------------------------------------------------------------------

    cThreadWin32::~cThreadWin32()
    {

    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    
    void cThreadWin32::Start()
    {
        mbThreadActive = true;
        mpThreadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)iThread::MainThreadFunc, (LPVOID)this, 0, NULL);
    }
    
    void cThreadWin32::Stop()
    {
        mbThreadActive = false;
        WaitForSingleObject(mpThreadHandle,INFINITE);
        mpThreadHandle = NULL;
    }

    void cThreadWin32::Sleep(unsigned int alSleepTime)
    {
        WaitForSingleObject(mpThreadHandle, alSleepTime);
    }

    void cThreadWin32::SetPriority(eThreadPrio aPrio)
    {
        iThread::SetPriority(aPrio);

        SetThreadPriority(mpThreadHandle, TranslateEnginePrio(aPrio));
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PRIVATE METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    int cThreadWin32::TranslateEnginePrio(eThreadPrio aPrio)
    {
        int lPrio = THREAD_PRIORITY_NORMAL;
        switch(aPrio)
        {
        case eThreadPrio_Low:
            lPrio = THREAD_PRIORITY_LOWEST;
            break;
        case eThreadPrio_High:
            lPrio = THREAD_PRIORITY_HIGHEST;
            break;
        }

        return lPrio;
    }


    //-----------------------------------------------------------------------

}

