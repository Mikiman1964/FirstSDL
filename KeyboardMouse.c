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
Rückgabewert:  -
Seiteneffekte: InputStates.x
------------------------------------------------------------------------------*/
void  UpdateInputStates(void) {
    Uint32 uMouseButtons;
    SDL_Event Event;

    InputStates.nMouseWheelY = 0;
    InputStates.uLastKey0 = 0;
    InputStates.uLastKey1 = 0;
    while (SDL_PollEvent(&Event) != 0) {                    // Macht Update für Keyboard und Maus
        InputStates.bQuit = (Event.type == SDL_QUIT);
        if (Event.type == SDL_MOUSEWHEEL) {
            InputStates.nMouseWheelY = Event.wheel.y;
        } else if (Event.type == SDL_TEXTINPUT) {
            InputStates.uLastKey0 = Event.text.text[0];
            InputStates.uLastKey1 = Event.text.text[1];

        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_RETURN]) {
            InputStates.uLastKey0 = 0x0A;
            InputStates.uLastKey1 = 0;
        }
    }
    uMouseButtons = SDL_GetMouseState(&InputStates.nMouseXpos,&InputStates.nMouseYpos);    // Maus abfragen
    InputStates.bLeftMouseButton = ((uMouseButtons & SDL_BUTTON_LMASK) != 0);
    InputStates.bMidMouseButton = ((uMouseButtons & SDL_BUTTON_MMASK) != 0);
    InputStates.bRightMouseButton = ((uMouseButtons & SDL_BUTTON_RMASK) != 0);
}


/*----------------------------------------------------------------------------
Name:           GetKey
------------------------------------------------------------------------------
Beschreibung: Prüft, ob eine bestimmte Taste gedrückt ist und gibt den Key-Code zurück.
              Der Keycode passt zum "kleinen grünen" Zeichensatz.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  uint32_t, 0 = keine Taste gedrückt, sonst Key-Code
Seiteneffekte: InputStates.x
------------------------------------------------------------------------------*/
uint32_t GetKey(void) {
    uint32_t uKey = 0;

    // ASCII-Bereich abfragen
    if ((InputStates.uLastKey0 != 0) && (InputStates.uLastKey1 == 0)) {
        uKey = InputStates.uLastKey0;
        if ((uKey >= 'a') && (uKey <= 'z')) {
            uKey = uKey - 32;
        } else if (uKey == 126) {   // Welle ~ wird zu TradeMark
            uKey = 101;
        } else if (uKey > 102) {
            uKey = 0;
        }
    } else if (InputStates.uLastKey0 == 0xFFFFFFC3) {
        // Umlaute abfragen
        if ((InputStates.uLastKey1 == 0xFFFFFFB6) || (InputStates.uLastKey1 == 0xFFFFFF96)) { // ö oder Ö
            uKey = 98;
        } else if ((InputStates.uLastKey1 == 0xFFFFFFA4) || (InputStates.uLastKey1 == 0xFFFFFF84)) { // ä oder Ä
            uKey = 97;
        } else if ((InputStates.uLastKey1 == 0xFFFFFFBC) || (InputStates.uLastKey1 == 0xFFFFFF9C)) { // ü oder Ü
            uKey = 99;
        }
    } else if ((InputStates.uLastKey0 == 0xFFFFFFC2) && (InputStates.uLastKey1 == 0xFFFFFFB0))  {
        // Gradzeichen °
        uKey = 100;
    }
   InputStates.uLastKey0 = 0;
   InputStates.uLastKey1= 0;
   return uKey;
}


/*----------------------------------------------------------------------------
Name:           WaitNoSpecialKey
------------------------------------------------------------------------------
Beschreibung: Wartet darauf, dass eine bestimmte Taste los gelassen wird.
Parameter
      Eingang: Scancode, SDL_Scancode, Taste, auf die gewartet werden soll.
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: InputStates.x
------------------------------------------------------------------------------*/
void WaitNoSpecialKey(SDL_Scancode ScanCode) {
    UpdateInputStates();

    while (InputStates.pKeyboardArray[ScanCode]) {
        UpdateInputStates();
        SDL_Delay(5);
    }
}


/*----------------------------------------------------------------------------
Name:           WaitNoKey
------------------------------------------------------------------------------
Beschreibung: Wartet darauf, dass folgende Tasten nicht mehr aktiv sind.
              * linke und rechte Maustaste
              * ESC
              * Space
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: InputStates.x
------------------------------------------------------------------------------*/
void WaitNoKey(void) {
    UpdateInputStates();

    while ( (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE] == 1) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_SPACE] == 1) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_LEFT] == 1) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_RIGHT] == 1) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_UP] == 1) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_DOWN] == 1) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_INSERT] == 1) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE] == 1) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_DELETE] == 1) ||
            (InputStates.bLeftMouseButton) || (InputStates.bRightMouseButton) ) {
        UpdateInputStates();
        SDL_Delay(5);
    }
}
