#ifndef HPL_LOWLEVELINPUT_H
#define HPL_LOWLEVELINPUT_H

namespace hpl
{

class iMouse;
class iKeyboard;
class iGamepad;

class iLowLevelInput
{
public:
    virtual ~iLowLevelInput() {}

    /**
     * Lock all input to current window.
     * \param abX
     * \return
     */
    virtual void LockInput(bool abX)=0;
    /**
     * Use relative mouse mode
     * \param abX
     * \return
     */
    virtual void RelativeMouse(bool abX)=0;
    /**
     * Called by cInput
     */
    virtual void BeginInputUpdate()=0;
    /**
     * called by cInput
     */
    virtual void EndInputUpdate()=0;

    virtual bool isQuitMessagePosted()=0;
    virtual void resetQuitMessagePosted()=0;

    /**
     *
     */
    virtual void InitGamepadSupport()=0;
    virtual void DropGamepadSupport()=0;

    virtual int GetPluggedGamepadNum()=0;

    virtual iMouse* CreateMouse()=0;
    virtual iKeyboard* CreateKeyboard()=0;
    virtual iGamepad* CreateGamepad(int alIndex)=0;
};
};
#endif // HPL_LOWLEVELINPUT_H
