#ifndef HPL_MUTEX_H
#define HPL_MUTEX_H


namespace hpl {

    class iMutex
    {
    public:
        iMutex();
        virtual ~iMutex(){}

        virtual bool Lock()=0;
        virtual bool Unlock()=0;
       
    protected:
    private:
    };
};
#endif // HPL_MUTEX_H
