#ifndef LUX_PROGRESS_LOG_HANDLER_H
#define LUX_PROGRESS_LOG_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------

class cLuxProgressLogHandler : public iLuxUpdateable
{
public:
    cLuxProgressLogHandler();
    ~cLuxProgressLogHandler();

    void LoadUserConfig();
    void SaveUserConfig();

    void OnStart();
    void Update(float afTimeStep);
    void Reset();

    void CreateAndResetLogFile();

    void AddLog(eLuxProgressLogLevel aLevel, const tString& asMessage);

private:
    tString LevelToString(eLuxProgressLogLevel aLevel);

    int mlCounter;
    int mlFileNameCount;
    bool mbActive;

    FILE *mpFile;
};

//----------------------------------------------


#endif // LUX_PROGRESS_LOG_HANDLER_H
