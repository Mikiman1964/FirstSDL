#include <SDL2/SDL.h>
#include "KeyboardMouse.h"
#include "mySDL.h"

char g_ScanCodeNames[][64] =
                         {  "0",
                            "1",
                            "2",
                            "3",
                            "4",
                            "5",
                            "6",
                            "7",
                            "8",
                            "9",
                            "A",
                            "B",
                            "C",
                            "D",
                            "E",
                            "F",
                            "G",
                            "H",
                            "I",
                            "J",
                            "K",
                            "L",
                            "M",
                            "N",
                            "O",
                            "P",
                            "Q",
                            "R",
                            "S",
                            "T",
                            "U",
                            "V",
                            "W",
                            "X",
                            "Y",
                            "Z",
                            "AC_BACK",
                            "AC_BOOKMARKS",
                            "AC_FORWARD",
                            "AC_HOME",
                            "AC_REFRESH",
                            "AC_SEARCH",
                            "AC_STOP",
                            "AGAIN",
                            "ALTERASE",
                            "APOSTROPHE",
                            "APPLICATION",
                            "AUDIOMUTE",
                            "AUDIONEXT",
                            "AUDIOPLAY",
                            "AUDIOPREV",
                            "AUDIOSTOP",
                            "BACKSLASH",
                            "BACKSPACE",
                            "BRIGHTNESSDOWN",
                            "BRIGHTNESSUP",
                            "CALCULATOR",
                            "CANCEL",
                            "CAPSLOCK",
                            "CLEAR",
                            "CLEARAGAIN",
                            "COMMA",
                            "COMPUTER",
                            "COPY",
                            "CRSEL",
                            "CURRENCYSUBUNIT",
                            "CURRENCYUNIT",
                            "CUT",
                            "DECIMALSEPARATOR",
                            "DELETE",
                            "DISPLAYSWITCH",
                            "DOWN",
                            "EJECT",
                            "END",
                            "EQUALS",
                            "ESCAPE",
                            "EXECUTE",
                            "EXSEL",
                            "F1",
                            "F10",
                            "F11",
                            "F12",
                            "F13",
                            "F14",
                            "F15",
                            "F16",
                            "F17",
                            "F18",
                            "F19",
                            "F2",
                            "F20",
                            "F21",
                            "F22",
                            "F23",
                            "F24",
                            "F3",
                            "F4",
                            "F5",
                            "F6",
                            "F7",
                            "F8",
                            "F9",
                            "FIND",
                            "GRAVE",
                            "HELP",
                            "HOME",
                            "INSERT",
                            "KBDILLUMDOWN",
                            "KBDILLUMTOGGLE",
                            "KBDILLUMUP",
                            "KP_0",
                            "KP_00",
                            "KP_000",
                            "KP_1",
                            "KP_2",
                            "KP_3",
                            "KP_4",
                            "KP_5",
                            "KP_6",
                            "KP_7",
                            "KP_8",
                            "KP_9",
                            "KP_A",
                            "KP_AMPERSAND",
                            "KP_AT",
                            "KP_B",
                            "KP_BACKSPACE",
                            "KP_BINARY",
                            "KP_C",
                            "KP_CLEAR",
                            "KP_CLEARENTRY",
                            "KP_COLON",
                            "KP_COMMA",
                            "KP_D",
                            "KP_DBLAMPERSAND",
                            "KP_DECIMAL",
                            "KP_DIVIDE",
                            "KP_E",
                            "KP_ENTER",
                            "KP_EQUALS",
                            "KP_EQUALSAS400",
                            "KP_EXCLAM",
                            "KP_F",
                            "KP_GREATER",
                            "KP_HASH",
                            "KP_HEXADECIMAL",
                            "KP_LEFTBRACE",
                            "KP_LEFTPAREN",
                            "KP_LESS",
                            "KP_MEMADD",
                            "KP_MEMCLEAR",
                            "KP_MEMDIVIDE",
                            "KP_MEMMULTIPLY",
                            "KP_MEMRECALL",
                            "KP_MEMSTORE",
                            "KP_MEMSUBTRACT",
                            "KP_MINUS",
                            "KP_MULTIPLY",
                            "KP_OCTAL",
                            "KP_PERCENT",
                            "KP_PERIOD",
                            "KP_PLUS",
                            "KP_PLUSMINUS",
                            "KP_POWER",
                            "KP_RIGHTBRACE",
                            "KP_RIGHTPAREN",
                            "KP_SPACE",
                            "KP_TAB",
                            "KP_VERTICALBAR",
                            "KP_XOR",
                            "LALT",
                            "LCTRL",
                            "LEFT",
                            "LEFTBRACKET",
                            "LGUI",
                            "LSHIFT",
                            "MAIL",
                            "MEDIASELECT",
                            "MENU",
                            "MINUS",
                            "MODE",
                            "MUTE",
                            "NUMLOCKCLEAR",
                            "OPER",
                            "OUT",
                            "PAGEDOWN",
                            "PAGEUP",
                            "PASTE",
                            "PAUSE",
                            "PERIOD",
                            "POWER",
                            "PRINTSCREEN",
                            "PRIOR",
                            "RALT",
                            "RCTRL",
                            "RETURN",
                            "RETURN2",
                            "RGUI",
                            "RIGHT",
                            "RIGHTBRACKET",
                            "RSHIFT",
                            "SCROLLLOCK",
                            "SELECT",
                            "SEMICOLON",
                            "SEPARATOR",
                            "SLASH",
                            "SLEEP",
                            "SPACE",
                            "STOP",
                            "SYSREQ",
                            "TAB",
                            "THOUSANDSSEPARATOR",
                            "UNDO",
                            "UNKNOWN",
                            "UP",
                            "VOLUMEDOWN",
                            "VOLUMEUP",
                            "WWW"
                         };



uint32_t g_uScanCodes[] = {
    SDL_SCANCODE_0,
    SDL_SCANCODE_1,
    SDL_SCANCODE_2,
    SDL_SCANCODE_3,
    SDL_SCANCODE_4,
    SDL_SCANCODE_5,
    SDL_SCANCODE_6,
    SDL_SCANCODE_7,
    SDL_SCANCODE_8,
    SDL_SCANCODE_9,
    SDL_SCANCODE_A,
    SDL_SCANCODE_B,
    SDL_SCANCODE_C,
    SDL_SCANCODE_D,
    SDL_SCANCODE_E,
    SDL_SCANCODE_F,
    SDL_SCANCODE_G,
    SDL_SCANCODE_H,
    SDL_SCANCODE_I,
    SDL_SCANCODE_J,
    SDL_SCANCODE_K,
    SDL_SCANCODE_L,
    SDL_SCANCODE_M,
    SDL_SCANCODE_N,
    SDL_SCANCODE_O,
    SDL_SCANCODE_P,
    SDL_SCANCODE_Q,
    SDL_SCANCODE_R,
    SDL_SCANCODE_S,
    SDL_SCANCODE_T,
    SDL_SCANCODE_U,
    SDL_SCANCODE_V,
    SDL_SCANCODE_W,
    SDL_SCANCODE_X,
    SDL_SCANCODE_Y,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_AC_BACK,
    SDL_SCANCODE_AC_BOOKMARKS,
    SDL_SCANCODE_AC_FORWARD,
    SDL_SCANCODE_AC_HOME,
    SDL_SCANCODE_AC_REFRESH,
    SDL_SCANCODE_AC_SEARCH,
    SDL_SCANCODE_AC_STOP,
    SDL_SCANCODE_AGAIN,
    SDL_SCANCODE_ALTERASE,
    SDL_SCANCODE_APOSTROPHE,
    SDL_SCANCODE_APPLICATION,
    SDL_SCANCODE_AUDIOMUTE,
    SDL_SCANCODE_AUDIONEXT,
    SDL_SCANCODE_AUDIOPLAY,
    SDL_SCANCODE_AUDIOPREV,
    SDL_SCANCODE_AUDIOSTOP,
    SDL_SCANCODE_BACKSLASH,
    SDL_SCANCODE_BACKSPACE,
    SDL_SCANCODE_BRIGHTNESSDOWN,
    SDL_SCANCODE_BRIGHTNESSUP,
    SDL_SCANCODE_CALCULATOR,
    SDL_SCANCODE_CANCEL,
    SDL_SCANCODE_CAPSLOCK,
    SDL_SCANCODE_CLEAR,
    SDL_SCANCODE_CLEARAGAIN,
    SDL_SCANCODE_COMMA,
    SDL_SCANCODE_COMPUTER,
    SDL_SCANCODE_COPY,
    SDL_SCANCODE_CRSEL,
    SDL_SCANCODE_CURRENCYSUBUNIT,
    SDL_SCANCODE_CURRENCYUNIT,
    SDL_SCANCODE_CUT,
    SDL_SCANCODE_DECIMALSEPARATOR,
    SDL_SCANCODE_DELETE,
    SDL_SCANCODE_DISPLAYSWITCH,
    SDL_SCANCODE_DOWN,
    SDL_SCANCODE_EJECT,
    SDL_SCANCODE_END,
    SDL_SCANCODE_EQUALS,
    SDL_SCANCODE_ESCAPE,
    SDL_SCANCODE_EXECUTE,
    SDL_SCANCODE_EXSEL,
    SDL_SCANCODE_F1,
    SDL_SCANCODE_F10,
    SDL_SCANCODE_F11,
    SDL_SCANCODE_F12,
    SDL_SCANCODE_F13,
    SDL_SCANCODE_F14,
    SDL_SCANCODE_F15,
    SDL_SCANCODE_F16,
    SDL_SCANCODE_F17,
    SDL_SCANCODE_F18,
    SDL_SCANCODE_F19,
    SDL_SCANCODE_F2,
    SDL_SCANCODE_F20,
    SDL_SCANCODE_F21,
    SDL_SCANCODE_F22,
    SDL_SCANCODE_F23,
    SDL_SCANCODE_F24,
    SDL_SCANCODE_F3,
    SDL_SCANCODE_F4,
    SDL_SCANCODE_F5,
    SDL_SCANCODE_F6,
    SDL_SCANCODE_F7,
    SDL_SCANCODE_F8,
    SDL_SCANCODE_F9,
    SDL_SCANCODE_FIND,
    SDL_SCANCODE_GRAVE,
    SDL_SCANCODE_HELP,
    SDL_SCANCODE_HOME,
    SDL_SCANCODE_INSERT,
    SDL_SCANCODE_KBDILLUMDOWN,
    SDL_SCANCODE_KBDILLUMTOGGLE,
    SDL_SCANCODE_KBDILLUMUP,
    SDL_SCANCODE_KP_0,
    SDL_SCANCODE_KP_00,
    SDL_SCANCODE_KP_000,
    SDL_SCANCODE_KP_1,
    SDL_SCANCODE_KP_2,
    SDL_SCANCODE_KP_3,
    SDL_SCANCODE_KP_4,
    SDL_SCANCODE_KP_5,
    SDL_SCANCODE_KP_6,
    SDL_SCANCODE_KP_7,
    SDL_SCANCODE_KP_8,
    SDL_SCANCODE_KP_9,
    SDL_SCANCODE_KP_A,
    SDL_SCANCODE_KP_AMPERSAND,
    SDL_SCANCODE_KP_AT,
    SDL_SCANCODE_KP_B,
    SDL_SCANCODE_KP_BACKSPACE,
    SDL_SCANCODE_KP_BINARY,
    SDL_SCANCODE_KP_C,
    SDL_SCANCODE_KP_CLEAR,
    SDL_SCANCODE_KP_CLEARENTRY,
    SDL_SCANCODE_KP_COLON,
    SDL_SCANCODE_KP_COMMA,
    SDL_SCANCODE_KP_D,
    SDL_SCANCODE_KP_DBLAMPERSAND,
    SDL_SCANCODE_KP_DECIMAL,
    SDL_SCANCODE_KP_DIVIDE,
    SDL_SCANCODE_KP_E,
    SDL_SCANCODE_KP_ENTER,
    SDL_SCANCODE_KP_EQUALS,
    SDL_SCANCODE_KP_EQUALSAS400,
    SDL_SCANCODE_KP_EXCLAM,
    SDL_SCANCODE_KP_F,
    SDL_SCANCODE_KP_GREATER,
    SDL_SCANCODE_KP_HASH,
    SDL_SCANCODE_KP_HEXADECIMAL,
    SDL_SCANCODE_KP_LEFTBRACE,
    SDL_SCANCODE_KP_LEFTPAREN,
    SDL_SCANCODE_KP_LESS,
    SDL_SCANCODE_KP_MEMADD,
    SDL_SCANCODE_KP_MEMCLEAR,
    SDL_SCANCODE_KP_MEMDIVIDE,
    SDL_SCANCODE_KP_MEMMULTIPLY,
    SDL_SCANCODE_KP_MEMRECALL,
    SDL_SCANCODE_KP_MEMSTORE,
    SDL_SCANCODE_KP_MEMSUBTRACT,
    SDL_SCANCODE_KP_MINUS,
    SDL_SCANCODE_KP_MULTIPLY,
    SDL_SCANCODE_KP_OCTAL,
    SDL_SCANCODE_KP_PERCENT,
    SDL_SCANCODE_KP_PERIOD,
    SDL_SCANCODE_KP_PLUS,
    SDL_SCANCODE_KP_PLUSMINUS,
    SDL_SCANCODE_KP_POWER,
    SDL_SCANCODE_KP_RIGHTBRACE,
    SDL_SCANCODE_KP_RIGHTPAREN,
    SDL_SCANCODE_KP_SPACE,
    SDL_SCANCODE_KP_TAB,
    SDL_SCANCODE_KP_VERTICALBAR,
    SDL_SCANCODE_KP_XOR,
    SDL_SCANCODE_LALT,
    SDL_SCANCODE_LCTRL,
    SDL_SCANCODE_LEFT,
    SDL_SCANCODE_LEFTBRACKET,
    SDL_SCANCODE_LGUI,
    SDL_SCANCODE_LSHIFT,
    SDL_SCANCODE_MAIL,
    SDL_SCANCODE_MEDIASELECT,
    SDL_SCANCODE_MENU,
    SDL_SCANCODE_MINUS,
    SDL_SCANCODE_MODE,
    SDL_SCANCODE_MUTE,
    SDL_SCANCODE_NUMLOCKCLEAR,
    SDL_SCANCODE_OPER,
    SDL_SCANCODE_OUT,
    SDL_SCANCODE_PAGEDOWN,
    SDL_SCANCODE_PAGEUP,
    SDL_SCANCODE_PASTE,
    SDL_SCANCODE_PAUSE,
    SDL_SCANCODE_PERIOD,
    SDL_SCANCODE_POWER,
    SDL_SCANCODE_PRINTSCREEN,
    SDL_SCANCODE_PRIOR,
    SDL_SCANCODE_RALT,
    SDL_SCANCODE_RCTRL,
    SDL_SCANCODE_RETURN,
    SDL_SCANCODE_RETURN2,
    SDL_SCANCODE_RGUI,
    SDL_SCANCODE_RIGHT,
    SDL_SCANCODE_RIGHTBRACKET,
    SDL_SCANCODE_RSHIFT,
    SDL_SCANCODE_SCROLLLOCK,
    SDL_SCANCODE_SELECT,
    SDL_SCANCODE_SEMICOLON,
    SDL_SCANCODE_SEPARATOR,
    SDL_SCANCODE_SLASH,
    SDL_SCANCODE_SLEEP,
    SDL_SCANCODE_SPACE,
    SDL_SCANCODE_STOP,
    SDL_SCANCODE_SYSREQ,
    SDL_SCANCODE_TAB,
    SDL_SCANCODE_THOUSANDSSEPARATOR,
    SDL_SCANCODE_UNDO,
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_UP,
    SDL_SCANCODE_VOLUMEDOWN,
    SDL_SCANCODE_VOLUMEUP,
    SDL_SCANCODE_WWW
};



INPUTSTATES InputStates;
extern VIDEO Video;

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
Seiteneffekte: InputStates.x, Video.x
------------------------------------------------------------------------------*/
void UpdateInputStates(void) {
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
    InputStates.nMouseXpos_Relative = InputStates.nMouseXpos_Absolute - Video.uXoffs;
    InputStates.nMouseYpos_Relative = InputStates.nMouseYpos_Absolute - Video.uYoffs;
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
Name:           ClearFkeys
------------------------------------------------------------------------------
Beschreibung: Setzt alle Zustände der F-Tasten auf "nicht gedrückt".

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: InputStates.x
------------------------------------------------------------------------------*/
void ClearFkeys(void) {
    uint32_t uKeyNum;

    for (uKeyNum = 1; uKeyNum <= 12; uKeyNum++) {
        InputStates.bFkey[uKeyNum] = false;
        InputStates.uLastFkeyTime[uKeyNum] = SDL_GetTicks();
    }
}


/*----------------------------------------------------------------------------
Name:           GetFkeys
------------------------------------------------------------------------------
Beschreibung: Fragt den Zustand der F-Tasten ab und setzt im Array InputStates.bFkey[x] den
              entsprechenden Wert auf true, wenn die Taste gedrückt ist.

              Hinweis 1: Der boolsche Wert InputStates.bFkey[x] muss beim Lesen entsprechend
              bestätigt werden, indem dieser auf false gesetzt wird.

              Hinweis 2: Vor Aufruf dieser Funktion muss UpdateInputStates() oder UpdateManKey()
              aufgerufen worden sein.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: InputStates.x
------------------------------------------------------------------------------*/
void GetFkeys(void) {
    uint32_t uKeyNum;
    uint32_t uTicks;

    uTicks = SDL_GetTicks();
    for (uKeyNum = 1; uKeyNum <= 12; uKeyNum++) {
        if (InputStates.pKeyboardArray[SDL_SCANCODE_F1 + uKeyNum - 1]) {
            if (uTicks - InputStates.uLastFkeyTime[uKeyNum] > 200) {
                InputStates.bFkey[uKeyNum] = true;
            }
            InputStates.uLastFkeyTime[uKeyNum] = uTicks;
        }
    }
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


/*----------------------------------------------------------------------------
Name:           GetScancodeIndex
------------------------------------------------------------------------------
Beschreibung: Ermittelt anhand einer gedrückten Taste den ScanCodeIndex. Dieser
              Index ist ein Zeiger auf die Arrays g_ScanCodeNames[] und g_uScanCodes[].
              Mit Hilfe dieses Index kann der SDL_ScanCode und dessen Name ermittelt werden.
              Hinweis: Funktion unterstützt nur eine gedrückte Taste (die erste Gefundene
              wird zurückgegeben).
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, ScanCodeIndex, -1 = keine Taste gedrückt, sonst Index
Seiteneffekte: InputStates.x, g_uScanCodes[]
------------------------------------------------------------------------------*/
int GetScancodeIndex() {
    uint32_t I;
    int  nIndex = -1;

    for (I = 0; I < (sizeof(g_uScanCodes) / sizeof(uint32_t)); I++) {
        if (InputStates.pKeyboardArray[g_uScanCodes[I]]) {
            nIndex = (int)I;
            break;
        }
    }
    return nIndex;
}


/*----------------------------------------------------------------------------
Name:           GetSdlScanCodeNameIndex
------------------------------------------------------------------------------
Beschreibung: Ermittelt anhand eines SDL_ScanCodes den zugehörigen Namen.
              Es wird ein Index zurückgegeben, mit dem über das Array g_ScanCodeNames[]
              der Name ermittelt werden kann.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, ScanCodeIndex, -1 = kein gültiger Index
Seiteneffekte: InputStates.x, g_uScanCodes[]
------------------------------------------------------------------------------*/
int GetSdlScanCodeNameIndex(uint32_t uSDL_ScanCode) {
    int nIndex = -1;
    uint32_t I;

    for (I = 0; I < (sizeof(g_uScanCodes) / sizeof(uint32_t)); I++) {
        if (uSDL_ScanCode == g_uScanCodes[I]) {
            nIndex = (int)I;
            break;
        }
    }
    return nIndex;
}
