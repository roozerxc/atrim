#ifndef HPL_SYSTEM_H
#define HPL_SYSTEM_H

namespace hpl
{

class iLowLevelSystem;

class cSystem
{
public:
    cSystem(iLowLevelSystem *apLowLevelSystem);
    ~cSystem();

    iLowLevelSystem* GetLowLevel();

private:
    iLowLevelSystem *mpLowLevelSystem;
};

};
#endif // HPL_SYSTEM_H
