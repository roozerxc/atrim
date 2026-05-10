#ifndef HPL_INPUTDEVICE_H
#define HPL_INPUTDEVICE_H

#include "system/SystemTypes.h"
#include "input/InputTypes.h"

namespace hpl
{

class iInputDevice
{
public:
    iInputDevice(tString asName,eInputDeviceType aType);
    virtual ~iInputDevice() {}

    /**
     *
     * \return name of the device
     */
    tString GetName();

    /**
     *
     * \return type of device.
     */
    eInputDeviceType GetType();

    /**
     * Update the device, called by cInput
     */
    virtual void Update()=0;

private:
    tString msName;
    eInputDeviceType mType;
};

};
#endif // HPL_INPUTDEVICE_H
