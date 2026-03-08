#ifndef HPL_ACTIONGAMEPADHAT_H
#define HPL_ACTIONGAMEPADHAT_H


#include "input/InputTypes.h"
#include "input/Action.h"

namespace hpl
{

class cInput;

class cActionGamepadHat : public iSubAction
{
public:
    cActionGamepadHat(cInput* apInput, int alPadIndex, eGamepadHat aHat, eGamepadHatState aHatState);

    bool IsTriggerd();
    float GetValue();

    tString GetInputName();

    tString GetInputType()
    {
        return "GamepadHat";
    }

    eGamepadHat GetHat()
    {
        return mHat;
    }
    eGamepadHatState GetHatState()
    {
        return mHatState;
    }
private:
    int mlPadIndex;
    eGamepadHat mHat;
    eGamepadHatState mHatState;
    cInput *mpInput;
};

};
#endif // HPL_ACTIONGAMEPADHAT_H
