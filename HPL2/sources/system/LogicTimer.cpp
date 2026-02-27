#include "system/LogicTimer.h"

#include "system/LowLevelSystem.h"
#include "system/Platform.h"

namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cLogicTimer::cLogicTimer(int alUpdatesPerSec, iLowLevelSystem *apLowLevelSystem)
    {
        mlMaxUpdates = alUpdatesPerSec/10;
        mlUpdateCount =0;
        
        mpLowLevelSystem = apLowLevelSystem;

        mfSpeedMul = 1.0f;

        SetUpdatesPerSec(alUpdatesPerSec);
    }

    //-----------------------------------------------------------------------

    cLogicTimer::~cLogicTimer()
    {
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    void cLogicTimer::Reset()
    {
        mlLocalTime = (double)cPlatform::GetApplicationTime();
    }

    //-----------------------------------------------------------------------

    bool cLogicTimer::WantUpdate()
    {
        ++mlUpdateCount;
        if(mlUpdateCount > mlMaxUpdates) return false;

        if(mlLocalTime< (double)cPlatform::GetApplicationTime())
        {
            Update();
            return true;
        }
        return false;
    }
    
    //-----------------------------------------------------------------------

    void cLogicTimer::EndUpdateLoop()
    {
        if(mlUpdateCount > mlMaxUpdates){
            Reset();
        }

        mlUpdateCount=0;
    }

    //-----------------------------------------------------------------------

    void cLogicTimer::SetUpdatesPerSec(int alUpdatesPerSec)
    {
        mlLocalTimeAdd = 1000.0/((double)alUpdatesPerSec);
        Reset();
    }

    //-----------------------------------------------------------------------

    void cLogicTimer::SetMaxUpdates(int alMax)
    {
        mlMaxUpdates = alMax;
    }

    //-----------------------------------------------------------------------

    int cLogicTimer::GetUpdatesPerSec()
    {
        return (int)(1000.0 / ((double)mlLocalTimeAdd));
    }
    
    //-----------------------------------------------------------------------

    float cLogicTimer::GetStepSize()
    {
        return ((float)mlLocalTimeAdd)/1000.0f;
    }

    //-----------------------------------------------------------------------
    
    //////////////////////////////////////////////////////////////////////////
    // PRIVATE METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    void cLogicTimer::Update()
    {
        mlLocalTime += mlLocalTimeAdd/mfSpeedMul;
    }

    //-----------------------------------------------------------------------

}
