#ifndef HPL_FRAME_PROFILER_H
#define HPL_FRAME_PROFILER_H

#include "system/SystemTypes.h"

namespace hpl
{

//---------------------------------------------

typedef void (*tFrameProfilerGpuSyncFunc)();

enum eFrameProfilerItem
{
    eFrameProfilerItem_Light,
    eFrameProfilerItem_Object,
    eFrameProfilerItem_LastEnum
};

//---------------------------------------------

/**
 * Frame profiler for finding what a frame is spent on, on CPU and GPU.
 *
 * Frames are measured in two alternating windows:
 *  - plain: sections are timed as they are, gives the CPU time (game logic, driver work).
 *  - synced: the GPU is flushed (glFinish) at the border of sections marked as GPU sections,
 *            so their time is CPU + GPU. Works on any card, no timer queries needed.
 * After both windows a report is written to the log. GPU cost of a section is roughly synced - plain.
 *
 * Items (single lights and objects) are only timed in the synced window and listed by cost.
 */
class cFrameProfiler
{
public:
    static void SetActive(bool abX);
    static inline bool IsActive()
    {
        return mbActive;
    }

    //Frames in each of the two windows.
    static void SetFramesPerWindow(int alX);
    //Number of reports to write before turning itself off, 0 = no limit.
    static void SetMaxReports(int alX);

    static void SetGpuSyncFunc(tFrameProfilerGpuSyncFunc apFunc);

    static void BeginFrame();
    static void EndFrame(bool abDiscard);

    static void Begin(const char *apName, bool abGpu);
    static void End();

    static void BeginItem(eFrameProfilerItem aType, const char *apName);
    static void EndItem();

    static void AddDrawCall(int alTriangles);

    static inline bool InSyncedWindow()
    {
        return mbActive && mlWindow == 1;
    }

private:
    static bool mbActive;
    static int mlWindow;
};

//---------------------------------------------

//Section that ends when leaving the scope.
class cFrameProfilerScope
{
public:
    inline cFrameProfilerScope(const char *apName, bool abGpu) : mbStarted(cFrameProfiler::IsActive())
    {
        if(mbStarted)
        {
            cFrameProfiler::Begin(apName, abGpu);
        }
    }
    inline ~cFrameProfilerScope()
    {
        if(mbStarted)
        {
            cFrameProfiler::End();
        }
    }
private:
    bool mbStarted;
};

//Item that ends when leaving the scope.
class cFrameProfilerItemScope
{
public:
    inline cFrameProfilerItemScope(eFrameProfilerItem aType, const tString &asName) : mbStarted(cFrameProfiler::InSyncedWindow())
    {
        if(mbStarted)
        {
            cFrameProfiler::BeginItem(aType, asName.c_str());
        }
    }
    inline ~cFrameProfilerItemScope()
    {
        if(mbStarted)
        {
            cFrameProfiler::EndItem();
        }
    }
private:
    bool mbStarted;
};

//---------------------------------------------

#define HPL_PROFILE_JOIN2(a, b) a##b
#define HPL_PROFILE_JOIN(a, b) HPL_PROFILE_JOIN2(a, b)

#define HPL_PROFILE_SCOPE(asName)       hpl::cFrameProfilerScope HPL_PROFILE_JOIN(profileScope_, __LINE__)(asName, false)
#define HPL_PROFILE_SCOPE_GPU(asName)   hpl::cFrameProfilerScope HPL_PROFILE_JOIN(profileScope_, __LINE__)(asName, true)
#define HPL_PROFILE_ITEM(aType, asName) hpl::cFrameProfilerItemScope HPL_PROFILE_JOIN(profileItem_, __LINE__)(aType, asName)

//---------------------------------------------

};
#endif // HPL_FRAME_PROFILER_H
