#ifndef HPL_SCRIPT_MANAGER_H
#define HPL_SCRIPT_MANAGER_H

#include "resources/ResourceManager.h"

namespace hpl
{

class cSystem;
class cResources;
class iScript;

class cScriptManager : public iResourceManager
{
public:
    cScriptManager(cSystem* apSystem,cResources *apResources);
    ~cScriptManager();

    /**
     * Create a new script.
     * \param asName name of the script.
     * \return
     */
    iScript* CreateScript(const tString& asName, tString *apCompileMessages=NULL);

    void Destroy(iResourceBase* apResource);
    void Unload(iResourceBase* apResource);

private:
    cSystem* mpSystem;
    cResources *mpResources;
};

};
#endif // HPL_SCRIPT_MANAGER_H
