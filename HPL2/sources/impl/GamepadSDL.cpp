#if USE_GAMEPAD
#include "impl/GamepadSDL.h"

#include "impl/LowLevelInputSDL.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "math/Math.h"
#include <limits>
#include <stdlib.h>

#ifdef _WIN32
#include <conio.h>
#endif

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// BUILT-IN LAYOUTS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

// Mappings use the SDL GameControllerDB syntax, with indices as SDL 1.2's
// winmm driver reports them (axes: X, Y, Z, R, U, V in that order, skipping
// the ones a pad does not have). For pads with X/Y/Z/Rz this is the same as
// the DirectInput indices in GameControllerDB "platform:Windows" lines.
//
// Users can override or add to these with GamepadMappings in the Input
// section of main_settings.cfg, see iGamepad::SetUserMappings().

// "Logitech style": buttons 1-4 = left, bottom, right, top face button,
// 5/6 = LB/RB, 7/8 = LT/RT, 9/10 = Back/Start, 11/12 = stick clicks,
// right stick on Z/Rz. Also used for unknown non-Xbox pads.
static const char *gsMapping_Logitech =
    "a:b1,b:b2,x:b0,y:b3,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,"
    "back:b8,start:b9,leftstick:b10,rightstick:b11,"
    "leftx:a0,lefty:a1,rightx:a2,righty:a3,"
    "dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8";

// "PlayStation adapter style": 1 Triangle, 2 Circle, 3 Cross, 4 Square,
// 5 L2, 6 R2, 7 L1, 8 R1, 9 Select, 10 Start, 11 L3, 12 R3,
// right stick X on Rz and Y on Z.
static const char *gsMapping_PlayStation =
    "a:b2,b:b1,x:b3,y:b0,leftshoulder:b6,rightshoulder:b7,lefttrigger:b4,righttrigger:b5,"
    "back:b8,start:b9,leftstick:b10,rightstick:b11,"
    "leftx:a0,lefty:a1,rightx:a3,righty:a2,"
    "dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8";

struct cGamepadSDLBuiltInMapping
{
    Uint16 mlVendor;
    Uint16 mlProduct;
    const char *msName;
    const char *msMapping;
};

static const cGamepadSDLBuiltInMapping gvBuiltInMappings[] =
{
    { 0x046D, 0xC216, "Logitech Dual Action", gsMapping_Logitech },
    { 0x0810, 0x0001, "Twin USB PlayStation adapter", gsMapping_PlayStation },
    { 0x0E8F, 0x0003, "PS3 style USB pad", gsMapping_PlayStation },

    { 0, 0, NULL, NULL }
};

static const Uint16 kVendor_Microsoft = 0x045E;

//-----------------------------------------------------------------------

static bool NameLooksLikeXbox(const tString& asName)
{
    tString sName = cString::ToLowerCase(asName);

    // XInput pads seen through DirectInput (Microsoft's driver) are named
    // "Controller (...)", e.g. "Controller (Gamepad F310)", and use the
    // Xbox 360 layout whoever made them.
    if(sName.compare(0, 12, "controller (") == 0)
    {
        return true;
    }

    return sName.find("xbox") != tString::npos ||
           sName.find("x-box") != tString::npos ||
           sName.find("xinput") != tString::npos ||
           sName.find("360") != tString::npos;
}

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

float cGamepadSDL::mfInvAxisMax = 1.0f / (float)cMath::Min(cMath::Abs(std::numeric_limits<Sint16>::min()), std::numeric_limits<Sint16>::max());
float cGamepadSDL::mfDeadZoneRadius = 3200.0f * cGamepadSDL::mfInvAxisMax;

//-----------------------------------------------------------------------

cGamepadSDL::cGamepadSDL(cLowLevelInputSDL *apLowLevelInputSDL, int alIndex) : iGamepad("SDL Portable Gamepad", alIndex)
{
    mpLowLevelInputSDL = apLowLevelInputSDL;
    mlVendorId = 0;
    mlProductId = 0;

    mpHandle = SDL_JoystickOpen(mlIndex);

    if(mpHandle)
    {
        const char *pName = SDL_JoystickName(mlIndex);
        msGamepadName = tString(pName ? pName : "");
        if(msGamepadName.empty())
        {
            msGamepadName = "Gamepad";
        }

#if defined(_WIN32) && !defined(_WIN32_WCE)
        SDL_JoystickGetUSBIDs(mlIndex, &mlVendorId, &mlProductId);
#endif

        SetUpMapping();

        int lNumButtons = mMapping.mbIdentity ? SDL_JoystickNumButtons(mpHandle) : kGamepadSDLNumButtons;
        int lNumAxes = mMapping.mbIdentity ? SDL_JoystickNumAxes(mpHandle) : 5;
        int lNumHats = mMapping.mbIdentity ? SDL_JoystickNumHats(mpHandle) : 1;

        mvButtonArray.resize(lNumButtons);
        mvButtonArray.assign(mvButtonArray.size(), false);

        mvAxisArray.resize(lNumAxes);
        mvAxisArray.assign(mvAxisArray.size(), 0.0f);

        mvHatArray.resize(lNumHats);
        mvHatArray.assign(mvHatArray.size(), eGamepadHatState_Centered);

        mvBallAbsPosArray.resize(SDL_JoystickNumBalls(mpHandle));
        mvBallAbsPosArray.assign(mvBallAbsPosArray.size(), cVector2l(0, 0));
        mvBallRelPosArray.resize(SDL_JoystickNumBalls(mpHandle));
        mvBallRelPosArray.assign(mvBallRelPosArray.size(), cVector2l(0, 0));

        mvRawButtonLog.assign(SDL_JoystickNumButtons(mpHandle), false);
        mvRawAxisLog.assign(SDL_JoystickNumAxes(mpHandle), 0);
        mvRawHatLog.assign(SDL_JoystickNumHats(mpHandle), (Uint8)SDL_HAT_CENTERED);

        Log("Gamepad %d: '%s' (VID %04X, PID %04X), %d buttons, %d axes, %d hats, layout: %s\n",
            mlIndex, msGamepadName.c_str(), mlVendorId, mlProductId,
            SDL_JoystickNumButtons(mpHandle), SDL_JoystickNumAxes(mpHandle), SDL_JoystickNumHats(mpHandle),
            mMapping.msName.c_str());
    }
    else
    {
        // An empty name makes cInput drop the pad
        msGamepadName = "";
    }
}

//-----------------------------------------------------------------------

cGamepadSDL::~cGamepadSDL()
{
    if(mpHandle)
    {
        SDL_JoystickClose(mpHandle);
        mpHandle = NULL;
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

int cGamepadSDL::GetNumButtons()
{
    return (int)mvButtonArray.size();
}

int cGamepadSDL::GetNumAxes()
{
    return (int)mvAxisArray.size();
}

int cGamepadSDL::GetNumHats()
{
    return (int)mvHatArray.size();
}

int cGamepadSDL::GetNumBalls()
{
    return (int)mvBallAbsPosArray.size();
}

//-----------------------------------------------------------------------

void cGamepadSDL::Update()
{
    mlstInputUpdates.clear();
    mlstButtonsPressed.clear();
    mlstButtonsReleased.clear();
    mlstHatStateChanges.clear();
    mlstAxisChanges.clear();

    if(mpHandle == NULL)
    {
        return;
    }

    // SDL_PollEvent() in cLowLevelInputSDL::BeginInputUpdate() has already
    // run SDL_JoystickUpdate(), so the joystick state below is current.

    if(iGamepad::GetLogRawInput())
    {
        LogRawInput();
    }

    if(mMapping.mbIdentity)
    {
        ///////////////////
        // Raw layout
        for(int i = 0; i < GetNumButtons(); ++i)
        {
            SetButton(i, SDL_JoystickGetButton(mpHandle, i) != 0);
        }
        for(int i = 0; i < GetNumAxes(); ++i)
        {
            SetAxis(i, ApplyDeadZone(RawAxis(i)));
        }
        for(int i = 0; i < GetNumHats(); ++i)
        {
            SetHat(i, static_cast<eGamepadHatState>(SDL_JoystickGetHat(mpHandle, i)));
        }
    }
    else
    {
        ///////////////////
        // Translated layout
        for(int i = 0; i < kGamepadSDLNumButtons; ++i)
        {
            SetButton(i, SourceIsDown(mMapping.mvButtons[i]));
        }

        static const int vDpadBits[4] = { eGamepadHatState_Up, eGamepadHatState_Right, eGamepadHatState_Down, eGamepadHatState_Left };
        int lHat = eGamepadHatState_Centered;
        for(int i = 0; i < 4; ++i)
        {
            if(SourceIsDown(mMapping.mvDpad[i]))
            {
                lHat |= vDpadBits[i];
            }
        }
        // Opposite directions pressed at once (axis based D-pads can't, but buttons can)
        if((lHat & eGamepadHatState_Up) && (lHat & eGamepadHatState_Down))
        {
            lHat &= ~(eGamepadHatState_Up | eGamepadHatState_Down);
        }
        if((lHat & eGamepadHatState_Left) && (lHat & eGamepadHatState_Right))
        {
            lHat &= ~(eGamepadHatState_Left | eGamepadHatState_Right);
        }
        SetHat(0, static_cast<eGamepadHatState>(lHat));

        float fLeftTrigger = SourceTriggerValue(mMapping.mvAxes[eGamepadSDLStick_LeftTrigger]);
        float fRightTrigger = SourceTriggerValue(mMapping.mvAxes[eGamepadSDLStick_RightTrigger]);

        SetAxis(0, ApplyDeadZone(SourceStickValue(mMapping.mvAxes[eGamepadSDLStick_LeftX])));
        SetAxis(1, ApplyDeadZone(SourceStickValue(mMapping.mvAxes[eGamepadSDLStick_LeftY])));
        SetAxis(2, ApplyDeadZone(cMath::Clamp(fLeftTrigger - fRightTrigger, -1.0f, 1.0f)));
        SetAxis(3, ApplyDeadZone(SourceStickValue(mMapping.mvAxes[eGamepadSDLStick_RightY])));
        SetAxis(4, ApplyDeadZone(SourceStickValue(mMapping.mvAxes[eGamepadSDLStick_RightX])));
    }

    ///////////////////
    // Trackballs
    for(int i = 0; i < GetNumBalls(); ++i)
    {
        int lDx = 0, lDy = 0;
        SDL_JoystickGetBall(mpHandle, i, &lDx, &lDy);

        cVector2l vDelta = cVector2l(lDx, lDy);
        mvBallRelPosArray[i] = vDelta;
        mvBallAbsPosArray[i] += vDelta;
    }
}

//-----------------------------------------------------------------------

bool cGamepadSDL::HasInputUpdates()
{
    return mlstInputUpdates.empty() == false;
}

cGamepadInputData cGamepadSDL::GetInputUpdate()
{
    cGamepadInputData input = mlstInputUpdates.front();
    mlstInputUpdates.pop_front();

    switch(input.mInputType)
    {
    case eGamepadInputType_Button:
    {
        if(input.mfInputValue == 0.0f)
        {
            mlstButtonsReleased.remove(input);
        }
        else
        {
            mlstButtonsPressed.remove(input);
        }
    }
    break;
    case eGamepadInputType_Hat:
        mlstHatStateChanges.remove(input);
        break;
    case eGamepadInputType_Axis:
        mlstAxisChanges.remove(input);
        break;
    }

    return input;
}

//-----------------------------------------------------------------------

bool cGamepadSDL::ButtonIsDown(eGamepadButton aButton)
{
    if(aButton >= 0 && aButton < GetNumButtons())
    {
        return mvButtonArray[aButton];
    }

    return false;
}

//-----------------------------------------------------------------------


cGamepadInputData cGamepadSDL::GetButton()
{
    cGamepadInputData button = mlstButtonsPressed.front();
    mlstButtonsPressed.pop_front();
    mlstInputUpdates.remove(button);

    return button;
}

//-----------------------------------------------------------------------

bool cGamepadSDL::ButtonIsPressed()
{
    return mlstButtonsPressed.empty() == false;
}

//-----------------------------------------------------------------------

cGamepadInputData cGamepadSDL::GetReleasedButton()
{
    cGamepadInputData button = mlstButtonsReleased.front();
    mlstButtonsReleased.pop_front();
    mlstInputUpdates.remove(button);

    return button;
}

//-----------------------------------------------------------------------

bool cGamepadSDL::ButtonIsReleased()
{
    return mlstButtonsReleased.empty() == false;
}

//-----------------------------------------------------------------------

float cGamepadSDL::GetAxisValue(eGamepadAxis aAxis)
{
    if(aAxis >= 0 && aAxis < GetNumAxes())
    {
        return mvAxisArray[aAxis];
    }

    return 0;
}

float cGamepadSDL::GetAxisDeadZoneRadiusValue()
{
    return mfDeadZoneRadius;
}

void cGamepadSDL::SetAxisDeadZoneRadiusValue(float afValue)
{
    mfDeadZoneRadius = afValue;
}

cGamepadInputData cGamepadSDL::GetUpdatedAxis()
{
    cGamepadInputData axis = mlstAxisChanges.front();
    mlstAxisChanges.pop_front();
    mlstInputUpdates.remove(axis);

    return axis;
}

bool cGamepadSDL::AxesUpdated()
{
    return mlstAxisChanges.empty() == false;
}

//-----------------------------------------------------------------------

eGamepadHatState cGamepadSDL::GetHatCurrentState(eGamepadHat aHat)
{
    if(aHat >= 0 && aHat < GetNumHats())
    {
        return mvHatArray[aHat];
    }

    return eGamepadHatState_Centered;
}

bool cGamepadSDL::HatIsInState(eGamepadHat aHat, eGamepadHatState aState)
{
    return (GetHatCurrentState(aHat)&aState) != 0;
}

bool cGamepadSDL::HatsChanged()
{
    return mlstHatStateChanges.empty() == false;
}

cGamepadInputData cGamepadSDL::GetHatState()
{
    cGamepadInputData change = mlstHatStateChanges.front();
    mlstHatStateChanges.pop_front();
    return change;
}

//-----------------------------------------------------------------------

cVector2l cGamepadSDL::GetBallAbsPos(eGamepadBall aBall)
{
    if(aBall >= 0 && aBall < GetNumBalls())
    {
        return mvBallAbsPosArray[aBall];
    }

    return 0;
}

cVector2l cGamepadSDL::GetBallRelPos(eGamepadBall aBall)
{
    if(aBall >= 0 && aBall < GetNumBalls())
    {
        return mvBallRelPosArray[aBall];
    }

    return 0;
}

//-----------------------------------------------------------------------

static void TrimString(tString& asStr)
{
    size_t lStart = asStr.find_first_not_of(" \t\r\n");
    if(lStart == tString::npos)
    {
        asStr = "";
        return;
    }
    size_t lEnd = asStr.find_last_not_of(" \t\r\n");
    asStr = asStr.substr(lStart, lEnd - lStart + 1);
}

static bool ParseIndex(const tString& asStr, size_t alPos, int& alOut, size_t& alEnd)
{
    if(alPos >= asStr.size() || asStr[alPos] < '0' || asStr[alPos] > '9')
    {
        return false;
    }
    alOut = 0;
    while(alPos < asStr.size() && asStr[alPos] >= '0' && asStr[alPos] <= '9')
    {
        alOut = alOut * 10 + (asStr[alPos] - '0');
        ++alPos;
    }
    alEnd = alPos;
    return true;
}

static bool ParseSource(const tString& asValue, cGamepadSDLSource& aSrc)
{
    cGamepadSDLSource src;
    size_t lPos = 0;
    size_t lEnd = 0;

    // Half axis prefix: "+a2" / "-a2"
    if(lPos < asValue.size() && (asValue[lPos] == '+' || asValue[lPos] == '-'))
    {
        src.mlHalf = asValue[lPos] == '+' ? 1 : -1;
        ++lPos;
    }

    if(lPos >= asValue.size())
    {
        return false;
    }

    char cType = asValue[lPos++];
    if(cType == 'b')
    {
        src.mType = eGamepadSDLSourceType_Button;
        if(ParseIndex(asValue, lPos, src.mlIndex, lEnd) == false)
        {
            return false;
        }
    }
    else if(cType == 'a')
    {
        src.mType = eGamepadSDLSourceType_Axis;
        if(ParseIndex(asValue, lPos, src.mlIndex, lEnd) == false)
        {
            return false;
        }
        // Inverted axis suffix: "a3~"
        if(lEnd < asValue.size() && asValue[lEnd] == '~')
        {
            src.mbInvert = true;
            ++lEnd;
        }
    }
    else if(cType == 'h')
    {
        src.mType = eGamepadSDLSourceType_Hat;
        if(ParseIndex(asValue, lPos, src.mlIndex, lEnd) == false)
        {
            return false;
        }
        if(lEnd >= asValue.size() || asValue[lEnd] != '.')
        {
            return false;
        }
        if(ParseIndex(asValue, lEnd + 1, src.mlHatMask, lEnd) == false)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    if(lEnd != asValue.size())
    {
        return false;
    }

    aSrc = src;
    return true;
}

//-----------------------------------------------------------------------

bool cGamepadSDL::ParseMapping(const tString& asMapping, cGamepadSDLMapping& aMapping)
{
    static const char *vButtonNames[kGamepadSDLNumButtons] =
    {
        "a", "b", "x", "y", "leftshoulder", "rightshoulder", "back", "start", "leftstick", "rightstick"
    };
    static const char *vDpadNames[4] = { "dpup", "dpright", "dpdown", "dpleft" };
    static const char *vAxisNames[eGamepadSDLStick_LastEnum] =
    {
        "leftx", "lefty", "rightx", "righty", "lefttrigger", "righttrigger"
    };

    cGamepadSDLMapping mapping;
    mapping.mbIdentity = false;
    int lEntries = 0;

    size_t lStart = 0;
    while(lStart <= asMapping.size())
    {
        size_t lComma = asMapping.find(',', lStart);
        if(lComma == tString::npos)
        {
            lComma = asMapping.size();
        }

        tString sEntry = asMapping.substr(lStart, lComma - lStart);
        lStart = lComma + 1;

        TrimString(sEntry);
        size_t lColon = sEntry.find(':');
        if(lColon == tString::npos)
        {
            continue;
        }

        tString sKey = cString::ToLowerCase(sEntry.substr(0, lColon));
        tString sValue = sEntry.substr(lColon + 1);
        TrimString(sKey);
        TrimString(sValue);

        cGamepadSDLSource *pTarget = NULL;
        for(int i = 0; i < kGamepadSDLNumButtons && pTarget == NULL; ++i)
        {
            if(sKey == vButtonNames[i])
            {
                pTarget = &mapping.mvButtons[i];
            }
        }
        for(int i = 0; i < 4 && pTarget == NULL; ++i)
        {
            if(sKey == vDpadNames[i])
            {
                pTarget = &mapping.mvDpad[i];
            }
        }
        for(int i = 0; i < eGamepadSDLStick_LastEnum && pTarget == NULL; ++i)
        {
            if(sKey == vAxisNames[i])
            {
                pTarget = &mapping.mvAxes[i];
            }
        }

        // Unknown keys (guide, platform, misc1, paddles, ...) are ignored
        if(pTarget == NULL)
        {
            continue;
        }

        if(ParseSource(sValue, *pTarget))
        {
            ++lEntries;
        }
        else
        {
            Log("  Gamepad mapping: could not parse '%s'\n", sEntry.c_str());
        }
    }

    if(lEntries == 0)
    {
        return false;
    }

    mapping.msName = aMapping.msName;
    aMapping = mapping;
    return true;
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cGamepadSDL::FindUserMapping(cGamepadSDLMapping& aMapping)
{
    // Format: "VVVV:PPPP,a:b1,b:b2,...;VVVV:PPPP,..." (hex IDs), or "*,..." for any pad.
    // A mapping of just "VVVV:PPPP,raw" uses the pad's raw layout.
    const tString& sAll = iGamepad::GetUserMappings();

    size_t lStart = 0;
    while(lStart < sAll.size())
    {
        size_t lSemi = sAll.find(';', lStart);
        if(lSemi == tString::npos)
        {
            lSemi = sAll.size();
        }

        tString sLine = sAll.substr(lStart, lSemi - lStart);
        lStart = lSemi + 1;
        TrimString(sLine);

        size_t lComma = sLine.find(',');
        if(lComma == tString::npos)
        {
            continue;
        }

        tString sId = sLine.substr(0, lComma);
        tString sMapping = sLine.substr(lComma + 1);
        TrimString(sId);
        TrimString(sMapping);

        bool bMatch = false;
        if(sId == "*")
        {
            bMatch = true;
        }
        else
        {
            size_t lColon = sId.find(':');
            if(lColon != tString::npos)
            {
                long lVid = strtol(sId.substr(0, lColon).c_str(), NULL, 16);
                long lPid = strtol(sId.substr(lColon + 1).c_str(), NULL, 16);
                bMatch = (lVid == (long)mlVendorId && lPid == (long)mlProductId);
            }
        }

        if(bMatch == false)
        {
            continue;
        }

        if(cString::ToLowerCase(sMapping) == "raw")
        {
            aMapping = cGamepadSDLMapping();
            aMapping.msName = "raw (GamepadMappings in main_settings.cfg)";
            return true;
        }

        aMapping.msName = "GamepadMappings in main_settings.cfg";
        if(ParseMapping(sMapping, aMapping))
        {
            return true;
        }

        Log("  Gamepad mapping for '%s' in main_settings.cfg is not valid, ignoring it\n", sId.c_str());
    }

    return false;
}

//-----------------------------------------------------------------------

void cGamepadSDL::SetUpMapping()
{
    ///////////////////
    // User override
    if(FindUserMapping(mMapping))
    {
        return;
    }

    ///////////////////
    // Xbox 360 pads through winmm already use the layout the bindings expect
    if(mlVendorId == kVendor_Microsoft || NameLooksLikeXbox(msGamepadName))
    {
        mMapping = cGamepadSDLMapping();
        mMapping.msName = "Xbox (raw)";
        return;
    }

    ///////////////////
    // Known pads
    for(int i = 0; gvBuiltInMappings[i].msMapping != NULL; ++i)
    {
        const cGamepadSDLBuiltInMapping& builtIn = gvBuiltInMappings[i];
        if(builtIn.mlVendor == mlVendorId && builtIn.mlProduct == mlProductId)
        {
            mMapping.msName = builtIn.msName;
            if(ParseMapping(builtIn.msMapping, mMapping))
            {
                return;
            }
        }
    }

    ///////////////////
    // Everything else: the most common DirectInput layout
    mMapping.msName = "generic DirectInput";
    if(ParseMapping(gsMapping_Logitech, mMapping) == false)
    {
        mMapping = cGamepadSDLMapping();
        mMapping.msName = "raw";
    }
}

//-----------------------------------------------------------------------

float cGamepadSDL::RawAxis(int alAxis)
{
    if(alAxis < 0 || alAxis >= SDL_JoystickNumAxes(mpHandle))
    {
        return 0.0f;
    }
    return cMath::Clamp((float)SDL_JoystickGetAxis(mpHandle, alAxis) * mfInvAxisMax, -1.0f, 1.0f);
}

//-----------------------------------------------------------------------

bool cGamepadSDL::SourceIsDown(const cGamepadSDLSource& aSrc)
{
    switch(aSrc.mType)
    {
    case eGamepadSDLSourceType_Button:
        if(aSrc.mlIndex < SDL_JoystickNumButtons(mpHandle))
        {
            return SDL_JoystickGetButton(mpHandle, aSrc.mlIndex) != 0;
        }
        return false;

    case eGamepadSDLSourceType_Hat:
        if(aSrc.mlIndex < SDL_JoystickNumHats(mpHandle))
        {
            return (SDL_JoystickGetHat(mpHandle, aSrc.mlIndex) & aSrc.mlHatMask) != 0;
        }
        return false;

    case eGamepadSDLSourceType_Axis:
    {
        float fVal = RawAxis(aSrc.mlIndex);
        if(aSrc.mbInvert)
        {
            fVal = -fVal;
        }
        if(aSrc.mlHalf < 0)
        {
            return fVal < -0.5f;
        }
        return fVal > 0.5f;
    }

    default:
        break;
    }

    return false;
}

//-----------------------------------------------------------------------

float cGamepadSDL::SourceStickValue(const cGamepadSDLSource& aSrc)
{
    if(aSrc.mType != eGamepadSDLSourceType_Axis)
    {
        return SourceIsDown(aSrc) ? 1.0f : 0.0f;
    }

    float fVal = RawAxis(aSrc.mlIndex);
    if(aSrc.mbInvert)
    {
        fVal = -fVal;
    }
    if(aSrc.mlHalf > 0)
    {
        return cMath::Max(fVal, 0.0f);
    }
    if(aSrc.mlHalf < 0)
    {
        return cMath::Max(-fVal, 0.0f);
    }
    return fVal;
}

//-----------------------------------------------------------------------

float cGamepadSDL::SourceTriggerValue(const cGamepadSDLSource& aSrc)
{
    if(aSrc.mType != eGamepadSDLSourceType_Axis)
    {
        return SourceIsDown(aSrc) ? 1.0f : 0.0f;
    }

    // A full axis trigger rests at -1 and is fully pressed at +1 (same as SDL2)
    if(aSrc.mlHalf == 0)
    {
        float fVal = RawAxis(aSrc.mlIndex);
        if(aSrc.mbInvert)
        {
            fVal = -fVal;
        }
        return cMath::Clamp((fVal + 1.0f) * 0.5f, 0.0f, 1.0f);
    }

    return SourceStickValue(aSrc);
}

//-----------------------------------------------------------------------

float cGamepadSDL::ApplyDeadZone(float afValue)
{
    if(cMath::Abs(afValue) < mfDeadZoneRadius)
    {
        return 0.0f;
    }
    return afValue;
}

//-----------------------------------------------------------------------

void cGamepadSDL::SetButton(int alButton, bool abPressed)
{
    if(alButton < 0 || alButton >= GetNumButtons())
    {
        return;
    }
    if(mvButtonArray[alButton] == abPressed)
    {
        return;
    }

    mvButtonArray[alButton] = abPressed;

    cGamepadInputData inputUpdate(mlIndex, eGamepadInputType_Button, alButton, abPressed ? 1.0f : 0.0f);
    if(abPressed)
    {
        mlstButtonsPressed.push_back(inputUpdate);
    }
    else
    {
        mlstButtonsReleased.push_back(inputUpdate);
    }
    mlstInputUpdates.push_back(inputUpdate);
}

//-----------------------------------------------------------------------

void cGamepadSDL::SetAxis(int alAxis, float afValue)
{
    if(alAxis < 0 || alAxis >= GetNumAxes())
    {
        return;
    }
    if(mvAxisArray[alAxis] == afValue)
    {
        return;
    }

    mvAxisArray[alAxis] = afValue;

    cGamepadInputData inputUpdate(mlIndex, eGamepadInputType_Axis, alAxis, afValue);
    mlstAxisChanges.push_back(inputUpdate);
    mlstInputUpdates.push_back(inputUpdate);
}

//-----------------------------------------------------------------------

void cGamepadSDL::SetHat(int alHat, eGamepadHatState aState)
{
    if(alHat < 0 || alHat >= GetNumHats())
    {
        return;
    }
    if(mvHatArray[alHat] == aState)
    {
        return;
    }

    mvHatArray[alHat] = aState;

    cGamepadInputData inputUpdate(mlIndex, eGamepadInputType_Hat, alHat, (float)aState);
    mlstHatStateChanges.push_back(inputUpdate);
    mlstInputUpdates.push_back(inputUpdate);
}

//-----------------------------------------------------------------------

void cGamepadSDL::LogRawInput()
{
    // Writes raw SDL indices to hpl.log so a mapping can be worked out for a new pad.
    for(size_t i = 0; i < mvRawButtonLog.size(); ++i)
    {
        bool bDown = SDL_JoystickGetButton(mpHandle, (int)i) != 0;
        if(bDown != mvRawButtonLog[i])
        {
            mvRawButtonLog[i] = bDown;
            Log("Gamepad %d raw: b%d %s\n", mlIndex, (int)i, bDown ? "down" : "up");
        }
    }

    for(size_t i = 0; i < mvRawAxisLog.size(); ++i)
    {
        // Only log when an axis moves past half way, to keep the log readable
        float fVal = RawAxis((int)i);
        int lZone = fVal > 0.5f ? 1 : (fVal < -0.5f ? -1 : 0);
        if(lZone != mvRawAxisLog[i])
        {
            mvRawAxisLog[i] = lZone;
            Log("Gamepad %d raw: a%d %s\n", mlIndex, (int)i, lZone > 0 ? "+" : (lZone < 0 ? "-" : "centered"));
        }
    }

    for(size_t i = 0; i < mvRawHatLog.size(); ++i)
    {
        Uint8 lHat = SDL_JoystickGetHat(mpHandle, (int)i);
        if(lHat != mvRawHatLog[i])
        {
            mvRawHatLog[i] = lHat;
            Log("Gamepad %d raw: h%d.%d\n", mlIndex, (int)i, (int)lHat);
        }
    }
}

//-----------------------------------------------------------------------

}
#endif
