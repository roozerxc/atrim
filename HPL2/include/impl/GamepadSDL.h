#ifndef HPL_GAMEPAD_SDL_H
#define HPL_GAMEPAD_SDL_H

#if USE_GAMEPAD
#include <vector>
#include <list>
#include "system/SystemTypes.h"
#include "input/Gamepad.h"
//#include "input/InputTypes.h"

#include "SDL/SDL.h"

namespace hpl
{

class cLowLevelInputSDL;

//------------------------------------------------------------------------
// Layout translation for non-XInput pads
//
// The game's default bindings (LuxInputHandler.cpp) are written for the
// Xbox 360 pad as seen through DirectInput/winmm:
//
//   Buttons: 0 A, 1 B, 2 X, 3 Y, 4 LB, 5 RB, 6 Back, 7 Start, 8 LS, 9 RS
//   Hat 0:   D-pad
//   Axes:    0 left X, 1 left Y, 2 LT(+)/RT(-), 3 right Y, 4 right X
//
// Every other DirectInput pad numbers its buttons and axes differently,
// so cGamepadSDL translates the raw SDL joystick into that layout using
// an SDL GameControllerDB style mapping string ("a:b1,b:b2,leftx:a0,...").
//------------------------------------------------------------------------

enum eGamepadSDLSourceType
{
    eGamepadSDLSourceType_None,
    eGamepadSDLSourceType_Button,
    eGamepadSDLSourceType_Axis,
    eGamepadSDLSourceType_Hat
};

struct cGamepadSDLSource
{
    cGamepadSDLSource() : mType(eGamepadSDLSourceType_None), mlIndex(0), mlHatMask(0), mlHalf(0), mbInvert(false) {}

    eGamepadSDLSourceType mType;
    int mlIndex;
    int mlHatMask;    // SDL_HAT_* bits, for hats
    int mlHalf;       // 0 = full axis, 1 = positive half only, -1 = negative half only
    bool mbInvert;
};

enum eGamepadSDLStick
{
    eGamepadSDLStick_LeftX,
    eGamepadSDLStick_LeftY,
    eGamepadSDLStick_RightX,
    eGamepadSDLStick_RightY,
    eGamepadSDLStick_LeftTrigger,
    eGamepadSDLStick_RightTrigger,

    eGamepadSDLStick_LastEnum
};

#define kGamepadSDLNumButtons 10

struct cGamepadSDLMapping
{
    cGamepadSDLMapping() : mbIdentity(true) {}

    bool mbIdentity;    // raw SDL indices are used as they are (Xbox 360 pads)
    tString msName;

    cGamepadSDLSource mvButtons[kGamepadSDLNumButtons];
    cGamepadSDLSource mvDpad[4];    // up, right, down, left
    cGamepadSDLSource mvAxes[eGamepadSDLStick_LastEnum];
};

//------------------------------------------------------------------------

class cGamepadSDL : public iGamepad
{
public:
    cGamepadSDL(cLowLevelInputSDL *apLowLevelInputSDL, int alIndex);
    ~cGamepadSDL();

    tString GetGamepadName()
    {
        return msGamepadName;
    }

    int GetNumButtons();
    int GetNumAxes();
    int GetNumHats();
    int GetNumBalls();

    void Update();

    //Gamepad specific
    bool HasInputUpdates();
    cGamepadInputData GetInputUpdate();

    bool ButtonIsDown(eGamepadButton aButton);
    cGamepadInputData GetButton();
    bool ButtonIsPressed();
    bool ButtonIsReleased();
    cGamepadInputData GetReleasedButton();

    float GetAxisValue(eGamepadAxis aAxis);
    float GetAxisDeadZoneRadiusValue();
    void SetAxisDeadZoneRadiusValue(float afValue);
    bool AxesUpdated();
    cGamepadInputData GetUpdatedAxis();

    eGamepadHatState GetHatCurrentState(eGamepadHat aHat);
    bool HatIsInState(eGamepadHat aHat, eGamepadHatState aState);
    bool HatsChanged();
    cGamepadInputData GetHatState();

    cVector2l GetBallAbsPos(eGamepadBall aBall);
    cVector2l GetBallRelPos(eGamepadBall aBall);

    static bool ParseMapping(const tString& asMapping, cGamepadSDLMapping& aMapping);

private:
    void SetUpMapping();
    bool FindUserMapping(cGamepadSDLMapping& aMapping);
    void ConvertDInputAxes(cGamepadSDLMapping& aMapping);

    float RawAxis(int alAxis);
    bool SourceIsDown(const cGamepadSDLSource& aSrc);
    float SourceStickValue(const cGamepadSDLSource& aSrc);
    float SourceTriggerValue(const cGamepadSDLSource& aSrc);
    float ApplyDeadZone(float afValue);

    void SetButton(int alButton, bool abPressed);
    void SetAxis(int alAxis, float afValue);
    void SetHat(int alHat, eGamepadHatState aState);

    void LogRawInput();

    tString msGamepadName;
    Uint16 mlVendorId;
    Uint16 mlProductId;

    cGamepadSDLMapping mMapping;

    std::vector<float>                mvAxisArray;
    std::vector<eGamepadAxisRange>    mvAxisRange;
    std::vector<bool>                mvButtonArray;
    std::vector<eGamepadHatState>    mvHatArray;
    std::vector<cVector2l>            mvBallAbsPosArray;
    std::vector<cVector2l>            mvBallRelPosArray;

    // Raw state, only kept for GamepadLogRawInput
    std::vector<bool>    mvRawButtonLog;
    std::vector<int>     mvRawAxisLog;
    std::vector<Uint8>   mvRawHatLog;

    std::list<cGamepadInputData> mlstInputUpdates;

    std::list<cGamepadInputData> mlstButtonsPressed;
    std::list<cGamepadInputData> mlstButtonsReleased;

    std::list<cGamepadInputData> mlstAxisChanges;

    std::list<cGamepadInputData> mlstHatStateChanges;

    SDL_Joystick        *mpHandle;
    cLowLevelInputSDL    *mpLowLevelInputSDL;

    static float mfInvAxisMax;
    static float mfDeadZoneRadius;
};

};
#endif

#endif // HPL_GAMEPAD_SDL_H
