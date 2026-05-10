#ifndef HPL_ACTIONGAMEPADBUTTON_H
#define HPL_ACTIONGAMEPADBUTTON_H


#include "input/InputTypes.h"
#include "input/Action.h"

namespace hpl
{

class cInput;

class cActionGamepadButton : public iSubAction
{
public:
    cActionGamepadButton(cInput* apInput, int alPadIndex, eGamepadButton aButton);

    bool IsTriggerd();
    float GetValue();

    tString GetInputName();

    tString GetInputType()
    {
        return "GamepadButton";
    }

    eGamepadButton GetButton()
    {
        return mButton;
    }
private:
    int mlPadIndex;
    eGamepadButton mButton;
    cInput *mpInput;
};

};
#endif // HPL_ACTIONGAMEPADBUTTON_H
