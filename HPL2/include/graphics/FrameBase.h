#ifndef HPL_FRAMEBASE_H
#define HPL_FRAMEBASE_H

#include "math/MathTypes.h"

namespace hpl {
    
    class iFrameBase
    {
    public:
        iFrameBase(){
            mlPicCount = 0;
        }
        ~iFrameBase() { }

        void SetPicCount(int alPicCount){mlPicCount=alPicCount;}
        void DecPicCount(){if(mlPicCount>0)mlPicCount--;}
        int GetPicCount(){return mlPicCount;}
        bool IsEmpty() {return mlPicCount<=0;}
        
    protected:
        int mlPicCount;
    };

};
#endif // HPL_FRAMEBASE_H
