#include "system/FrameProfiler.h"

#include "system/LowLevelSystem.h"
#include "system/Platform.h"

#include <algorithm>
#include <map>
#include <vector>
#include <string.h>
#include <stdio.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// DATA
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

namespace
{
    struct cProfSection
    {
        tString msName;
        int mlParent;
        int mlDepth;
        bool mbGpu;

        double mdStart;

        //This frame
        double mdFrameTime;
        int mlFrameCalls;
        int mlFrameDraws;
        int mlFrameTris;

        //Sum over the frames of each window (0 = plain, 1 = synced)
        double mdTime[2];
        double mdMaxTime[2];
        int mlCalls[2];
        int mlDraws[2];
        int mlTris[2];
    };

    struct cProfItem
    {
        cProfItem() : mdTime(0), mlCalls(0), mlDraws(0), mlTris(0) {}
        double mdTime;
        int mlCalls;
        int mlDraws;
        int mlTris;
    };

    typedef std::map<tString, cProfItem> tProfItemMap;
    typedef std::pair<double, const tProfItemMap::value_type *> tProfItemSort;

    std::vector<cProfSection> gvSections;
    std::vector<int> gvStack;
    tProfItemMap gmItems[eFrameProfilerItem_LastEnum];

    tFrameProfilerGpuSyncFunc gpGpuSyncFunc = NULL;

    int glFramesPerWindow = 120;
    int glMaxReports = 0;
    int glReportsDone = 0;

    bool gbInFrame = false;
    int glWindowFrames = 0;
    double gdFrameStart = 0;
    double gdFrameTimeSum[2] = {0, 0};
    double gdFrameTimeMax[2] = {0, 0};
    int glFramesCounted[2] = {0, 0};

    int glItemDepth = 0;
    cProfItem *gpCurrentItem = NULL;
    double gdItemStart = 0;

    const int kMaxItemsInReport = 12;

    float gfSpikeThresholdMs = 0;
    int glSpikesThisWindow = 0;
    const int kMaxSpikesPerWindow = 4;
    const int kMaxSectionsInSpike = 8;

    typedef std::pair<double, int> tSpikeSort;
    bool SortSpike(const tSpikeSort &aA, const tSpikeSort &aB)
    {
        return aA.first > aB.first;
    }

    //One line per slow plain frame: where its time went. The window averages hide these.
    void LogSpike(double adFrameTime, double adAvg, int alWindowFrame)
    {
        std::vector<tSpikeSort> vSorted;
        for(size_t i = 0; i < gvSections.size(); ++i)
        {
            if(gvSections[i].mdFrameTime >= 0.0005)
            {
                vSorted.push_back(tSpikeSort(gvSections[i].mdFrameTime, (int)i));
            }
        }
        std::sort(vSorted.begin(), vSorted.end(), SortSpike);

        tString sLine;
        char vBuf[64];
        for(size_t i = 0; i < vSorted.size() && i < (size_t)kMaxSectionsInSpike; ++i)
        {
            const cProfSection &section = gvSections[vSorted[i].second];
            tString sName = section.msName;
            if(section.mlParent >= 0)
            {
                sName = gvSections[section.mlParent].msName + "/" + sName;
            }
            sprintf(vBuf, " %.1f", vSorted[i].first * 1000.0);
            sLine += (i > 0 ? ", " : " ") + sName + vBuf;
        }

        Log("Frame profiler spike: %.1f ms (window avg %.1f ms, frame %d):%s\n", adFrameTime * 1000.0, adAvg * 1000.0,
            alWindowFrame, sLine.c_str());
    }

    //-----------------------------------------------------------------------

    inline void GpuSync()
    {
        if(gpGpuSyncFunc)
        {
            gpGpuSyncFunc();
        }
    }

    void ResetData()
    {
        gvSections.clear();
        gvStack.clear();
        for(int i = 0; i < eFrameProfilerItem_LastEnum; ++i)
        {
            gmItems[i].clear();
        }
        for(int i = 0; i < 2; ++i)
        {
            gdFrameTimeSum[i] = 0;
            gdFrameTimeMax[i] = 0;
            glFramesCounted[i] = 0;
        }
        glSpikesThisWindow = 0;
        glWindowFrames = 0;
        glItemDepth = 0;
        gpCurrentItem = NULL;
    }

    int FindOrAddSection(const char *apName, int alParent, bool abGpu)
    {
        for(size_t i = 0; i < gvSections.size(); ++i)
        {
            if(gvSections[i].mlParent == alParent && strcmp(gvSections[i].msName.c_str(), apName) == 0)
            {
                return (int)i;
            }
        }

        cProfSection section;
        section.msName = apName;
        section.mlParent = alParent;
        section.mlDepth = alParent >= 0 ? gvSections[alParent].mlDepth + 1 : 0;
        section.mbGpu = abGpu;
        section.mdStart = 0;
        section.mdFrameTime = 0;
        section.mlFrameCalls = 0;
        section.mlFrameDraws = 0;
        section.mlFrameTris = 0;
        for(int i = 0; i < 2; ++i)
        {
            section.mdTime[i] = 0;
            section.mdMaxTime[i] = 0;
            section.mlCalls[i] = 0;
            section.mlDraws[i] = 0;
            section.mlTris[i] = 0;
        }
        gvSections.push_back(section);

        return (int)gvSections.size() - 1;
    }

    //Prints a section and then its children, in the order they were first run.
    void PrintSection(int alIdx)
    {
        const cProfSection &section = gvSections[alIdx];
        double dFrames0 = glFramesCounted[0] > 0 ? (double)glFramesCounted[0] : 1.0;
        double dFrames1 = glFramesCounted[1] > 0 ? (double)glFramesCounted[1] : 1.0;

        double dPlain = section.mdTime[0] * 1000.0 / dFrames0;
        double dSynced = section.mdTime[1] * 1000.0 / dFrames1;

        tString sName = tString((size_t)(section.mlDepth * 2), ' ') + section.msName;

        //No snprintf in older MSVC, the value is a few ms so the buffer is plenty.
        char sGpu[64] = "       -";
        if(section.mbGpu)
        {
            sprintf(sGpu, "%8.2f", dSynced > dPlain ? dSynced - dPlain : 0.0);
        }

        Log(" %-34.34s %8.2f %8.2f %s %8.2f %7.1f %7.1f %9.0f\n",
            sName.c_str(), dPlain, dSynced, sGpu,
            section.mdMaxTime[1] * 1000.0,
            (double)section.mlCalls[1] / dFrames1,
            (double)section.mlDraws[1] / dFrames1,
            (double)section.mlTris[1] / dFrames1);

        for(size_t i = alIdx + 1; i < gvSections.size(); ++i)
        {
            if(gvSections[i].mlParent == alIdx)
            {
                PrintSection((int)i);
            }
        }
    }

    bool SortItems(const tProfItemSort &aA, const tProfItemSort &aB)
    {
        return aA.first > aB.first;
    }

    void PrintItems(eFrameProfilerItem aType, const char *apTitle)
    {
        tProfItemMap &mapItems = gmItems[aType];
        if(mapItems.empty())
        {
            return;
        }

        std::vector<tProfItemSort> vSorted;
        double dTotal = 0;
        for(tProfItemMap::iterator it = mapItems.begin(); it != mapItems.end(); ++it)
        {
            vSorted.push_back(tProfItemSort(it->second.mdTime, &(*it)));
            dTotal += it->second.mdTime;
        }
        std::sort(vSorted.begin(), vSorted.end(), SortItems);

        double dFrames = glFramesCounted[1] > 0 ? (double)glFramesCounted[1] : 1.0;

        Log(" %s: %d different, %.2f ms/frame in total. Most expensive (CPU+GPU):\n", apTitle, (int)vSorted.size(), dTotal * 1000.0 / dFrames);
        Log("   %-44s %8s %7s %7s %9s\n", "name", "ms/frame", "n/frame", "draws", "tris");
        for(size_t i = 0; i < vSorted.size() && i < (size_t)kMaxItemsInReport; ++i)
        {
            const cProfItem &item = vSorted[i].second->second;
            //n/frame is how many times it was rendered per frame, draws and tris are per time.
            double dCalls = item.mlCalls > 0 ? (double)item.mlCalls : 1.0;
            Log("   %-44.44s %8.3f %7.2f %7.1f %9.0f\n",
                vSorted[i].second->first.c_str(),
                item.mdTime * 1000.0 / dFrames,
                dCalls / dFrames,
                (double)item.mlDraws / dCalls,
                (double)item.mlTris / dCalls);
        }
    }

    void PrintReport()
    {
        double dFrames0 = glFramesCounted[0] > 0 ? (double)glFramesCounted[0] : 1.0;
        double dFrames1 = glFramesCounted[1] > 0 ? (double)glFramesCounted[1] : 1.0;
        double dPlain = gdFrameTimeSum[0] * 1000.0 / dFrames0;
        double dSynced = gdFrameTimeSum[1] * 1000.0 / dFrames1;

        Log("------------------ FRAME PROFILE ------------------\n");
        Log(" Frames: %d plain + %d synced. Times are ms per frame.\n", glFramesCounted[0], glFramesCounted[1]);
        Log(" Frame avg: %.2f ms (%.1f fps), worst %.2f ms. With GPU sync: %.2f ms, worst %.2f ms\n",
            dPlain, dPlain > 0 ? 1000.0 / dPlain : 0.0, gdFrameTimeMax[0] * 1000.0,
            dSynced, gdFrameTimeMax[1] * 1000.0);
        Log(" plain = CPU only, synced = CPU + GPU, gpu = synced - plain, worst/calls/draws/tris from the synced frames\n");
        Log(" %-34s %8s %8s %8s %8s %7s %7s %9s\n", "section", "plain", "synced", "gpu", "worst", "calls", "draws", "tris");

        for(size_t i = 0; i < gvSections.size(); ++i)
        {
            if(gvSections[i].mlParent < 0)
            {
                PrintSection((int)i);
            }
        }

        PrintItems(eFrameProfilerItem_Light, "Lights");
        PrintItems(eFrameProfilerItem_Object, "Objects");
        Log("---------------------------------------------------\n");
    }
}

//-----------------------------------------------------------------------

bool cFrameProfiler::mbActive = false;
int cFrameProfiler::mlWindow = 0;
bool cFrameProfiler::mbTrace = false;

static tWString gsTraceFile;
static FILE *gpTraceFile = NULL;
static unsigned int glTraceFrame = 0;
static bool gbTraceSync = false;

static void TraceOpen()
{
    if(gpTraceFile)
    {
        fclose(gpTraceFile);
    }
#ifdef _WIN32
    gpTraceFile = _wfopen(gsTraceFile.c_str(), L"w");
#else
    gpTraceFile = fopen(tString(gsTraceFile.begin(), gsTraceFile.end()).c_str(), "w");
#endif
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cFrameProfiler::SetActive(bool abX)
{
    if(mbActive == abX)
    {
        return;
    }

    //Takes effect at the next frame start, BeginFrame() resets everything.
    mbActive = abX;
    gbInFrame = false;
    mlWindow = 0;
    glReportsDone = 0;
    ResetData();
}

void cFrameProfiler::SetFramesPerWindow(int alX)
{
    glFramesPerWindow = alX < 1 ? 1 : alX;
}

void cFrameProfiler::SetMaxReports(int alX)
{
    glMaxReports = alX;
}

void cFrameProfiler::SetSpikeThreshold(float afMs)
{
    gfSpikeThresholdMs = afMs;
}

void cFrameProfiler::SetGpuSyncFunc(tFrameProfilerGpuSyncFunc apFunc)
{
    gpGpuSyncFunc = apFunc;
}

//-----------------------------------------------------------------------

void cFrameProfiler::SetTraceFile(const tWString &asFile)
{
    if(gpTraceFile)
    {
        fclose(gpTraceFile);
        gpTraceFile = NULL;
    }
    gsTraceFile = asFile;
    mbTrace = asFile.empty() == false;
}

//-----------------------------------------------------------------------

void cFrameProfiler::SetTraceSync(bool abX)
{
    gbTraceSync = abX;
}

//-----------------------------------------------------------------------

void cFrameProfiler::Trace(const char *apName, bool abCommit)
{
    if(mbTrace == false)
    {
        return;
    }

    //Everything before this name is done on the GPU once this returns.
    if(gbTraceSync)
    {
        GpuSync();
        abCommit = true;
    }
    if(gpTraceFile == NULL)
    {
        TraceOpen();
        if(gpTraceFile == NULL)
        {
            mbTrace = false;
            return;
        }
        fprintf(gpTraceFile, "frame %u\n", glTraceFrame);
    }

    fputs(apName, gpTraceFile);
    fputc('\n', gpTraceFile);
    fflush(gpTraceFile);
    if(abCommit)
    {
#ifdef _WIN32
        _commit(_fileno(gpTraceFile));
#else
        fsync(fileno(gpTraceFile));
#endif
    }
}

//-----------------------------------------------------------------------

void cFrameProfiler::BeginFrame()
{
    //Trace: start the file over, it only holds the frame in progress.
    if(mbTrace)
    {
        //Sync mode: the previous frame (including the buffer swap) is done before the file is restarted.
        if(gbTraceSync)
        {
            GpuSync();
        }
        ++glTraceFrame;
        TraceOpen();
        if(gpTraceFile)
        {
            fprintf(gpTraceFile, "frame %u\n", glTraceFrame);
            if(gbTraceSync)
            {
                fflush(gpTraceFile);
            }
        }
    }

    if(mbActive == false)
    {
        return;
    }

    gvStack.clear();
    glItemDepth = 0;
    gpCurrentItem = NULL;

    //Do not let work from before the frame end up in it.
    if(mlWindow == 1)
    {
        GpuSync();
    }

    gbInFrame = true;
    gdFrameStart = cPlatform::GetApplicationTimeX();
}

//-----------------------------------------------------------------------

void cFrameProfiler::EndFrame(bool abDiscard)
{
    if(mbActive == false || gbInFrame == false)
    {
        return;
    }

    //Close anything left open.
    while(gvStack.empty() == false)
    {
        End();
    }

    if(mlWindow == 1)
    {
        GpuSync();
    }
    double dFrameTime = cPlatform::GetApplicationTimeX() - gdFrameStart;
    gbInFrame = false;

    //////////////////////////////
    // Spikes (plain frames only, synced frames are slow by design)
    if(abDiscard == false && mlWindow == 0 && gfSpikeThresholdMs >= 0 && glSpikesThisWindow < kMaxSpikesPerWindow)
    {
        double dAvg = glFramesCounted[0] > 0 ? gdFrameTimeSum[0] / (double)glFramesCounted[0] : 0.0;
        double dThreshold = gfSpikeThresholdMs / 1000.0;
        if(gfSpikeThresholdMs == 0)
        {
            dThreshold = dAvg * 3.0 > 0.012 ? dAvg * 3.0 : 0.012;
        }
        //Auto needs a few frames for the average
        if(dFrameTime > dThreshold && (gfSpikeThresholdMs > 0 || glFramesCounted[0] >= 10))
        {
            LogSpike(dFrameTime, dAvg, glFramesCounted[0] + 1);
            ++glSpikesThisWindow;
        }
    }

    //////////////////////////////
    // Move the frame's values to the window (or drop them)
    for(size_t i = 0; i < gvSections.size(); ++i)
    {
        cProfSection &section = gvSections[i];
        if(abDiscard == false)
        {
            section.mdTime[mlWindow] += section.mdFrameTime;
            section.mlCalls[mlWindow] += section.mlFrameCalls;
            section.mlDraws[mlWindow] += section.mlFrameDraws;
            section.mlTris[mlWindow] += section.mlFrameTris;
            if(section.mdFrameTime > section.mdMaxTime[mlWindow])
            {
                section.mdMaxTime[mlWindow] = section.mdFrameTime;
            }
        }
        section.mdFrameTime = 0;
        section.mlFrameCalls = 0;
        section.mlFrameDraws = 0;
        section.mlFrameTris = 0;
    }

    if(abDiscard)
    {
        return;
    }

    gdFrameTimeSum[mlWindow] += dFrameTime;
    if(dFrameTime > gdFrameTimeMax[mlWindow])
    {
        gdFrameTimeMax[mlWindow] = dFrameTime;
    }
    glFramesCounted[mlWindow]++;

    //////////////////////////////
    // Next window or report
    if(++glWindowFrames < glFramesPerWindow)
    {
        return;
    }
    glWindowFrames = 0;

    if(mlWindow == 0)
    {
        mlWindow = 1;
        return;
    }

    //Draw calls are counted where they happen, add them up to the parents (children always come after).
    for(int i = (int)gvSections.size() - 1; i >= 0; --i)
    {
        int lParent = gvSections[i].mlParent;
        if(lParent < 0)
        {
            continue;
        }
        for(int j = 0; j < 2; ++j)
        {
            gvSections[lParent].mlDraws[j] += gvSections[i].mlDraws[j];
            gvSections[lParent].mlTris[j] += gvSections[i].mlTris[j];
        }
    }

    PrintReport();

    mlWindow = 0;
    ResetData();

    ++glReportsDone;
    if(glMaxReports > 0 && glReportsDone >= glMaxReports)
    {
        Log("Frame profiler: wrote %d reports, turning off.\n", glReportsDone);
        mbActive = false;
    }
}

//-----------------------------------------------------------------------

void cFrameProfiler::Begin(const char *apName, bool abGpu)
{
    if(mbActive == false || gbInFrame == false)
    {
        return;
    }

    int lParent = gvStack.empty() ? -1 : gvStack.back();
    int lIdx = FindOrAddSection(apName, lParent, abGpu);
    gvStack.push_back(lIdx);

    if(abGpu && mlWindow == 1)
    {
        GpuSync();
    }

    gvSections[lIdx].mdStart = cPlatform::GetApplicationTimeX();
}

//-----------------------------------------------------------------------

void cFrameProfiler::End()
{
    if(mbActive == false || gbInFrame == false || gvStack.empty())
    {
        return;
    }

    cProfSection &section = gvSections[gvStack.back()];
    gvStack.pop_back();

    if(section.mbGpu && mlWindow == 1)
    {
        GpuSync();
    }

    section.mdFrameTime += cPlatform::GetApplicationTimeX() - section.mdStart;
    section.mlFrameCalls++;
}

//-----------------------------------------------------------------------

void cFrameProfiler::BeginItem(eFrameProfilerItem aType, const char *apName)
{
    if(mbActive == false || gbInFrame == false || mlWindow != 1)
    {
        return;
    }

    //Only the outermost item is measured (objects drawn into a light's shadow map belong to the light).
    if(glItemDepth++ > 0)
    {
        return;
    }

    gpCurrentItem = &gmItems[aType][apName];

    GpuSync();
    gdItemStart = cPlatform::GetApplicationTimeX();
}

//-----------------------------------------------------------------------

void cFrameProfiler::EndItem()
{
    if(mbActive == false || gbInFrame == false || mlWindow != 1 || glItemDepth <= 0)
    {
        return;
    }

    if(--glItemDepth > 0)
    {
        return;
    }

    GpuSync();
    if(gpCurrentItem)
    {
        gpCurrentItem->mdTime += cPlatform::GetApplicationTimeX() - gdItemStart;
        gpCurrentItem->mlCalls++;
    }
    gpCurrentItem = NULL;
}

//-----------------------------------------------------------------------

void cFrameProfiler::AddDrawCall(int alTriangles)
{
    if(mbActive == false || gbInFrame == false)
    {
        return;
    }

    if(gvStack.empty() == false)
    {
        cProfSection &section = gvSections[gvStack.back()];
        section.mlFrameDraws++;
        section.mlFrameTris += alTriangles;
    }

    if(gpCurrentItem)
    {
        gpCurrentItem->mlDraws++;
        gpCurrentItem->mlTris += alTriangles;
    }
}

//-----------------------------------------------------------------------

}
