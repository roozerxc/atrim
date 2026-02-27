#ifndef HPL_KEYBOARD_H
#define HPL_KEYBOARD_H

#include "input/InputTypes.h"
#include "input/InputDevice.h"

namespace hpl {

    //------------------------------

    class iKeyboard : public iInputDevice
    {
    public:
        iKeyboard(tString asName);
        virtual ~iKeyboard(){}
        
        /**
         * 
         * \param aKey The key to check 
         * \return true if pressed else false 
         */
        virtual bool KeyIsDown(eKey aKey)=0;
        /**
         * Can be checked many times to see all key presses
         * \return key that is currently pressed. eKey_NONE is no key. 
         */
        virtual cKeyPress GetKey()=0;
        /**
         *
         * \return If ANY key is pressed
         */
        virtual bool KeyIsPressed()=0;
        /**
         * \todo Implement!
         * \return The current modifiers.
         */
        virtual int GetModifier();
        /**
         *
         * \return key that has been released
         */
        virtual cKeyPress GetReleasedKey()=0;
        /**
         *
         * \return If ANY key is released
         */
        virtual bool KeyIsReleased()=0;
        /**
         * \param eKey The key to change to string.
         * \return The name of the key as a string. 
         */
        virtual tString KeyToString(eKey);
        /**
         * \param eKeyModifier The mod key to change to string.
         * \return The name of the mod key as a string. 
         */
        virtual tString ModifierKeyToString(eKeyModifier);
        /**
         * \param tString Name of the key
         * \return enum of the key.
         */
        virtual eKey StringToKey(const tString&);
    };

};

#endif // HPL_KEYBOARD_H
