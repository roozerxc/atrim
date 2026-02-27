#include "input/ActionGamepadButton.h"
#include "input/Input.h"
#include "input/Gamepad.h"

#include "system/String.h"

namespace hpl
{

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    cActionGamepadButton::cActionGamepadButton(cInput *apInput, int alPadIndex, eGamepadButton aButton)
    {
        mlPadIndex = alPadIndex;
        mButton = aButton;
        mpInput = apInput;
    }

    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // PUBLIC METHODS
    //////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------

    bool cActionGamepadButton::IsTriggerd()
    {
        iGamepad* pPad = mpInput->GetGamepad(mlPadIndex);
        if(pPad)
            return pPad->ButtonIsDown(mButton);

        return false;
    }

    //-----------------------------------------------------------------------

    float cActionGamepadButton::GetValue()
    {
        if(IsTriggerd())return 1.0;
        else return 0.0;
    }

    //-----------------------------------------------------------------------
    
    tString  cActionGamepadButton::GetInputName()
    {
        return iGamepad::ButtonToString(mButton);
        /*iGamepad* pPad = mpInput->GetGamepad(mlPadIndex);
        if(pPad)
            return pPad->ButtonToString(mButton);

        return "None";
        */
    }

    //-----------------------------------------------------------------------
}
