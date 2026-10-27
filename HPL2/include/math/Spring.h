#ifndef HPL_SPRING_H
#define HPL_SPRING_H

#include "math/MathTypes.h"

namespace hpl
{

template <class T>
class cSpring
{
public:
    float k;
    float b;

    //////////////////////////////////////////
    // Constructors
    /////////////////////////////////////////
    cSpring() {}

    cSpring(float afK, float afB)
    {
        k = afK;
        b = afB;
    }

    //////////////////////////////////////////
    // Public
    /////////////////////////////////////////

    T Output(T aError, T aV)
    {
        return aError * k - aV * b;
    }
};

//---------------------------------

typedef cSpring<float> cSpringf;
typedef cSpring<cVector3f> cSpringVec3;
};
#endif // HPL_SPRING_H
