#include "input/Mouse.h"

namespace hpl
{
//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iMouse::iMouse(tString asName) : iInputDevice(asName, eInputDeviceType_Mouse)
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

tString iMouse::ButtonToString(eMouseButton aButton)
{
    switch(aButton)
    {
    case eMouseButton_Left:
        return "LeftMouse";
    case eMouseButton_Middle:
        return "MiddleMouse";
    case eMouseButton_Right:
        return "RightMouse";
    case eMouseButton_WheelUp:
        return "WheelUp";
    case eMouseButton_WheelDown:
        return "WheelDown";
    case eMouseButton_Button6:
        return "Mouse6";
    case eMouseButton_Button7:
        return "Mouse7";
    case eMouseButton_Button8:
        return "Mouse8";
    case eMouseButton_Button9:
        return "Mouse9";
    }
    return "Unknown";
}

//-----------------------------------------------------------------------

eMouseButton iMouse::StringToButton(const tString& asButton)
{
    if(asButton=="LeftMouse") return eMouseButton_Left;
    if(asButton=="MiddleMouse") return eMouseButton_Middle;
    if(asButton=="RightMouse") return eMouseButton_Right;
    if(asButton=="WheelUp") return eMouseButton_WheelUp;
    if(asButton=="WheelDown") return eMouseButton_WheelDown;
    if(asButton=="Mouse6") return eMouseButton_Button6;
    if(asButton=="Mouse7") return eMouseButton_Button7;
    if(asButton=="Mouse8") return eMouseButton_Button8;
    if(asButton=="Mouse9") return eMouseButton_Button9;

    return eMouseButton_LastEnum;
}

//-----------------------------------------------------------------------

}
