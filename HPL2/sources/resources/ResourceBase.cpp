#include "resources/ResourceBase.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"


namespace hpl {

    bool iResourceBase::mbLogCreateAndDelete=false;


    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    iResourceBase::iResourceBase(const tString& asName, const tWString& asFullPath,unsigned long alPrio){
        mlTime = (unsigned long)time(NULL);
        mlPrio = alPrio;
        mlHandle = 0;
        mlUserCount =0;
        msName = asName;
        mbLogDestruction = false;
        msFullPath = asFullPath;
    }

    iResourceBase::~iResourceBase()
    {
        if(mbLogDestruction && mbLogCreateAndDelete)
            Log("  Destroyed resource '%s'\n",msName.c_str());
    }
    //-----------------------------------------------------------------------

    void iResourceBase::IncUserCount()
    {
        mlUserCount++;
        mlTime = (unsigned long)time(NULL);
    }
    
    //-----------------------------------------------------------------------

    void iResourceBase::SetFullPath(const tWString& asPath)
    {
        msFullPath = asPath;
    }
    
    //-----------------------------------------------------------------------

}
