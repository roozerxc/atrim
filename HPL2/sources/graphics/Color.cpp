#include "graphics/Color.h"

namespace hpl {

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cColor::cColor(float afR, float afG, float afB, float afA)
    {
        r = afR; g = afG; b = afB; a = afA;
    }
    
    //-----------------------------------------------------------------------

    cColor::cColor(float afR, float afG, float afB)
    {
        r = afR; g = afG; b = afB; a = 1;
    }

    //-----------------------------------------------------------------------

    cColor::cColor()
    {
        r = 0; g = 0; b = 0; a = 1;
    }
    
    //-----------------------------------------------------------------------

    cColor::cColor(float afVal)
    {
        r = afVal; g = afVal; b = afVal; a = 1;
    }
    
    //-----------------------------------------------------------------------

    cColor::cColor(float afVal, float afA)
    {
        r = afVal; g = afVal; b = afVal; a = afA;
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////
    
    //-----------------------------------------------------------------------

    void cColor::FromVec(float *apV)
    {
        r = apV[0];
        g = apV[1];
        b = apV[2];
        a = apV[3];
    }
    
    //-----------------------------------------------------------------------
    
    cColor cColor::operator*(float afVal) const
    {
        cColor col;
        col.r = r * afVal;
        col.g = g * afVal;
        col.b = b * afVal;
        col.a = a * afVal;
        return col;
    }

    cColor cColor::operator/(float afVal) const
    {
        cColor col;
        col.r = r / afVal;
        col.g = g / afVal;
        col.b = b / afVal;
        col.a = a / afVal;
        return col;
    }

    //-----------------------------------------------------------------------

    
    cColor cColor::operator+(const cColor &aCol) const
    {
        return cColor(
            r + aCol.r,
            g + aCol.g,
            b + aCol.b,
            a + aCol.a
            );
    }

    cColor cColor::operator-(const cColor &aCol) const
    {
        return cColor(
            r - aCol.r,
            g - aCol.g,
            b - aCol.b,
            a - aCol.a
            );
    }

    cColor cColor::operator*(const cColor &aCol) const
    {
        return cColor(
            r * aCol.r,
            g * aCol.g,
            b * aCol.b,
            a * aCol.a
            );
    }

    cColor cColor::operator/(const cColor &aCol) const
    {
        return cColor(
            r / aCol.r,
            g / aCol.g,
            b / aCol.b,
            a / aCol.a
            );
    }

    //-----------------------------------------------------------------------

    bool cColor::operator==(cColor aCol) const
    {
        if(r == aCol.r && g == aCol.g && b == aCol.b && a == aCol.a) 
            return true;
        else 
            return false;
    }

    //-----------------------------------------------------------------------
    
    tString cColor::ToString() const
    {
        char buf[512];
        sprintf(buf,"%f : %f : %f : %f",r,g,b,a);
        tString str = buf;
        return str;
    }


    //-----------------------------------------------------------------------

    tString cColor::ToFileString() const
    {
        char buf[512];
        sprintf(buf,"%g %g %g %g",r,g,b,a);
        tString str = buf;
        return str;
    }


    //-----------------------------------------------------------------------
}
