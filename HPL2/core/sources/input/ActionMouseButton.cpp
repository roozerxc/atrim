#include "input/ActionMouseButton.h"
#include "input/Input.h"
#include "input/Mouse.h"

namespace hpl
{
//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cActionMouseButton::cActionMouseButton(cInput *apInput, eMouseButton aButton)
{
    mButton = aButton;
    mpInput = apInput;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cActionMouseButton::IsTriggerd()
{
    return mpInput->GetMouse()->ButtonIsDown(mButton);
}

//-----------------------------------------------------------------------

float cActionMouseButton::GetValue()
{
    if(IsTriggerd())return 1.0;
    else return 0.0;
}

//-----------------------------------------------------------------------

tString  cActionMouseButton::GetInputName()
{
    return mpInput->GetMouse()->ButtonToString(mButton);
}

//-----------------------------------------------------------------------
}
