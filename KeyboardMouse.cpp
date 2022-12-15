#include <SDL2/SDL.h>
#include "KeyboardMouse.h"


INPUTSTATES InputStates;

/*----------------------------------------------------------------------------
Name:           InitInputStates
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Eingangsgeräte (Keyboard / Maus)
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = alles OK, sonst Fehler
Seiteneffekte: InputsStates.x
------------------------------------------------------------------------------*/
int InitInputStates(void) {
    memset(&InputStates,0,sizeof(InputStates));
    InputStates.pKeyboardArray = SDL_GetKeyboardState(NULL);
    if (InputStates.pKeyboardArray) {
        return 0;
    } else {
        return -1;
    }
}


/*----------------------------------------------------------------------------
Name:           UpdateInputStates
------------------------------------------------------------------------------
Beschreibung: Holt neue Zustände von den Eingangsgeräten (Keyboard / Maus)
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = alles OK, sonst Fehler
Seiteneffekte: InputStates.x
------------------------------------------------------------------------------*/
int UpdateInputStates(void) {
    SDL_Event Event;
    Uint32 uMouseButtons;

    if (InputStates.pKeyboardArray != NULL) {
        InputStates.nMouseWheelY = 0;
        while (SDL_PollEvent(&Event) != 0) {                    // Macht Update für Keyboard und Maus
            InputStates.bQuit = (Event.type == SDL_QUIT);
            if (Event.type == SDL_MOUSEWHEEL) {
                InputStates.nMouseWheelY = Event.wheel.y;
            }
        }
        uMouseButtons = SDL_GetMouseState(&InputStates.nMouseXpos,&InputStates.nMouseYpos);    // Maus abfragen
        InputStates.bLeftMouseButton = ((uMouseButtons & SDL_BUTTON_LMASK) != 0);
        InputStates.bRightMouseButton = ((uMouseButtons & SDL_BUTTON_RMASK) != 0);
        return 0;
    } else {
        return -1;
    }
}


/*----------------------------------------------------------------------------
Name:           WaitForKey
------------------------------------------------------------------------------
Beschreibung: Wartet auf einen bestimmten Tastendruck.
Parameter
      Eingang: Scancode, SDL_Scancode, Taste, auf die gewartet werden soll.
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: InputStates.x
------------------------------------------------------------------------------*/
void WaitForKey(SDL_Scancode ScanCode) {
    UpdateInputStates();

    while (InputStates.pKeyboardArray[ScanCode] == 0) {
        UpdateInputStates();
        SDL_Delay(5);
    }
}
