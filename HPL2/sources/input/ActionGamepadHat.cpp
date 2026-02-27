#include "input/ActionGamepadHat.h"
#include "input/Input.h"
#include "input/Gamepad.h"

#include "system/String.h"

namespace hpl
{
    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cActionGamepadHat::cActionGamepadHat(cInput *apInput, int alPadIndex, eGamepadHat aHat, eGamepadHatState aHatState)
    {
        mlPadIndex = alPadIndex;
        mHat = aHat;
        mHatState = aHatState;
        mpInput = apInput;
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    bool cActionGamepadHat::IsTriggerd()
    {
        iGamepad* pPad = mpInput->GetGamepad(mlPadIndex);
        if(pPad)
            return pPad->HatIsInState(mHat, mHatState);

        return false;
    }

    //-----------------------------------------------------------------------

    float cActionGamepadHat::GetValue()
    {
        if(IsTriggerd())return 1.0;
        else return 0.0;
    }

    //-----------------------------------------------------------------------
    
    tString  cActionGamepadHat::GetInputName()
    {
        return iGamepad::HatToString(mHat) + "." + iGamepad::HatStateToString(mHatState);
        /*iGamepad* pPad = mpInput->GetGamepad(mlPadIndex);
        if(pPad)
            return pPad->HatToString(mHat) + "-" + pPad->HatStateToString(mHatState);

        return "None";
        */
    }

    //-----------------------------------------------------------------------
}
