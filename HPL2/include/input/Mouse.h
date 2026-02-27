#ifndef HPL_MOUSE_H
#define HPL_MOUSE_H

#include "input/InputTypes.h"
#include "input/InputDevice.h"
#include "math/MathTypes.h"

namespace hpl {

    class iMouse : public iInputDevice
    {
    public:
        iMouse(tString asName);
        virtual ~iMouse(){}

        /**
         * Check if a mouse button is down
         * \param eMouseButton the button to check
         * \return 
         */
        virtual bool ButtonIsDown(eMouseButton)=0;
        /**
         * Get the absolute pos of the mouse.
         * \return 
         */
        virtual cVector2l GetAbsPosition()=0;
        /**
         * Get the relative movement.
         * \return 
         */
        virtual cVector2l GetRelPosition()=0;
        /**
         * \param eMouseButton The button to change to string.
         * \return The name of the button as a string. 
         */
        virtual tString ButtonToString(eMouseButton);
        /**
         * \param tString Name of the button
         * \return enum of the button.
         */
        virtual eMouseButton StringToButton(const tString&);
    };

};

#endif // HPL_MOUSE_H
