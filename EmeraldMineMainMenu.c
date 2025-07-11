#include "gfx/textures.h"
#include <ctype.h>
#include <math.h>
#include "buttons_checkboxes.h"
#include "config.h"
#include "copper.h"
#include "editor.h"
#include "EmeraldMineMainMenu.h"
#include "gamecontroller.h"
#include "GetTextureIndexByElement.h"
#include "highscores.h"
#include "KeyboardMouse.h"
#include "loadlevel.h"
#include "man.h"
#include "modplay.h"
#include "mySDL.h"
#include "mystd.h"
#include "scroller.h"
#include "sdlmixer_SDL_mixer.h"
#include "sound.h"
#include "teleporter.h"

MAINMENU MainMenu;
extern char g_ScanCodeNames[][64];
extern uint32_t g_uScanCodes[];
extern INPUTSTATES InputStates;
extern MANKEY ManKey;
extern VIDEO Video;
extern PLAYFIELD Playfield;
extern uint32_t g_LevelgroupFilesCount;
extern NAMES Names;
extern CONFIG Config;
extern LEVELGROUPFILE LevelgroupFiles[EMERALD_MAX_LEVELGROUPFILES];
extern LEVELGROUP SelectedLevelgroup;
extern ACTUALPLAYER Actualplayer;
extern HIGHSCOREFILE HighscoreFile;
extern AUDIOPLAYER Audioplayer;
extern GAMESOUND GameSound;
extern GAMECONTROLLER GameController;
extern JOYSTICK Joystick;
extern IMPORTLEVEL ImportLevel;
extern FPS Fps;

/*----------------------------------------------------------------------------
Name:           GetRainbowColors
------------------------------------------------------------------------------
Beschreibung: Berechnung eines Regenbogen-Farbverlaufs.
              Als Eingabe dient ein Wert zwischen 0 und 254, der die Position
              im Regenbogen angibt.
              0 = oberer Bereich des Regenbogens (rot)
              254 = unterer Bereich des Regenbogens (violett)
              Quelle: https://www.mikrocontroller.net/topic/238304
              Benutzername: kbuchegg: Karl-Heinz Buchegger
              Bei ungültigen Eingabewerten wird der Regenbogen umgebrochen, d.h.
              er fängt wieder am Anfang an.
              Quelle:
              https://www.mikrocontroller.net/topic/238304
              Karl-Heinz BuchEgg
Parameter
      Eingang: uWert, uint16_t, Position im Regenbogen (0 ... 254)
      Ausgang: -
      Rückgabewert: uint32_t, RGB (höchtes Byte unbesetzt = NN)
                    NNRRGGBB
Seiteneffekte: -
------------------------------------------------------------------------------*/
uint32_t GetRainbowColors(uint16_t uWert) {
    int nRed, nGreen, nBlue;
    int nPhase;
    int nLevel;
    uint32_t uRGB = 0;

    uWert = uWert % 254;
    nPhase = uWert / 51;
    nLevel = uWert % 51;
    switch (nPhase) {
        case 0:
            nRed = 255;
            nGreen = nLevel * 5;
            nBlue = 0;
            break;
        case 1:
            nRed = 255 - nLevel * 5;
            nGreen = 255;
            nBlue = 0;
            break;
        case 2:
            nRed = 0;
            nGreen = 255;
            nBlue = nLevel * 5;
            break;
        case 3:
            nRed = 0;
            nGreen = 255 - nLevel * 5;
            nBlue = 255;
            break;
        case 4:
            nRed = nLevel * 5;
            nGreen = 0;
            nBlue  = 255;
            break;
        default:
            nRed = nGreen = nBlue = 255;
    }
    uRGB = uRGB | ((uint8_t)nRed) << 16;
    uRGB = uRGB | ((uint8_t)nGreen) << 8;
    uRGB = uRGB | (uint8_t)nBlue;
    return uRGB;
}


/*----------------------------------------------------------------------------
Name:           GetMainMenuButton
------------------------------------------------------------------------------
Beschreibung: Prüft, welcher Gfx-Button im EmeraldMinemainMenü gedrückt ist.
                Möglich sind zur Zeit folgende Buttons:
                    * Level up
                    * Level down
                    * Settings
                    * Exit
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = kein Button gedrückt,
                    1 = EMERALD_STEEL_ARROW_DOWN_PRESSED,
                    2 = EMERALD_STEEL_ARROW_UP_PRESSED
                    3 = EMERALD_STEEL_SETTINGS_PRESSED
                    4 = EMERALD_STEEL_EXIT_PRESSED
Seiteneffekte: InputStates.x, MainMenu.x
------------------------------------------------------------------------------*/
int GetMainMenuButton(void) {
    int nButton;

    nButton = 0;
    if (InputStates.bLeftMouseButton) {
        // Beide Level-Buttons auf selber Y-Position
        if ((InputStates.nMouseYpos_Absolute >= 192) && (InputStates.nMouseYpos_Absolute < 224)) {
            if ((InputStates.nMouseXpos_Relative >= 352) && (InputStates.nMouseXpos_Relative < 384)) {
                nButton = EMERALD_STEEL_ARROW_DOWN_PRESSED;
            } else if ((InputStates.nMouseXpos_Relative >= 384) && (InputStates.nMouseXpos_Relative < 416)) {
                nButton = EMERALD_STEEL_ARROW_UP_PRESSED;
            }
        }
        // Settings- und Exit-Button
        else if ((MainMenu.nState == 0) && ((InputStates.nMouseYpos_Absolute >= 0) && (InputStates.nMouseYpos_Absolute < 32))) {
            if ((InputStates.nMouseXpos_Relative >= 0) && (InputStates.nMouseXpos_Relative < 32)) {
                nButton = EMERALD_STEEL_SETTINGS_PRESSED;
            } else if ((InputStates.nMouseXpos_Relative >= 1248) && (InputStates.nMouseXpos_Relative < 1280)) {
                nButton = EMERALD_STEEL_EXIT_PRESSED;
            }
        }
    }
    return nButton;
}


/*----------------------------------------------------------------------------
Name:           GetPlayerListButton
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Spielerlisten-Button (down/up) gedrückt ist.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = kein Button gedrückt, 1 = EMERALD_STEEL_ARROW_DOWN_PRESSED, 2 = EMERALD_STEEL_ARROW_UP_PRESSED
Seiteneffekte: InputStates.x
------------------------------------------------------------------------------*/
int GetPlayerListButton(void) {
    int nButton;
    bool bMouseInNamesArea;

    bMouseInNamesArea = ((InputStates.nMouseXpos_Relative >= 32) && (InputStates.nMouseXpos_Relative < 1088) &&
                         (InputStates.nMouseYpos_Absolute >= 448) && (InputStates.nMouseYpos_Absolute < (448 + MAX_NAMES_IN_LIST * FONT_H)));
    nButton = 0;
    if (Names.uNameCount > MAX_NAMES_IN_LIST) {
        if (InputStates.bLeftMouseButton) {
            if  ((InputStates.nMouseXpos_Relative >= 1088) && (InputStates.nMouseXpos_Relative < (1088 + FONT_W))) {
                // Button Levelgruppen Pfeil hoch
                if ( (InputStates.nMouseYpos_Absolute >= 448) && (InputStates.nMouseYpos_Absolute < (448 + FONT_H))) {
                    nButton = EMERALD_STEEL_ARROW_UP_PRESSED;
                } else if ( (InputStates.nMouseYpos_Absolute >= 544) && (InputStates.nMouseYpos_Absolute < (544 + FONT_H))) {
                // Button Levelgruppen Pfeil runter
                    nButton = EMERALD_STEEL_ARROW_DOWN_PRESSED;
                }
            }
        } else if (bMouseInNamesArea) {
            if (InputStates.nMouseWheelY < 0) {
                nButton = EMERALD_STEEL_ARROW_DOWN_PRESSED;
            } else if (InputStates.nMouseWheelY > 0) {
                nButton = EMERALD_STEEL_ARROW_UP_PRESSED;
            }
        }

    }
    return nButton;
}


/*----------------------------------------------------------------------------
Name:           GetLevelgroupListButton
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Levelgruppen-Button (down/up) gedrückt ist.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = kein Button gedrückt, 1 = EMERALD_STEEL_ARROW_DOWN_PRESSED, 2 = EMERALD_STEEL_ARROW_UP_PRESSED
Seiteneffekte: InputStates.x, g_LevelgroupFilesCount, MainMenu.x
------------------------------------------------------------------------------*/
int GetLevelgroupListButton(void) {
    int nButton;
    bool bMouseInLevelgroup;

    bMouseInLevelgroup = ((InputStates.nMouseXpos_Relative >= 32) && (InputStates.nMouseXpos_Relative < 896) &&
                          (InputStates.nMouseYpos_Absolute >= 608) && (InputStates.nMouseYpos_Absolute < (608 + MainMenu.uMaxLevelgroupsInList * FONT_H)));
    nButton = 0;
    if (g_LevelgroupFilesCount > MainMenu.uMaxLevelgroupsInList) {
        if (InputStates.bLeftMouseButton) {
            if  ((InputStates.nMouseXpos_Relative >= 896) && (InputStates.nMouseXpos_Relative < (896 + FONT_W))) {
                if ( (InputStates.nMouseYpos_Absolute >= 608) && (InputStates.nMouseYpos_Absolute < (608 + FONT_H))) { // Button Levelgruppen Pfeil hoch?
                    nButton = EMERALD_STEEL_ARROW_UP_PRESSED;
                } else if ( (InputStates.nMouseYpos_Absolute >= 704) && (InputStates.nMouseYpos_Absolute < (704 + FONT_H))) { // Button Levelgruppen Pfeil runter?
                    nButton = EMERALD_STEEL_ARROW_DOWN_PRESSED;
                }
            }
        } else if (bMouseInLevelgroup) {
            if (InputStates.nMouseWheelY < 0) {
                nButton = EMERALD_STEEL_ARROW_DOWN_PRESSED;
            } else if (InputStates.nMouseWheelY > 0) {
                nButton = EMERALD_STEEL_ARROW_UP_PRESSED;
            }
        }
    }
    return nButton;
}


/*----------------------------------------------------------------------------
Name:           SetMenuText
------------------------------------------------------------------------------
Beschreibung: Setzt einen Text in einen Menüscreen. Falls das Ende einer Zeile
              erreicht wird, wird der Text abgeschnitten.
Parameter
      Eingang: pMenuScreen, uint16_t *, Zeiger auf Menüscreen
               pszText, char*, Zeiger auf Text
               nXpos, int, X-Menükoordinate (keine Pixelkoordinate)
                    wenn < 0, dann wird horizontal zentriert
               nYpos, int, Y-Menükoordinate (keine Pixelkoordinate)
                    wenn < 0, dann wird vertikal zentriert
               uFont, uint32_t, möglich ist Folgendes:
                    EMERALD_FONT_BLUE
                    EMERALD_FONT_STEEL_BLUE
                    EMERALD_FONT_GREEN
                    EMERALD_FONT_STEEL_GREEN
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int SetMenuText(uint16_t *pMenuScreen, char *pszText, int nXpos, int nYpos,uint32_t uFont) {
    int nErrorCode;
    uint32_t uXdim;
    uint32_t uYdim;
    uint32_t uStringLen;

    nErrorCode = -1;
    uXdim = DEFAULT_WINDOW_W / FONT_W;
    uYdim = DEFAULT_WINDOW_H / FONT_H;
    if ((pMenuScreen != NULL) && (pszText != NULL)) {
        uStringLen = (uint32_t)strlen(pszText);
        // bei "ungültiger" X-Position eine horizontale Zentrierung berechnen
        if ((nXpos < 0) || (nXpos >= (uXdim - 1))) {
            nXpos = (uXdim / 2) - (uStringLen / 2);
            if (nXpos< 0) {
                nXpos = 0;
            }
        }
       // bei "ungültiger" Y-Position eine vertikale Zentrierung berechnen
       if ((nYpos < 0) && (nYpos >= (uYdim - 1))) {
            nYpos = uYdim / 2;
       }
        while ( (*pszText != 0) && (nXpos < (uXdim - 1)) ) {
            if (nXpos > 0) {
                pMenuScreen[nYpos * uXdim + nXpos] = GetFontElementByChar(*pszText,uFont);
            }
            nXpos++;
            pszText++;
        }
        nErrorCode = 0;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetFontElementByChar
------------------------------------------------------------------------------
Beschreibung: Ermittelt ein Zeichensatz-Element anhand eines Zeichens.
            Diese Funktion ist leider notwendig, da beim Anlegen der Elemente
            nicht nach ASCII-Vorlage vorgegangen wurde. Die ursprünglichen
            Elemente müssen an ihrem Platz verbleiben, ansonsten werden alte
            Levelgruppen inkompatibel.
Parameter
      Eingang: sign, char, Zeichen
               uFont, uint32_t, möglich ist Folgendes:
                    EMERALD_FONT_BLUE
                    EMERALD_FONT_STEEL_BLUE
                    EMERALD_FONT_GREEN
                    EMERALD_FONT_STEEL_GREEN
      Ausgang: -
Rückgabewert:  uint32_t, Element, falls keine Konvertierung möglich, wird EMERALD_SPACE zurückgegeben
Seiteneffekte: -
------------------------------------------------------------------------------*/
uint16_t GetFontElementByChar(char sign,uint32_t uFont) {
    uint16_t uElement;

    uElement = EMERALD_SPACE;
    sign = toupper(sign);
    if ((sign >= 'A') && (sign <= 'Z')) {
        switch (uFont) {
            case (EMERALD_FONT_BLUE):
                uElement = EMERALD_FONT_BLUE_A + sign - 'A';
                break;
            case (EMERALD_FONT_BLUE_STEEL):
                uElement = EMERALD_FONT_BLUE_STEEL_A + sign - 'A';
                break;
            case (EMERALD_FONT_GREEN):
                uElement = EMERALD_FONT_GREEN_A + sign - 'A';
                break;
            case (EMERALD_FONT_GREEN_STEEL):
                uElement = EMERALD_FONT_GREEN_STEEL_A + sign - 'A';
                break;
            default:
                uElement = EMERALD_FONT_BLUE_A + sign - 'A';
                break;
        }
    } else if ((sign >= '0') && (sign <= '9')) {
        switch (uFont) {
            case (EMERALD_FONT_BLUE):
                uElement = EMERALD_FONT_BLUE_0 + sign - 0x30;
                break;
            case (EMERALD_FONT_BLUE_STEEL):
                uElement = EMERALD_FONT_BLUE_STEEL_0 + sign - 0x30;
                break;
            case (EMERALD_FONT_GREEN):
                uElement = EMERALD_FONT_GREEN_0 + sign - 0x30;
                break;
            case (EMERALD_FONT_GREEN_STEEL):
                uElement = EMERALD_FONT_GREEN_STEEL_0 + sign - 0x30;
                break;
            default:
                uElement = EMERALD_FONT_BLUE_0 + sign - 0x30;
                break;
        }
    } else {
        switch (sign) {
            case (' '):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_SPACE;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_STEEL;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_SPACE;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_STEEL;
                        break;
                    default:
                        uElement = EMERALD_SPACE;
                        break;
                }
                break;
            case ('!'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_EXCLAMATION;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_EXCLAMATION;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_EXCLAMATION;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_EXCLAMATION;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_EXCLAMATION;
                        break;
                }
                break;
            case ('>'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_ARROW_RIGHT;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_ARROW_RIGHT;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_ARROW_RIGHT;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        break;
                        uElement = EMERALD_FONT_GREEN_STEEL_ARROW_RIGHT;
                    default:
                        uElement = EMERALD_FONT_BLUE_ARROW_RIGHT;
                        break;
                }
                break;
            case ('<'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_ARROW_LEFT;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_ARROW_LEFT;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_ARROW_LEFT;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        break;
                        uElement = EMERALD_FONT_GREEN_STEEL_ARROW_LEFT;
                    default:
                        uElement = EMERALD_FONT_BLUE_ARROW_LEFT;
                        break;
                }
                break;
            case ('\''):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_APOSTROPHE;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_APOSTROPHE;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_APOSTROPHE;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_APOSTROPHE;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_APOSTROPHE;
                        break;
                }
                break;
            case ('('):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_BRACE_OPEN;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_BRACE_OPEN;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_BRACE_OPEN;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_BRACE_OPEN;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_BRACE_OPEN;
                        break;
                }
                break;
            case (')'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_BRACE_CLOSE;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_BRACE_CLOSE;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_BRACE_CLOSE;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_BRACE_CLOSE;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_BRACE_CLOSE;
                        break;
                }
                break;
            case ('+'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_PLUS;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_PLUS;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_PLUS;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_PLUS;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_PLUS;
                        break;
                }
                break;
            case (','):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_COMMA;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_COMMA;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_COMMA;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_COMMA;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_COMMA;
                        break;
                }
                break;
            case ('-'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_MINUS;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_MINUS;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_MINUS;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_MINUS;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_MINUS;
                        break;
                }
                break;
            case ('.'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_DOT;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_DOT;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_DOT;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_DOT;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_DOT;
                        break;
                }
                break;
            case ('/'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_SLASH;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_SLASH;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_SLASH;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_SLASH;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_SLASH;
                        break;
                }
                break;
            case (':'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_DOUBLE_DOT;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_DOUBLE_DOT;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_DOUBLE_DOT;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_DOUBLE_DOT;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_DOUBLE_DOT;
                        break;
                }
                break;
            case ('@'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_PLATE;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_PLATE;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_PLATE;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_PLATE;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_PLATE;
                        break;
                }
                break;
            case ('?'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_QUESTION_MARK;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_QUESTION_MARK;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_QUESTION_MARK;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_QUESTION_MARK;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_QUESTION_MARK;
                        break;
                }
                break;
            case (';'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_SEMICOLON;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_SEMICOLON;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_SEMICOLON;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_SEMICOLON;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_SEMICOLON;
                        break;
                }
                break;
            case ('\"'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_DOUBLE_QUOTE;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_DOUBLE_QUOTE;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_DOUBLE_QUOTE;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_DOUBLE_QUOTE;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_DOUBLE_QUOTE;
                        break;
                }
                break;
            case ('|'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_OE;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_OE;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_OE;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_OE;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_OE;
                        break;
                }
                break;
            case ('{'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_AE;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_AE;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_AE;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_AE;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_AE;
                        break;
                }
                break;
            case ('}'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BLUE_UE;
                        break;
                    case (EMERALD_FONT_BLUE_STEEL):
                        uElement = EMERALD_FONT_BLUE_STEEL_UE;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_UE;
                        break;
                    case (EMERALD_FONT_GREEN_STEEL):
                        uElement = EMERALD_FONT_GREEN_STEEL_UE;
                        break;
                    default:
                        uElement = EMERALD_FONT_BLUE_UE;
                        break;
                }
                break;
            case ('&'): // Smiley
                uElement = EMERALD_SMILEY;
                break;
            case ('_'): // Paff
                uElement = EMERALD_FONT_PAFF;
                break;
            case ('#'): // Man
                uElement = EMERALD_MAN;
                break;
            case ('*'):
                uElement = EMERALD_SPACE;
                break;
        }
    }
    return uElement;
}


/*----------------------------------------------------------------------------
Name:           EvaluateGame
------------------------------------------------------------------------------
Beschreibung: Bewertet das zurückliegende Spiel und speichert neue Werte
              (Totalscore, Handicap usw.) für den aktuellen Spieler.
Parameter
      Eingang: -
      Ausgang: pnNewHighscoreIndex, int *, HighscoreIndex (>= 0), -1 = kein neuer Index
               puLevelPlayed, uint32_t *, Zeiger auf Levelnummer, in der ein Highscore erreicht wurde
Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: Playfield.x, Actualplayer.x, SelectedLevelgroup.x
------------------------------------------------------------------------------*/
int EvaluateGame(int *pnNewHighscoreIndex, uint32_t *puLevelPlayed) {
    int nErrorCode;
    uint32_t uLevelPlayed;

    nErrorCode = -1;
    if ((pnNewHighscoreIndex != NULL) && (puLevelPlayed != NULL)) {
        *pnNewHighscoreIndex = -1;
        uLevelPlayed = Actualplayer.uLevel;
        Actualplayer.uTotalScore = Actualplayer.uTotalScore + Playfield.uTotalScore;
        Actualplayer.uGamesPlayed++;
        Actualplayer.uPlayTimeS = Actualplayer.uPlayTimeS + (Playfield.uPlayTimeEnd - Playfield.uPlayTimeStart) / 1000;
        if (Playfield.bWellDone) {
            Actualplayer.uGamesWon++;
            if (Actualplayer.uLevel == Actualplayer.uHandicap) {
                if (Actualplayer.uHandicap < (SelectedLevelgroup.uLevelCount - 1)) {
                    Actualplayer.uHandicap++;
                    Actualplayer.uLevel = Actualplayer.uHandicap;
                }
            }
        }
        nErrorCode = InsertGamesValuesIntoNamesFile(Actualplayer.szPlayername,SelectedLevelgroup.uMd5Hash);
        if (nErrorCode == 0) {
            nErrorCode = WriteNamesFile();
            if (nErrorCode == 0) {
                if (Playfield.uTotalScore > 0) {
                    nErrorCode = InsertScore(Actualplayer.szPlayername,uLevelPlayed,Playfield.uTotalScore,Playfield.bWellDone);
                    if (nErrorCode >= 0) {  // Gab es einen neuen Highscore?
                        *pnNewHighscoreIndex = nErrorCode;
                        *puLevelPlayed = uLevelPlayed;
                        nErrorCode = WriteHighScoreFile(SelectedLevelgroup.uMd5Hash);
                    } else if (nErrorCode == -2) {  // Alles OK, aber kein neuer Highscore
                        nErrorCode = 0;
                    }
                }
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           MenuSelectLevelgroup
------------------------------------------------------------------------------
Beschreibung: Erledigt die Auswahl einer Levelgruppe aus dem Hauptmenü und
              blendet die Balken für die Auswahl einer Levelgruppe ein, wenn sich
              der Mauspfeil über den Namen einer Gruppe befindet.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: Actualplayer.x, SelectedLevelgroup.x, InputStates.x, MainMenu.x, Video.x
------------------------------------------------------------------------------*/
int MenuSelectLevelgroup(SDL_Renderer *pRenderer) {
    int nErrorCode = 0;
    uint32_t uBeamPosition;
    char szPlayername[EMERALD_PLAYERNAME_LEN + 1];

    uBeamPosition = GetLevelgroupBeamPosition();
    if (uBeamPosition != 0xFFFFFFFF) {
        nErrorCode = DrawBeam(pRenderer,FONT_W + Video.uXoffs,608 + FONT_H * uBeamPosition, DEFAULT_WINDOW_W - 2 * FONT_W, FONT_H, 0x20,0x20,0xF0,0xC0,K_ABSOLUTE);
        if ((InputStates.bLeftMouseButton) && (nErrorCode == 0)) {
            if (SelectAlternativeLevelgroup(LevelgroupFiles[MainMenu.pLevelgroupList[uBeamPosition]].uMd5Hash,true) == 0) {
                // SDL_Log("Select %s, OK",SelectedLevelgroup.szLevelgroupname);
                if (Actualplayer.bValid) {  // Der aktuelle Name wird nochmals ausgewählt, damit dieser ggf. den Levelgruppen-Hash bekommt
                    strcpy(szPlayername,Actualplayer.szPlayername); // Muss kopiert werden, da Selectname die Struktur Actualplayer.x löscht
                    // SDL_Log("%s: selecting name: %s",__FUNCTION__,szPlayername);
                    nErrorCode = SelectName(szPlayername,SelectedLevelgroup.uMd5Hash);
                    if (nErrorCode != 0) {
                        SDL_Log("%s: Selectname() failed",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: no valid name for actual player, ignoring ...",__FUNCTION__);
                }
            } else {
                // Falls das nicht klappt, liegt ein schwerer Fehler vor -> beenden
                nErrorCode = -1;
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Levelgroup problem","Can not select levelgroup!",NULL);
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           MenuSelectName
------------------------------------------------------------------------------
Beschreibung: Erledigt die Auswahl eines Namens aus dem Hauptmenü und
              blendet die Balken für die Auswahl eines Namens an, wenn sich
              der Mauspfeil über einem Namen befindet.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: MainMenu.x, Names.x, SelectedLevelgroup.x, InputStates.x, Video.x
------------------------------------------------------------------------------*/
int MenuSelectName(SDL_Renderer *pRenderer) {
    int nErrorCode = 0;
    uint32_t uBeamPosition;

    uBeamPosition = GetNamesBeamPosition();
    if (uBeamPosition != 0xFFFFFFFF) {
        nErrorCode = DrawBeam(pRenderer,Video.uXoffs + FONT_W,448 + FONT_H * uBeamPosition, DEFAULT_WINDOW_W - 7 * FONT_W, FONT_H, 0x20,0x20,0xF0,0xC0,K_ABSOLUTE);
        if ((InputStates.bLeftMouseButton) && (nErrorCode == 0)) {
            if (SelectName(Names.Name[MainMenu.uNamesList[uBeamPosition]].szName,SelectedLevelgroup.uMd5Hash) == 0) {
                // SDL_Log("select name: %s, OK",Names.Name[MainMenu.uNamesList[uBeamPosition]].szName);
                strcpy(Config.szPlayername,Names.Name[MainMenu.uNamesList[uBeamPosition]].szName);
                memcpy(Config.uLevelgroupMd5Hash,SelectedLevelgroup.uMd5Hash,16);
                nErrorCode = WriteConfigFile();
            } else {
                SDL_Log("select name: %s, ERROR",Names.Name[MainMenu.uNamesList[uBeamPosition]].szName);
                nErrorCode = -1;
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           InitLists
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Anzeigelisten für die Levelgruppen und Namen.
              Vor Aufruf dieser Funktion müssen die Funktionen
               GetLevelgroupFiles()   und
               ReadNamesFile()
              erfolgreich aufgerufen worden sein.
              Die Level-Titel-Liste wird ebenfalls initialisiert, wenn
              eine Levelgruppe ausgewählt ist.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: MainMenu.x, g_LevelgroupFilesCount, SelectedLevelgroup.x
               ImportLevel.x
------------------------------------------------------------------------------*/
void InitLists(void) {
    uint32_t I;

    // Levelgruppenliste vorbereiten
    memset(MainMenu.pLevelgroupList,0xFF,MainMenu.uMaxLevelgroupsInList);
    for (I = 0; (I < MainMenu.uMaxLevelgroupsInList) && (I < g_LevelgroupFilesCount); I++) {
        MainMenu.pLevelgroupList[I] = I;
    }
    // Namensliste vorbereiten
    memset(MainMenu.uNamesList,0xFF,sizeof(MainMenu.uNamesList));
    for (I = 0; (I < MAX_NAMES_IN_LIST) && (I < Names.uNameCount); I++) {
        MainMenu.uNamesList[I] = I;
    }
    // Level-Titel-Liste initialisieren
    InitLevelTitleList();
}


/*----------------------------------------------------------------------------
Name:           ActivateInputPlayernameMode
------------------------------------------------------------------------------
Beschreibung: Aktiviert die Namenseingabe zum Hinzufügen eines neuen Spielernamens.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: MainMenu.x
------------------------------------------------------------------------------*/
void ActivateInputPlayernameMode(void) {
    MainMenu.nState = 1; // Status auf Namen-Eingabemodus stellen
    SetMenuText(MainMenu.pMenuScreen,"                               ",7,5,EMERALD_FONT_GREEN);
    SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,false);
    MainMenu.pMenuScreen[13 * MainMenu.uXdim + 75] = EMERALD_SPACE;
    MainMenu.pMenuScreen[13 * MainMenu.uXdim + 76] = EMERALD_SPACE;
    MainMenu.pMenuScreen[13 * MainMenu.uXdim + 77] = EMERALD_SPACE;
    MainMenu.pMenuScreen[13 * MainMenu.uXdim + 78] = EMERALD_SPACE;
}


/*----------------------------------------------------------------------------
Name:           PreInitMainMenu
------------------------------------------------------------------------------
Beschreibung: Initialisiert den Menu-Screen-Speicher auf NULL und die aktuelle
              Menu-Screen-Speichergröße auf 0.
              Diese Funktion darf nur einmalig zu Programmstart aufgerufen werden.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: MainMenu.x
------------------------------------------------------------------------------*/
void PreInitMainMenu(void) {
    MainMenu.pMenuScreen = NULL;
    MainMenu.pLevelgroupList = NULL;
    MainMenu.puLevelTitleList = NULL;
    MainMenu.puLevelTitleListCopy = NULL;
    MainMenu.uMaxLevelTitlesInList = 0;
    MainMenu.uMaxLevelgroupsInList = 0;
    MainMenu.bCenterDefault = false;
}


/*----------------------------------------------------------------------------
Name:           InitMainMenu
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Struktur MainMenu.x.
              Diese Funktion alloziert Speicher für die Menu-Screens.
              Dieser ist abhängig von der Y-Auflösung des Fensters/Screens.
Parameter
      Eingang: bCenterDefault, bool, true = es handelt sich um ein zentriertes Default-Fenster (1280x768)
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: MainMenu.x, Config.x, Video.x
------------------------------------------------------------------------------*/
int InitMainMenu(bool bCenterDefault) {
    uint32_t uNewMenuScreenMemorySize;
    int nErrorCode = 0;

    MainMenu.bCenterDefault = bCenterDefault;
    MainMenu.uXdim = DEFAULT_WINDOW_W / FONT_W;
    if (bCenterDefault) {
        MainMenu.uYdim = DEFAULT_WINDOW_H / FONT_H;
    } else {
        MainMenu.uYdim = Config.uResY / FONT_H;
    }
    // Sicherheitsabfrage
    if (MainMenu.uYdim >= DEFAULT_WINDOW_H / FONT_H) {
        // Maximale Anzahl der zu sehenden Levelgruppen in der Liste
        MainMenu.uMaxLevelgroupsInList = MIN_LEVELGROUPS_IN_LIST + MainMenu.uYdim - DEFAULT_WINDOW_H / FONT_H;
        uNewMenuScreenMemorySize = ((DEFAULT_WINDOW_W / FONT_W) * (Config.uResY / FONT_H)) * sizeof(uint16_t);
        MainMenu.pMenuScreen = realloc(MainMenu.pMenuScreen,uNewMenuScreenMemorySize);
        if (MainMenu.pMenuScreen != NULL) {
            MainMenu.pLevelgroupList = realloc(MainMenu.pLevelgroupList,MainMenu.uMaxLevelgroupsInList);
            if (MainMenu.pLevelgroupList != NULL) {
                MainMenu.nState = 0;             // 1 = Eingabe eines neuen Namens
                Video.uXoffs = (Config.uResX - DEFAULT_WINDOW_W) / 2;
                Video.uYoffs = (Config.uResY - DEFAULT_WINDOW_H) / 2;
                memset(MainMenu.szTempName,0,sizeof(MainMenu.szTempName));
                MainMenu.uFlashIndex = 0;
                MainMenu.uCursorPos = 0;
                MainMenu.uMaxFlashIndex = sizeof(MainMenu.uFlashBrightness) - 1;
                memcpy(MainMenu.uFlashBrightness,"\x00\10\x14\x19\x1E\x28\x32\x4B\x5A\x5F\x64\x5F\x5A\x4B\x32\x28\x1E\x19\x14\x0A",sizeof(MainMenu.uFlashBrightness));// Für den Stein-Cursor
                // Ab hier Anzahl Level-Titel berechnen für Levelauswahl
                // - 4: Die ersten 3 Zeilen und letzte Zeile sind Mauer
                // jeweils 10 Pixel oben und unten (20) Abstand halten
                // Eine Zeile nimmt 20 Pixel ein
                MainMenu.uMaxLevelTitlesInList = (((MainMenu.uYdim - 4) * FONT_H) - 20) / 20;
                // Speicher für Level-Titel-Liste und Kopie allozieren
                MainMenu.puLevelTitleList = realloc(MainMenu.puLevelTitleList,MainMenu.uMaxLevelTitlesInList * sizeof(uint16_t));
                if (MainMenu.puLevelTitleList != NULL) {
                    MainMenu.puLevelTitleListCopy = realloc(MainMenu.puLevelTitleListCopy,MainMenu.uMaxLevelTitlesInList * sizeof(uint16_t));
                    if (MainMenu.puLevelTitleListCopy == NULL) {
                        SDL_Log("%s: realloc() for leveltitle list [copy] failed, titles: %u",__FUNCTION__,MainMenu.uMaxLevelTitlesInList);
                        nErrorCode = -1;
                    }
                } else {
                    SDL_Log("%s: realloc() for leveltitle list failed, titles: %u",__FUNCTION__,MainMenu.uMaxLevelTitlesInList);
                    nErrorCode = -1;
                }
            } else {
                SDL_Log("%s: realloc() for levelgroup list failed: memory size: %u",__FUNCTION__,MainMenu.uMaxLevelgroupsInList);
                nErrorCode = -1;
            }
        } else {
            SDL_Log("%s: realloc() for menu screen failed: memory size: %u",__FUNCTION__,uNewMenuScreenMemorySize);
            nErrorCode = -1;
        }
    } else {
        // sollte nie eintreten, dann wäre die minimale Y-Auflösung unterschritten und zu wenig Platz
        MainMenu.uMaxLevelgroupsInList = MIN_LEVELGROUPS_IN_LIST;
        SDL_Log("%s: vertical resolution to small: %u",__FUNCTION__,Config.uResY);
        nErrorCode = -1;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ScrollLevelGroups
------------------------------------------------------------------------------
Beschreibung: Scrollt die Levelgruppen im Hauptmenü.
Parameter
      Eingang: nButton, int, Levelgruppenlisten-Button, der gedrückt wurde
                        0 = kein Button gedrückt, 1 = EMERALD_STEEL_ARROW_DOWN_PRESSED, 2 = EMERALD_STEEL_ARROW_UP_PRESSED
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: MainMenu.x, g_LevelgroupFilesCount
------------------------------------------------------------------------------*/
void ScrollLevelGroups(int nButton) {
    uint32_t I;

    if (g_LevelgroupFilesCount > MainMenu.uMaxLevelgroupsInList) {
        if (nButton == EMERALD_STEEL_ARROW_UP_PRESSED) {            // Button Levelgruppen Pfeil hoch?
            if (MainMenu.pLevelgroupList[0] > 0) {
                for (I = 0; I < MainMenu.uMaxLevelgroupsInList; I++) {
                    if (MainMenu.pLevelgroupList[I] != 0xFF) {
                        MainMenu.pLevelgroupList[I]--;
                    }
                }
            }

        } else if (nButton == EMERALD_STEEL_ARROW_DOWN_PRESSED) {   // Button Levelgruppen Pfeil runter?
            if (MainMenu.pLevelgroupList[MainMenu.uMaxLevelgroupsInList - 1] < (g_LevelgroupFilesCount - 1)) {
                for (I = 0; I < MainMenu.uMaxLevelgroupsInList; I++) {
                    if (MainMenu.pLevelgroupList[I] != 0xFF) {
                        MainMenu.pLevelgroupList[I]++;
                    }
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ScrollPlayernames
------------------------------------------------------------------------------
Beschreibung: Scrollt die Playernamen im Hauptmenü.
Parameter
      Eingang: nButton, int, Playerlisten-Button, der gedrückt wurde
                        0 = kein Button gedrückt, 1 = EMERALD_STEEL_ARROW_DOWN_PRESSED, 2 = EMERALD_STEEL_ARROW_UP_PRESSED
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: MainMenu.x, Names.x
------------------------------------------------------------------------------*/
void ScrollPlayernames(int nButton) {
    uint32_t I;

    if (Names.uNameCount > MAX_NAMES_IN_LIST) {
        if (nButton == EMERALD_STEEL_ARROW_UP_PRESSED) {  // Button Levelgruppen Pfeil hoch?
            if (MainMenu.uNamesList[0] > 0) {
                for (I = 0; I < MAX_NAMES_IN_LIST; I++) {
                    if (MainMenu.uNamesList[I] != 0xFF) {
                        MainMenu.uNamesList[I]--;
                    }
                }
            }
        } else if (nButton == EMERALD_STEEL_ARROW_DOWN_PRESSED) {   // Button Levelgruppen Pfeil runter?
            if (MainMenu.uNamesList[MAX_NAMES_IN_LIST - 1] < (Names.uNameCount - 1)) {
                for (I = 0; I < MAX_NAMES_IN_LIST; I++) {
                    if (MainMenu.uNamesList[I] != 0xFF) {
                        MainMenu.uNamesList[I]++;
                    }
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           RenderMenuElements
------------------------------------------------------------------------------
Beschreibung: Alle Menü-Elemente im Menü-Screen pMenuScreen werden in den Renderer geschrieben.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Playfield.x, MainMenu.x, Video.x
------------------------------------------------------------------------------*/
int RenderMenuElements(SDL_Renderer *pRenderer) {
    uint32_t I;
    int nErrorCode = 0;
    uint32_t uTextureIndex;
    uint32_t X,Y;
    float fAngle;
    SDL_Rect DestR;

    for (I = 0; ((I < ((DEFAULT_WINDOW_W / FONT_W) * MainMenu.uYdim)) && (nErrorCode == 0)); I++) {
        X = I % MainMenu.uXdim;
        Y = I / MainMenu.uXdim;
        uTextureIndex = GetTextureIndexByElement(MainMenu.pMenuScreen[I],Playfield.uFrameCounter % 16,&fAngle);
        DestR.x = Video.uXoffs + X * FONT_W;
        if (MainMenu.bCenterDefault) {
            DestR.y = Video.uYoffs + Y * FONT_H;
        } else {
            DestR.y = Y * FONT_H;
        }
        DestR.w = FONT_W;
        DestR.h = FONT_H;
        if (nErrorCode == 0) {
            if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle,NULL, SDL_FLIP_NONE) != 0) {
                nErrorCode = -1;
                SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetNamesBeamPosition
------------------------------------------------------------------------------
Beschreibung: Ermittelt, ob und wo ein Balken eingeblendet werden muss, wenn der Mauspfeil sich über
              der Liste der Spieler-Namen befindet.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  uint32_t, 0xFFFFFFFF = keine Einblendung, sonst 0 bis x für einen der Spieler Namen
Seiteneffekte: InputStates.x, MainMenu.x
------------------------------------------------------------------------------*/
uint32_t GetNamesBeamPosition(void) {
    uint32_t uBeamPosition = 0xFFFFFFFF;
    uint32_t bMouseFound = false;
    uint32_t I;

    // Prüfen, ob Mauspfeil über Spielernamen-Liste steht und ggf. Balken einblenden
    if ((InputStates.nMouseXpos_Relative >= FONT_W) && (InputStates.nMouseXpos_Relative < 1088)) {
        for (I = 0; (I < MAX_NAMES_IN_LIST) && (!bMouseFound); I++) {
            if ((InputStates.nMouseYpos_Absolute >= (448 + (I * FONT_H))) && (InputStates.nMouseYpos_Absolute < (480 + (I * FONT_H)))) {
                if (MainMenu.uNamesList[I] != 0xFF) {
                    uBeamPosition = I;
                    bMouseFound =  true;
                }
            }
        }
    }
    return uBeamPosition;
}


/*----------------------------------------------------------------------------
Name:           GetLevelgroupBeamPosition
------------------------------------------------------------------------------
Beschreibung: Ermittelt, ob und wo ein Balken eingeblendet werden muss, wenn der Mauspfeil sich über
              der Liste der Levelgruppen-Namen befindet.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  uint32_t, 0xFFFFFFFF = keine Einblendung, sonst 0 bis x für einen der Levelgruppen-Namen
Seiteneffekte: InputStates.x, MainMenu.x
------------------------------------------------------------------------------*/
uint32_t GetLevelgroupBeamPosition(void) {
    uint32_t uBeamPosition = 0xFFFFFFFF;
    uint32_t bMouseFound = false;
    uint32_t I;

    // Prüfen, ob Mauspfeil über Levelgruppenliste steht und ggf. Balken einblenden
    if ((InputStates.nMouseXpos_Relative >= FONT_W) && (InputStates.nMouseXpos_Relative < 896)) {
        for (I = 0; (I < MainMenu.uMaxLevelgroupsInList) && (!bMouseFound); I++) {
            if ((InputStates.nMouseYpos_Absolute >= (608 + (I * FONT_H))) && (InputStates.nMouseYpos_Absolute < (640 + (I * FONT_H)))) {
                if (MainMenu.pLevelgroupList[I] != 0xFF) {
                    uBeamPosition = I;
                    bMouseFound = true;
                }
            }
        }
    }
    return uBeamPosition;
}


/*----------------------------------------------------------------------------
Name:           SetMenuBorderAndClear
------------------------------------------------------------------------------
Beschreibung: Löscht das Menü und zeichnet die Umrandung.

Parameter
      Eingang: -
      Ausgang: -

Rückgabewert:  -
Seiteneffekte: MainMenu.x
------------------------------------------------------------------------------*/
void SetMenuBorderAndClear(void) {
    uint32_t I;

    for (I = 0; I < ((DEFAULT_WINDOW_W / FONT_W) * MainMenu.uYdim); I++) {
        MainMenu.pMenuScreen[I] = EMERALD_SPACE;
    }
    // Border zeichnen
    for (I = 0; I < MainMenu.uXdim; I++) {
        MainMenu.pMenuScreen[I] = EMERALD_STEEL; // Stahl oben
        MainMenu.pMenuScreen[I + MainMenu.uXdim * (MainMenu.uYdim - 1)] = EMERALD_STEEL; // Stahl unten
    }
    for (I = 0; I < MainMenu.uYdim; I++) {
        MainMenu.pMenuScreen[I * MainMenu.uXdim] = EMERALD_STEEL; // Stahl links
        MainMenu.pMenuScreen[I * MainMenu.uXdim + MainMenu.uXdim - 1] = EMERALD_STEEL; // Stahl rechts
    }
}

/*----------------------------------------------------------------------------
Name:           SetStaticMenuElements
------------------------------------------------------------------------------
Beschreibung: Setzt statische Elemente für das Emerald-Mine-Hauptmenü.

Parameter
      Eingang: -
      Ausgang: -

Rückgabewert:  -
Seiteneffekte: SelectedLevelgroup.x, MainMenu.x, Config.x
------------------------------------------------------------------------------*/
void SetStaticMenuElements(void) {
    uint32_t I;

    SetMenuBorderAndClear();
    MainMenu.pMenuScreen[39] = EMERALD_STEEL_EXIT;
    // Rubine oben rechts/links
    MainMenu.pMenuScreen[2 * MainMenu.uXdim + 2] = EMERALD_RUBY;
    MainMenu.pMenuScreen[2 * MainMenu.uXdim + 3] = EMERALD_RUBY;
    MainMenu.pMenuScreen[2 * MainMenu.uXdim + 4] = EMERALD_RUBY;
    MainMenu.pMenuScreen[2 * MainMenu.uXdim + 35] = EMERALD_RUBY;
    MainMenu.pMenuScreen[2 * MainMenu.uXdim + 36] = EMERALD_RUBY;
    MainMenu.pMenuScreen[2 * MainMenu.uXdim + 37] = EMERALD_RUBY;
    SetMenuText(MainMenu.pMenuScreen,"WELCOME TO EMERALD MINE SDL2",6,2,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.pMenuScreen,"LEVELGROUP:",2,4,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.pMenuScreen,SelectedLevelgroup.szLevelgroupname,13,4,EMERALD_FONT_GREEN);
    //SetMenuText(MainMenu.pMenuScreen,"1234567890123456789012345",13,4,EMERALD_FONT_GREEN);
    SetMenuText(MainMenu.pMenuScreen,"NAME:",2,5,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.pMenuScreen,"LEVEL:",2,6,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.pMenuScreen,"TOTALSCORE:",2,7,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.pMenuScreen,"GAMES PLAYED:",2,8,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.pMenuScreen,"PLAYTIME:",2,9,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.pMenuScreen,"GAMES WON:",2,10,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.pMenuScreen,"HANDICAP:",2,11,EMERALD_FONT_BLUE);
    // Seitenwand Levelgroup (ohne Pfeile)
    MainMenu.pMenuScreen[20 * MainMenu.uXdim + 28] = EMERALD_STEEL;
    MainMenu.pMenuScreen[21 * MainMenu.uXdim + 28] = EMERALD_STEEL;
    MainMenu.pMenuScreen[22 * MainMenu.uXdim + 28] = EMERALD_STEEL;
    for (I = 0; I < (MainMenu.uMaxLevelgroupsInList - MIN_LEVELGROUPS_IN_LIST); I++) {
        MainMenu.pMenuScreen[(23 + I) * MainMenu.uXdim + 28] = EMERALD_STEEL;
    }
    // Seitenwand Player (ohne Pfeile)
    MainMenu.pMenuScreen[15 * MainMenu.uXdim + 34] = EMERALD_STEEL;
    MainMenu.pMenuScreen[16 * MainMenu.uXdim + 34] = EMERALD_STEEL;
    // Button-Untergrund "CREATE PLAYER"
    MainMenu.pMenuScreen[13 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
    MainMenu.pMenuScreen[13 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
    MainMenu.pMenuScreen[13 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
    MainMenu.pMenuScreen[13 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
    // Button-Untergrund "LEVELEDITOR"
    if (Config.uResY >= MIN_Y_RESOLUTION_FOR_LEVELEDITOR) {
        MainMenu.pMenuScreen[15 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
        MainMenu.pMenuScreen[15 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
        MainMenu.pMenuScreen[15 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
        MainMenu.pMenuScreen[15 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
    } else {
        MainMenu.pMenuScreen[15 * MainMenu.uXdim + 75] = EMERALD_LEVELEDITOR_MESSAGE_1_4;
        MainMenu.pMenuScreen[15 * MainMenu.uXdim + 76] = EMERALD_LEVELEDITOR_MESSAGE_2_4;
        MainMenu.pMenuScreen[15 * MainMenu.uXdim + 77] = EMERALD_LEVELEDITOR_MESSAGE_3_4;
        MainMenu.pMenuScreen[15 * MainMenu.uXdim + 78] = EMERALD_LEVELEDITOR_MESSAGE_4_4;
    }
    // Button-Untergrund "HIGHSCORES"
    MainMenu.pMenuScreen[16 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
    MainMenu.pMenuScreen[16 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
    MainMenu.pMenuScreen[16 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
    MainMenu.pMenuScreen[16 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
}


/*----------------------------------------------------------------------------
Name:           SetDynamicMenuElements
------------------------------------------------------------------------------
Beschreibung: Setzt dynamische Elemente für das Emerald-Mine-Hauptmenü.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Names.x, g_LevelgroupFilesCount, MainMenu.x, Actualplayer.x, Config.x
------------------------------------------------------------------------------*/
int SetDynamicMenuElements(void) {
    char szText[256];
    int nErrorCode = 0;
    bool bShowLeveleditorButton;
    uint32_t I;
    uint32_t uKey;

    bShowLeveleditorButton = (Config.uResY >= MIN_Y_RESOLUTION_FOR_LEVELEDITOR);
    sprintf(szText," AVAILABLE LEVELGROUPS:%03u    LEVELS  ",g_LevelgroupFilesCount);
    SetMenuText(MainMenu.pMenuScreen,szText,1,18,EMERALD_FONT_BLUE_STEEL);
    sprintf(szText,"      AVAILABLE PLAYERS:%03u           ",Names.uNameCount);
    SetMenuText(MainMenu.pMenuScreen,szText,1,13,EMERALD_FONT_BLUE_STEEL);
    // Zeigt mindestens 4 (MIN_LEVELGROUPS_IN_LIST) Levelgruppen mit Levelanzahl im unteren Bereich an
    for (I = 0; I < MainMenu.uMaxLevelgroupsInList; I++) {
        if (MainMenu.pLevelgroupList[I] != 0xFF) {
            SetMenuText(MainMenu.pMenuScreen,"                         ",2,19 + I,EMERALD_FONT_BLUE);
            SetMenuText(MainMenu.pMenuScreen,LevelgroupFiles[MainMenu.pLevelgroupList[I]].szLevelgroupname,2,19 + I,EMERALD_FONT_BLUE);
            sprintf(szText,"%04u",LevelgroupFiles[MainMenu.pLevelgroupList[I]].uLevelCount);
            SetMenuText(MainMenu.pMenuScreen,szText,32,19 + I,EMERALD_FONT_BLUE);
        }
    }
    // Hier die Pfeile der Levelgruppenauswahl, wenn mehr als anzeigbare Levelgruppen verfügbar sind
    if (g_LevelgroupFilesCount > MainMenu.uMaxLevelgroupsInList) {
        MainMenu.pMenuScreen[19 * MainMenu.uXdim + 28] = EMERALD_STEEL_ARROW_UP;
        MainMenu.pMenuScreen[22 * MainMenu.uXdim + 28] = EMERALD_STEEL_ARROW_DOWN;
    } else {
        MainMenu.pMenuScreen[19 * MainMenu.uXdim + 28] = EMERALD_STEEL;
        MainMenu.pMenuScreen[22 * MainMenu.uXdim + 28] = EMERALD_STEEL;
    }
    // Zeigt bis zu 4 Namen im mittleren Bereich an
    //SetMenuText(MainMenu.pMenuScreen,"1234567890123456789012345678901",2,13,EMERALD_FONT_BLUE);
    for (I = 0; I < MAX_NAMES_IN_LIST; I++) {
        if (MainMenu.uNamesList[I] != 0xFF) {
            SetMenuText(MainMenu.pMenuScreen,"                               ",2,14 + I,EMERALD_FONT_BLUE);
            SetMenuText(MainMenu.pMenuScreen,Names.Name[MainMenu.uNamesList[I]].szName,2,14 + I,EMERALD_FONT_BLUE);
        } else {
            SetMenuText(MainMenu.pMenuScreen,"                               ",2,14 + I,EMERALD_FONT_BLUE);
        }
    }
    if (Names.uNameCount > MAX_NAMES_IN_LIST) {
        MainMenu.pMenuScreen[14 * MainMenu.uXdim + 34] = EMERALD_STEEL_ARROW_UP;
        MainMenu.pMenuScreen[17 * MainMenu.uXdim + 34] = EMERALD_STEEL_ARROW_DOWN;
    } else {
        MainMenu.pMenuScreen[14 * MainMenu.uXdim + 34] = EMERALD_STEEL;
        MainMenu.pMenuScreen[17 * MainMenu.uXdim + 34] = EMERALD_STEEL;
    }
    SetMenuText(MainMenu.pMenuScreen,"-       ",8,6,EMERALD_FONT_GREEN);   // Level
    SetMenuText(MainMenu.pMenuScreen,"-       ",13,7,EMERALD_FONT_GREEN);  // Totalscore
    SetMenuText(MainMenu.pMenuScreen,"-       ",15,8,EMERALD_FONT_GREEN);  // Games Played
    SetMenuText(MainMenu.pMenuScreen,"-                   ",11,9,EMERALD_FONT_GREEN);  // Playtime
    SetMenuText(MainMenu.pMenuScreen,"-       ",12,10,EMERALD_FONT_GREEN); // Games Won
    SetMenuText(MainMenu.pMenuScreen,"-       ",11,11,EMERALD_FONT_GREEN); // Handicap
    SetMenuText(MainMenu.pMenuScreen,"                               ",7,5,EMERALD_FONT_GREEN); // ausgewählter Playername
    if (MainMenu.nState == 0) {
        if (Names.uNameCount > 0) {
            if (Actualplayer.bValid) {
                SetMenuText(MainMenu.pMenuScreen,Actualplayer.szPlayername,7,5,EMERALD_FONT_GREEN);
                sprintf(szText,"%u       ",Actualplayer.uLevel);
                SetMenuText(MainMenu.pMenuScreen,szText,8,6,EMERALD_FONT_GREEN);
                //MainMenu.pMenuScreen[6 * MainMenu.uXdim + 8] = EMERALD_FONT_STEEL_ARROW_LEFT;
                //MainMenu.pMenuScreen[6 * MainMenu.uXdim + 12] = EMERALD_FONT_STEEL_ARROW_RIGHT;
                MainMenu.pMenuScreen[6 * MainMenu.uXdim + 11] = EMERALD_STEEL_ARROW_DOWN;
                MainMenu.pMenuScreen[6 * MainMenu.uXdim + 12] = EMERALD_STEEL_ARROW_UP;
                sprintf(szText,"%u       ",Actualplayer.uTotalScore);
                SetMenuText(MainMenu.pMenuScreen,szText,13,7,EMERALD_FONT_GREEN);
                sprintf(szText,"%u       ",Actualplayer.uGamesPlayed);
                SetMenuText(MainMenu.pMenuScreen,szText,15,8,EMERALD_FONT_GREEN);
                sprintf(szText,"%f DAYS      ",(float)Actualplayer.uPlayTimeS / 86400);
                SetMenuText(MainMenu.pMenuScreen,szText,11,9,EMERALD_FONT_GREEN);
                sprintf(szText,"%u       ",Actualplayer.uGamesWon);
                SetMenuText(MainMenu.pMenuScreen,szText,12,10,EMERALD_FONT_GREEN);
                sprintf(szText,"%u       ",Actualplayer.uHandicap);
                SetMenuText(MainMenu.pMenuScreen,szText,11,11,EMERALD_FONT_GREEN);
                SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,true);
                MainMenu.pMenuScreen[14 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
                MainMenu.pMenuScreen[14 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
                MainMenu.pMenuScreen[14 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
                MainMenu.pMenuScreen[14 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
            } else {
                SetMenuText(MainMenu.pMenuScreen,"PLEASE SELECT PLAYER",7,5,EMERALD_FONT_GREEN);
                SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,false);
                MainMenu.pMenuScreen[14 * MainMenu.uXdim + 75] = EMERALD_SPACE;
                MainMenu.pMenuScreen[14 * MainMenu.uXdim + 76] = EMERALD_SPACE;
                MainMenu.pMenuScreen[14 * MainMenu.uXdim + 77] = EMERALD_SPACE;
                MainMenu.pMenuScreen[14 * MainMenu.uXdim + 78] = EMERALD_SPACE;
            }
        } else {
            SetMenuText(MainMenu.pMenuScreen,"PLEASE CREATE A PLAYER",7,5,EMERALD_FONT_GREEN);
            // Delete Player
            MainMenu.pMenuScreen[14 * MainMenu.uXdim + 75] = EMERALD_SPACE;
            MainMenu.pMenuScreen[14 * MainMenu.uXdim + 76] = EMERALD_SPACE;
            MainMenu.pMenuScreen[14 * MainMenu.uXdim + 77] = EMERALD_SPACE;
            MainMenu.pMenuScreen[14 * MainMenu.uXdim + 78] = EMERALD_SPACE;
            SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,false);
        }
    } else if (MainMenu.nState == 1) {
        if (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) {  // Verlassen der Namens-Eingabe
            nErrorCode = SDL_SetTextureColorMod(GetTextureByIndex(TEX_STONE),100,100,100); // Cursor-Stein auf volle Helligkeit stellen
            SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,true);
            MainMenu.pMenuScreen[13 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
            MainMenu.pMenuScreen[13 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
            MainMenu.pMenuScreen[13 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
            MainMenu.pMenuScreen[13 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
            if (bShowLeveleditorButton) {
                SetButtonActivity(BUTTONLABEL_LEVELEDITOR,true);
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
            } else {
                SetButtonActivity(BUTTONLABEL_LEVELEDITOR,false);
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 75] = EMERALD_LEVELEDITOR_MESSAGE_1_4;
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 76] = EMERALD_LEVELEDITOR_MESSAGE_2_4;
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 77] = EMERALD_LEVELEDITOR_MESSAGE_3_4;
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 78] = EMERALD_LEVELEDITOR_MESSAGE_4_4;
            }
            SetButtonActivity(BUTTONLABEL_HIGHSCORES,true);
            MainMenu.pMenuScreen[16 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
            MainMenu.pMenuScreen[16 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
            MainMenu.pMenuScreen[16 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
            MainMenu.pMenuScreen[16 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
            memset(MainMenu.szTempName,0,sizeof(MainMenu.szTempName));
            MainMenu.uCursorPos = 0;
            MainMenu.nState = 0;
            WaitNoKey();
        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_RETURN]) {
            nErrorCode = SDL_SetTextureColorMod(GetTextureByIndex(TEX_STONE),100,100,100);
            SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,true);
            MainMenu.pMenuScreen[13 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
            MainMenu.pMenuScreen[13 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
            MainMenu.pMenuScreen[13 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
            MainMenu.pMenuScreen[13 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
            if (bShowLeveleditorButton) {
                SetButtonActivity(BUTTONLABEL_LEVELEDITOR,true);
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
            } else {
                SetButtonActivity(BUTTONLABEL_LEVELEDITOR,false);
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 75] = EMERALD_LEVELEDITOR_MESSAGE_1_4;
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 76] = EMERALD_LEVELEDITOR_MESSAGE_2_4;
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 77] = EMERALD_LEVELEDITOR_MESSAGE_3_4;
                MainMenu.pMenuScreen[15 * MainMenu.uXdim + 78] = EMERALD_LEVELEDITOR_MESSAGE_4_4;
            }
            SetButtonActivity(BUTTONLABEL_HIGHSCORES,true);
            MainMenu.pMenuScreen[16 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
            MainMenu.pMenuScreen[16 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
            MainMenu.pMenuScreen[16 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
            MainMenu.pMenuScreen[16 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
            if (InsertNewName(MainMenu.szTempName) == 0) {
                if (ReadNamesFile() == 0) {
                    InitLists();
                    // SDL_Log("Inserting name: %s ... OK",MainMenu.szTempName);
                } else {
                    SDL_Log("Inserting name: %s ... ERRROR 2",MainMenu.szTempName);
                }
            } else {
                SDL_Log("Inserting name: %s ... ERROR",MainMenu.szTempName);
            }
            memset(MainMenu.szTempName,0,sizeof(MainMenu.szTempName));
            MainMenu.uCursorPos = 0;
            MainMenu.nState = 0;
        } else {
            SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,false);
            SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,false);
            SetButtonActivity(BUTTONLABEL_LEVELEDITOR,false);
            SetButtonActivity(BUTTONLABEL_HIGHSCORES,false);
            // Create Player
            MainMenu.pMenuScreen[13 * MainMenu.uXdim + 75] = EMERALD_SPACE;
            MainMenu.pMenuScreen[13 * MainMenu.uXdim + 76] = EMERALD_SPACE;
            MainMenu.pMenuScreen[13 * MainMenu.uXdim + 77] = EMERALD_SPACE;
            MainMenu.pMenuScreen[13 * MainMenu.uXdim + 78] = EMERALD_SPACE;
            // Delete Player
            MainMenu.pMenuScreen[14 * MainMenu.uXdim + 75] = EMERALD_SPACE;
            MainMenu.pMenuScreen[14 * MainMenu.uXdim + 76] = EMERALD_SPACE;
            MainMenu.pMenuScreen[14 * MainMenu.uXdim + 77] = EMERALD_SPACE;
            MainMenu.pMenuScreen[14 * MainMenu.uXdim + 78] = EMERALD_SPACE;
            // Leveleditor
            MainMenu.pMenuScreen[15 * MainMenu.uXdim + 75] = EMERALD_SPACE;
            MainMenu.pMenuScreen[15 * MainMenu.uXdim + 76] = EMERALD_SPACE;
            MainMenu.pMenuScreen[15 * MainMenu.uXdim + 77] = EMERALD_SPACE;
            MainMenu.pMenuScreen[15 * MainMenu.uXdim + 78] = EMERALD_SPACE;
            // Highscores
            MainMenu.pMenuScreen[16 * MainMenu.uXdim + 75] = EMERALD_SPACE;
            MainMenu.pMenuScreen[16 * MainMenu.uXdim + 76] = EMERALD_SPACE;
            MainMenu.pMenuScreen[16 * MainMenu.uXdim + 77] = EMERALD_SPACE;
            MainMenu.pMenuScreen[16 * MainMenu.uXdim + 78] = EMERALD_SPACE;

            // Eingabe eines Zeichens für den Spielernamen
            nErrorCode = SDL_SetTextureColorMod(GetTextureByIndex(TEX_STONE),MainMenu.uFlashBrightness[MainMenu.uFlashIndex],MainMenu.uFlashBrightness[MainMenu.uFlashIndex],MainMenu.uFlashBrightness[MainMenu.uFlashIndex]);
            MainMenu.uFlashIndex++;
            if (MainMenu.uFlashIndex > MainMenu.uMaxFlashIndex) {
                MainMenu.uFlashIndex = 0;
            }
            uKey = FilterBigFontKey(GetKey());
            if (uKey != 0) {
               // SDL_Log("Key = %u",uKey);
               if (MainMenu.uCursorPos < EMERALD_PLAYERNAME_LEN) {
                    MainMenu.szTempName[MainMenu.uCursorPos] = uKey;
                    MainMenu.uCursorPos++;
               }
            } else  if (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]) {
                 // SDL_Log("Key = BACKSPACE");
                 if (MainMenu.uCursorPos > 0) {
                    MainMenu.szTempName[MainMenu.uCursorPos - 1] = 0;
                    MainMenu.uCursorPos--;
                 }
                 do {
                    UpdateInputStates();
                 } while (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]);
            }
            SetMenuText(MainMenu.pMenuScreen,"                                   ",7,5,EMERALD_FONT_GREEN); // löscht auch alte Cursorposition
            SetMenuText(MainMenu.pMenuScreen,MainMenu.szTempName,7,5,EMERALD_FONT_GREEN);
            MainMenu.pMenuScreen[5 * MainMenu.uXdim + 7 + MainMenu.uCursorPos] = EMERALD_STONE;    // Stein als Cursor setzen
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           EmeraldMineMainMenu
------------------------------------------------------------------------------
Beschreibung: Baut das Hauptmenü auf. Hier kann Levelgruppe, Name und Level ausgewählt werden.
              Mit Hauptschleifen-Funktion für das Spiel.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -

Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Playfield.x, InputStates.x, MainMenu.x
               SelectedLevelgroup.x, Config.x, Audioplayer.x, Actualplayer.x
               Video.x, GameSound.x, Fps.x
------------------------------------------------------------------------------*/
int EmeraldMineMainMenu(SDL_Renderer *pRenderer) {
    int nErrorCode;
    uint8_t uMusicVolume;
    uint32_t uHighscoreLevel;
    uint32_t uScrollerLeftPosX;
    uint32_t uScrollerRightPosX;
    uint32_t uScrollFastCounter;
    bool bMenuRun;
    bool bEndGame;
    bool bPrepareExit;
    bool bShowHighscores;
    bool bStartEditor;
    bool bStartSettings;
    int nColorDimm;
    int nButton;
    int nLastButton;
    int nPlayerlistButton;
    int nLastPlayerlistButton;
    int nLevelgrouplistButton;
    int nLastLevelgrouplistButton;
    int nNewHighscoreIndex;
    char szText[256];
    char szLastPlayername[EMERALD_PLAYERNAME_LEN + 1];                       // Aktuellen Spielernamen merken
    SCROLLER Scroller;
    uint8_t szMessage[] = {"START THE GAME WITH THE FIRE BUTTON (LEFT CTRL).  \
    GAME MUSIC BY MAKTONE, VOYCE/DELIGHT AND JESPER KYD.  \
    MODPLAYER BY MICHAL PROCHAZKA (WWW.PROCHAZKAML.EU).  \
    XMPLAYER BY ROMAIN DALMASO/DAN SPENCER.  \
    DATA (DE)COMPRESSOR 'MINIZ' BY RICH GELDREICH AND TENACIOUS SOFTWARE LLC.  \
    XML READER 'EZXML' BY AARON VOISINE.  \
    BASE64 DECODER BY CAMERON HARPER.  \
    GAME GRAPHICS AND SOUNDS TAKEN FROM DIAMOND CAVES 3, A GAME BY PETER ELZNER.             \
    PLEASE SEND YOUR OWN LEVELGROUP TO DIETER1964@GMX.NET           "};

    memset(&Fps,0,sizeof(Fps));
    SetGameSpeed(GAMESPEED_NORMAL);
    InitTeleporter();
    InitClipboard();
    InitMainMenu(WINDOW_FULL_SIZE);
    uScrollerLeftPosX = Video.uXoffs + FONT_W;
    uScrollerRightPosX = Video.uXoffs + DEFAULT_WINDOW_W - FONT_W;
    if (InitScroller(&Scroller,1,uScrollerLeftPosX,uScrollerRightPosX,0,szMessage, 0,0,0,1,false,false) != 0) {
        return -1;
    }
    Scroller.nYpos = FONT_H * 12;   // Scroller in 13. Zeile (ab 0 gezählt) von oben
    // Namen einlesen
    if (ReadNamesFile() != 0) {
        return -1;
    }
    if (SetMusic(6) != 0) {  // 6. 2kad04.mod  von Maktone
        return -1;
    }
    if (CreateButton(BUTTONLABEL_CREATE_PLAYER,"CREATE PLAYER",1127 + Video.uXoffs,454,true,false) != 0) {
        return -1;
    }
    if (CreateButton(BUTTONLABEL_DELETE_PLAYER,"DELETE PLAYER",1127 + Video.uXoffs,486,false,false) != 0) {
        return -1;
    }
    if (CreateButton(BUTTONLABEL_LEVELEDITOR,"LEVELEDITOR",1135 + Video.uXoffs,518,true,false) != 0) {
        return -1;
    }
    if (Config.uResY >= MIN_Y_RESOLUTION_FOR_LEVELEDITOR) {
        SetButtonActivity(BUTTONLABEL_LEVELEDITOR,true);
    } else {
        SetButtonActivity(BUTTONLABEL_LEVELEDITOR,false);
    }
    if (CreateButton(BUTTONLABEL_HIGHSCORES,"HIGHSCORES",1139 + Video.uXoffs,550,true,false) != 0) {
        return -1;
    }
    SDL_PauseAudioDevice(Audioplayer.audio_device, 0);
    SetStaticMenuElements();
    uMusicVolume = 0;
    SetMusicVolume(uMusicVolume);
    nErrorCode = 0;
    nColorDimm = 0;
    bPrepareExit = false;
    bMenuRun = true;
    bEndGame = false;
    bStartEditor =  false;
    bStartSettings = false;
    bShowHighscores = false;
    nButton = 0;   // 0 = nicht gedrückt, 1 = down, 2 = up
    nLastButton = 0;
    nPlayerlistButton = 0;
    nLastPlayerlistButton = 0;
    nLevelgrouplistButton = 0;
    nLastLevelgrouplistButton = 0;
    nNewHighscoreIndex = -1;
    uScrollFastCounter = 0;
    if (GetLevelgroupFiles() == 0) {    // Wenn das nicht funktioniert, kann nicht weitergemacht werden!
        if (CleanUpHighScoreDir() == 0) {
            if (CleanNameHashes() == 0) {
                InitLists();
                // ShowAvailableLevelgroups();
                if (SelectAlternativeLevelgroup(Config.uLevelgroupMd5Hash,true) == 0) {
                    // ShowSelectedLevelgroup();
                    if (SelectName(Config.szPlayername,SelectedLevelgroup.uMd5Hash) != 0) {
                        // Falls letzter Name nicht mehr auswählbar ist, diesen aus dem Konfigurationsfile löschen
                        memset(Config.szPlayername,0,sizeof(Config.szPlayername));
                        nErrorCode = WriteConfigFile();
                    }
                    while ((bMenuRun) && (nErrorCode == 0)) {
                        if (Actualplayer.bValid) {
                            DoScroller(pRenderer,&Scroller);
                        }
                        UpdateManKey(); // Ruft UpdateInputStates() auf
                        MeasureFps();
                        if (((InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) || (InputStates.bQuit) || (nButton == EMERALD_STEEL_EXIT_PRESSED)) && (nColorDimm == 100) && (MainMenu.nState == 0)) {
                            bPrepareExit = true;
                            bEndGame = true;        // Spiel beenden
                        } else if ( (MainMenu.nState == 0) && (Actualplayer.bValid) && (nColorDimm == 100) && (SelectedLevelgroup.bOK) && ((ManKey.bFire) || (InputStates.pKeyboardArray[SDL_SCANCODE_SPACE])) ) {
                            bPrepareExit = true;    // Level starten
                        } else if ( ((InputStates.pKeyboardArray[SDL_SCANCODE_H]) || IsButtonPressed(BUTTONLABEL_HIGHSCORES)) && (nColorDimm == 100) && (MainMenu.nState == 0) ) {
                            bPrepareExit = true;    // Highscores zeigen
                            bShowHighscores = true;
                        } else if ((MainMenu.nState == 0) && (IsButtonPressed(BUTTONLABEL_DELETE_PLAYER))) {
                            if (Actualplayer.bValid) {
                                // SDL_Log("Deleting name :%s",Actualplayer.szPlayername);
                                nErrorCode = DeleteName(Actualplayer.szPlayername);
                                if (nErrorCode == 0) {
                                    memset(&Actualplayer,0,sizeof(Actualplayer));
                                    InitLists();
                                }
                            }
                        } else if ( (MainMenu.nState == 0) && (nColorDimm == 100) && (SelectedLevelgroup.bOK) && (IsButtonPressed(BUTTONLABEL_LEVELEDITOR) )) {
                            bPrepareExit = true;    // Highscores zeigen
                            bStartEditor =  true;
                        } else if (nButton == EMERALD_STEEL_SETTINGS_PRESSED) {
                            bPrepareExit = true;    // zu den Einstellungen
                            bStartSettings = true;
                        }
                        if (bPrepareExit) {
                            if (nColorDimm > 0) {
                                nColorDimm = nColorDimm - 4;
                                SetAllTextureColors(nColorDimm);
                                uMusicVolume = uMusicVolume - 4;
                                SetMusicVolume(uMusicVolume);
                            } else {
                                if (bEndGame) {
                                    bMenuRun = false;
                                } else if (bShowHighscores) {
                                    SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,false);
                                    SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,false);
                                    SetButtonActivity(BUTTONLABEL_LEVELEDITOR,false);
                                    SetButtonActivity(BUTTONLABEL_HIGHSCORES,false);
                                    nErrorCode = ShowHighScores(pRenderer,Actualplayer.uLevel,-1);
                                    SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,true);
                                    if (Actualplayer.bValid) {
                                        SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,true);
                                    }
                                    if (Config.uResY >= MIN_Y_RESOLUTION_FOR_LEVELEDITOR) {
                                        SetButtonActivity(BUTTONLABEL_LEVELEDITOR,true);
                                    }
                                    SetButtonActivity(BUTTONLABEL_HIGHSCORES,true);
                                    SetStaticMenuElements();
                                    bPrepareExit = false;
                                    nColorDimm = 0;
                                    uMusicVolume = 0;
                                    SetAllTextureColors(nColorDimm);
                                    SetMusicVolume(uMusicVolume);
                                    if (SetMusic(6) != 0) {
                                        return -1;
                                    }
                                    bShowHighscores = false;
                                    WaitNoKey();
                                } else if (bStartEditor) {
                                    SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,false);
                                    SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,false);
                                    SetButtonActivity(BUTTONLABEL_LEVELEDITOR,false);
                                    SetButtonActivity(BUTTONLABEL_HIGHSCORES,false);
                                    // Welchen Grund hatte das Zurücksetzen des aktuellen Spielers?
                                    // Grund: Die alten Spielerdaten (vor Leveleditor-Aufruf) stehen dann im Menü.
                                    strcpy(szLastPlayername,Actualplayer.szPlayername); // Aktuellen Spielernamen merken
                                    // SDL_Log("Last Player:%s",szLastPlayername);
                                    memset(&Actualplayer,0,sizeof(Actualplayer));
                                    StartCheckDC3ImportDirectoryThread();   // Startet Thread, der das DC3-Import-Verzeichnis überwacht
                                    nErrorCode = PreEditorMenu(pRenderer);
                                    CloseCheckDC3ImportDirectoryThread();  // Thread wieder beenden
                                    if (nErrorCode == 0) {
                                        // Eine ggf. geänderte Levelgruppe nun mit Highscorefile ausstatten
                                        nErrorCode = SelectAlternativeLevelgroup(Config.uLevelgroupMd5Hash,true);
                                    }
                                    SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,true);
                                    if (Actualplayer.bValid) {
                                        SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,true);
                                    }
                                    if (Config.uResY >= MIN_Y_RESOLUTION_FOR_LEVELEDITOR) {
                                        SetButtonActivity(BUTTONLABEL_LEVELEDITOR,true);
                                    }
                                    SetButtonActivity(BUTTONLABEL_HIGHSCORES,true);
                                    nColorDimm = 0;
                                    uMusicVolume = 0;
                                    SetStaticMenuElements();
                                    bPrepareExit = false;
                                    bStartEditor = false;
                                    if (SetMusic(6) != 0) {      // MOD 6, 2kad04
                                        return -1;
                                    }
                                    WaitNoKey();
                                    // Spieler neu auswählen, damit Spielerdaten ggf. zurückgesetzt sind, wenn Levelgruppe geändert wurde
                                    if (SelectName(szLastPlayername,SelectedLevelgroup.uMd5Hash) != 0) {
                                        return -1;
                                    }
                                } else if (bStartSettings) {
                                    SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,false);
                                    SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,false);
                                    SetButtonActivity(BUTTONLABEL_LEVELEDITOR,false);
                                    SetButtonActivity(BUTTONLABEL_HIGHSCORES,false);
                                    FreeScroller(&Scroller);    // Scroller muss nach Settings ggf. neu initialisiert und gestartet werden
                                    CloseJoystickOrGameController();
                                    nErrorCode = SettingsMenu(pRenderer);
                                    // Nach Settings wegen möglicher Auflösungsänderung MainMenu und Levelgroup-/Player-Listen neu aufbauen
                                    InitMainMenu(WINDOW_FULL_SIZE);
                                    InitLists();
                                    // Buttons müssen wegen Auflösungsänderung ggf. neu positioniert werden
                                    SetButtonPosition(BUTTONLABEL_CREATE_PLAYER,1127 + Video.uXoffs,454);
                                    SetButtonPosition(BUTTONLABEL_DELETE_PLAYER,1127 + Video.uXoffs,486);
                                    SetButtonPosition(BUTTONLABEL_LEVELEDITOR,1135 + Video.uXoffs,518);
                                    SetButtonPosition(BUTTONLABEL_HIGHSCORES,1139 + Video.uXoffs,550);
                                    uScrollerLeftPosX = Video.uXoffs + FONT_W;
                                    uScrollerRightPosX = Video.uXoffs + DEFAULT_WINDOW_W - FONT_W;
                                    if (InitScroller(&Scroller,1,uScrollerLeftPosX,uScrollerRightPosX,0,szMessage, 0,0,0,1,false,false) != 0) {
                                        return -1;
                                    }
                                    Scroller.nYpos = FONT_H * 12;   // Scroller in 13. Zeile (ab 0 gezählt) von oben
                                    SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,true);
                                    if (Actualplayer.bValid) {
                                        SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,true);
                                    }
                                    if (Config.uResY >= MIN_Y_RESOLUTION_FOR_LEVELEDITOR) {
                                        SetButtonActivity(BUTTONLABEL_LEVELEDITOR,true);
                                    }
                                    SetButtonActivity(BUTTONLABEL_HIGHSCORES,true);
                                    nColorDimm = 0;
                                    uMusicVolume = 0;
                                    SetStaticMenuElements();
                                    bPrepareExit = false;
                                    bStartSettings = false;
                                    if (SetMusic(6) != 0) {      // MOD 6, 2kad04
                                        return -1;
                                    }
                                    WaitNoKey();
                                } else if (nErrorCode == 0) {
                                    // Das Spielergebnis (Erfolg oder Versagen) kann in Playfield.x abgefragt werden.
                                    // SDL_Log("Start Game with level %u",Actualplayer.uLevel);
                                    nErrorCode = RunGame(pRenderer,Actualplayer.uLevel);
                                    SDL_Log("%s: RunGame() ErrorCode: %u",__FUNCTION__,nErrorCode);
                                    if (nErrorCode == 0) {
                                        nErrorCode = EvaluateGame(&nNewHighscoreIndex,&uHighscoreLevel);
                                    }
                                    if ((nNewHighscoreIndex >= 0) && (nErrorCode == 0) && (Config.bShowHighscores)) {
                                        // SDL_Log("%s: New highscore at index: %u",__FUNCTION__,nNewHighscoreIndex);
                                        SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,false);
                                        SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,false);
                                        SetButtonActivity(BUTTONLABEL_LEVELEDITOR,false);
                                        SetButtonActivity(BUTTONLABEL_HIGHSCORES,false);
                                        nErrorCode = ShowHighScores(pRenderer,uHighscoreLevel,nNewHighscoreIndex);
                                        SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,true);
                                        if (Actualplayer.bValid) {
                                            SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,true);
                                        }
                                        if (Config.uResY >= MIN_Y_RESOLUTION_FOR_LEVELEDITOR) {
                                            SetButtonActivity(BUTTONLABEL_LEVELEDITOR,true);
                                        }
                                        SetButtonActivity(BUTTONLABEL_HIGHSCORES,true);
                                    }
                                    SetStaticMenuElements();
                                    bPrepareExit = false;
                                    nColorDimm = 0;
                                    uMusicVolume = 0;
                                    SetAllTextureColors(nColorDimm);
                                    SetMusicVolume(uMusicVolume);
                                    if (SetMusic(6) != 0) {      // MOD 6, 2kad04
                                        return -1;
                                    }
                                    WaitNoKey();
                                }
                            }
                        }
                        // printf("x:%u  y:%u\n",InputStates.nMouseXpos,InputStates.nMouseYpos);
                        if (!bPrepareExit) {
                            if (MainMenu.nState == 0) {     // Levelgruppen- und Spieler-Auswahl nur während Namenseingabe nicht aktiv
                                nErrorCode = MenuSelectLevelgroup(pRenderer);
                                if (nErrorCode == 0) {
                                    nErrorCode = MenuSelectName(pRenderer);
                                }
                                nLastPlayerlistButton = nPlayerlistButton;
                                nPlayerlistButton = GetPlayerListButton();
                                nLastLevelgrouplistButton = nLevelgrouplistButton;
                                nLevelgrouplistButton = GetLevelgroupListButton();
                                // Für das schnelle Scrollen der Levelgruppen- und Player-Liste
                                if ( ((nLastPlayerlistButton == EMERALD_STEEL_ARROW_DOWN_PRESSED) && (nPlayerlistButton == EMERALD_STEEL_ARROW_DOWN_PRESSED)) ||
                                     ((nLastPlayerlistButton == EMERALD_STEEL_ARROW_UP_PRESSED) && (nPlayerlistButton == EMERALD_STEEL_ARROW_UP_PRESSED)) ||
                                     ((nLastLevelgrouplistButton == EMERALD_STEEL_ARROW_DOWN_PRESSED) && (nLevelgrouplistButton == EMERALD_STEEL_ARROW_DOWN_PRESSED)) ||
                                     ((nLastLevelgrouplistButton == EMERALD_STEEL_ARROW_UP_PRESSED) && (nLevelgrouplistButton == EMERALD_STEEL_ARROW_UP_PRESSED)) ) {
                                    uScrollFastCounter++;
                                } else {
                                    uScrollFastCounter = 0;
                                }
                                if ( ((nLastPlayerlistButton == 0) || (uScrollFastCounter > 10)) && (nPlayerlistButton != 0) ) {
                                    ScrollPlayernames(nPlayerlistButton);
                                }
                                if ( ((nLastLevelgrouplistButton == 0) || (uScrollFastCounter > 10)) && (nLevelgrouplistButton != 0) ) {
                                    ScrollLevelGroups(nLevelgrouplistButton);
                                }
                            }
                            SetMenuText(MainMenu.pMenuScreen,"                         ",13,4,EMERALD_FONT_GREEN);
                            SetMenuText(MainMenu.pMenuScreen,SelectedLevelgroup.szLevelgroupname,13,4,EMERALD_FONT_GREEN);
                            SetDynamicMenuElements();
                        }
                        nLastButton = nButton;
                        nButton = GetMainMenuButton();
                        if ((nLastButton == 0) && (Actualplayer.bValid)) {
                            if (nButton == EMERALD_STEEL_ARROW_DOWN_PRESSED) {
                                if (Actualplayer.uLevel > 0) {
                                    Actualplayer.uLevel--;
                                }
                            } else if (nButton == EMERALD_STEEL_ARROW_UP_PRESSED) {
                                if (Actualplayer.uLevel < Actualplayer.uHandicap) {
                                    Actualplayer.uLevel++;
                                }
                            }
                            if (nButton != 0) {
                                SetMenuText(MainMenu.pMenuScreen,Actualplayer.szPlayername,7,5,EMERALD_FONT_GREEN);
                                sprintf(szText,"%u       ",Actualplayer.uLevel);
                            }
                        }
                        nLastButton = nButton;
                        // if (nButton != 0) SDL_Log("Button:%d",nButton);

                        if ((!bPrepareExit) && (nColorDimm < 100)) {
                            nColorDimm = nColorDimm + 4;
                            SetAllTextureColors(nColorDimm);
                            uMusicVolume = uMusicVolume + 4;
                            SetMusicVolume(uMusicVolume);
                        }
                        PlayMusic(false);
                        if (nErrorCode == 0) {
                            nErrorCode = RenderMenuElements(pRenderer);
                            if ((nErrorCode == 0) && (MainMenu.nState == 0)) {
                                nErrorCode = RenderSettingsbutton(pRenderer);
                            }
                        }

                                PrintLittleFont(pRenderer,FONT_W + Video.uXoffs,MainMenu.uYdim * FONT_H - 25,0,Fps.szFrameaPerSecond,K_ABSOLUTE,1);
                                PrintLittleFont(pRenderer,FONT_W + Video.uXoffs,MainMenu.uYdim * FONT_H - 24,3,Fps.szFrameaPerSecond,K_ABSOLUTE,1);



                        ShowButtons(pRenderer,K_ABSOLUTE);
                        if ((MainMenu.nState == 0) && (IsButtonPressed(BUTTONLABEL_CREATE_PLAYER))) {
                            ActivateInputPlayernameMode();  // Eingabemodus für Namenseingabe aktivieren
                        }
                        RenderPresentAndClear(pRenderer);
                        Playfield.uFrameCounter++;
                    }
                } else {
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Levelgroup problem","Can not select levelgroup!",NULL);
                    nErrorCode = -1;
                }
            }
        }
    }
    SetAllTextureColors(100);
    FreeClipboard();
    FreeButton(BUTTONLABEL_CREATE_PLAYER);
    FreeButton(BUTTONLABEL_DELETE_PLAYER);
    FreeButton(BUTTONLABEL_LEVELEDITOR);
    FreeButton(BUTTONLABEL_HIGHSCORES);
    FreeScroller(&Scroller);
    CleanUpMemoryAndSDL(pRenderer);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CleanUpMemoryAndSDL
------------------------------------------------------------------------------
Beschreibung: Gibt allozierten Speicher und SDL-Resourcen (Renderer und Fenster)
              wieder frei.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Video.x, Audioplayer.x, GameSound.x, MainMenu.x
------------------------------------------------------------------------------*/
void CleanUpMemoryAndSDL(SDL_Renderer *pRenderer) {
    RestoreDesktop();
    SDL_CloseAudioDevice(Audioplayer.audio_device);
    FreeWavChunks();
    SAFE_FREE(Audioplayer.pTheMusic);
    SAFE_FREE(Audioplayer.pMusicAll);
    SAFE_FREE(GameSound.pWavAll);
    Mix_CloseAudio();
    FreeTextures();
    FreeCopper();
    SAFE_FREE(MainMenu.pMenuScreen);
    SAFE_FREE(MainMenu.pLevelgroupList);
    SAFE_FREE(MainMenu.puLevelTitleList);
    SAFE_FREE(MainMenu.puLevelTitleListCopy);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(Video.pWindow);
    SDL_Quit();
}


/*----------------------------------------------------------------------------
Name:           RotateColors
------------------------------------------------------------------------------
Beschreibung: Rotiert eine Farbenpalette nach oben, d.h. zu "kleineren" Speicheradressen.
Parameter
      Eingang: pColors, RGBCOLOR *, Farbpalette
               nCount, int, Anzahl Farben in der Palette
               nCycles, int, Anzahl Rotierungen
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void RotateColors(RGBCOLOR *pColors,int nCount,int nCycles) {
    RGBCOLOR TmpColor;
    uint32_t I,C;

    if (pColors != NULL) {
        for (C = 0; C < nCycles; C++) {
            // Farbe scrollen
            TmpColor = pColors[0];
            for (I = 0; I < (nCount - 1); I++) {
                pColors[I] = pColors[I + 1];
            }
            pColors[I] = TmpColor;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ShowControllersAndJoysticks
------------------------------------------------------------------------------
Beschreibung: Zeigt die Gamecontroller, Joysticks und das Keyboard im
              Settingsmenü an.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -

Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Video.x
------------------------------------------------------------------------------*/
int ShowControllersAndJoysticks(SDL_Renderer *pRenderer) {
    uint32_t I;
    SDL_Rect DestR;
    int nErrorCode = 0;

    for (I = 0; (I < MAX_GAMECONTROLLERS) && (nErrorCode == 0); I++) {
        DestR.x = Video.uXoffs + 80;
        DestR.y = Video.uYoffs + 112 + I * 160;
        DestR.w = 117;
        DestR.h = 95;
        nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_GAME_CONTROLLER),NULL,&DestR,0,NULL, SDL_FLIP_NONE);
    }
    for (I = 0; (I < MAX_JOYSTICKS) && (nErrorCode == 0); I++) {
        DestR.x = Video.uXoffs + 342;
        DestR.y = Video.uYoffs + 90 + I * 160;
        DestR.w = 123;
        DestR.h = 129;
        nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_JOYSTICK),NULL,&DestR,0,NULL, SDL_FLIP_NONE);
    }
    DestR.x = Video.uXoffs + 70;
    DestR.y = Video.uYoffs + 570;
    DestR.w = 405;
    DestR.h = 146;
    // Keyboard
    if (nErrorCode == 0) {
        nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_KEYBOARD),NULL,&DestR,0,NULL, SDL_FLIP_NONE);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ShowRec
------------------------------------------------------------------------------
Beschreibung: Zeigt einen farbigen Rahmen für die Gamecontroller, Joysticks
              und das Keyboard im Settingsmenü an.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               pColors, RGBCOLOR *, Zeiger auf Farbpalette für Farb-Rahmen (mind. 700 Elemente)
               nXpos, int, X-Position für Rahmen, relative Position im Menü
               nYpos, int, Y-Position für Rahmen, relative Position im Menü
               nWidth, int, Breite des Rahmen in Pixeln
               nHeight, int, Höhe des Rahmen in Pixeln
               nDimm, int, Dimmwert in Prozent (100 = volle Helligkeit
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Video.x
------------------------------------------------------------------------------*/
int ShowRec(SDL_Renderer *pRenderer, RGBCOLOR *pColors, int nXpos, int nYpos, int nWidth, int nHeight, int nDimm) {
    int nP;
    int nX,nY;
    int nErrorCode = 0;

    nP = 0;
    // Obere Line
    for (nX = 0; (nX < nWidth) && (nErrorCode == 0); nX++) {
        nErrorCode = nErrorCode + SDL_SetRenderDrawColor(pRenderer,(pColors[nP].uRed * nDimm) / 100,(pColors[nP].uGreen * nDimm) / 100,(pColors[nP].uBlue * nDimm) / 100,pColors[nP].uAlpha);
        nErrorCode = nErrorCode + SDL_RenderDrawPoint(pRenderer,nXpos + nX + Video.uXoffs, nYpos + Video.uYoffs);
        nErrorCode = nErrorCode + SDL_RenderDrawPoint(pRenderer,nXpos + nX + Video.uXoffs, nYpos + 1 + Video.uYoffs);
        nErrorCode = nErrorCode + SDL_RenderDrawPoint(pRenderer,nXpos + nX + Video.uXoffs, nYpos + 2 + Video.uYoffs);
        nP++;
    }
    // Rechte senkrechte Linie
    for (nY = 0; (nY < nHeight - 2) && (nErrorCode == 0); nY++) {
        nErrorCode = nErrorCode + SDL_SetRenderDrawColor(pRenderer,(pColors[nP].uRed * nDimm) / 100,(pColors[nP].uGreen * nDimm) / 100,(pColors[nP].uBlue * nDimm) / 100,pColors[nP].uAlpha);
        nErrorCode = nErrorCode + SDL_RenderDrawPoint(pRenderer,nXpos + nX + Video.uXoffs, nYpos + nY + Video.uYoffs);
        nErrorCode = nErrorCode + SDL_RenderDrawPoint(pRenderer,nXpos + nX - 1 + Video.uXoffs, nYpos + nY + Video.uYoffs);
        nErrorCode = nErrorCode + SDL_RenderDrawPoint(pRenderer,nXpos + nX - 2 + Video.uXoffs, nYpos + nY + Video.uYoffs);
        nP++;
    }
    // Untere Linie
    // for (ohne init., da nX noch auf richtigem Wert)
    for (; (nX > 0) && (nErrorCode == 0); nX--) {
        nErrorCode = nErrorCode + SDL_SetRenderDrawColor(pRenderer,(pColors[nP].uRed * nDimm) / 100,(pColors[nP].uGreen * nDimm) / 100,(pColors[nP].uBlue * nDimm) / 100,pColors[nP].uAlpha);
        nErrorCode = nErrorCode + SDL_RenderDrawPoint(pRenderer,nXpos + nX + Video.uXoffs, nYpos + nY + Video.uYoffs);
        nErrorCode = nErrorCode + SDL_RenderDrawPoint(pRenderer,nXpos + nX + Video.uXoffs, nYpos + nY - 1 + Video.uYoffs);
        nErrorCode = nErrorCode + SDL_RenderDrawPoint(pRenderer,nXpos + nX + Video.uXoffs, nYpos + nY + 1 + Video.uYoffs);
        nP++;
    }
    // Linke senkrechte Linie
    // for (ohne init., da nY noch auf richtigem Wert)
    for (; (nY > 0) && (nErrorCode == 0); nY--) {
        nErrorCode = nErrorCode + SDL_SetRenderDrawColor(pRenderer,(pColors[nP].uRed * nDimm) / 100,(pColors[nP].uGreen * nDimm) / 100,(pColors[nP].uBlue * nDimm) / 100,pColors[nP].uAlpha);
        nErrorCode = nErrorCode + SDL_RenderDrawPoint(pRenderer,nXpos + nX + Video.uXoffs, nYpos + nY + Video.uYoffs);
        nErrorCode = nErrorCode + SDL_RenderDrawPoint(pRenderer,nXpos + nX + 1 + Video.uXoffs, nYpos + nY + Video.uYoffs);
        nErrorCode = nErrorCode + SDL_RenderDrawPoint(pRenderer,nXpos + nX + 2 + Video.uXoffs, nYpos + nY + Video.uYoffs);
        nP++;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           PrepareSettingsMenu
------------------------------------------------------------------------------
Beschreibung: Baut einen Teil des Settingsmenüs, wie Beschriftung und Mauern / Abtrennungen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: MainMenu.x
------------------------------------------------------------------------------*/
void PrepareSettingsMenu(void) {
    uint32_t I;

    InitMainMenu(WINDOW_CENTER_DEFAULT);
    SetMenuBorderAndClear();

    SetMenuText(MainMenu.pMenuScreen,"INPUT DEVICES",2,1,EMERALD_FONT_BLUE);
    for (I = 0; I < EMERALD_HIGHSCORE_LISTLEN + 2; I++) {
        MainMenu.pMenuScreen[(I + 1) * MainMenu.uXdim + 16] = EMERALD_STEEL;    // Senkrechte zwischen Joysticks und anderen Settings
        MainMenu.pMenuScreen[(I + 1) * MainMenu.uXdim + 28] = EMERALD_STEEL;    // Senkrechte zwischen Inputs-Settings und Video-Settings
    }
    for (I = 0; I < 15; I++) {
        MainMenu.pMenuScreen[2 * MainMenu.uXdim + 1 + I] = EMERALD_STEEL;
        MainMenu.pMenuScreen[7 * MainMenu.uXdim + 1 + I] = EMERALD_STEEL;
        MainMenu.pMenuScreen[12 * MainMenu.uXdim + 1 + I] = EMERALD_STEEL;
        MainMenu.pMenuScreen[17 * MainMenu.uXdim + 1 + I] = EMERALD_STEEL;
    }
    for (I = 0; I < 15; I++) {
        MainMenu.pMenuScreen[(I + 3) * MainMenu.uXdim + 8] = EMERALD_STEEL;     // Senkrechte Abtrennung zwischen Controller & Joysticks
    }
}

/*----------------------------------------------------------------------------
Name:           SettingsMenu
------------------------------------------------------------------------------
Beschreibung: Baut das Setup-Menü auf. Hier kann das Eingabegerät, Fenstergröße,
              usw. konfiguriert werden
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Playfield.x, InputStates.x, MainMenu.x, Config.x,
               GameController.x, Joystick.x, Video.x,
               g_uScanCodes[], g_ScanCodeNames[]
------------------------------------------------------------------------------*/
int SettingsMenu(SDL_Renderer *pRenderer) {
    SDL_Rect RecAxisButton;
    SDL_Rect KeyboardButton;
    int nDisplays;
    int nErrorCode = 0;
    int nKeyboardButton = 0;        // Für Richtungsauswahl bzw. Firebutton
    int nKeybindingState = 0;
    int nButton = 0;
    int nLastButton = 0;
    int nAxisButton = 0;
    int nLastAxisButton = 0;
    int nColorDimm = 0;
    int nXpos;
    int nYpos;
    int nRec1PixelCount;
    int nRec2PixelCount;
    int nScanCodeIndex;
    uint8_t uMusicVolume = 0;
    uint32_t I, II;
    uint32_t uRainbowColor;
    uint32_t uLastDetectionTime;    // letzter Zeitpunkt der Joystick-/Gamecontroller-Erkennung
    uint8_t uBlinkColor;
    bool bExit = false;
    bool bPrepareExit = false;
    bool bShowRec1;                 // Rahmen 1 (Gamecontroller/Joystick) anzeigen, sonst Rahmen 2 (Keyboard)
    bool bActive;
    char szText[256];
    char szSubString[32];

    // Farbpunkte für Rahmen 1
    RGBCOLOR RgbColorsRec1[2 * COLOR_REC_1_W + 2 * (COLOR_REC_1_H - 2)];    // 700
    // Farbpunkte für Rahmen 2
    RGBCOLOR RgbColorsRec2[2 * COLOR_REC_2_W + 2 * (COLOR_REC_2_H - 2)];
    CHECKBOX Checkbox_StartDynamiteKeyboard;
    CHECKBOX Checkbox_GC_Dir_Digitalpad;
    CHECKBOX Checkbox_GC_Dir_LeftAxis;
    CHECKBOX Checkbox_GC_Dir_RightAxis;
    CHECKBOX Checkbox_GC_Btn_Fire_A;
    CHECKBOX Checkbox_GC_Btn_Fire_B;
    CHECKBOX Checkbox_GC_Btn_Fire_X;
    CHECKBOX Checkbox_GC_Btn_Fire_Y;
    CHECKBOX Checkbox_GC_Btn_Dynamite_A;
    CHECKBOX Checkbox_GC_Btn_Dynamite_B;
    CHECKBOX Checkbox_GC_Btn_Dynamite_X;
    CHECKBOX Checkbox_GC_Btn_Dynamite_Y;
    CHECKBOX Checkbox_GC_Btn_Exit_A;
    CHECKBOX Checkbox_GC_Btn_Exit_B;
    CHECKBOX Checkbox_GC_Btn_Exit_X;
    CHECKBOX Checkbox_GC_Btn_Exit_Y;
    CHECKBOX Checkbox_GC_Btn_Exit_None;
    CHECKBOX Checkbox_Joy_Btn_Fire_A;
    CHECKBOX Checkbox_Joy_Btn_Fire_B;
    CHECKBOX Checkbox_Joy_Btn_Fire_X;
    CHECKBOX Checkbox_Joy_Btn_Fire_Y;
    CHECKBOX Checkbox_Joy_Btn_Dynamite_A;
    CHECKBOX Checkbox_Joy_Btn_Dynamite_B;
    CHECKBOX Checkbox_Joy_Btn_Dynamite_X;
    CHECKBOX Checkbox_Joy_Btn_Dynamite_Y;
    CHECKBOX Checkbox_Joy_Btn_Exit_A;
    CHECKBOX Checkbox_Joy_Btn_Exit_B;
    CHECKBOX Checkbox_Joy_Btn_Exit_X;
    CHECKBOX Checkbox_Joy_Btn_Exit_Y;
    CHECKBOX Checkbox_Joy_Btn_Exit_None;
    CHECKBOX Checkbox_Gamemusic;
    CHECKBOX Checkbox_ShowHighScores;
    CHECKBOX Checkbox_FullScreen;
    CHECKBOX Checkbox_Resolution[MAX_SHOWABLE_DISPLAYMODES];
    CHECKBOX Checkbox_Display1;
    CHECKBOX Checkbox_Display2;
    // AB hier alles zum Stern
    float fColorDimmP;
    Sint16 CenterX;
    Sint16 CenterY;
    Sint16 NewCenterX;
    Sint16 NewCenterY;
    Sint16 StarX[MAX_STAR_POINTS];
    Sint16 StarY[MAX_STAR_POINTS];
    Sint16 NewStarX[MAX_STAR_POINTS];
    Sint16 NewStarY[MAX_STAR_POINTS];
    float fAngle;
    uint32_t uRainBowColor;
    uint8_t uRed,uGreen,uBlue;
    float fF;
    bool bUp = true;
    uint8_t uRain;
    // Idee: https://www.youtube.com/watch?v=Tm7SOZzGUIM
    // Damit sich das Objekt um seinen eigenen Mittelpunkt dreht, muss sich der Mittelpunkt des Objekts im
    // Koordinatensystem an 0/0 befinden.
    StarX[0] = 0;    // Obere Spitze des Sterns
    StarY[0] = -254;
    StarX[1] = 58;
    StarY[1] = -86;
    StarX[2] = 240;
    StarY[2] = -68;
    StarX[3] = 93;
    StarY[3] = 24;
    StarX[4] = 148;
    StarY[4] = 196;
    StarX[5] = 0;
    StarY[5] = 91;
    StarX[6] = -148;
    StarY[6] = 196;
    StarX[7] = -93;
    StarY[7] = 24;
    StarX[8] = -240;
    StarY[8] = -86;
    StarX[9] = -57;
    StarY[9] = -86;
    CenterX = Config.uResX / 2;
    CenterY = Config.uResY / 2;
    uRain = 0;
    fAngle = 0;
    fF = 0.5;

    nDisplays = SDL_GetNumVideoDisplays();

    if (RegisterCheckbox(&Checkbox_StartDynamiteKeyboard,Config.bStartDynamiteWithSpace,"START DYNAMITE WITH SPACE",600,70,false,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.uControllerDirections == 0);
    if (RegisterCheckbox(&Checkbox_GC_Dir_Digitalpad,bActive,"DIGITALPAD",600,132,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.uControllerDirections == 1);
    if (RegisterCheckbox(&Checkbox_GC_Dir_LeftAxis,bActive,"LEFT AXIS",600,154,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.uControllerDirections == 2);
    if (RegisterCheckbox(&Checkbox_GC_Dir_RightAxis,bActive,"RIGHT AXIS",600,176,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cControllerFireButton == 'A');
    if (RegisterCheckbox(&Checkbox_GC_Btn_Fire_A,bActive,"A",600,238,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cControllerFireButton == 'B');
    if (RegisterCheckbox(&Checkbox_GC_Btn_Fire_B,bActive,"B",600,260,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cControllerFireButton == 'X');
    if (RegisterCheckbox(&Checkbox_GC_Btn_Fire_X,bActive,"X",600,282,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cControllerFireButton == 'Y');
    if (RegisterCheckbox(&Checkbox_GC_Btn_Fire_Y,bActive,"Y",600,304,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cControllerStartDynamiteButton == 'A');
    if (RegisterCheckbox(&Checkbox_GC_Btn_Dynamite_A,bActive,"A",600,366,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cControllerStartDynamiteButton == 'B');
    if (RegisterCheckbox(&Checkbox_GC_Btn_Dynamite_B,bActive,"B",600,388,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cControllerStartDynamiteButton == 'X');
    if (RegisterCheckbox(&Checkbox_GC_Btn_Dynamite_X,bActive,"X",600,410,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cControllerStartDynamiteButton == 'Y');
    if (RegisterCheckbox(&Checkbox_GC_Btn_Dynamite_Y,bActive,"Y",600,432,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cControllerExitButton == 'A');
    if (RegisterCheckbox(&Checkbox_GC_Btn_Exit_A,bActive,"A",600,496,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cControllerExitButton == 'B');
    if (RegisterCheckbox(&Checkbox_GC_Btn_Exit_B,bActive,"B",600,518,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cControllerExitButton == 'X');
    if (RegisterCheckbox(&Checkbox_GC_Btn_Exit_X,bActive,"X",600,540,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cControllerExitButton == 'Y');
    if (RegisterCheckbox(&Checkbox_GC_Btn_Exit_Y,bActive,"Y",600,562,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cControllerExitButton == 'N');
    if (RegisterCheckbox(&Checkbox_GC_Btn_Exit_None,bActive,"NONE",600,584,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cJoystickFireButton == 'A');
    if (RegisterCheckbox(&Checkbox_Joy_Btn_Fire_A,bActive,"A",600,238,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cJoystickFireButton == 'B');
    if (RegisterCheckbox(&Checkbox_Joy_Btn_Fire_B,bActive,"B",600,260,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cJoystickFireButton == 'X');
    if (RegisterCheckbox(&Checkbox_Joy_Btn_Fire_X,bActive,"X",600,282,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cJoystickFireButton == 'Y');
    if (RegisterCheckbox(&Checkbox_Joy_Btn_Fire_Y,bActive,"Y",600,304,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cJoystickStartDynamiteButton == 'A');
    if (RegisterCheckbox(&Checkbox_Joy_Btn_Dynamite_A,bActive,"A",600,366,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cJoystickStartDynamiteButton == 'B');
    if (RegisterCheckbox(&Checkbox_Joy_Btn_Dynamite_B,bActive,"B",600,388,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cJoystickStartDynamiteButton == 'X');
    if (RegisterCheckbox(&Checkbox_Joy_Btn_Dynamite_X,bActive,"X",600,410,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cJoystickStartDynamiteButton == 'Y');
    if (RegisterCheckbox(&Checkbox_Joy_Btn_Dynamite_Y,bActive,"Y",600,432,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cJoystickExitButton == 'A');
    if (RegisterCheckbox(&Checkbox_Joy_Btn_Exit_A,bActive,"A",600,496,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cJoystickExitButton == 'B');
    if (RegisterCheckbox(&Checkbox_Joy_Btn_Exit_B,bActive,"B",600,518,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cJoystickExitButton == 'X');
    if (RegisterCheckbox(&Checkbox_Joy_Btn_Exit_X,bActive,"X",600,540,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cJoystickExitButton == 'Y');
    if (RegisterCheckbox(&Checkbox_Joy_Btn_Exit_Y,bActive,"Y",600,562,true,CHK_USE) != 0) {
        return -1;
    }
    bActive = (Config.cJoystickExitButton == 'N');
    if (RegisterCheckbox(&Checkbox_Joy_Btn_Exit_None,bActive,"NONE",600,584,true,CHK_USE) != 0) {
        return -1;
    }
    if (RegisterCheckbox(&Checkbox_Gamemusic,Config.bGameMusic,"GAME MUSIC",980,70,false,CHK_USE) != 0) {
        return -1;
    }
    if (RegisterCheckbox(&Checkbox_ShowHighScores,Config.bShowHighscores,"SHOW HIGHSCORES",980,92,false,CHK_USE) != 0) {
        return -1;
    }
    if (RegisterCheckbox(&Checkbox_FullScreen,Config.bFullScreen,"FULL SCREEN",980,156,false,!Video.bMustUseFullScreen) != 0) {
        return -1;
    }
    bActive = (Config.uDisplay == 0);   // Primary Display
    if (RegisterCheckbox(&Checkbox_Display1,bActive,"DISPLAY 1 (RESTART APP)",980,178,true,(nDisplays > 1)) != 0) {
        return -1;
    }
    bActive = (Config.uDisplay == 1);   // Secondary Display
    if (RegisterCheckbox(&Checkbox_Display2,bActive,"DISPLAY 2 (RESTART APP)",980,200,true,(nDisplays > 1)) != 0) {
        return -1;
    }
    if ((Video.nShowableDisplayModeCount > 0 ) && (Video.nShowableDisplayModeCount <= MAX_SHOWABLE_DISPLAYMODES)) {
        for (I = 0; I < Video.nShowableDisplayModeCount; I++) {
            bActive = ((Video.ShowableDisplayModes[I].nW == Config.uResX) && (Video.ShowableDisplayModes[I].nH == Config.uResY));
            sprintf(szText,"%d X %d",Video.ShowableDisplayModes[I].nW,Video.ShowableDisplayModes[I].nH);
            if (RegisterCheckbox(&Checkbox_Resolution[I],bActive,szText,980,260 + I * 22,true,CHK_USE) != 0) {
                return -1;
            }
        }
    } else {
        return -1;
    }

    nRec1PixelCount = 2 * COLOR_REC_1_W + 2 * (COLOR_REC_1_H - 2);
    // Rahmen von Rechteck 1 mit Farben füllen
    for (I = 0; I < nRec1PixelCount; I++) {
        uRainbowColor = GetRainbowColors(I % 254);
        RgbColorsRec1[I].uBlue = uRainbowColor;
        RgbColorsRec1[I].uGreen = uRainbowColor >> 8;
        RgbColorsRec1[I].uRed = uRainbowColor >> 16;
        RgbColorsRec1[I].uAlpha = 0xFF;
    }
    nRec2PixelCount = 2 * COLOR_REC_2_W + 2 * (COLOR_REC_2_H - 2);
    for (I = 0; I < nRec2PixelCount; I++) {
        uRainbowColor = GetRainbowColors(I % 254);
        RgbColorsRec2[I].uBlue = uRainbowColor;
        RgbColorsRec2[I].uGreen = uRainbowColor >> 8;
        RgbColorsRec2[I].uRed = uRainbowColor >> 16;
        RgbColorsRec2[I].uAlpha = 0xFF;
    }
    uLastDetectionTime = SDL_GetTicks();
    // Der Highscore-Button wird hier auch verwendet -> kein Problem, da lokal
    if (CreateButton(BUTTONLABEL_EXIT_HIGHSCORES,"Back to main menu",1100,742,true,false) != 0) {
        return -1;
    }
    if (SetMusic(2) != 0) {
        return -1;
    }
    WaitNoKey();
    PrepareSettingsMenu();
    do {
        UpdateInputStates();
        /////////////////////  Stern-Start
        fColorDimmP = (float)nColorDimm / 100;
        NewCenterX = 400 * cos(fAngle);
        NewCenterY = 150 * sin(fAngle);
        // SDL_RenderDrawPoint(pRenderer,NewCenterX + CenterX,NewCenterY + CenterY);
        // Neue Punkte mit Matrix-Multiplikation berechnen
        for (I = 0; I < MAX_STAR_POINTS; I++) {
            NewStarX[I] = (StarX[I] * cos(-fAngle * 2 * fF) - StarY[I] * sin(fAngle * 4)) + CenterX + NewCenterX ;
            NewStarY[I] = (StarX[I] * sin(fAngle * 4) + StarY[I] * cos(-fAngle * 2 * fF)) + CenterY  + NewCenterY;
            //SDL_RenderDrawPoint(pRenderer,NewStarX[I],NewStarY[I]);
        }
        if (bUp) {
            fF = fF + 0.001;
            if (fF > 1) {
                bUp = false;
            }
        } else {
            fF = fF - 0.001;
            if (fF < 0.5) {
                bUp = true;
            }
        }
        fAngle = fAngle + 0.01;
        if (fAngle > 10 * 3.14159) fAngle = 0;


        if (bUp) {
            uRed = 0xFF;
            uGreen = 0xFF;
            uBlue = 0xFF;
        } else {
            uRainBowColor = GetRainbowColors(uRain);
            uBlue = uRainBowColor;
            uGreen = uRainBowColor >> 8;
            uRed = uRainBowColor >> 16;
            uRain++;
            if (uRain == 255) uRain = 0;
        }
        SDL_SetRenderDrawColor(pRenderer,(float)uRed * fColorDimmP,(float)uGreen * fColorDimmP,(float)uBlue * fColorDimmP,SDL_ALPHA_OPAQUE);
        // Linien
        for (I = 0; I < MAX_STAR_POINTS; I++) {
            if (I < (MAX_STAR_POINTS - 1)) {
                SDL_RenderDrawLine(pRenderer,NewStarX[I],NewStarY[I],NewStarX[I + 1],NewStarY[I + 1]);
            } else {
                SDL_RenderDrawLine(pRenderer,NewStarX[I],NewStarY[I],NewStarX[0],NewStarY[0]);
            }
        }
        /////////////////////  Stern-Ende
        PrintLittleFont(pRenderer,570, 50,0,"KEYBOARD",K_RELATIVE,1);
        PrintLittleFont(pRenderer,950, 50,0,"OTHER SETTINGS",K_RELATIVE,1);
        if (!Video.bMustUseFullScreen) {
            PrintLittleFont(pRenderer,950, 138,0,"VIDEO",K_RELATIVE,1);
        }
        PrintLittleFont(pRenderer,950, 242,0,"VIDEO RESOLUTION",K_RELATIVE,1);
        if (Config.uInputdevice == 1) {         // Joystick?
            Checkbox_Joy_Btn_Fire_A.bUse = CHK_USE;
            Checkbox_Joy_Btn_Fire_B.bUse = CHK_USE;
            Checkbox_Joy_Btn_Fire_X.bUse = CHK_USE;
            Checkbox_Joy_Btn_Fire_Y.bUse = CHK_USE;
            Checkbox_Joy_Btn_Dynamite_A.bUse = CHK_USE;
            Checkbox_Joy_Btn_Dynamite_B.bUse = CHK_USE;
            Checkbox_Joy_Btn_Dynamite_X.bUse = CHK_USE;
            Checkbox_Joy_Btn_Dynamite_Y.bUse = CHK_USE;
            Checkbox_Joy_Btn_Exit_A.bUse = CHK_USE;
            Checkbox_Joy_Btn_Exit_B.bUse = CHK_USE;
            Checkbox_Joy_Btn_Exit_X.bUse = CHK_USE;
            Checkbox_Joy_Btn_Exit_Y.bUse = CHK_USE;
            Checkbox_Joy_Btn_Exit_None.bUse = CHK_USE;
            Checkbox_GC_Dir_Digitalpad.bUse = CHK_UNUSE;
            Checkbox_GC_Dir_LeftAxis.bUse = CHK_UNUSE;
            Checkbox_GC_Dir_RightAxis.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Fire_A.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Fire_B.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Fire_X.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Fire_Y.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Dynamite_A.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Dynamite_B.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Dynamite_X.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Dynamite_Y.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Exit_A.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Exit_B.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Exit_X.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Exit_Y.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Exit_None.bUse = CHK_UNUSE;
            PrintLittleFont(pRenderer,570,220,0,"JOYSTICK FIRE BUTTON",K_RELATIVE,1);
            PrintLittleFont(pRenderer,570,348,0,"JOYSTICK DYNAMITE BUTTON",K_RELATIVE,1);
            PrintLittleFont(pRenderer,570,478,0,"JOYSTICK EXIT BUTTON",K_RELATIVE,1);
        } else if (Config.uInputdevice == 2) {  // GameController?
            Checkbox_GC_Dir_Digitalpad.bUse = CHK_USE;
            Checkbox_GC_Dir_LeftAxis.bUse = CHK_USE;
            Checkbox_GC_Dir_RightAxis.bUse = CHK_USE;
            Checkbox_GC_Btn_Fire_A.bUse = CHK_USE;
            Checkbox_GC_Btn_Fire_B.bUse = CHK_USE;
            Checkbox_GC_Btn_Fire_X.bUse = CHK_USE;
            Checkbox_GC_Btn_Fire_Y.bUse = CHK_USE;
            Checkbox_GC_Btn_Dynamite_A.bUse = CHK_USE;
            Checkbox_GC_Btn_Dynamite_B.bUse = CHK_USE;
            Checkbox_GC_Btn_Dynamite_X.bUse = CHK_USE;
            Checkbox_GC_Btn_Dynamite_Y.bUse = CHK_USE;
            Checkbox_GC_Btn_Exit_A.bUse = CHK_USE;
            Checkbox_GC_Btn_Exit_B.bUse = CHK_USE;
            Checkbox_GC_Btn_Exit_X.bUse = CHK_USE;
            Checkbox_GC_Btn_Exit_Y.bUse = CHK_USE;
            Checkbox_GC_Btn_Exit_None.bUse = CHK_USE;
            Checkbox_Joy_Btn_Fire_A.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Fire_B.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Fire_X.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Fire_Y.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Dynamite_A.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Dynamite_B.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Dynamite_X.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Dynamite_Y.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Exit_A.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Exit_B.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Exit_X.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Exit_Y.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Exit_None.bUse = CHK_UNUSE;
            PrintLittleFont(pRenderer,570,114,0,"GAMECONTROLLER DIRECTION",K_RELATIVE,1);
            PrintLittleFont(pRenderer,570,220,0,"GAMECONTROLLER FIRE BUTTON",K_RELATIVE,1);
            PrintLittleFont(pRenderer,570,348,0,"GAMECONTROLLER DYNAMITE BUTTON",K_RELATIVE,1);
            PrintLittleFont(pRenderer,570,478,0,"GAMECONTROLLER EXIT BUTTON",K_RELATIVE,1);
            sprintf(szText,"AXIS LEFT: -%05d",abs(Config.nAxisLeftThreshold));
            PrintLittleFont(pRenderer,570, 620,0,szText,K_RELATIVE,1);
            sprintf(szText,"AXIS RIGHT: %05d",Config.nAxisRightThreshold);
            PrintLittleFont(pRenderer,570, 647,0,szText,K_RELATIVE,1);
            sprintf(szText,"AXIS UP:   -%05d",abs(Config.nAxisUpThreshold));
            PrintLittleFont(pRenderer,570, 674,0,szText,K_RELATIVE,1);
            sprintf(szText,"AXIS DOWN:  %05d",Config.nAxisDownThreshold);
            PrintLittleFont(pRenderer,570, 701,0,szText,K_RELATIVE,1);
            // Minus-Buttons für AXIS
            RecAxisButton.x = Video.uXoffs + 760;
            RecAxisButton.y = Video.uYoffs + 616;
            RecAxisButton.w = 22;
            RecAxisButton.h = 22;
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_FONT_BLUE_STEEL_MINUS),NULL,&RecAxisButton,0,NULL, SDL_FLIP_NONE);
            RecAxisButton.x = Video.uXoffs + 760;
            RecAxisButton.y = Video.uYoffs + 643;
            RecAxisButton.w = 22;
            RecAxisButton.h = 22;
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_FONT_BLUE_STEEL_MINUS),NULL,&RecAxisButton,0,NULL, SDL_FLIP_NONE);
            RecAxisButton.x = Video.uXoffs + 760;
            RecAxisButton.y = Video.uYoffs + 670;
            RecAxisButton.w = 22;
            RecAxisButton.h = 22;
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_FONT_BLUE_STEEL_MINUS),NULL,&RecAxisButton,0,NULL, SDL_FLIP_NONE);
            RecAxisButton.x = Video.uXoffs + 760;
            RecAxisButton.y = Video.uYoffs + 697;
            RecAxisButton.w = 22;
            RecAxisButton.h = 22;
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_FONT_BLUE_STEEL_MINUS),NULL,&RecAxisButton,0,NULL, SDL_FLIP_NONE);
            // Plus-Buttons für AXIS
            RecAxisButton.x = Video.uXoffs + 810;
            RecAxisButton.y = Video.uYoffs + 616;
            RecAxisButton.w = 22;
            RecAxisButton.h = 22;
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_FONT_BLUE_STEEL_PLUS),NULL,&RecAxisButton,0,NULL, SDL_FLIP_NONE);
            RecAxisButton.x = Video.uXoffs + 810;
            RecAxisButton.y = Video.uYoffs + 643;
            RecAxisButton.w = 22;
            RecAxisButton.h = 22;
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_FONT_BLUE_STEEL_PLUS),NULL,&RecAxisButton,0,NULL, SDL_FLIP_NONE);
            RecAxisButton.x = Video.uXoffs + 810;
            RecAxisButton.y = Video.uYoffs + 670;
            RecAxisButton.w = 22;
            RecAxisButton.h = 22;
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_FONT_BLUE_STEEL_PLUS),NULL,&RecAxisButton,0,NULL, SDL_FLIP_NONE);
            RecAxisButton.x = Video.uXoffs + 810;
            RecAxisButton.y = Video.uYoffs + 697;
            RecAxisButton.w = 22;
            RecAxisButton.h = 22;
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_FONT_BLUE_STEEL_PLUS),NULL,&RecAxisButton,0,NULL, SDL_FLIP_NONE);
            nLastAxisButton = nAxisButton;
            nAxisButton = GetSettingsMenuAxisButton();
            if ((nAxisButton == 0) && (nLastAxisButton != 0)) {
                // SDL_Log("AXIS-Button: %d",nLastAxisButton);
                nErrorCode = WriteConfigFile();
            }
            switch (nAxisButton) {
                case (1):       // AXIS LEFT MINUS
                    if (Config.nAxisLeftThreshold > (-32768 + 100)) {
                        Config.nAxisLeftThreshold = Config.nAxisLeftThreshold - 100;
                    }
                    break;
                case (2):       // AXIS LEFT PLUS
                    if (Config.nAxisLeftThreshold < - 100) {
                        Config.nAxisLeftThreshold = Config.nAxisLeftThreshold + 100;
                    }
                    break;
                case (3):       // AXIS RIGHT MINUS
                    if (Config.nAxisRightThreshold > 100) {
                        Config.nAxisRightThreshold = Config.nAxisRightThreshold - 100;
                    }
                    break;
                case (4):       // AXIS RIGHT PLUS
                    if (Config.nAxisRightThreshold < 32767 - 100) {
                        Config.nAxisRightThreshold = Config.nAxisRightThreshold + 100;
                    }
                    break;
                case (5):       // AXIS UP MINUS
                    if (Config.nAxisUpThreshold > (-32768 + 100)) {
                        Config.nAxisUpThreshold = Config.nAxisUpThreshold - 100;
                    }
                    break;
                case (6):       // AXIS UP PLUS
                    if (Config.nAxisUpThreshold < - 100) {
                        Config.nAxisUpThreshold = Config.nAxisUpThreshold + 100;
                    }
                    break;
                case (7):       // AXIS DOWN MINUS
                    if (Config.nAxisDownThreshold > 100) {
                        Config.nAxisDownThreshold = Config.nAxisDownThreshold - 100;
                    }
                    break;
                case (8):       // AXIS DOWN PLUS
                    if (Config.nAxisDownThreshold < 32767 - 100) {
                        Config.nAxisDownThreshold = Config.nAxisDownThreshold + 100;
                    }
                    break;
            }

        } else {    // Keyboard
            Checkbox_Joy_Btn_Fire_A.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Fire_B.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Fire_X.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Fire_Y.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Dynamite_A.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Dynamite_B.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Dynamite_X.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Dynamite_Y.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Exit_A.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Exit_B.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Exit_X.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Exit_Y.bUse = CHK_UNUSE;
            Checkbox_Joy_Btn_Exit_None.bUse = CHK_UNUSE;
            Checkbox_GC_Dir_Digitalpad.bUse = CHK_UNUSE;
            Checkbox_GC_Dir_LeftAxis.bUse = CHK_UNUSE;
            Checkbox_GC_Dir_RightAxis.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Fire_A.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Fire_B.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Fire_X.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Fire_Y.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Dynamite_A.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Dynamite_B.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Dynamite_X.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Dynamite_Y.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Exit_A.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Exit_B.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Exit_X.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Exit_Y.bUse = CHK_UNUSE;
            Checkbox_GC_Btn_Exit_None.bUse = CHK_UNUSE;
            PrintLittleFont(pRenderer,600,100,0,"KEYBINDING",K_RELATIVE,1);
            PrintLittleFont(pRenderer,630,123,0,"LEFT:",K_RELATIVE,1);
            nScanCodeIndex = GetSdlScanCodeNameIndex(Config.uKeyboardScancodeLeft);
            if (nScanCodeIndex != -1) {
                PrintLittleFont(pRenderer,700,123,0,g_ScanCodeNames[nScanCodeIndex],K_RELATIVE,1);
            } else {
                PrintLittleFont(pRenderer,700,123,0,"UNKNOWN",K_RELATIVE,1);
            }
            PrintLittleFont(pRenderer,630,146,0,"RIGHT:",K_RELATIVE,1);
            nScanCodeIndex = GetSdlScanCodeNameIndex(Config.uKeyboardScancodeRight);
            if (nScanCodeIndex != -1) {
                PrintLittleFont(pRenderer,700,146,0,g_ScanCodeNames[nScanCodeIndex],K_RELATIVE,1);
            } else {
                PrintLittleFont(pRenderer,700,146,0,"UNKNOWN",K_RELATIVE,1);
            }
            PrintLittleFont(pRenderer,630,169,0,"UP:",K_RELATIVE,1);
            nScanCodeIndex = GetSdlScanCodeNameIndex(Config.uKeyboardScancodeUp);
            if (nScanCodeIndex != -1) {
                PrintLittleFont(pRenderer,700,169,0,g_ScanCodeNames[nScanCodeIndex],K_RELATIVE,1);
            } else {
                PrintLittleFont(pRenderer,700,169,0,"UNKNOWN",K_RELATIVE,1);
            }
            PrintLittleFont(pRenderer,630,192,0,"DOWN:",K_RELATIVE,1);
            nScanCodeIndex = GetSdlScanCodeNameIndex(Config.uKeyboardScancodeDown);
            if (nScanCodeIndex != -1) {
                PrintLittleFont(pRenderer,700,192,0,g_ScanCodeNames[nScanCodeIndex],K_RELATIVE,1);
            } else {
                PrintLittleFont(pRenderer,700,192,0,"UNKNOWN",K_RELATIVE,1);
            }
            PrintLittleFont(pRenderer,630,215,0,"FIRE:",K_RELATIVE,1);
            nScanCodeIndex = GetSdlScanCodeNameIndex(Config.uKeyboardScancodeFire);
            if (nScanCodeIndex != -1) {
                PrintLittleFont(pRenderer,700,215,0,g_ScanCodeNames[nScanCodeIndex],K_RELATIVE,1);
            } else {
                PrintLittleFont(pRenderer,700,215,0,"UNKNOWN",K_RELATIVE,1);
            }
            // Buttons für Tastenbelegung zeichnen
            KeyboardButton.w = 18;
            KeyboardButton.h = 18;
            KeyboardButton.x = Video.uXoffs + 601;
            KeyboardButton.y = Video.uYoffs + 121;
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_STEEL_GROW_ALL),NULL,&KeyboardButton,0,NULL, SDL_FLIP_NONE);
            KeyboardButton.y = Video.uYoffs + 144;
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_STEEL_GROW_ALL),NULL,&KeyboardButton,0,NULL, SDL_FLIP_NONE);
            KeyboardButton.y = Video.uYoffs + 167;
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_STEEL_GROW_ALL),NULL,&KeyboardButton,0,NULL, SDL_FLIP_NONE);
            KeyboardButton.y = Video.uYoffs + 190;
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_STEEL_GROW_ALL),NULL,&KeyboardButton,0,NULL, SDL_FLIP_NONE);
            KeyboardButton.y = Video.uYoffs + 213;
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_STEEL_GROW_ALL),NULL,&KeyboardButton,0,NULL, SDL_FLIP_NONE);
            nKeyboardButton = GetSettingsMenuKeyboardButton();
            switch (nKeyboardButton) {
                case (1):
                    nKeybindingState = nKeyboardButton;
                    strcpy(szSubString,"'LEFT'");
                    break;
                case (2):
                    nKeybindingState = nKeyboardButton;
                    strcpy(szSubString,"'RIGHT'");
                    break;
                case (3):
                    nKeybindingState = nKeyboardButton;
                    strcpy(szSubString,"'UP'");
                    break;
                case (4):
                    nKeybindingState = nKeyboardButton;
                    strcpy(szSubString,"'DOWN'");
                    break;
                case (5):
                    nKeybindingState = nKeyboardButton;
                    strcpy(szSubString,"'FIRE'");
                    break;
                case (6):
                    nKeybindingState = 0;
                    break;
            }
            if (nKeybindingState > 0) {
                if ((Playfield.uFrameCounter >> 3) % 2 == 0) {
                    uBlinkColor = 0;
                } else {
                    uBlinkColor = 2;
                }
                PrintLittleFont(pRenderer,600,250,uBlinkColor,"PRESS A KEY FOR ",K_RELATIVE,1);
                PrintLittleFont(pRenderer,760,250,uBlinkColor,szSubString,K_RELATIVE,1);
                nScanCodeIndex = GetScancodeIndex(); // ScanCode einer gedrückten Taste ermitteln
                if (nScanCodeIndex != -1) {
                    // SDL_Log("ScanCode: %s",g_ScanCodeNames[nScanCodeIndex]);
                    switch (nKeybindingState) {
                        case (1):   // links
                            Config.uKeyboardScancodeLeft = g_uScanCodes[nScanCodeIndex];
                            break;
                        case (2):   // rechts
                            Config.uKeyboardScancodeRight = g_uScanCodes[nScanCodeIndex];
                            break;
                        case (3):   // oben
                            Config.uKeyboardScancodeUp = g_uScanCodes[nScanCodeIndex];
                            break;
                        case (4):   // unten
                            Config.uKeyboardScancodeDown = g_uScanCodes[nScanCodeIndex];
                            break;
                        case (5):   // fire
                            Config.uKeyboardScancodeFire = g_uScanCodes[nScanCodeIndex];
                            break;
                    }
                    nKeybindingState = 0;
                    nErrorCode = WriteConfigFile();
                }
            }
        }
        if (Checkbox_StartDynamiteKeyboard.bChanged) {
            if (Checkbox_StartDynamiteKeyboard.bActive) {
                Config.bStartDynamiteWithSpace = true;
            } else {
                Config.bStartDynamiteWithSpace = false;
            }
            Checkbox_StartDynamiteKeyboard.bChanged = false; // bestätigen
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_Gamemusic.bChanged) {
            if (Checkbox_Gamemusic.bActive) {
                Config.bGameMusic = true;
            } else {
                Config.bGameMusic = false;
            }
            Checkbox_Gamemusic.bChanged = false; // bestätigen
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_ShowHighScores.bChanged) {
            if (Checkbox_ShowHighScores.bActive) {
                Config.bShowHighscores = true;
            } else {
                Config.bShowHighscores = false;
            }
            Checkbox_ShowHighScores.bChanged = false; // bestätigen
            nErrorCode = WriteConfigFile();
        }
        ////////////////// RADIO FULL SCREEN ////////////////////////////
        if (Checkbox_FullScreen.bChanged) {
            if (Checkbox_FullScreen.bActive) {
                Config.bFullScreen = true;
                nErrorCode = SDL_SetWindowFullscreen(Video.pWindow,SDL_WINDOW_FULLSCREEN);
                if (nErrorCode != 0) {
                    SDL_Log("%s: SDL_SetWindowFullscreen() failed, Error: %s",__FUNCTION__,SDL_GetError());
                }
            } else {
                Config.bFullScreen = false;
                // Unter Ubuntu 16.04 (vielleicht auch bei anderen Linuxen) ist es wichtig, nach Ausschalten des Fullscreens wieder
                // zunächst die ursprüngliche Desktop-Auflösung herzustellen.
                SDL_SetWindowSize(Video.pWindow,Video.DesktopDisplayMode.w,Video.DesktopDisplayMode.h);    // Ist erst in SDL3 eine int-Funktion
                nErrorCode = SDL_SetWindowFullscreen(Video.pWindow,0); //  0 = Fullscreen  ausschalten
                SDL_SetWindowSize(Video.pWindow,Config.uResX,Config.uResY);            // Ist erst in SDL3 eine int-Funktion
                nErrorCode = CenterWindow(Config.uResX,Config.uResY);
                InitMainMenu(WINDOW_CENTER_DEFAULT);
            }
            Checkbox_FullScreen.bChanged = false; // bestätigen
            nErrorCode = WriteConfigFile();
        }
        ////////////////// RADIO RESOLUTION ////////////////////////////
        for (I = 0; I < Video.nShowableDisplayModeCount; I++) {
            if (Checkbox_Resolution[I].bChanged) {
                Checkbox_Resolution[I].bChanged = false;
                Checkbox_Resolution[I].bActive = true;
                // SDL_Log("Resolution Checkbox :%d ticked",I);
                for (II = 0; II < Video.nShowableDisplayModeCount; II++) {
                    if (II != I) {
                        Checkbox_Resolution[II].bActive = false;
                    }
                }
                // Auflösung wurde geändert
                if ((Config.uResX != Video.ShowableDisplayModes[I].nW) || (Config.uResY != Video.ShowableDisplayModes[I].nH)) {
                    Config.uResX = Video.ShowableDisplayModes[I].nW;
                    Config.uResY = Video.ShowableDisplayModes[I].nH;
                    // SDL_Log("Resolution changed to %d X %d",Video.ShowableDisplayModes[I].nW,Video.ShowableDisplayModes[I].nH);
                    // Fenstergröße ändern
                    if (Video.bMustUseFullScreen || Config.bFullScreen) {
                        nErrorCode = SetWindowDisplayMode(Config.uResX,Config.uResY,Video.DesktopDisplayMode.format,60);
                    } else {
                        // Fenster muss nur zentriert werden, wenn kein FullScreen
                        nErrorCode = CenterWindow(Config.uResX,Config.uResY);
                    }
                    SDL_SetWindowSize(Video.pWindow,Config.uResX,Config.uResY); // Ist erst in SDL3 eine int-Funktion
                    InitMainMenu(WINDOW_CENTER_DEFAULT);
                    nErrorCode = WriteConfigFile();
                    // Stern muss neu initialisiert und zentriert werden
                    CenterX = Config.uResX / 2;
                    CenterY = Config.uResY / 2;
                    uRain = 0;
                    fAngle = 0;
                }
            }
        }
        ////////////////// RADIO DIRECTION /////////////////////////////
        if (Checkbox_GC_Dir_Digitalpad.bChanged) {
            Checkbox_GC_Dir_Digitalpad.bChanged = false;
            Checkbox_GC_Dir_Digitalpad.bActive = true;
            Checkbox_GC_Dir_LeftAxis.bActive = false;
            Checkbox_GC_Dir_RightAxis.bActive = false;
            Config.uControllerDirections = 0;
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_GC_Dir_LeftAxis.bChanged) {
            Checkbox_GC_Dir_LeftAxis.bChanged = false;
            Checkbox_GC_Dir_Digitalpad.bActive = false;
            Checkbox_GC_Dir_LeftAxis.bActive = true;
            Checkbox_GC_Dir_RightAxis.bActive = false;
            Config.uControllerDirections = 1;
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_GC_Dir_RightAxis.bChanged) {
            Checkbox_GC_Dir_RightAxis.bChanged = false;
            Checkbox_GC_Dir_Digitalpad.bActive = false;
            Checkbox_GC_Dir_LeftAxis.bActive = false;
            Checkbox_GC_Dir_RightAxis.bActive = true;
            Config.uControllerDirections = 2;
            nErrorCode = WriteConfigFile();
        }
        /////////////// RADIO FIRE BUTTON //////////////////////////////
        if (Checkbox_GC_Btn_Fire_A.bChanged) {
            Checkbox_GC_Btn_Fire_A.bChanged = false;
            Checkbox_GC_Btn_Fire_A.bActive = true;
            Checkbox_GC_Btn_Fire_B.bActive = false;
            Checkbox_GC_Btn_Fire_X.bActive = false;
            Checkbox_GC_Btn_Fire_Y.bActive = false;
            Config.cControllerFireButton = 'A';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_GC_Btn_Fire_B.bChanged) {
            Checkbox_GC_Btn_Fire_B.bChanged = false;
            Checkbox_GC_Btn_Fire_B.bActive = true;
            Checkbox_GC_Btn_Fire_A.bActive = false;
            Checkbox_GC_Btn_Fire_X.bActive = false;
            Checkbox_GC_Btn_Fire_Y.bActive = false;
            Config.cControllerFireButton = 'B';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_GC_Btn_Fire_X.bChanged) {
            Checkbox_GC_Btn_Fire_X.bChanged = false;
            Checkbox_GC_Btn_Fire_X.bActive = true;
            Checkbox_GC_Btn_Fire_A.bActive = false;
            Checkbox_GC_Btn_Fire_B.bActive = false;
            Checkbox_GC_Btn_Fire_Y.bActive = false;
            Config.cControllerFireButton = 'X';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_GC_Btn_Fire_Y.bChanged) {
            Checkbox_GC_Btn_Fire_Y.bChanged = false;
            Checkbox_GC_Btn_Fire_Y.bActive = true;
            Checkbox_GC_Btn_Fire_A.bActive = false;
            Checkbox_GC_Btn_Fire_B.bActive = false;
            Checkbox_GC_Btn_Fire_X.bActive = false;
            Config.cControllerFireButton = 'Y';
            nErrorCode = WriteConfigFile();
        }
        /////////////// RADIO START DYNAMITE BUTTON ////////////////////
        if (Checkbox_GC_Btn_Dynamite_A.bChanged) {
            Checkbox_GC_Btn_Dynamite_A.bChanged = false;
            Checkbox_GC_Btn_Dynamite_A.bActive = true;
            Checkbox_GC_Btn_Dynamite_B.bActive = false;
            Checkbox_GC_Btn_Dynamite_X.bActive = false;
            Checkbox_GC_Btn_Dynamite_Y.bActive = false;
            Config.cControllerStartDynamiteButton = 'A';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_GC_Btn_Dynamite_B.bChanged) {
            Checkbox_GC_Btn_Dynamite_B.bChanged = false;
            Checkbox_GC_Btn_Dynamite_B.bActive = true;
            Checkbox_GC_Btn_Dynamite_A.bActive = false;
            Checkbox_GC_Btn_Dynamite_X.bActive = false;
            Checkbox_GC_Btn_Dynamite_Y.bActive = false;
            Config.cControllerStartDynamiteButton = 'B';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_GC_Btn_Dynamite_X.bChanged) {
            Checkbox_GC_Btn_Dynamite_X.bChanged = false;
            Checkbox_GC_Btn_Dynamite_X.bActive = true;
            Checkbox_GC_Btn_Dynamite_A.bActive = false;
            Checkbox_GC_Btn_Dynamite_B.bActive = false;
            Checkbox_GC_Btn_Dynamite_Y.bActive = false;
            Config.cControllerStartDynamiteButton = 'X';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_GC_Btn_Dynamite_Y.bChanged) {
            Checkbox_GC_Btn_Dynamite_Y.bChanged = false;
            Checkbox_GC_Btn_Dynamite_Y.bActive = true;
            Checkbox_GC_Btn_Dynamite_A.bActive = false;
            Checkbox_GC_Btn_Dynamite_B.bActive = false;
            Checkbox_GC_Btn_Dynamite_X.bActive = false;
            Config.cControllerStartDynamiteButton = 'Y';
            nErrorCode = WriteConfigFile();
        }
        /////////////// RADIO EXIT BUTTON //////////////////////////////
        if (Checkbox_GC_Btn_Exit_A.bChanged) {
            Checkbox_GC_Btn_Exit_A.bChanged = false;
            Checkbox_GC_Btn_Exit_A.bActive = true;
            Checkbox_GC_Btn_Exit_B.bActive = false;
            Checkbox_GC_Btn_Exit_X.bActive = false;
            Checkbox_GC_Btn_Exit_Y.bActive = false;
            Checkbox_GC_Btn_Exit_None.bActive = false;
            Config.cControllerExitButton = 'A';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_GC_Btn_Exit_B.bChanged) {
            Checkbox_GC_Btn_Exit_B.bChanged = false;
            Checkbox_GC_Btn_Exit_B.bActive = true;
            Checkbox_GC_Btn_Exit_A.bActive = false;
            Checkbox_GC_Btn_Exit_X.bActive = false;
            Checkbox_GC_Btn_Exit_Y.bActive = false;
            Checkbox_GC_Btn_Exit_None.bActive = false;
            Config.cControllerExitButton = 'B';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_GC_Btn_Exit_X.bChanged) {
            Checkbox_GC_Btn_Exit_X.bChanged = false;
            Checkbox_GC_Btn_Exit_X.bActive = true;
            Checkbox_GC_Btn_Exit_A.bActive = false;
            Checkbox_GC_Btn_Exit_B.bActive = false;
            Checkbox_GC_Btn_Exit_Y.bActive = false;
            Checkbox_GC_Btn_Exit_None.bActive = false;
            Config.cControllerExitButton = 'X';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_GC_Btn_Exit_Y.bChanged) {
            Checkbox_GC_Btn_Exit_Y.bChanged = false;
            Checkbox_GC_Btn_Exit_Y.bActive = true;
            Checkbox_GC_Btn_Exit_A.bActive = false;
            Checkbox_GC_Btn_Exit_B.bActive = false;
            Checkbox_GC_Btn_Exit_X.bActive = false;
            Checkbox_GC_Btn_Exit_None.bActive = false;
            Config.cControllerExitButton = 'Y';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_GC_Btn_Exit_None.bChanged) {
            Checkbox_GC_Btn_Exit_None.bChanged = false;
            Checkbox_GC_Btn_Exit_Y.bActive = false;
            Checkbox_GC_Btn_Exit_A.bActive = false;
            Checkbox_GC_Btn_Exit_B.bActive = false;
            Checkbox_GC_Btn_Exit_X.bActive = false;
            Checkbox_GC_Btn_Exit_None.bActive = true;
            Config.cControllerExitButton = 'N';
            nErrorCode = WriteConfigFile();
        }

        /////////////// JOYSTICK RADIO FIRE BUTTON //////////////////////////////
        if (Checkbox_Joy_Btn_Fire_A.bChanged) {
            Checkbox_Joy_Btn_Fire_A.bChanged = false;
            Checkbox_Joy_Btn_Fire_A.bActive = true;
            Checkbox_Joy_Btn_Fire_B.bActive = false;
            Checkbox_Joy_Btn_Fire_X.bActive = false;
            Checkbox_Joy_Btn_Fire_Y.bActive = false;
            Config.cJoystickFireButton = 'A';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_Joy_Btn_Fire_B.bChanged) {
            Checkbox_Joy_Btn_Fire_B.bChanged = false;
            Checkbox_Joy_Btn_Fire_B.bActive = true;
            Checkbox_Joy_Btn_Fire_A.bActive = false;
            Checkbox_Joy_Btn_Fire_X.bActive = false;
            Checkbox_Joy_Btn_Fire_Y.bActive = false;
            Config.cJoystickFireButton = 'B';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_Joy_Btn_Fire_X.bChanged) {
            Checkbox_Joy_Btn_Fire_X.bChanged = false;
            Checkbox_Joy_Btn_Fire_X.bActive = true;
            Checkbox_Joy_Btn_Fire_A.bActive = false;
            Checkbox_Joy_Btn_Fire_B.bActive = false;
            Checkbox_Joy_Btn_Fire_Y.bActive = false;
            Config.cJoystickFireButton = 'X';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_Joy_Btn_Fire_Y.bChanged) {
            Checkbox_Joy_Btn_Fire_Y.bChanged = false;
            Checkbox_Joy_Btn_Fire_Y.bActive = true;
            Checkbox_Joy_Btn_Fire_A.bActive = false;
            Checkbox_Joy_Btn_Fire_B.bActive = false;
            Checkbox_Joy_Btn_Fire_X.bActive = false;
            Config.cJoystickFireButton = 'Y';
            nErrorCode = WriteConfigFile();
        }
        /////////////// JOYSTICK RADIO START DYNAMITE BUTTON ////////////////////
        if (Checkbox_Joy_Btn_Dynamite_A.bChanged) {
            Checkbox_Joy_Btn_Dynamite_A.bChanged = false;
            Checkbox_Joy_Btn_Dynamite_A.bActive = true;
            Checkbox_Joy_Btn_Dynamite_B.bActive = false;
            Checkbox_Joy_Btn_Dynamite_X.bActive = false;
            Checkbox_Joy_Btn_Dynamite_Y.bActive = false;
            Config.cJoystickStartDynamiteButton = 'A';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_Joy_Btn_Dynamite_B.bChanged) {
            Checkbox_Joy_Btn_Dynamite_B.bChanged = false;
            Checkbox_Joy_Btn_Dynamite_B.bActive = true;
            Checkbox_Joy_Btn_Dynamite_A.bActive = false;
            Checkbox_Joy_Btn_Dynamite_X.bActive = false;
            Checkbox_Joy_Btn_Dynamite_Y.bActive = false;
            Config.cJoystickStartDynamiteButton = 'B';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_Joy_Btn_Dynamite_X.bChanged) {
            Checkbox_Joy_Btn_Dynamite_X.bChanged = false;
            Checkbox_Joy_Btn_Dynamite_X.bActive = true;
            Checkbox_Joy_Btn_Dynamite_A.bActive = false;
            Checkbox_Joy_Btn_Dynamite_B.bActive = false;
            Checkbox_Joy_Btn_Dynamite_Y.bActive = false;
            Config.cJoystickStartDynamiteButton = 'X';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_Joy_Btn_Dynamite_Y.bChanged) {
            Checkbox_Joy_Btn_Dynamite_Y.bChanged = false;
            Checkbox_Joy_Btn_Dynamite_Y.bActive = true;
            Checkbox_Joy_Btn_Dynamite_A.bActive = false;
            Checkbox_Joy_Btn_Dynamite_B.bActive = false;
            Checkbox_Joy_Btn_Dynamite_X.bActive = false;
            Config.cJoystickStartDynamiteButton = 'Y';
            nErrorCode = WriteConfigFile();
        }
        /////////////// JOYSTICK RADIO EXIT BUTTON //////////////////////////////
        if (Checkbox_Joy_Btn_Exit_A.bChanged) {
            Checkbox_Joy_Btn_Exit_A.bChanged = false;
            Checkbox_Joy_Btn_Exit_A.bActive = true;
            Checkbox_Joy_Btn_Exit_B.bActive = false;
            Checkbox_Joy_Btn_Exit_X.bActive = false;
            Checkbox_Joy_Btn_Exit_Y.bActive = false;
            Checkbox_Joy_Btn_Exit_None.bActive = false;
            Config.cJoystickExitButton = 'A';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_Joy_Btn_Exit_B.bChanged) {
            Checkbox_Joy_Btn_Exit_B.bChanged = false;
            Checkbox_Joy_Btn_Exit_B.bActive = true;
            Checkbox_Joy_Btn_Exit_A.bActive = false;
            Checkbox_Joy_Btn_Exit_X.bActive = false;
            Checkbox_Joy_Btn_Exit_Y.bActive = false;
            Checkbox_Joy_Btn_Exit_None.bActive = false;
            Config.cJoystickExitButton = 'B';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_Joy_Btn_Exit_X.bChanged) {
            Checkbox_Joy_Btn_Exit_X.bChanged = false;
            Checkbox_Joy_Btn_Exit_X.bActive = true;
            Checkbox_Joy_Btn_Exit_A.bActive = false;
            Checkbox_Joy_Btn_Exit_B.bActive = false;
            Checkbox_Joy_Btn_Exit_Y.bActive = false;
            Checkbox_Joy_Btn_Exit_None.bActive = false;
            Config.cJoystickExitButton = 'X';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_Joy_Btn_Exit_Y.bChanged) {
            Checkbox_Joy_Btn_Exit_Y.bChanged = false;
            Checkbox_Joy_Btn_Exit_Y.bActive = true;
            Checkbox_Joy_Btn_Exit_A.bActive = false;
            Checkbox_Joy_Btn_Exit_B.bActive = false;
            Checkbox_Joy_Btn_Exit_X.bActive = false;
            Checkbox_Joy_Btn_Exit_None.bActive = false;
            Config.cJoystickExitButton = 'Y';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_Joy_Btn_Exit_None.bChanged) {
            Checkbox_Joy_Btn_Exit_None.bChanged = false;
            Checkbox_Joy_Btn_Exit_Y.bActive = false;
            Checkbox_Joy_Btn_Exit_A.bActive = false;
            Checkbox_Joy_Btn_Exit_B.bActive = false;
            Checkbox_Joy_Btn_Exit_X.bActive = false;
            Checkbox_GC_Btn_Exit_None.bActive = true;
            Config.cJoystickExitButton = 'N';
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_Display1.bChanged) {
            Checkbox_Display1.bChanged = false;
            Checkbox_Display1.bActive = true;
            Checkbox_Display2.bActive = false;
            Config.uDisplay = 0;
            nErrorCode = WriteConfigFile();
        }
        if (Checkbox_Display2.bChanged) {
            Checkbox_Display2.bChanged = false;
            Checkbox_Display2.bActive = true;
            Checkbox_Display1.bActive = false;
            Config.uDisplay = 1;
            nErrorCode = WriteConfigFile();
        }
        if (IsButtonPressed(BUTTONLABEL_EXIT_HIGHSCORES) || (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) || (InputStates.bQuit)) {
            bPrepareExit = true;
        }
        //printf("x:%u  y:%u\n",InputStates.nMouseXpos,InputStates.nMouseYpos);
        if ((!bPrepareExit) && (nColorDimm < 100)) {
            nColorDimm = nColorDimm + 4;
            SetAllTextureColors(nColorDimm);
            uMusicVolume = uMusicVolume + 4;
            SetMusicVolume(uMusicVolume);
        } else if (bPrepareExit) {
            if (nColorDimm > 0) {
                nColorDimm = nColorDimm - 4;
                SetAllTextureColors(nColorDimm);
                uMusicVolume = uMusicVolume -4;
                SetMusicVolume(uMusicVolume);
            } else {
                bExit = true;
            }
        }
        if (Config.uInputdevice == 1) { // Joystick?
            nXpos = 288;
            if (Config.uDeviceIndex > (MAX_JOYSTICKS - 1)) {    // Bei falschem Wert auf Joystick 0 stellen
                Config.uDeviceIndex = 0;
            }
            nYpos = 96 + Config.uDeviceIndex * 160;
            bShowRec1 = true;
        } else if (Config.uInputdevice == 2) {  // Gamecontroller?
            nXpos = 32;
            if (Config.uDeviceIndex > (MAX_GAMECONTROLLERS - 1)) {    // Bei falschem Wert auf Gamecontroller 0 stellen
                Config.uDeviceIndex = 0;
            }
            nYpos = 96 + Config.uDeviceIndex * 160;
            bShowRec1 = true;
        } else {
            nXpos = 32;
            nYpos = 576;
            bShowRec1 = false;
        }
        if (SDL_GetTicks() - uLastDetectionTime > 2000) {
            DetectJoystickAndGameController();
            uLastDetectionTime = SDL_GetTicks();
        }
        for (I = 0; I < MAX_GAMECONTROLLERS; I++) {
            if (GameController.ID[I] != -1) {
                DrawGrid(pRenderer,32,96 + I * 160, 224,128,0,(128 * nColorDimm) / 100,0,255,4);
            } else {
                DrawGrid(pRenderer,32,96 + I * 160, 224,128,(128 * nColorDimm) / 100,0,0,255,4);
            }
        }
        for (I = 0; I < MAX_JOYSTICKS; I++) {
            if (Joystick.ID[I] != -1) {
                DrawGrid(pRenderer,288,96 + I * 160, 224,128,0,(128 * nColorDimm) / 100,0,255,4);
            } else {
                DrawGrid(pRenderer,288,96 + I * 160, 224,128,(128 * nColorDimm) / 100,0,0,255,4);
            }
        }
        nErrorCode = SDL_SetRenderDrawColor(pRenderer,0,0,0,SDL_ALPHA_OPAQUE);
        if (nErrorCode == 0) {
            nErrorCode = RenderMenuElements(pRenderer);
            if (nErrorCode == 0) {
                nErrorCode = ShowControllersAndJoysticks(pRenderer);
                if (nErrorCode == 0) {
                    if (bShowRec1) {
                        // Farbe für Rahmen 1 scrollen
                        RotateColors(RgbColorsRec1,nRec1PixelCount,5);
                        nErrorCode = ShowRec(pRenderer,RgbColorsRec1,nXpos,nYpos,COLOR_REC_1_W,COLOR_REC_1_H,nColorDimm);
                    } else {
                        // Farbe für Rahmen 2 scrollen
                        RotateColors(RgbColorsRec2,nRec2PixelCount,5);
                        nErrorCode = ShowRec(pRenderer,RgbColorsRec2,nXpos,nYpos,COLOR_REC_2_W,COLOR_REC_2_H,nColorDimm);
                    }
                }
            }
        }
        ShowCheckboxes(pRenderer,nColorDimm,K_RELATIVE);
        nLastButton = nButton;
        nButton = GetSettingsMenuButton();
        if (((nButton != 0) && (nLastButton != nButton)) && (nErrorCode == 0)) {
            // SDL_Log("Button: %d",nButton);
            switch (nButton) {
                case (1):   // Gamecontroller 0
                    Config.uInputdevice = 2;
                    Config.uDeviceIndex = 0;
                    break;
                case (2):   // Gamecontroller 1
                    Config.uInputdevice = 2;
                    Config.uDeviceIndex = 1;
                    break;
                case (3):   // Gamecontroller 2
                    Config.uInputdevice = 2;
                    Config.uDeviceIndex = 2;
                    break;
                case (4):   // Joystick 0
                    Config.uInputdevice = 1;
                    Config.uDeviceIndex = 0;
                    break;
                case (5):   // Joystick 1
                    Config.uInputdevice = 1;
                    Config.uDeviceIndex = 1;
                    break;
                case (6):   // Joystick 2
                    Config.uInputdevice = 1;
                    Config.uDeviceIndex = 2;
                    break;
                case (7):   // Keyboard
                    Config.uInputdevice = 0;
                    Config.uDeviceIndex = 0;
                    break;
            }
            nErrorCode = WriteConfigFile();
        }
        PlayMusic(false);
        ShowButtons(pRenderer,K_RELATIVE);
        RenderPresentAndClear(pRenderer);
        SDL_Delay(5);
        Playfield.uFrameCounter++;
    } while ((!bExit) && (nErrorCode == 0));
    FreeButton(BUTTONLABEL_EXIT_HIGHSCORES);
    DeRegisterCheckbox(&Checkbox_StartDynamiteKeyboard);
    DeRegisterCheckbox(&Checkbox_GC_Dir_Digitalpad);
    DeRegisterCheckbox(&Checkbox_GC_Dir_LeftAxis);
    DeRegisterCheckbox(&Checkbox_GC_Dir_RightAxis);
    DeRegisterCheckbox(&Checkbox_GC_Btn_Fire_A);
    DeRegisterCheckbox(&Checkbox_GC_Btn_Fire_B);
    DeRegisterCheckbox(&Checkbox_GC_Btn_Fire_X);
    DeRegisterCheckbox(&Checkbox_GC_Btn_Fire_Y);
    DeRegisterCheckbox(&Checkbox_GC_Btn_Dynamite_A);
    DeRegisterCheckbox(&Checkbox_GC_Btn_Dynamite_B);
    DeRegisterCheckbox(&Checkbox_GC_Btn_Dynamite_X);
    DeRegisterCheckbox(&Checkbox_GC_Btn_Dynamite_Y);
    DeRegisterCheckbox(&Checkbox_GC_Btn_Exit_A);
    DeRegisterCheckbox(&Checkbox_GC_Btn_Exit_B);
    DeRegisterCheckbox(&Checkbox_GC_Btn_Exit_X);
    DeRegisterCheckbox(&Checkbox_GC_Btn_Exit_Y);
    DeRegisterCheckbox(&Checkbox_GC_Btn_Exit_None);
    DeRegisterCheckbox(&Checkbox_Joy_Btn_Fire_A);
    DeRegisterCheckbox(&Checkbox_Joy_Btn_Fire_B);
    DeRegisterCheckbox(&Checkbox_Joy_Btn_Fire_X);
    DeRegisterCheckbox(&Checkbox_Joy_Btn_Fire_Y);
    DeRegisterCheckbox(&Checkbox_Joy_Btn_Dynamite_A);
    DeRegisterCheckbox(&Checkbox_Joy_Btn_Dynamite_B);
    DeRegisterCheckbox(&Checkbox_Joy_Btn_Dynamite_X);
    DeRegisterCheckbox(&Checkbox_Joy_Btn_Dynamite_Y);
    DeRegisterCheckbox(&Checkbox_Joy_Btn_Exit_A);
    DeRegisterCheckbox(&Checkbox_Joy_Btn_Exit_B);
    DeRegisterCheckbox(&Checkbox_Joy_Btn_Exit_X);
    DeRegisterCheckbox(&Checkbox_Joy_Btn_Exit_Y);
    DeRegisterCheckbox(&Checkbox_Joy_Btn_Exit_None);
    DeRegisterCheckbox(&Checkbox_Gamemusic);
    DeRegisterCheckbox(&Checkbox_ShowHighScores);
    DeRegisterCheckbox(&Checkbox_FullScreen);
    DeRegisterCheckbox(&Checkbox_Display1);
    DeRegisterCheckbox(&Checkbox_Display2);
    for (I = 0; I < Video.nShowableDisplayModeCount; I++) {
        DeRegisterCheckbox(&Checkbox_Resolution[I]);
    }
    DetectJoystickAndGameController();
    OpenJoystickOrGameController();
    WaitNoKey();
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetSettingsMenuAxisButton
------------------------------------------------------------------------------
Beschreibung: Prüft, welcher Plus-/Minus-Button für die AXIS-Bereiche gedrückt ist.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = kein Button gedrückt
                    1 = Minus LEFT AXIS
                    2 = Plus LEFT AXIS
                    3 = Minus RIGHT AXIS
                    4 = Plus RIGHT AXIS
                    5 = Minus UP-AXIS
                    6 = Plus UP AXIS
                    7 = Minus DOWN AXIS
                    8 = Plus DOWN AXIS
Seiteneffekte: InputStates.x
------------------------------------------------------------------------------*/
int GetSettingsMenuAxisButton(void) {
    int nButton = 0;

    if (InputStates.bLeftMouseButton) {
        if ((InputStates.nMouseXpos_Relative >= 760) && (InputStates.nMouseXpos_Relative < 782)) {
            // Linke Spalte: Minusbuttons
            if ((InputStates.nMouseYpos_Relative >= 616) && (InputStates.nMouseYpos_Relative < 638)) {
                nButton = 1;    // Minus LEFT AXIS
            } else if ((InputStates.nMouseYpos_Relative >= 643) && (InputStates.nMouseYpos_Relative < 665)) {
                nButton = 3;    // Minus RIGHT AXIS
            } else if ((InputStates.nMouseYpos_Relative >= 670) && (InputStates.nMouseYpos_Relative < 692)) {
                nButton = 5;    // Minus UP AXIS
            } else if ((InputStates.nMouseYpos_Relative >= 697) && (InputStates.nMouseYpos_Relative < 719)) {
                nButton = 7;    // Minus DOWN AXIS
            }
        } else if ((InputStates.nMouseXpos_Relative >= 810) && (InputStates.nMouseXpos_Relative < 832)) {
            // Rechte Spalte: Plusbuttons
            if ((InputStates.nMouseYpos_Relative >= 616) && (InputStates.nMouseYpos_Relative < 638)) {
                nButton = 2;    // Plus LEFT AXIS
            } else if ((InputStates.nMouseYpos_Relative >= 643) && (InputStates.nMouseYpos_Relative < 665)) {
                nButton = 4;    // Plus RIGHT AXIS
            } else if ((InputStates.nMouseYpos_Relative >= 670) && (InputStates.nMouseYpos_Relative < 692)) {
                nButton = 6;    // Plus UP AXIS
            } else if ((InputStates.nMouseYpos_Relative >= 697) && (InputStates.nMouseYpos_Relative < 719)) {
                nButton = 8;    // Plus DOWN AXIS
            }
        }
    }
    return nButton;
}


/*----------------------------------------------------------------------------
Name:           GetSettingsMenuButton
------------------------------------------------------------------------------
Beschreibung: Prüft, welcher Bereich bzw. Button im Settings-Menü gedrückt ist.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = kein Button gedrückt
                    1 = GameController 0
                    2 = GameController 1
                    3 = GameController 2
                    4 = Joystick 0
                    5 = Joystick 1
                    6 = Joystick 2
                    7 = Keyboard
Seiteneffekte: InputStates.x
------------------------------------------------------------------------------*/
int GetSettingsMenuButton(void) {
    int nButton = 0;

    if (InputStates.bLeftMouseButton) {
        // Gamecontroller liegen bei X 32 bis 255
        if (((InputStates.nMouseXpos_Relative >= 32) && (InputStates.nMouseXpos_Relative < 256)) && ((InputStates.nMouseYpos_Relative >= 96) && (InputStates.nMouseYpos_Relative < 224))) {
            nButton = 1;    // Gamecontroller 0
        } else if (((InputStates.nMouseXpos_Relative >= 32) && (InputStates.nMouseXpos_Relative < 256)) && ((InputStates.nMouseYpos_Relative >= 256) && (InputStates.nMouseYpos_Relative < 384))) {
            nButton = 2;    // Gamecontroller 1
        } else if (((InputStates.nMouseXpos_Relative >= 32) && (InputStates.nMouseXpos_Relative < 256)) && ((InputStates.nMouseYpos_Relative >= 416) && (InputStates.nMouseYpos_Relative < 544))) {
            nButton = 3;    // Gamecontroller 2
        } else if (((InputStates.nMouseXpos_Relative >= 32) && (InputStates.nMouseXpos_Relative < 512)) && ((InputStates.nMouseYpos_Relative >= 576) && (InputStates.nMouseYpos_Relative < 736))) {
            nButton = 7;    // Keyboard
        // Joysticks liegen bei X 288 bis 512
        } else if (((InputStates.nMouseXpos_Relative >= 288) && (InputStates.nMouseXpos_Relative < 512)) && ((InputStates.nMouseYpos_Relative >= 96) && (InputStates.nMouseYpos_Relative < 224))) {
            nButton = 4;    // Joystick 0
        } else if (((InputStates.nMouseXpos_Relative >= 288) && (InputStates.nMouseXpos_Relative < 512)) && ((InputStates.nMouseYpos_Relative >= 256) && (InputStates.nMouseYpos_Relative < 384))) {
            nButton = 5;    // Joystick 1
        } else if (((InputStates.nMouseXpos_Relative >= 288) && (InputStates.nMouseXpos_Relative < 512)) && ((InputStates.nMouseYpos_Relative >= 416) && (InputStates.nMouseYpos_Relative < 544))) {
            nButton = 6;    // Joystick 2
        }
    }
    return nButton;
}


/*----------------------------------------------------------------------------
Name:           GetSettingsMenuKeyboardButton
------------------------------------------------------------------------------
Beschreibung: Prüft, welcher Bereich Button für die Tastenbelegung  im Settings-Menü gedrückt ist.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = kein Button gedrückt
                    1 = left
                    2 = right
                    3 = up
                    4 = down
                    5 = fire
                    6 = Andere Stelle im Menü
Seiteneffekte: InputStates.x
------------------------------------------------------------------------------*/
int GetSettingsMenuKeyboardButton(void) {
    int nButton = 0;

    if (InputStates.bLeftMouseButton) {
        nButton = 6;
        if ((InputStates.nMouseXpos_Relative >= 601) && (InputStates.nMouseXpos_Relative < 619)) {
            if ((InputStates.nMouseYpos_Relative >= 121) && (InputStates.nMouseYpos_Relative <= 138)) {
                nButton = 1;    // LEFT
            } else if ((InputStates.nMouseYpos_Relative >= 143) && (InputStates.nMouseYpos_Relative <= 160)) {
                nButton = 2;    // RIGHT
            } else if ((InputStates.nMouseYpos_Relative >= 166) && (InputStates.nMouseYpos_Relative <= 183)) {
                nButton = 3;    // UP
            } else if ((InputStates.nMouseYpos_Relative >= 189) && (InputStates.nMouseYpos_Relative <= 206)) {
                nButton = 4;    // DOWN
            } else if ((InputStates.nMouseYpos_Relative >= 212) && (InputStates.nMouseYpos_Relative <= 229)) {
                nButton = 5;    // FIRE
            }
        }
    }
    return nButton;
}


/*----------------------------------------------------------------------------
Name:           RenderSettingsbutton
------------------------------------------------------------------------------
Beschreibung: Zeigt im Hauptmenü des Spiels oben links den drehenden Settingsbutton.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer

Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: MainMenu.x, Video.x
------------------------------------------------------------------------------*/
int RenderSettingsbutton(SDL_Renderer *pRenderer) {
    int nErrorCode = 0;
    SDL_Rect DestR;

    DestR.x = Video.uXoffs + 1;
    DestR.y = 1;
    DestR.w = 30;
    DestR.h = 30;
    if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_SETTINGS),NULL,&DestR,MainMenu.fSettingsbuttonAngle,NULL, SDL_FLIP_NONE) != 0) {
        nErrorCode = -1;
        SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
    }
    MainMenu.fSettingsbuttonAngle = MainMenu.fSettingsbuttonAngle + 2;
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ShowAuthorAndLevelname
------------------------------------------------------------------------------
Beschreibung: Zeigt vor dem Spiel den Level-Author und den Levelnamen.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               uLevel, uint32_t, Levelnummer

Rückgabewert:  int, 0 = kein Fehler, 5 = Abbruch durch ESC, sonst Fehler
Seiteneffekte: Playfield.x, InputStates.x, ManKey.x, MainMenu.x
------------------------------------------------------------------------------*/
int ShowAuthorAndLevelname(SDL_Renderer *pRenderer, uint32_t uLevel) {
    int nErrorCode = 0;
    int nColorDimm;
    char szText[EMERALD_AUTHOR_LEN + 128];
    uint32_t I;
    uint8_t uMusicVolume;
    bool bMenuRun;
    bool bPrepareExit;
    bool bPlayGame;

    bPlayGame = true;
    bMenuRun = true;
    bPrepareExit = false;
    SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
    SetMenuBorderAndClear();
    nColorDimm = 0;
    uMusicVolume = 0;
    SetMusicVolume(uMusicVolume);
    SetAllTextureColors(nColorDimm);
    SetMenuText(MainMenu.pMenuScreen,SelectedLevelgroup.szLevelgroupname,-1,1,EMERALD_FONT_BLUE);
    for (I = 1; (I < (MainMenu.uXdim - 1)); I++) {
        MainMenu.pMenuScreen[2 * MainMenu.uXdim + I] = EMERALD_STEEL;
        MainMenu.pMenuScreen[12 * MainMenu.uXdim + I] = EMERALD_STEEL;
    }
    SetMenuText(MainMenu.pMenuScreen,Playfield.szLevelTitle,-1,5,EMERALD_FONT_BLUE);
    sprintf(szText,"LEVEL:%03d",uLevel);
    SetMenuText(MainMenu.pMenuScreen,szText,-1,7,EMERALD_FONT_BLUE);
    sprintf(szText,"BY:%s",Playfield.szLevelAuthor);
    SetMenuText(MainMenu.pMenuScreen,szText,-1,9,EMERALD_FONT_BLUE);

    MainMenu.pMenuScreen[16 * MainMenu.uXdim + 2] = EMERALD_EMERALD;
    SetMenuText(MainMenu.pMenuScreen,"PRESS FIRE (LEFT CTRL) TO PLAY",4,16,EMERALD_FONT_BLUE);
    MainMenu.pMenuScreen[19 * MainMenu.uXdim + 2] = EMERALD_RUBY;
    SetMenuText(MainMenu.pMenuScreen,"PRESS ESC TO CANCEL",4,19,EMERALD_FONT_BLUE);

    do {
        UpdateManKey();
        RenderMenuElements(pRenderer);
        if ((!bPrepareExit) && (nColorDimm < 100)) {
            nColorDimm = nColorDimm + 4;
            SetAllTextureColors(nColorDimm);
        }
        if (nColorDimm == 100) {
            if (ManKey.bFire) {
                bPrepareExit = true;
            } else if ((InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) || InputStates.bQuit || ManKey.bExit) {
                bPrepareExit = true;
                bPlayGame = false;
            }
        }
        if (bPrepareExit) {
            if (nColorDimm > 0) {
                nColorDimm = nColorDimm - 4;
                SetAllTextureColors(nColorDimm);
                uMusicVolume = uMusicVolume - 4;
                SetMusicVolume(uMusicVolume);
            } else {
                bMenuRun = false;
            }
        }
        RenderPresentAndClear(pRenderer);
        Playfield.uFrameCounter++;
    } while (bMenuRun);
    if ((nErrorCode == 0) && (!bPlayGame)) {
        nErrorCode = 5;
    }
    WaitNoSpecialKey(SDL_SCANCODE_ESCAPE);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           DimmMainMenu
------------------------------------------------------------------------------
Beschreibung: Dimmt das MainMenu auf oder ab. Gleichzeitig wird ein spielendes
              MOD-File ein- bzw. ausgeblendet
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               bDimmUp, bool, true = aufdimmen, false = abdimmen

Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: MainMenu.x
------------------------------------------------------------------------------*/
int DimmMainMenu(SDL_Renderer *pRenderer, bool bDimmUp) {
    int nColorDimm;
    int nErrorCode = 0;
    uint8_t uMusicVolume;

    if (bDimmUp) {
        nColorDimm = 0;
        uMusicVolume = 0;
        do {
            nColorDimm = nColorDimm + 2;
            uMusicVolume = uMusicVolume + 2;
            SetAllTextureColors(nColorDimm);
            SetMusicVolume(uMusicVolume);
            PlayMusic(false);
            nErrorCode = RenderMenuElements(pRenderer);
            if (nErrorCode == 0) {
                RenderPresentAndClear(pRenderer);
                SDL_Delay(5);
                Playfield.uFrameCounter++;
            }
         } while ((nErrorCode == 0) && (nColorDimm < 100));
    } else {
        nColorDimm = 100;
        uMusicVolume = 100;
        do {
            nColorDimm = nColorDimm - 2;
            uMusicVolume = uMusicVolume - 2;
            SetAllTextureColors(nColorDimm);
            SetMusicVolume(uMusicVolume);
            PlayMusic(false);
            nErrorCode = RenderMenuElements(pRenderer);
            if (nErrorCode == 0) {
                RenderPresentAndClear(pRenderer);
                SDL_Delay(5);
                Playfield.uFrameCounter++;
            }
        } while ((nErrorCode == 0) && (nColorDimm > 0));
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ShowHighScores
------------------------------------------------------------------------------
Beschreibung: Zeigt die Highscoreliste eines bestimmten Levels einer Levelgruppe an.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               uLevel, uint32_t, Levelnummer
               nNewHighScoreIndex, int, Markierung einer Zeile für neuen Highscore
                    Falls der Marker negativ oder ungültig ist, wird keine Markierung ausgeführt.
                    Die Nummerierung wird ab 0 durchgeführt, d.h. 0 = erster Platz.

Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: Playfield.x, InputStates.x, ManKey.x, MainMenu.x, HighscoreFile,
               SelectedLevelgroup.x, Video.x
------------------------------------------------------------------------------*/
int ShowHighScores(SDL_Renderer *pRenderer, uint32_t uLevel, int nNewHighScoreIndex) {
    int nErrorCode = 0;
    int nColorDimm;
    char szText[1024];
    char szNum[16];
    uint32_t I;
    uint8_t uMusicVolume;
    uint32_t uRainBowRGB;
    uint32_t uScore;
    bool bMenuRun;
    bool bPrepareExit;
    bool bPlayGame;
    bool bWellDone;         // Level wurde geschafft
    int nRed,nGreen,nBlue;
    uint8_t uRand;

    InitMainMenu(WINDOW_CENTER_DEFAULT);
    nRed = 0;
    nGreen = 0;
    nBlue = 0xFF;
    bPlayGame = true;
    bMenuRun = true;
    bPrepareExit = false;
    SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
    SetMenuBorderAndClear();
    nColorDimm = 0;
    uRand = randn(0,3);             // Zufallszahl 0 bis 3
    // 7. 2kad02.mod (2000AD cracktro02), 8. the brewery.mod, 9. class05_1999.mod oder 10. softworld.mod jeweils von Maktone
    if (SetMusic(7 + uRand) != 0) {
        return -1;
    }
    if (CreateButton(BUTTONLABEL_EXIT_HIGHSCORES,"Back to main menu",1100,742,true,false) != 0) {
        return -1;
    }
    uMusicVolume = 0;
    SetMusicVolume(uMusicVolume);
    SetAllTextureColors(nColorDimm);
    SetMenuText(MainMenu.pMenuScreen,"TOP TWENTY",-1,0,EMERALD_FONT_BLUE_STEEL);
    sprintf(szText,"%s/LEVEL:%03u",SelectedLevelgroup.szLevelgroupname,uLevel);
    SetMenuText(MainMenu.pMenuScreen,szText,-1,1,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.pMenuScreen,"   NAME                          SCORE",1,2,EMERALD_FONT_BLUE_STEEL);
    for (I = 0; I < EMERALD_HIGHSCORE_LISTLEN; I++) {
        sprintf(szNum,"%02d",I + 1);    // Positionsnummer
        SetMenuText(MainMenu.pMenuScreen,szNum,1,I + 3,EMERALD_FONT_BLUE);  // Positionsnummer
        MainMenu.pMenuScreen[(I + 3) * MainMenu.uXdim + 3] = EMERALD_STEEL; // Senkrechte Abtrennung Pos.Nr zu Name
        MainMenu.pMenuScreen[(I + 3) * MainMenu.uXdim + 33] = EMERALD_STEEL;// Senkrechte Abtrennung Name zu Score
        // Namen und Score
        if (strlen(HighscoreFile.TopTwenty[uLevel].szTopTwenty[I]) > 0) {
            SetMenuText(MainMenu.pMenuScreen,HighscoreFile.TopTwenty[uLevel].szTopTwenty[I],4,I + 3,EMERALD_FONT_BLUE);
        } else {
            SetMenuText(MainMenu.pMenuScreen,"-",4,I + 3,EMERALD_FONT_BLUE);
        }

        uScore = HighscoreFile.TopTwenty[uLevel].uHighScore[I] & 0x7FFFFFFF;    // WellDone-Flag ausmaskieren
        bWellDone = ((HighscoreFile.TopTwenty[uLevel].uHighScore[I] & 0x80000000) != 0);
        sprintf(szNum,"%04u",uScore);
        SetMenuText(MainMenu.pMenuScreen,szNum,35,I + 3,EMERALD_FONT_BLUE);
        if (bWellDone) {
            MainMenu.pMenuScreen[(I + 3) * MainMenu.uXdim + 39] = EMERALD_STEEL_PLAYERHEAD;
        }
    }
    // Markierung eines neuen Highscores durchführen
    if ((nNewHighScoreIndex >= 0) && (nNewHighScoreIndex < EMERALD_HIGHSCORE_LISTLEN)) {
        MainMenu.pMenuScreen[(nNewHighScoreIndex + 3) * MainMenu.uXdim + 3] = EMERALD_WHEEL; // Senkrechte Abtrennung Pos.Nr zu Name
        MainMenu.pMenuScreen[(nNewHighScoreIndex + 3) * MainMenu.uXdim + 33] = EMERALD_WHEEL; // Senkrechte Abtrennung Name zu HighScore
    }
    do {
        for (I = 0; I < EMERALD_HIGHSCORE_LISTLEN; I++) {
            uRainBowRGB = GetRainbowColors(I * 5 + Playfield.uFrameCounter);
            nRed = (((uRainBowRGB >> 16) & 0xFF) * nColorDimm) / 100;
            nGreen = (((uRainBowRGB >> 8) & 0xFF) * nColorDimm) / 100;
            nBlue = ((uRainBowRGB & 0xFF) * nColorDimm) / 100;
            SDL_SetRenderDrawColor(pRenderer,nRed,nGreen,nBlue,SDL_ALPHA_OPAQUE);  // Farbe für Line setzen
            SDL_RenderDrawLine(pRenderer,Video.uXoffs + 0, Video.uYoffs + ((I + 3) * FONT_H), Video.uXoffs + DEFAULT_WINDOW_W - 1, Video.uYoffs + ((I + 3) * FONT_H));
            SDL_SetRenderDrawColor(pRenderer,0,0,0, SDL_ALPHA_OPAQUE);  // Muss am Ende stehen, damit Hintergrund wieder dunkel wird
        }
        UpdateManKey();
        PlayMusic(false);
        RenderMenuElements(pRenderer);
        if ((!bPrepareExit) && (nColorDimm < 100)) {
            nColorDimm = nColorDimm + 2;
            SetAllTextureColors(nColorDimm);
            uMusicVolume = uMusicVolume + 2;
            SetMusicVolume(uMusicVolume);
        }
        if (nColorDimm == 100) {
            if ((ManKey.bFire) || (IsButtonPressed(BUTTONLABEL_EXIT_HIGHSCORES))) {
                bPrepareExit = true;
            } else if ((InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) || InputStates.bQuit) {
                bPrepareExit = true;
                bPlayGame = false;
            }
        }
        if (bPrepareExit) {
            if (nColorDimm > 0) {
                nColorDimm = nColorDimm - 2;
                SetAllTextureColors(nColorDimm);
                uMusicVolume = uMusicVolume - 2;
                SetMusicVolume(uMusicVolume);
            } else {
                bMenuRun = false;
            }
        }
        ShowButtons(pRenderer,K_RELATIVE);
        RenderPresentAndClear(pRenderer);
        Playfield.uFrameCounter++;
    } while (bMenuRun);
    if ((nErrorCode == 0) && (!bPlayGame)) {
        nErrorCode = 5;
    }
    FreeButton(BUTTONLABEL_EXIT_HIGHSCORES);
    WaitNoKey();
    InitMainMenu(WINDOW_FULL_SIZE);
    InitLists();
    return nErrorCode;
}
