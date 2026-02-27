#ifndef HPL_ACTION_HAPTIC_H
#define HPL_ACTION_HAPTIC_H

#include "input/InputTypes.h"
#include "input/Action.h"
#include "haptic/Haptic.h"

namespace hpl {
    
    class cHaptic;
    
    class cActionHaptic : public iSubAction
    {
    public:
        cActionHaptic(cHaptic *apHaptic, int mlButton);

        bool IsTriggerd();
        float GetValue();

        tString GetInputName();

        tString GetInputType(){return "HapticDeviceButton";}
        
        int GetButton(){ return mlButton;}
    private:
        int mlButton;
        cHaptic *mpHaptic;
    };

};
#endif // HPL_ACTIONMOUSEBUTTON_H
