#ifndef HPL_TIMER_H
#define HPL_TIMER_H


namespace hpl {

    class iTimer
    {
    public:
        virtual ~iTimer(){}
        
        virtual void Start()=0;
        virtual void Stop()=0;

        virtual double GetTimeInMicroSec()=0;     
        double GetTimeInMilliSec(){ return  GetTimeInMicroSec()* 0.001;}
        double GetTimeInSec(){ return GetTimeInMicroSec()* 0.000001;}
    };

};
#endif // HPL_TIMER_H
