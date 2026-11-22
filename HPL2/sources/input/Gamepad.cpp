#if USE_GAMEPAD
#include "input/Gamepad.h"
#include "input/InputTypes.h"

#include "system/Platform.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"

#include <map>
#include <stdio.h>
#include <stdlib.h>

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

tString iGamepad::TranslateSDL2InputString(const tString& asInput)
{
    ///////////////////////////
    // Split on '.', skipping empty parts like cString::GetStringVec does
    // (vanilla's own default_user_keys.cfg has "GamepadAxis..Axis 0.Positive")
    std::vector<tString> vParts;
    tString sPart;
    for(size_t i = 0; i <= asInput.size(); ++i)
    {
        if(i == asInput.size() || asInput[i] == '.')
        {
            if(sPart.empty() == false)
            {
                vParts.push_back(sPart);
            }
            sPart = "";
        }
        else
        {
            sPart += asInput[i];
        }
    }

    if(vParts.size() < 2)
    {
        return asInput;
    }

    ///////////////////////////
    // Buttons
    if(vParts[0] == "GamepadButton")
    {
        static const char *vNames[][2] =
        {
            { "Button A", "GamepadButton.Button 0" },
            { "Button B", "GamepadButton.Button 1" },
            { "Button X", "GamepadButton.Button 2" },
            { "Button Y", "GamepadButton.Button 3" },
            { "Button LeftShoulder", "GamepadButton.Button 4" },
            { "Button RightShoulder", "GamepadButton.Button 5" },
            { "Button Back", "GamepadButton.Button 6" },
            { "Button Start", "GamepadButton.Button 7" },
            { "Button LeftStick", "GamepadButton.Button 8" },
            { "Button RightStick", "GamepadButton.Button 9" },
            { "Button DpadUp", "GamepadHat.Hat 0.Up" },
            { "Button DpadDown", "GamepadHat.Hat 0.Down" },
            { "Button DpadLeft", "GamepadHat.Hat 0.Left" },
            { "Button DpadRight", "GamepadHat.Hat 0.Right" },
            { "Button Guide", "" },
            { NULL, NULL }
        };
        for(int i = 0; vNames[i][0] != NULL; ++i)
        {
            if(vParts[1] == vNames[i][0])
            {
                return vNames[i][1];
            }
        }
        return asInput;
    }

    ///////////////////////////
    // Axes
    if(vParts[0] == "GamepadAxis" && vParts.size() >= 3)
    {
        const tString& sAxis = vParts[1];
        const tString& sRange = vParts[2];

        if(sAxis == "Axis LeftX")
        {
            return "GamepadAxis.Axis 0." + sRange;
        }
        if(sAxis == "Axis LeftY")
        {
            return "GamepadAxis.Axis 1." + sRange;
        }
        if(sAxis == "Axis RightY")
        {
            return "GamepadAxis.Axis 3." + sRange;
        }
        if(sAxis == "Axis RightX")
        {
            return "GamepadAxis.Axis 4." + sRange;
        }
        // Both triggers share axis 2: LT pushes it positive, RT negative
        if(sAxis == "Axis LeftTrigger")
        {
            return "GamepadAxis.Axis 2.Positive";
        }
        if(sAxis == "Axis RightTrigger")
        {
            return "GamepadAxis.Axis 2.Negative";
        }
    }

    return asInput;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONTROLLER DATABASE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

struct cControllerDBEntry
{
    tString msName;
    tString msMapping;
};

typedef std::map<unsigned int, cControllerDBEntry> tControllerDBMap;

static tControllerDBMap& GetControllerDB()
{
    static tControllerDBMap gmapDB;
    return gmapDB;
}

//-----------------------------------------------------------------------

static int HexDigit(char c)
{
    if(c >= '0' && c <= '9') return c - '0';
    if(c >= 'a' && c <= 'f') return c - 'a' + 10;
    if(c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// Little endian 16 bit value from 4 hex characters
static int HexLE16(const tString& asGUID, size_t alPos)
{
    int lVal = 0;
    for(int i = 0; i < 4; ++i)
    {
        int lDigit = HexDigit(asGUID[alPos + i]);
        if(lDigit < 0)
        {
            return -1;
        }
        lVal = lVal * 16 + lDigit;
    }
    // "6d04" -> 0x6d04 -> swap bytes -> 0x046d
    return ((lVal & 0xff) << 8) | ((lVal >> 8) & 0xff);
}

static bool HexIsZero(const tString& asGUID, size_t alPos, size_t alCount)
{
    for(size_t i = 0; i < alCount; ++i)
    {
        if(asGUID[alPos + i] != '0')
        {
            return false;
        }
    }
    return true;
}

// SDL2 joystick GUIDs carry the USB IDs in one of two layouts:
//   SDL 2.0.5+:  BBBB CCCC VVVV 0000 PPPP 0000 vvvv 0000  (bus, CRC, vendor, product, version)
//   older Windows DirectInput:  VVVV PPPP 0000 0000 0000 "PIDVID" in hex
static bool ParseControllerGUID(const tString& asGUID, unsigned int& alVendor, unsigned int& alProduct)
{
    if(asGUID.size() != 32)
    {
        return false;
    }

    tString sLower = cString::ToLowerCase(asGUID);
    int lVendor = -1, lProduct = -1;

    if(sLower.compare(20, 12, "504944564944") == 0)
    {
        lVendor = HexLE16(sLower, 0);
        lProduct = HexLE16(sLower, 4);
    }
    else if((sLower.compare(0, 4, "0300") == 0 || sLower.compare(0, 4, "0500") == 0) &&
            HexIsZero(sLower, 12, 4) && HexIsZero(sLower, 20, 4) &&
            HexIsZero(sLower, 28, 4)) // driver signature: 0 is DirectInput, 'h' HIDAPI, 'x' XInput, ...
    {
        lVendor = HexLE16(sLower, 8);
        lProduct = HexLE16(sLower, 16);
    }

    if(lVendor <= 0 || lProduct < 0)
    {
        return false;
    }

    alVendor = (unsigned int)lVendor;
    alProduct = (unsigned int)lProduct;
    return true;
}

//-----------------------------------------------------------------------

int iGamepad::AddControllerDBLines(const tString& asText)
{
    tControllerDBMap& mapDB = GetControllerDB();
    int lAdded = 0;

    size_t lStart = 0;
    while(lStart < asText.size())
    {
        size_t lEnd = asText.find('\n', lStart);
        if(lEnd == tString::npos)
        {
            lEnd = asText.size();
        }
        tString sLine = asText.substr(lStart, lEnd - lStart);
        lStart = lEnd + 1;

        // Trim
        size_t lFirst = sLine.find_first_not_of(" \t\r");
        if(lFirst == tString::npos || sLine[lFirst] == '#')
        {
            continue;
        }
        size_t lLast = sLine.find_last_not_of(" \t\r");
        sLine = sLine.substr(lFirst, lLast - lFirst + 1);

        // Only DirectInput (Windows) layouts apply to winmm. Lines without a
        // platform are taken too, in case someone writes their own.
        size_t lPlatform = sLine.find("platform:");
        if(lPlatform != tString::npos && sLine.compare(lPlatform, 16, "platform:Windows") != 0)
        {
            continue;
        }

        size_t lComma1 = sLine.find(',');
        if(lComma1 == tString::npos)
        {
            continue;
        }
        size_t lComma2 = sLine.find(',', lComma1 + 1);
        if(lComma2 == tString::npos)
        {
            continue;
        }

        unsigned int lVendor = 0, lProduct = 0;
        if(ParseControllerGUID(sLine.substr(0, lComma1), lVendor, lProduct) == false)
        {
            continue;
        }

        unsigned int lKey = (lVendor << 16) | lProduct;
        if(mapDB.find(lKey) != mapDB.end())
        {
            continue;
        }

        cControllerDBEntry entry;
        entry.msName = sLine.substr(lComma1 + 1, lComma2 - lComma1 - 1);
        entry.msMapping = sLine.substr(lComma2 + 1);
        mapDB[lKey] = entry;
        ++lAdded;
    }

    return lAdded;
}

//-----------------------------------------------------------------------

int iGamepad::LoadControllerDBFile(const tWString& asFile)
{
    FILE *pFile = cPlatform::OpenFile(asFile, _W("rb"));
    if(pFile == NULL)
    {
        return -1;
    }

    tString sText;
    char vBuffer[4096];
    size_t lRead;
    while((lRead = fread(vBuffer, 1, sizeof(vBuffer), pFile)) > 0)
    {
        sText.append(vBuffer, lRead);
    }
    fclose(pFile);

    int lAdded = AddControllerDBLines(sText);
    Log("Loaded %d gamepad layouts from '%s'\n", lAdded, cString::To8Char(asFile).c_str());
    return lAdded;
}

//-----------------------------------------------------------------------

bool iGamepad::FindControllerDBEntry(unsigned int alVendor, unsigned int alProduct, tString& asName, tString& asMapping)
{
    tControllerDBMap& mapDB = GetControllerDB();
    tControllerDBMap::iterator it = mapDB.find((alVendor << 16) | alProduct);
    if(it == mapDB.end())
    {
        return false;
    }

    asName = it->second.msName;
    asMapping = it->second.msMapping;
    return true;
}

//-----------------------------------------------------------------------

tString iGamepad::msUserMappings = "";
bool iGamepad::mbLogRawInput = false;

//-----------------------------------------------------------------------

iGamepad::iGamepad(tString asName, int alIndex) : iInputDevice(asName, eInputDeviceType_Gamepad)
{
    mlIndex = alIndex;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

eGamepadAxisRange iGamepad::GetAxisRangeFromValue(float afX)
{
    eGamepadAxisRange range = eGamepadAxisRange_Dead;
    if(afX > 0.0f)
    {
        range = eGamepadAxisRange_Positive;
    }
    else if(afX < 0.0f)
    {
        range = eGamepadAxisRange_Negative;
    }

    return range;
}

//-----------------------------------------------------------------------

tString iGamepad::ButtonToString(eGamepadButton aButton)
{
    switch(aButton)
    {
    case eGamepadButton_0:
        return "Button 0";
    case eGamepadButton_1:
        return "Button 1";
    case eGamepadButton_2:
        return "Button 2";
    case eGamepadButton_3:
        return "Button 3";
    case eGamepadButton_4:
        return "Button 4";
    case eGamepadButton_5:
        return "Button 5";
    case eGamepadButton_6:
        return "Button 6";
    case eGamepadButton_7:
        return "Button 7";
    case eGamepadButton_8:
        return "Button 8";
    case eGamepadButton_9:
        return "Button 9";
    case eGamepadButton_10:
        return "Button 10";
    case eGamepadButton_11:
        return "Button 11";
    case eGamepadButton_12:
        return "Button 12";
    case eGamepadButton_13:
        return "Button 13";
    case eGamepadButton_14:
        return "Button 14";
    case eGamepadButton_15:
        return "Button 15";
    case eGamepadButton_16:
        return "Button 16";
    case eGamepadButton_17:
        return "Button 17";
    case eGamepadButton_18:
        return "Button 18";
    case eGamepadButton_19:
        return "Button 19";
    case eGamepadButton_20:
        return "Button 20";
    case eGamepadButton_21:
        return "Button 21";
    case eGamepadButton_22:
        return "Button 22";
    case eGamepadButton_23:
        return "Button 23";
    case eGamepadButton_24:
        return "Button 24";
    case eGamepadButton_25:
        return "Button 25";
    case eGamepadButton_26:
        return "Button 26";
    case eGamepadButton_27:
        return "Button 27";
    case eGamepadButton_28:
        return "Button 28";
    case eGamepadButton_29:
        return "Button 29";
    }

    return "Unknown";
}

//-----------------------------------------------------------------------

eGamepadButton iGamepad::StringToButton(const tString& asButton)
{
    if(asButton == "Button 0")
    {
        return eGamepadButton_0;
    }
    if(asButton == "Button 1")
    {
        return eGamepadButton_1;
    }
    if(asButton == "Button 2")
    {
        return eGamepadButton_2;
    }
    if(asButton == "Button 3")
    {
        return eGamepadButton_3;
    }
    if(asButton == "Button 4")
    {
        return eGamepadButton_4;
    }
    if(asButton == "Button 5")
    {
        return eGamepadButton_5;
    }
    if(asButton == "Button 6")
    {
        return eGamepadButton_6;
    }
    if(asButton == "Button 7")
    {
        return eGamepadButton_7;
    }
    if(asButton == "Button 8")
    {
        return eGamepadButton_8;
    }
    if(asButton == "Button 9")
    {
        return eGamepadButton_9;
    }
    if(asButton == "Button 10")
    {
        return eGamepadButton_10;
    }
    if(asButton == "Button 11")
    {
        return eGamepadButton_11;
    }
    if(asButton == "Button 12")
    {
        return eGamepadButton_12;
    }
    if(asButton == "Button 13")
    {
        return eGamepadButton_13;
    }
    if(asButton == "Button 14")
    {
        return eGamepadButton_14;
    }
    if(asButton == "Button 15")
    {
        return eGamepadButton_15;
    }
    if(asButton == "Button 16")
    {
        return eGamepadButton_16;
    }
    if(asButton == "Button 17")
    {
        return eGamepadButton_17;
    }
    if(asButton == "Button 18")
    {
        return eGamepadButton_18;
    }
    if(asButton == "Button 19")
    {
        return eGamepadButton_19;
    }
    if(asButton == "Button 20")
    {
        return eGamepadButton_20;
    }
    if(asButton == "Button 21")
    {
        return eGamepadButton_21;
    }
    if(asButton == "Button 22")
    {
        return eGamepadButton_22;
    }
    if(asButton == "Button 23")
    {
        return eGamepadButton_23;
    }
    if(asButton == "Button 24")
    {
        return eGamepadButton_24;
    }
    if(asButton == "Button 25")
    {
        return eGamepadButton_25;
    }
    if(asButton == "Button 26")
    {
        return eGamepadButton_26;
    }
    if(asButton == "Button 27")
    {
        return eGamepadButton_27;
    }
    if(asButton == "Button 28")
    {
        return eGamepadButton_28;
    }
    if(asButton == "Button 29")
    {
        return eGamepadButton_29;
    }

    return eGamepadButton_LastEnum;
}

//-----------------------------------------------------------------------

tString iGamepad::AxisToString(eGamepadAxis aAxis)
{
    switch(aAxis)
    {
    case eGamepadAxis_0:
        return "Axis 0";
    case eGamepadAxis_1:
        return "Axis 1";
    case eGamepadAxis_2:
        return "Axis 2";
    case eGamepadAxis_3:
        return "Axis 3";
    case eGamepadAxis_4:
        return "Axis 4";
    case eGamepadAxis_5:
        return "Axis 5";
    case eGamepadAxis_7:
        return "Axis 7";
    case eGamepadAxis_8:
        return "Axis 8";
    case eGamepadAxis_9:
        return "Axis 9";
    }
    return "None";
}

//-----------------------------------------------------------------------

eGamepadAxis iGamepad::StringToAxis(const tString& asAxis)
{
    if(asAxis == "Axis 0")
    {
        return eGamepadAxis_0;
    }
    if(asAxis == "Axis 1")
    {
        return eGamepadAxis_1;
    }
    if(asAxis == "Axis 2")
    {
        return eGamepadAxis_2;
    }
    if(asAxis == "Axis 3")
    {
        return eGamepadAxis_3;
    }
    if(asAxis == "Axis 4")
    {
        return eGamepadAxis_4;
    }
    if(asAxis == "Axis 5")
    {
        return eGamepadAxis_5;
    }
    if(asAxis == "Axis 6")
    {
        return eGamepadAxis_6;
    }
    if(asAxis == "Axis 7")
    {
        return eGamepadAxis_7;
    }
    if(asAxis == "Axis 8")
    {
        return eGamepadAxis_8;
    }
    if(asAxis == "Axis 9")
    {
        return eGamepadAxis_9;
    }
    return eGamepadAxis_LastEnum;
}

//-----------------------------------------------------------------------

tString iGamepad::AxisRangeToString(eGamepadAxisRange aRange)
{
    switch(aRange)
    {
    case eGamepadAxisRange_Positive:
        return "Positive";
    case eGamepadAxisRange_Negative:
        return "Negative";
    }

    return "None";
}

//-----------------------------------------------------------------------

eGamepadAxisRange iGamepad::StringToAxisRange(const tString& asRange)
{
    if(asRange == "Positive")
    {
        return eGamepadAxisRange_Positive;
    }
    if(asRange == "Negative")
    {
        return eGamepadAxisRange_Negative;
    }

    return eGamepadAxisRange_LastEnum;
}

//-----------------------------------------------------------------------

tString iGamepad::HatToString(eGamepadHat aHat)
{
    switch(aHat)
    {
    case eGamepadHat_0:
        return "Hat 0";
    case eGamepadHat_1:
        return "Hat 1";
    case eGamepadHat_2:
        return "Hat 2";
    case eGamepadHat_3:
        return "Hat 3";
    case eGamepadHat_4:
        return "Hat 4";
    case eGamepadHat_5:
        return "Hat 5";
    case eGamepadHat_6:
        return "Hat 6";
    case eGamepadHat_7:
        return "Hat 7";
    case eGamepadHat_8:
        return "Hat 8";
    case eGamepadHat_9:
        return "Hat 9";
    }

    return "None";
}

//-----------------------------------------------------------------------

eGamepadHat iGamepad::StringToHat(const tString& asHat)
{
    if(asHat == "Hat 0")
    {
        return eGamepadHat_0;
    }
    if(asHat == "Hat 1")
    {
        return eGamepadHat_1;
    }
    if(asHat == "Hat 2")
    {
        return eGamepadHat_2;
    }
    if(asHat == "Hat 3")
    {
        return eGamepadHat_3;
    }
    if(asHat == "Hat 4")
    {
        return eGamepadHat_4;
    }
    if(asHat == "Hat 5")
    {
        return eGamepadHat_5;
    }
    if(asHat == "Hat 6")
    {
        return eGamepadHat_6;
    }
    if(asHat == "Hat 7")
    {
        return eGamepadHat_7;
    }
    if(asHat == "Hat 8")
    {
        return eGamepadHat_8;
    }
    if(asHat == "Hat 9")
    {
        return eGamepadHat_9;
    }

    return eGamepadHat_LastEnum;
}

//-----------------------------------------------------------------------

tString iGamepad::HatStateToString(eGamepadHatState aHatState)
{
    switch(aHatState)
    {
    case eGamepadHatState_Centered:
        return "Centered";
    case eGamepadHatState_Up:
        return "Up";
    case eGamepadHatState_Right:
        return "Right";
    case eGamepadHatState_Down:
        return "Down";
    case eGamepadHatState_Left:
        return "Left";
    case eGamepadHatState_RightUp:
        return "RightUp";
    case eGamepadHatState_RightDown:
        return "RightDown";
    case eGamepadHatState_LeftUp:
        return "LeftUp";
    case eGamepadHatState_LeftDown:
        return "LeftDown";
    }

    return "None";
}

//-----------------------------------------------------------------------

eGamepadHatState iGamepad::StringToHatState(const tString& asHatState)
{
    if(asHatState == "Centered")
    {
        return eGamepadHatState_Centered;
    }
    if(asHatState == "Up")
    {
        return eGamepadHatState_Up;
    }
    if(asHatState == "Right")
    {
        return eGamepadHatState_Right;
    }
    if(asHatState == "Down")
    {
        return eGamepadHatState_Down;
    }
    if(asHatState == "Left")
    {
        return eGamepadHatState_Left;
    }
    if(asHatState == "RightUp")
    {
        return eGamepadHatState_RightUp;
    }
    if(asHatState == "RightDown")
    {
        return eGamepadHatState_RightDown;
    }
    if(asHatState == "LeftUp")
    {
        return eGamepadHatState_LeftUp;
    }
    if(asHatState == "LeftDown")
    {
        return eGamepadHatState_LeftDown;
    }

    return eGamepadHatState_LastEnum;
}

//-----------------------------------------------------------------------

tString iGamepad::BallToString(eGamepadBall aBall)
{
    switch(aBall)
    {
    case eGamepadBall_0:
        return "Trackball 0";
    }

    return "None";
}

//-----------------------------------------------------------------------

eGamepadBall iGamepad::StringToBall(const tString& asBall)
{
    if(asBall == "Trackball 0")
    {
        return eGamepadBall_0;
    }

    return eGamepadBall_LastEnum;
}

//-----------------------------------------------------------------------
}
#endif
