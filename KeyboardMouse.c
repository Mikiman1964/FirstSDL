#include <SDL2/SDL.h>
#include "KeyboardMouse.h"


INPUTSTATES InputStates;
extern uint32_t ge_uXoffs;             // X-Offset für die Zentrierung von Elementen
extern uint32_t ge_uYoffs;             // X-Offset für die Zentrierung von Elementen

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
        SDL_Log("%s: failed!",__FUNCTION__);
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
Seiteneffekte: InputStates.x, ge_uXoffs, ge_uYoffs
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
    uMouseButtons = SDL_GetMouseState(&InputStates.nMouseXpos_Absolute,&InputStates.nMouseYpos_Absolute);    // Maus abfragen
    InputStates.nMouseXpos_Relative = InputStates.nMouseXpos_Absolute - ge_uXoffs;
    InputStates.nMouseYpos_Relative = InputStates.nMouseYpos_Absolute - ge_uYoffs;
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
Name:           FilterBigFontKey
------------------------------------------------------------------------------
Beschreibung: Filtert einen Tastencode, der aus GetKey() stammt, für den
              Spiel-Zeichensatz (EMERALD_FONT_xxx)
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  uint32_t, Tastencode für Spielzeichensatz, 0 = kein geeigneter Code
Seiteneffekte: InputStates.x
------------------------------------------------------------------------------*/
uint32_t FilterBigFontKey(uint32_t uKey) {
    uint32_t uRetKey = 0;

    if (  ((uKey >= 65) && (uKey <= 90)) ||         // A bis Z
           (uKey == 32)                  ||         // SPACE
           (uKey == 33)                  ||         // !
          ((uKey >= 39) && (uKey <= 58)) ||         // ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 :
           // Auskommentiert: Könnte Probleme mit XML-Tags machen
           //(uKey == 60)                  ||         // <--
           //(uKey == 62)                  ||         // -->
           (uKey == 63)                  ) {        // ?
                uRetKey = uKey;
    }
    return uRetKey;
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
    bool bKeyActive;

    do {
        UpdateInputStates();
        bKeyActive = (
            (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_SPACE]) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_LEFT]) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_RIGHT]) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_UP]) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_DOWN]) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_INSERT]) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_DELETE]) ||
            (InputStates.pKeyboardArray[SDL_SCANCODE_RETURN]) ||
            (InputStates.bLeftMouseButton) || (InputStates.bRightMouseButton));
        if (bKeyActive) {
            SDL_Delay(5);
        }
    } while (bKeyActive);
    InputStates.bQuit = false;
}
