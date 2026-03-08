#ifndef HPL_KEYBOARD_SDL_H
#define HPL_KEYBOARD_SDL_H

#include <vector>
#include <list>
#include "system/SystemTypes.h"
#include "input/Keyboard.h"

namespace hpl
{

#define MAX_KEY_PRESSES (20)

class cLowLevelInputSDL;

class cKeyboardSDL : public iKeyboard
{
public:
    cKeyboardSDL(cLowLevelInputSDL *apLowLevelInputSDL);

    void Update();

    //Keyboard specific
    bool KeyIsDown(eKey aKey);
    cKeyPress GetKey();
    bool KeyIsPressed();
    //int GetModifier();
    bool KeyIsReleased();
    cKeyPress GetReleasedKey();

private:
    eKey SDLToKey(int alKey);
    void ClearKeyList();
    eKey AsciiToKey(int alChar);

    void AddKeyToList(int alSDLMod, eKey aKey, int alUnicode, std::list<cKeyPress>& alstKeys);

    std::vector<bool> mvKeyArray;
    std::vector<eKey> mvWorldKeyMap;
    std::list<cKeyPress> mlstKeysPressed;
    std::list<cKeyPress> mlstKeysReleased;

    cLowLevelInputSDL *mpLowLevelInputSDL;
};

};

#endif // HPL_KEYBOARD_SDL_H
