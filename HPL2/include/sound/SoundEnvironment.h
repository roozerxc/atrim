#ifndef HPL_SOUNDENVIRONMENT_H
#define HPL_SOUNDENVIRONMENT_H

#include "system/String.h"

namespace hpl
{
class cSoundManager;

class iSoundEnvironment
{
public:
    iSoundEnvironment ( ) {}
    virtual ~iSoundEnvironment () {}

    virtual bool CreateFromFile(const tString &asFile)
    {
        return false;
    }

    tString& GetName()
    {
        return mstrName;
    }
    tString& GetFileName()
    {
        return msFileName;
    }
    void SetFileName(const tString & asFileName)
    {
        msFileName = asFileName;
    }

protected:
    tString mstrName;
    tString msFileName;
};

}


#endif    // HPL_SOUNDENVIRONMENT_H

