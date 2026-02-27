#ifndef HPL_LOGICTIMER_H
#define HPL_LOGICTIMER_H


namespace hpl {

    class iLowLevelSystem;

    class cLogicTimer
    {
    public:
        /**
         *
         * \param alUpdatesPerSec Number of updates per second.
         * \param *apLowLevelSystem 
         * \return 
         */
        cLogicTimer(int alUpdatesPerSec,iLowLevelSystem *apLowLevelSystem);
        ~cLogicTimer();

        /**
         * Reset the time. Do this when the logical update of the game has been ide, ie while loading.
         */
        void Reset();
        /**
         * Check with the time if it is time for another logical update
         * \return true if logic should be updated, else false.
         */
        bool WantUpdate();

        /**
         * Resets various variables that makes the graphics is never frozen.
         */
        void EndUpdateLoop();

        /**
         * Set the number of times per second to update
         * \param alUpdatesPerSec 
         */
        void SetUpdatesPerSec(int alUpdatesPerSec);

        /**
         * Sets the maximum updates in a row.
         * \param alUpdatesPerSec 
         */
        void SetMaxUpdates(int alMax);

        /**
         * Get the number of updates per second.
         */
        int GetUpdatesPerSec();
        /**
         * Get the size of each step in seconds.
         */
        float GetStepSize();

        double GetLocalTime(){ return mlLocalTime;}
        double GetLocalTimeAdd(){ return mlLocalTimeAdd;}

        void SetSpeedMul(float afX){ mfSpeedMul = afX;}
        
    private:
        void Update();

        double mlLocalTime;
        double mlLocalTimeAdd;

        double mfSpeedMul;

        int mlMaxUpdates;
        int mlUpdateCount;

        iLowLevelSystem *mpLowLevelSystem;
    };

};
#endif // HPL_LOGICTIMER_H
