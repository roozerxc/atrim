#ifndef HPL_ACTIONGAMEPADAXIS_H
#define HPL_ACTIONGAMEPADAXIS_H

#include "input/InputTypes.h"
#include "input/Action.h"

namespace hpl
{

class cInput;

class cActionGamepadAxis : public iSubAction
{
public:
    cActionGamepadAxis(cInput* apInput, int alPadIndex, eGamepadAxis aAxis, eGamepadAxisRange aRange, float afMinThreshold, float afMaxThreshold);

    bool IsTriggerd();
    float GetValue();

    tString GetInputName();

    tString GetInputType()
    {
        return "GamepadAxis";
    }

    eGamepadAxis GetAxis()
    {
        return mAxis;
    }
    eGamepadAxisRange GetRange()
    {
        return mRange;
    }
private:
    int mlPadIndex;
    eGamepadAxis mAxis;
    eGamepadAxisRange mRange;

    float mfMinThreshold;
    float mfMaxThreshold;
    cInput *mpInput;
};

};
#endif // HPL_ACTIONGAMEPADAXIS_H
