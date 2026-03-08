#ifndef HPL_ACTIONKEYBOARD_H
#define HPL_ACTIONKEYBOARD_H


#include "input/InputTypes.h"
#include "input/Action.h"

namespace hpl
{

class cInput;

class cActionKeyboard : public iSubAction
{
public:
    cActionKeyboard(cInput* apInput, eKey mKey);

    bool IsTriggerd();
    float GetValue();

    tString GetInputName();

    tString GetInputType()
    {
        return "Keyboard";
    }

    eKey GetKey()
    {
        return mKey;
    }

private:
    eKey mKey;
    cInput *mpInput;
};

};
#endif // HPL_ACTIONKEYBOARD_H
