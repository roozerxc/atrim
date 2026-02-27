#ifndef HPL_ACTIONMOUSEBUTTON_H
#define HPL_ACTIONMOUSEBUTTON_H


#include "input/InputTypes.h"
#include "input/Action.h"

namespace hpl {
    
    class cInput;
    
    class cActionMouseButton : public iSubAction
    {
    public:
        cActionMouseButton(cInput* apInput, eMouseButton mButton);

        bool IsTriggerd();
        float GetValue();

        tString GetInputName();

        tString GetInputType(){return "MouseButton";}
        
        eMouseButton GetButton(){ return mButton;}
    private:
        eMouseButton mButton;
        cInput *mpInput;
    };

};
#endif // HPL_ACTIONMOUSEBUTTON_H
