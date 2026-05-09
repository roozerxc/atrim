#include "input/ActionGamepadAxis.h"
#include "input/Input.h"
#include "input/Gamepad.h"

#include "math/Math.h"
#include "system/String.h"

namespace hpl
{
//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cActionGamepadAxis::cActionGamepadAxis(cInput* apInput, int alPadIndex, eGamepadAxis aAxis, eGamepadAxisRange aRange, float afMinThreshold, float afMaxThreshold)
{
    mlPadIndex = alPadIndex;
    mAxis = aAxis;
    mRange = aRange;
    mfMinThreshold = afMinThreshold;
    mfMaxThreshold = afMaxThreshold;

    mpInput = apInput;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cActionGamepadAxis::IsTriggerd()
{
    iGamepad* pPad = mpInput->GetGamepad(mlPadIndex);
    if(pPad)
    {
        float fVal = pPad->GetAxisValue(mAxis);
        bool bValid = (mRange==eGamepadAxisRange_Positive && fVal>0.0f) ||
                      (mRange==eGamepadAxisRange_Negative && fVal<0.0f);

        fVal = cMath::Abs(fVal);
        if(bValid && mfMinThreshold<=fVal && fVal<=mfMaxThreshold)
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------

float cActionGamepadAxis::GetValue()
{
    iGamepad* pPad = mpInput->GetGamepad(mlPadIndex);
    if(IsTriggerd()) return cMath::Abs(pPad->GetAxisValue(mAxis));
    else return 0.0;
}

//-----------------------------------------------------------------------

tString  cActionGamepadAxis::GetInputName()
{
    return iGamepad::AxisToString(mAxis) + "." + iGamepad::AxisRangeToString(mRange);

    //iGamepad* pPad = mpInput->GetGamepad(mlPadIndex);
    //if(pPad)
    //    return pPad->AxisRangeToString(mRange) + " " + pPad->AxisToString(mAxis);

    //return "None";
}

//-----------------------------------------------------------------------
}
