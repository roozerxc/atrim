#include "LuxBase.h"

//---------------------------------------
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

//---------------------------------------

cLuxBase* gpBase = NULL;

//---------------------------------------

int hplMain(const tString &asCommandline)
{
    //////////////////////////
    // Game creation and exit
    gpBase = hplNew( cLuxBase, ());

    //Init and run if all okay
    if(gpBase->Init(asCommandline))
    {
        gpBase->Run();
        gpBase->Exit();
    }
    //Error occurred
    else
    {
        if(gpBase->msErrorMessage==_W(""))
            gpBase->msErrorMessage = _W("Error occured");

        cPlatform::CreateMessageBox(_W("Error!"),gpBase->msErrorMessage.c_str());
        //No Exit, since it was not sure everything was created as it should.
    }

    hplDelete(gpBase);

    cMemoryManager::LogResults();

    return 0;
}
