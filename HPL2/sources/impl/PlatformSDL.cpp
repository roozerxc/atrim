#include "system/Platform.h"

#include "system/String.h"

#include "system/LowLevelSystem.h"

#include "SDL/SDL.h"

#include "impl/TimerSDL.h"
#include "impl/ThreadSDL.h"
#include "impl/MutexSDL.h"

#include <set>
#include <algorithm>

#include <sys/time.h>
#include <time.h>

namespace hpl
{

    static bool bTimerInitialized = false;
    static struct timespec lTimerStart;

//////////////////////////////////////////////////////////////////////////
// APPLICATION
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

unsigned long cPlatform::GetApplicationTime()
{
    return SDL_GetTicks();
}

//-----------------------------------------------------------------------

double cPlatform::GetApplicationTimeX()
{
    struct timespec lCurrentTime;

    if(!bTimerInitialized)
    {
        clock_gettime(CLOCK_MONOTONIC, &lTimerStart);
        bTimerInitialized = true;
    }

    clock_gettime(CLOCK_MONOTONIC, &lCurrentTime);

    double dSeconds     = (double)(lCurrentTime.tv_sec  - lTimerStart.tv_sec);
    double dNanoSeconds = (double)(lCurrentTime.tv_nsec - lTimerStart.tv_nsec) / 1000000000.0;

    return dSeconds + dNanoSeconds;
}

//-----------------------------------------------------------------------

void cPlatform::Sleep ( const unsigned int alMillisecs )
{
    SDL_Delay ( alMillisecs );
}

void cPlatform::CopyTextToClipboard(const tWString &asText)
{
}

//-----------------------------------------------------------------------

tWString cPlatform::LoadTextFromClipboard()
{
    tWString tstr;
    return tstr;
}

void cPlatform::GetDisplayResolution(int alDisplay, int& alWidth, int& alHeight)
{
    alWidth = 1024;
    alHeight = 768;
}

//-----------------------------------------------------------------------
void cPlatform::GetAvailableVideoModes(tVideoModeVec& avDestVidModes, int alMinBpp, int alMinRefreshRate)
{
    const SDL_VideoInfo *info = SDL_GetVideoInfo();
    if (!info)
    {
        return;
    }
    SDL_Rect **modes = SDL_ListModes(info->vfmt, SDL_OPENGL | SDL_FULLSCREEN);
    avDestVidModes.clear();
    if (modes == NULL)
    {
        return;
    }
    for (int i=0; modes[i]; i++)
    {
        avDestVidModes.push_back(cVideoMode(cVector2l(modes[i]->w, modes[i]->h),info->vfmt->BitsPerPixel,1));
    }
    sort(avDestVidModes.begin(), avDestVidModes.end(), VideoComp());
}

tWString cPlatform::GetDisplayName(int alDisplay)
{
    return cString::To16Char(SDL_GetDisplayName(alDisplay));
}

#ifndef HPL_MINIMAL
//-----------------------------------------------------------------------

iTimer * cPlatform::CreateTimer()
{
    return hplNew(cTimerSDL, () );
}

//////////////////////////////////////////////////////////////////////////
// THREADING
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
iThread* cPlatform::CreateThread(iThreadClass* apThreadClass)
{
    iThread* pThread = hplNew(cThreadSDL, ());
    pThread->SetThreadClass(apThreadClass);

    return pThread;
}

//-----------------------------------------------------------------------

iMutex* cPlatform::CreateMutEx()
{
    return hplNew(cMutexSDL, ());
}
#endif
}
