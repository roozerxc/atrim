#ifndef HPL_SYSTEM_H
#define HPL_SYSTEM_H

namespace hpl {

    class iLowLevelSystem;
    class cLogicTimer;

    class cSystem
    {
    public:
        cSystem(iLowLevelSystem *apLowLevelSystem);
        ~cSystem();

        iLowLevelSystem* GetLowLevel();

        /**
         * Creates a logic timer.
         * \param alUpdatesPerSec Frequency of the timer.
         * \return 
         */
        cLogicTimer * CreateLogicTimer(unsigned int alUpdatesPerSec);
    
    private:
        iLowLevelSystem *mpLowLevelSystem;
    };

};
#endif // HPL_SYSTEM_H
