#ifndef HPL_THREAD_H
#define HPL_THREAD_H


namespace hpl {

    enum eThreadPrio
    {
        eThreadPrio_Low,
        eThreadPrio_Normal,
        eThreadPrio_High,

        eThreadPrio_LastEnum,
    };

    class iThreadClass
    {
    public:
        virtual ~iThreadClass() {}
        virtual void UpdateThread()=0;
    };

    class iThread
    {
    public:
        iThread();
        virtual ~iThread(){}

        void SetThreadClass(iThreadClass* apThreadClass) { mpThreadClass = apThreadClass; }
        void SetUpdateRate(float afUpdateRate);
        void SetSleepTime(unsigned int alSleepTime) { mlSleepTime = alSleepTime; }

        bool IsActive() { return mbThreadActive; }

        unsigned long GetSleepTime() { return mlSleepTime; }
        
        virtual void Start()=0;
        virtual void Stop()=0;
        virtual void Sleep(unsigned int alSleepTime)=0;

        virtual void SetPriority(eThreadPrio aPrio)=0;
        eThreadPrio GetPriority() { return mPrio; }

    protected:
        static int MainThreadFunc(void* apThread);
        virtual int TranslateEnginePrio(eThreadPrio aPrio)=0;

        unsigned long mlSleepTime;
        bool mbThreadActive;

    private:
        iThreadClass* mpThreadClass;
        eThreadPrio mPrio;
    };
};
#endif // HPL_THREAD_H
