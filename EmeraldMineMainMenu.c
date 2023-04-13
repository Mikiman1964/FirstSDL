#include <ctype.h>
#include "buttons.h"
#include "editor.h"
#include "EmeraldMineMainMenu.h"
#include "GetTextureIndexByElement.h"
#include "highscores.h"
#include "KeyboardMouse.h"
#include "loadlevel.h"
#include "man.h"
#include "modplay.h"
#include "mySDL.h"
#include "mystd.h"


extern INPUTSTATES InputStates;
extern MANKEY ManKey;
extern PLAYFIELD Playfield;
extern uint32_t g_LevelgroupFilesCount;
extern NAMES Names;
extern CONFIG Config;
extern LEVELGROUPFILE LevelgroupFiles[EMERALD_MAX_LEVELGROUPFILES];
extern LEVELGROUP SelectedLevelgroup;
extern ACTUALPLAYER Actualplayer;
extern HIGHSCOREFILE HighscoreFile;
extern AUDIOPLAYER Audioplayer;
MAINMENU MainMenu;


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
              Bei ung�ltigen Eingabewerten wird der Regenbogen umgebrochen, d.h.
              er f�ngt wieder am Anfang an.
              Quelle:
              https://www.mikrocontroller.net/topic/238304
              Karl-Heinz BuchEgg
Parameter
      Eingang: uWert, uint16_t, Position im Regenbogen (0 ... 254)
      Ausgang: -
      R�ckgabewert: uint32_t, RGB (h�chtes Byte unbesetzt = NN)
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
Name:           GetLevelButton
------------------------------------------------------------------------------
Beschreibung: Pr�ft, ob ein Levelbutton (down/up) gedr�ckt ist.
Parameter
      Eingang: -
      Ausgang: -
R�ckgabewert:  int, 0 = kein Button gedr�ckt, 1 = EMERALD_STEEL_ARROW_DOWN_PRESSED, 2 = EMERALD_STEEL_ARROW_UP_PRESSED
Seiteneffekte: InputStates.x, MainMenu.x
------------------------------------------------------------------------------*/
int GetLevelButton(void) {
    int nButton;

    nButton = 0;
    if (InputStates.bLeftMouseButton) {
        // Beide Buttons auf selber Y-Position
        if ((InputStates.nMouseYpos >= (192 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (224 + MainMenu.uYoffs))) {
            if ((InputStates.nMouseXpos >= (352 + MainMenu.uXoffs)) && (InputStates.nMouseXpos < (384 + MainMenu.uXoffs))) {
                nButton = EMERALD_STEEL_ARROW_DOWN_PRESSED;
            } else if ((InputStates.nMouseXpos >= (384 + MainMenu.uXoffs)) && (InputStates.nMouseXpos < (416 + MainMenu.uXoffs))) {
                nButton = EMERALD_STEEL_ARROW_UP_PRESSED;
            }
        }
    }
    return nButton;
}


/*----------------------------------------------------------------------------
Name:           GetPlayerListButton
------------------------------------------------------------------------------
Beschreibung: Pr�ft, ob ein Spielerlisten-Button (down/up) gedr�ckt ist.
Parameter
      Eingang: -
      Ausgang: -
R�ckgabewert:  int, 0 = kein Button gedr�ckt, 1 = EMERALD_STEEL_ARROW_DOWN_PRESSED, 2 = EMERALD_STEEL_ARROW_UP_PRESSED
Seiteneffekte: InputStates.x, MainMenu.x
------------------------------------------------------------------------------*/
int GetPlayerListButton(void) {
    int nButton;

    nButton = 0;
    if ((InputStates.bLeftMouseButton) && (Names.uNameCount > MAX_NAMES_IN_LIST)) {
        if  ((InputStates.nMouseXpos >= (1088 + MainMenu.uXoffs)) && (InputStates.nMouseXpos < (1088 + FONT_W + MainMenu.uXoffs))) {
            // Button Levelgruppen Pfeil hoch
            if ( (InputStates.nMouseYpos >= (448 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (448 + FONT_H + MainMenu.uYoffs))) {
                nButton = EMERALD_STEEL_ARROW_UP_PRESSED;
            } else if ( (InputStates.nMouseYpos >= (544 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (544 + FONT_H + MainMenu.uYoffs))) {
            // Button Levelgruppen Pfeil runter
                nButton = EMERALD_STEEL_ARROW_DOWN_PRESSED;
            }
        }
    }
    return nButton;
}


/*----------------------------------------------------------------------------
Name:           GetLevelgroupListButton
------------------------------------------------------------------------------
Beschreibung: Pr�ft, ob ein Levelgruppen-Button (down/up) gedr�ckt ist.
Parameter
      Eingang: -
      Ausgang: -
R�ckgabewert:  int, 0 = kein Button gedr�ckt, 1 = EMERALD_STEEL_ARROW_DOWN_PRESSED, 2 = EMERALD_STEEL_ARROW_UP_PRESSED
Seiteneffekte: InputStates.x, MainMenu.x, g_LevelgroupFilesCount
------------------------------------------------------------------------------*/
int GetLevelgroupListButton(void) {
    int nButton;

    nButton = 0;
    if (g_LevelgroupFilesCount > MAX_LEVELGROUPS_IN_LIST) {
        if (InputStates.bLeftMouseButton) {
            if  ((InputStates.nMouseXpos >= (896 + MainMenu.uXoffs)) && (InputStates.nMouseXpos < (896 + FONT_W + MainMenu.uXoffs))) {
                if ( (InputStates.nMouseYpos >= (608 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (608 + FONT_H + MainMenu.uYoffs))) { // Button Levelgruppen Pfeil hoch?
                    nButton = EMERALD_STEEL_ARROW_UP_PRESSED;
                } else if ( (InputStates.nMouseYpos >= (704 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (704 + FONT_H + MainMenu.uYoffs))) { // Button Levelgruppen Pfeil runter?
                    nButton = EMERALD_STEEL_ARROW_DOWN_PRESSED;
                }
            }
        }
    }
    return nButton;
}


/*----------------------------------------------------------------------------
Name:           SetMenuText
------------------------------------------------------------------------------
Beschreibung: Setzt einen Text in einen Men�screen. Falls das Ende einer Zeile
              erreicht wird, wird der Text abgeschnitten.
Parameter
      Eingang: pMenuScreen, uint16_t *, Zeiger auf Men�screen
               pszText, char*, Zeiger auf Text
               nXpos, int, X-Men�koordinate (keine Pixelkoordinate)
                    wenn < 0, dann wird horizontal zentriert
               nYpos, int, Y-Men�koordinate (keine Pixelkoordinate)
                    wenn < 0, dann wird vertikal zentriert
               uFont, uint32_t, m�glich ist Folgendes:
                    EMERALD_FONT_BLUE
                    EMERALD_FONT_STEEL_BLUE
                    EMERALD_FONT_GREEN
                    EMERALD_FONT_STEEL_GREEN
      Ausgang: -
R�ckgabewert:  int, 0 = Alles OK, sonst Fehler
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
        // bei "ung�ltiger" X-Position eine horizontale Zentrierung berechnen
        if ((nXpos < 0) || (nXpos >= (uXdim - 1))) {
            nXpos = (uXdim / 2) - (uStringLen / 2);
            if (nXpos< 0) {
                nXpos = 0;
            }
        }
       // bei "ung�ltiger" Y-Position eine vertikale Zentrierung berechnen
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
Parameter
      Eingang: sign, char, Zeichen
               uFont, uint32_t, m�glich ist Folgendes:
                    EMERALD_FONT_BLUE
                    EMERALD_FONT_STEEL_BLUE
                    EMERALD_FONT_GREEN
                    EMERALD_FONT_STEEL_GREEN
      Ausgang: -
R�ckgabewert:  uint32_t, Element, falls keine Konvertierung m�glich, wird EMERALD_SPACE zur�ckgegeben
Seiteneffekte: -
------------------------------------------------------------------------------*/
uint16_t GetFontElementByChar(char sign,uint32_t uFont) {
    uint16_t uElement;

    uElement = EMERALD_SPACE;
    sign = toupper(sign);
    if ((sign >= 'A') && (sign <= 'Z')) {
        switch (uFont) {
            case (EMERALD_FONT_BLUE):
                uElement = EMERALD_FONT_A + sign - 'A';
                break;
            case (EMERALD_FONT_STEEL_BLUE):
                uElement = EMERALD_FONT_STEEL_A + sign - 'A';
                break;
            case (EMERALD_FONT_GREEN):
                uElement = EMERALD_FONT_GREEN_A + sign - 'A';
                break;
            case (EMERALD_FONT_STEEL_GREEN):
                uElement = EMERALD_FONT_STEEL_GREEN_A + sign - 'A';
                break;
            default:
                uElement = EMERALD_FONT_A + sign - 'A';
                break;
        }
    } else if ((sign >= '0') && (sign <= '9')) {
        switch (uFont) {
            case (EMERALD_FONT_BLUE):
                uElement = EMERALD_FONT_0 + sign - 0x30;
                break;
            case (EMERALD_FONT_STEEL_BLUE):
                uElement = EMERALD_FONT_STEEL_0 + sign - 0x30;
                break;
            case (EMERALD_FONT_GREEN):
                uElement = EMERALD_FONT_GREEN_0 + sign - 0x30;
                break;
            case (EMERALD_FONT_STEEL_GREEN):
                uElement = EMERALD_FONT_STEEL_GREEN_0 + sign - 0x30;
                break;
            default:
                uElement = EMERALD_FONT_0 + sign - 0x30;
                break;
        }
    } else {
        switch (sign) {
            case (' '):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_SPACE;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_STEEL;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_SPACE;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
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
                        uElement = EMERALD_FONT_EXCLAMATION;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_FONT_STEEL_EXCLAMATION;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_EXCLAMATION;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
                        uElement = EMERALD_FONT_STEEL_GREEN_EXCLAMATION;
                        break;
                    default:
                        uElement = EMERALD_FONT_EXCLAMATION;
                        break;
                }
                break;
            case ('>'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_ARROW_RIGHT;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_FONT_STEEL_ARROW_RIGHT;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_ARROW_RIGHT;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
                        break;
                        uElement = EMERALD_FONT_STEEL_GREEN_ARROW_RIGHT;
                    default:
                        uElement = EMERALD_FONT_ARROW_RIGHT;
                        break;
                }
                break;
            case ('<'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_ARROW_LEFT;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_FONT_STEEL_ARROW_LEFT;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_ARROW_LEFT;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
                        break;
                        uElement = EMERALD_FONT_STEEL_GREEN_ARROW_LEFT;
                    default:
                        uElement = EMERALD_FONT_ARROW_LEFT;
                        break;
                }
                break;
            case ('\''):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_APOSTROPHE;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_FONT_STEEL_APOSTROPHE;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_APOSTROPHE;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
                        uElement = EMERALD_FONT_STEEL_GREEN_APOSTROPHE;
                        break;
                    default:
                        uElement = EMERALD_FONT_APOSTROPHE;
                        break;
                }
                break;
            case ('('):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BRACE_OPEN;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_FONT_STEEL_BRACE_OPEN;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_BRACE_OPEN;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
                        uElement = EMERALD_FONT_STEEL_GREEN_BRACE_OPEN;
                        break;
                    default:
                        uElement = EMERALD_FONT_BRACE_OPEN;
                        break;
                }
                break;
            case (')'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_BRACE_CLOSE;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_FONT_STEEL_BRACE_CLOSE;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_BRACE_CLOSE;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
                        uElement = EMERALD_FONT_STEEL_GREEN_BRACE_CLOSE;
                        break;
                    default:
                        uElement = EMERALD_FONT_BRACE_CLOSE;
                        break;
                }
                break;
            case ('+'):
            case ('&'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_PLUS;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_FONT_STEEL_PLUS;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_PLUS;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
                        uElement = EMERALD_FONT_STEEL_GREEN_PLUS;
                        break;
                    default:
                        uElement = EMERALD_FONT_PLUS;
                        break;
                }
                break;
            case (','):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_COMMA;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_FONT_STEEL_COMMA;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_COMMA;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
                        uElement = EMERALD_FONT_STEEL_GREEN_COMMA;
                        break;
                    default:
                        uElement = EMERALD_FONT_COMMA;
                        break;
                }
                break;
            case ('-'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_MINUS;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_FONT_STEEL_MINUS;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_MINUS;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
                        uElement = EMERALD_FONT_STEEL_GREEN_MINUS;
                        break;
                    default:
                        uElement = EMERALD_FONT_MINUS;
                        break;
                }
                break;
            case ('.'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_POINT;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_FONT_STEEL_POINT;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_POINT;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
                        uElement = EMERALD_FONT_STEEL_GREEN_POINT;
                        break;
                    default:
                        uElement = EMERALD_FONT_POINT;
                        break;
                }
                break;
            case ('/'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_SLASH;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_FONT_STEEL_SLASH;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_SLASH;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
                        uElement = EMERALD_FONT_STEEL_GREEN_SLASH;
                        break;
                    default:
                        uElement = EMERALD_FONT_SLASH;
                        break;
                }
                break;
            case (':'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_DOUBLE_POINT;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_FONT_STEEL_DOUBLE_POINT;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_DOUBLE_POINT;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
                        uElement = EMERALD_FONT_STEEL_GREEN_DOUBLE_POINT;
                        break;
                    default:
                        uElement = EMERALD_FONT_DOUBLE_POINT;
                        break;
                }
                break;
            case ('*'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_PLATE;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_FONT_STEEL_PLATE;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_PLATE;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
                        uElement = EMERALD_FONT_STEEL_GREEN_PLATE;
                        break;
                    default:
                        uElement = EMERALD_FONT_PLATE;
                        break;
                }
                break;
            case ('?'):
                switch (uFont) {
                    case (EMERALD_FONT_BLUE):
                        uElement = EMERALD_FONT_QUESTION_MARK;
                        break;
                    case (EMERALD_FONT_STEEL_BLUE):
                        uElement = EMERALD_FONT_STEEL_QUESTION_MARK;
                        break;
                    case (EMERALD_FONT_GREEN):
                        uElement = EMERALD_FONT_GREEN_QUESTION_MARK;
                        break;
                    case (EMERALD_FONT_STEEL_GREEN):
                        uElement = EMERALD_FONT_STEEL_GREEN_QUESTION_MARK;
                        break;
                    default:
                        uElement = EMERALD_FONT_QUESTION_MARK;
                        break;
                }
                break;
        }
    }
    return uElement;
}


/*----------------------------------------------------------------------------
Name:           EvaluateGame
------------------------------------------------------------------------------
Beschreibung: Bewertet das zur�ckliegende Spiel und speichert neue Werte
              (Totalscore, Handicap usw.) f�r den aktuellen Spieler.
Parameter
      Eingang: -
      Ausgang: pnNewHighscoreIndex, int *, HighscoreIndex (>= 0), -1 = kein neuer Index
               puLevelPlayed, uint32_t *, Zeiger auf Levelnummer, in der ein Highscore erreicht wurde
R�ckgabewert:  int, 0 = kein Fehler, sonst Fehler
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
                    nErrorCode = InsertScore(Actualplayer.szPlayername,uLevelPlayed,Playfield.uTotalScore);
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
Beschreibung: Erledigt die Auswahl einer Levelgruppe aus dem Hauptmen� heraus und
              blendet die Balken f�r die Auswahl einer Levelgruppe ein, wenn sich
              der Mauspfeil �ber den Namen einer Gruppe befindet.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
R�ckgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: MainMenu.x, Actualplayer.x, SelectedLevelgroup.x, InputStates.x
------------------------------------------------------------------------------*/
int MenuSelectLevelgroup(SDL_Renderer *pRenderer) {
    int nErrorCode = 0;
    uint32_t uBeamPosition;
    char szPlayername[EMERALD_PLAYERNAME_LEN + 1];

    uBeamPosition = GetLevelgroupBeamPosition();
    if (uBeamPosition != 0xFFFFFFFF) {
        nErrorCode = DrawBeam(pRenderer,MainMenu.uXoffs + FONT_W, MainMenu.uYoffs + 608 + FONT_H * uBeamPosition, DEFAULT_WINDOW_W - 2 * FONT_W, FONT_H, 0x20,0x20,0xF0,0xC0);
        if ((InputStates.bLeftMouseButton) && (nErrorCode == 0)) {
            if (SelectAlternativeLevelgroup(LevelgroupFiles[uBeamPosition].uMd5Hash,true) == 0) {
                SDL_Log("Select %s, OK",SelectedLevelgroup.szLevelgroupname);
                if (Actualplayer.bValid) {  // Der aktuelle Name wird nochmals ausgew�hlt, damit dieser ggf. den Levelgruppen-Hash bekommt
                    strcpy(szPlayername,Actualplayer.szPlayername); // Muss kopiert werden, da Selectname die Struktur Actualplayer.x l�scht
                    SDL_Log("%s: selecting name: %s",__FUNCTION__,szPlayername);
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
Beschreibung: Erledigt die Auswahl eines Namens aus dem Hauptmen� heraus und
              blendet die Balken f�r die Auswahl eines Namens an, wenn sich
              der Mauspfeil �ber einem Namen befindet.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
R�ckgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: MainMenu.x, Names.x, SelectedLevelgroup.x, InputStates.x
------------------------------------------------------------------------------*/
int MenuSelectName(SDL_Renderer *pRenderer) {
    int nErrorCode = 0;
    uint32_t uBeamPosition;

    uBeamPosition = GetNamesBeamPosition();
    if (uBeamPosition != 0xFFFFFFFF) {
        nErrorCode = DrawBeam(pRenderer,MainMenu.uXoffs + FONT_W, MainMenu.uYoffs + 448 + FONT_H * uBeamPosition, DEFAULT_WINDOW_W - 7 * FONT_W, FONT_H, 0x20,0x20,0xF0,0xC0);
        if ((InputStates.bLeftMouseButton) && (nErrorCode == 0)) {
            if (SelectName(Names.Name[MainMenu.uNamesList[uBeamPosition]].szName,SelectedLevelgroup.uMd5Hash) == 0) {
                SDL_Log("select name: %s, OK",Names.Name[MainMenu.uNamesList[uBeamPosition]].szName);
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
Beschreibung: Initialisiert die Anzeigelisten f�r die Levelgruppen und Namen.
              Vor Aufruf dieser Funktion m�ssen die Funktionen
               GetLevelgroupFiles()   und
               ReadNamesFile()
              erfolgreich aufgerufen worden sein.
              Die Level-Titel-Liste wird ebenfalls initialisiert, wenn
              eine Levelgruppe ausgew�hlt ist.

Parameter
      Eingang: -
      Ausgang: -
R�ckgabewert:  -
Seiteneffekte: MainMenu.x, g_LevelgroupFilesCount, SelectedLevelgroup.x
------------------------------------------------------------------------------*/
void InitLists(void) {
    uint32_t I;

    // Levelgruppenliste vorbereiten
    memset(MainMenu.uLevelgroupList,0xFF,sizeof(MainMenu.uLevelgroupList));
    for (I = 0; (I < MAX_LEVELGROUPS_IN_LIST) && (I < g_LevelgroupFilesCount); I++) {
        MainMenu.uLevelgroupList[I] = I;
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
Beschreibung: Aktiviert die Namenseingabe zum Hinzuf�gen eines neuen Spielernamens.
Parameter
      Eingang: -
      Ausgang: -
R�ckgabewert:  -
Seiteneffekte: MainMenu.x
------------------------------------------------------------------------------*/
void ActivateInputPlayernameMode(void) {
    MainMenu.nState = 1; // Status auf Namen-Eingabemodus stellen
    SetMenuText(MainMenu.uMenuScreen,"                               ",7,5,EMERALD_FONT_GREEN);
    SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,false);
    MainMenu.uMenuScreen[13 * MainMenu.uXdim + 75] = EMERALD_SPACE;
    MainMenu.uMenuScreen[13 * MainMenu.uXdim + 76] = EMERALD_SPACE;
    MainMenu.uMenuScreen[13 * MainMenu.uXdim + 77] = EMERALD_SPACE;
    MainMenu.uMenuScreen[13 * MainMenu.uXdim + 78] = EMERALD_SPACE;
}


/*----------------------------------------------------------------------------
Name:           InitMainMenu
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Struktur MainMenu.x
Parameter
      Eingang: -
      Ausgang: -
R�ckgabewert:  -
Seiteneffekte: MainMenu.x, Config.x
------------------------------------------------------------------------------*/
void InitMainMenu(void) {
    MainMenu.uXdim = DEFAULT_WINDOW_W / FONT_W;
    MainMenu.uYdim = DEFAULT_WINDOW_H / FONT_H;
    MainMenu.nState = 0;             // 1 = Eingabe eines neuen Namens
    MainMenu.uXoffs = (Config.uResX - DEFAULT_WINDOW_W) / 2;
    MainMenu.uYoffs = (Config.uResY - DEFAULT_WINDOW_H) / 2;
    memset(MainMenu.szTempName,0,sizeof(MainMenu.szTempName));
    MainMenu.uFlashIndex = 0;
    MainMenu.uCursorPos = 0;
    MainMenu.uMaxFlashIndex = sizeof(MainMenu.uFlashBrightness) - 1;
    memcpy(MainMenu.uFlashBrightness,"\x00\10\x14\x19\x1E\x28\x32\x4B\x5A\x5F\x64\x5F\x5A\x4B\x32\x28\x1E\x19\x14\x0A",sizeof(MainMenu.uFlashBrightness));// F�r den Stein-Cursor
}


/*----------------------------------------------------------------------------
Name:           ScrollLevelGroups
------------------------------------------------------------------------------
Beschreibung: Scrollt die Levelgruppen im Hauptmen�.
Parameter
      Eingang: nButton, int, Levelgruppenlisten-Button, der gedr�ckt wurde
                        0 = kein Button gedr�ckt, 1 = EMERALD_STEEL_ARROW_DOWN_PRESSED, 2 = EMERALD_STEEL_ARROW_UP_PRESSED
      Ausgang: -
R�ckgabewert:  -
Seiteneffekte: MainMenu.x, g_LevelgroupFilesCount
------------------------------------------------------------------------------*/
void ScrollLevelGroups(int nButton) {
    uint32_t I;

    if (g_LevelgroupFilesCount > 4) {
        if (nButton == EMERALD_STEEL_ARROW_UP_PRESSED) {            // Button Levelgruppen Pfeil hoch?
            if (MainMenu.uLevelgroupList[0] > 0) {
                for (I = 0; I < 4; I++) {
                    if (MainMenu.uLevelgroupList[I] != 0xFF) {
                        MainMenu.uLevelgroupList[I]--;
                    }
                }
            }

        } else if (nButton == EMERALD_STEEL_ARROW_DOWN_PRESSED) {   // Button Levelgruppen Pfeil runter?
            if (MainMenu.uLevelgroupList[3] < (g_LevelgroupFilesCount - 1)) {
                for (I = 0; I < 4; I++) {
                    if (MainMenu.uLevelgroupList[I] != 0xFF) {
                        MainMenu.uLevelgroupList[I]++;
                    }
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ScrollPlayernames
------------------------------------------------------------------------------
Beschreibung: Scrollt die Playernamen im Hauptmen�.
Parameter
      Eingang: nButton, int, Playerlisten-Button, der gedr�ckt wurde
                        0 = kein Button gedr�ckt, 1 = EMERALD_STEEL_ARROW_DOWN_PRESSED, 2 = EMERALD_STEEL_ARROW_UP_PRESSED
      Ausgang: -
R�ckgabewert:  -
Seiteneffekte: MainMenu.x, Names.x
------------------------------------------------------------------------------*/
void ScrollPlayernames(int nButton) {
    uint32_t I;

    if (Names.uNameCount > 4) {
        if (nButton == EMERALD_STEEL_ARROW_UP_PRESSED) {  // Button Levelgruppen Pfeil hoch?
            if (MainMenu.uNamesList[0] > 0) {
                for (I = 0; I < 4; I++) {
                    if (MainMenu.uNamesList[I] != 0xFF) {
                        MainMenu.uNamesList[I]--;
                    }
                }
            }
        } else if (nButton == EMERALD_STEEL_ARROW_DOWN_PRESSED) {   // Button Levelgruppen Pfeil runter?
            if (MainMenu.uNamesList[3] < (Names.uNameCount - 1)) {
                for (I = 0; I < 4; I++) {
                    if (MainMenu.uNamesList[I] != 0xFF) {
                        MainMenu.uNamesList[I]++;
                    }
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           DrawBeam
------------------------------------------------------------------------------
Beschreibung: Zeichnet ein Rechteck / einen Balken f�r die Levelgruppen- und Namen-Auswahl.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               uXpos, uint32_t, X-Position f�r Balken
               uYpos, uint32_t, X-Position f�r Balken
               uWidth, uint32_t, Breite des Balkens
               uHeight, uint32_t, H�he des Balkens
               uRed, uint8_t, Rot-Anteil f�r Farbe des Balkens
               uGreen, uint8_t, Gr�n-Anteil f�r Farbe des Balkens
               uBlue, uint8_t, Blau-Anteil f�r Farbe des Balkens
               uTransp, uint8_t, Transparenz der Farbe
      Ausgang: -
R�ckgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int DrawBeam(SDL_Renderer *pRenderer,uint32_t uXpos, uint32_t uYpos, uint32_t uWidth, uint32_t uHeight, uint8_t uRed, uint32_t uGreen, uint32_t uBlue, uint8_t uTransp) {
    int nErrorCode = -1;
    SDL_Rect DestR;

    // Balken zeichnen
    DestR.x = uXpos;
    DestR.y = uYpos;
    DestR.w = uWidth;
    DestR.h = uHeight;
    if (SDL_SetRenderDrawColor(pRenderer,uRed,uGreen,uBlue,uTransp) == 0) {   // dunkelblaue, halbtransparente Fensterfl�che
        if (SDL_RenderFillRect(pRenderer,&DestR) == 0) {
            nErrorCode = SDL_SetRenderDrawColor(pRenderer,0,0,0,SDL_ALPHA_OPAQUE);
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           RenderMenuElements
------------------------------------------------------------------------------
Beschreibung: Alle Men�-Elemente im Men�-Screen uMenuScreen werden in den Renderer geschrieben.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
R�ckgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Playfield.x, MainMenu.x
------------------------------------------------------------------------------*/
int RenderMenuElements(SDL_Renderer *pRenderer) {
    uint32_t I;
    int nErrorCode = 0;
    uint32_t uTextureIndex;
    uint32_t X,Y;
    float fAngle;
    SDL_Rect DestR;

    for (I = 0; ((I < ((DEFAULT_WINDOW_W / FONT_W) * (DEFAULT_WINDOW_H / FONT_H))) && (nErrorCode == 0)); I++) {
        X = I % MainMenu.uXdim;
        Y = I / MainMenu.uXdim;
        uTextureIndex = GetTextureIndexByElement(MainMenu.uMenuScreen[I],Playfield.uFrameCounter % 16,&fAngle);
        DestR.x = MainMenu.uXoffs + X * FONT_W;
        DestR.y = MainMenu.uYoffs + Y * FONT_H;
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
Beschreibung: Ermittelt, ob und wo ein Balken eingeblendet werden muss, wenn der Mauspfeil sich �ber
              der Liste der Spieler-Namen befindet.
Parameter
      Eingang: -
      Ausgang: -
R�ckgabewert:  uint32_t, 0xFFFFFFFF = keine Einblendung, sonst 0 bis 3 f�r einen der 4 Spieler Namen
Seiteneffekte: InputStates.x, MainMenu.x
------------------------------------------------------------------------------*/
uint32_t GetNamesBeamPosition(void) {
    uint32_t uBeamPosition = 0xFFFFFFFF;
    uint32_t bMouseFound = false;
    uint32_t I;

    // Pr�fen, ob Mauspfeil �ber Spielernamen-Liste steht und ggf. Balken einblenden
    if ((InputStates.nMouseXpos >= (FONT_W + MainMenu.uXoffs)) && (InputStates.nMouseXpos < (1088 + MainMenu.uXoffs))) {
        for (I = 0; (I < MAX_NAMES_IN_LIST) && (!bMouseFound); I++) {
            if ((InputStates.nMouseYpos >= (448 + (I * FONT_H) + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (480 + (I * FONT_H) + MainMenu.uYoffs))) {
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
Beschreibung: Ermittelt, ob und wo ein Balken eingeblendet werden muss, wenn der Mauspfeil sich �ber
              der Liste der Levelgruppen-Namen befindet.
Parameter
      Eingang: -
      Ausgang: -
R�ckgabewert:  uint32_t, 0xFFFFFFFF = keine Einblendung, sonst 0 bis 3 f�r einen der 4 Levelgruppen-Namen
Seiteneffekte: InputStates.x, MainMenu.x
------------------------------------------------------------------------------*/
uint32_t GetLevelgroupBeamPosition(void) {
    uint32_t uBeamPosition = 0xFFFFFFFF;
    uint32_t bMouseFound = false;
    uint32_t I;

    // Pr�fen, ob Mauspfeil �ber Levelgruppenliste steht und ggf. Balken einblenden
    if ((InputStates.nMouseXpos >= (FONT_W + MainMenu.uXoffs)) && (InputStates.nMouseXpos < (896 + MainMenu.uXoffs))) {
        for (I = 0; (I < MAX_LEVELGROUPS_IN_LIST) && (!bMouseFound); I++) {
            if ((InputStates.nMouseYpos >= (608 + (I * FONT_H) + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (640 + (I * FONT_H) + MainMenu.uYoffs))) {
                if (MainMenu.uLevelgroupList[I] != 0xFF) {
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
Beschreibung: L�scht das Men� und zeichnet die Umrandung.

Parameter
      Eingang: -
      Ausgang: -

R�ckgabewert:  -
Seiteneffekte: MainMenu.x
------------------------------------------------------------------------------*/
void SetMenuBorderAndClear(void) {
    uint32_t I;

    for (I = 0; I < ((DEFAULT_WINDOW_W / FONT_W) * (DEFAULT_WINDOW_H / FONT_H));I++) {
        MainMenu.uMenuScreen[I] = EMERALD_SPACE;
    }
    // Border zeichnen
    for (I = 0; I < MainMenu.uXdim; I++) {
        MainMenu.uMenuScreen[I] = EMERALD_STEEL; // Stahl oben
        MainMenu.uMenuScreen[I + MainMenu.uXdim * (MainMenu.uYdim - 1)] = EMERALD_STEEL; // Stahl unten
    }
    for (I = 0; I < MainMenu.uYdim; I++) {
        MainMenu.uMenuScreen[I * MainMenu.uXdim] = EMERALD_STEEL; // Stahl links
        MainMenu.uMenuScreen[I * MainMenu.uXdim + MainMenu.uXdim - 1] = EMERALD_STEEL; // Stahl rechts
    }
}

/*----------------------------------------------------------------------------
Name:           SetStaticMenuElements
------------------------------------------------------------------------------
Beschreibung: Setzt statische Elemente f�r das Emerald-Mine-Hauptmen�.

Parameter
      Eingang: -
      Ausgang: -

R�ckgabewert:  -
Seiteneffekte: SelectedLevelgroup.x, MainMenu.x
------------------------------------------------------------------------------*/
void SetStaticMenuElements(void) {
    SetMenuBorderAndClear();
    // Rubine oben rechts/links
    MainMenu.uMenuScreen[2 * MainMenu.uXdim + 2] = EMERALD_RUBY;
    MainMenu.uMenuScreen[2 * MainMenu.uXdim + 3] = EMERALD_RUBY;
    MainMenu.uMenuScreen[2 * MainMenu.uXdim + 4] = EMERALD_RUBY;
    MainMenu.uMenuScreen[2 * MainMenu.uXdim + 35] = EMERALD_RUBY;
    MainMenu.uMenuScreen[2 * MainMenu.uXdim + 36] = EMERALD_RUBY;
    MainMenu.uMenuScreen[2 * MainMenu.uXdim + 37] = EMERALD_RUBY;
    SetMenuText(MainMenu.uMenuScreen,"WELCOME TO EMERALD MINE SDL2",6,2,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.uMenuScreen,"LEVELGROUP:",2,4,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.uMenuScreen,SelectedLevelgroup.szLevelgroupname,13,4,EMERALD_FONT_GREEN);
    //SetMenuText(MainMenu.uMenuScreen,"1234567890123456789012345",13,4,EMERALD_FONT_GREEN);
    SetMenuText(MainMenu.uMenuScreen,"NAME:",2,5,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.uMenuScreen,"LEVEL:",2,6,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.uMenuScreen,"TOTALSCORE:",2,7,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.uMenuScreen,"GAMES PLAYED:",2,8,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.uMenuScreen,"PLAYTIME:",2,9,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.uMenuScreen,"GAMES WON:",2,10,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.uMenuScreen,"HANDICAP:",2,11,EMERALD_FONT_BLUE);
    // Seitenwand Levelgroup (ohne Pfeile)
    MainMenu.uMenuScreen[20 * MainMenu.uXdim + 28] = EMERALD_STEEL;
    MainMenu.uMenuScreen[21 * MainMenu.uXdim + 28] = EMERALD_STEEL;
    MainMenu.uMenuScreen[22 * MainMenu.uXdim + 28] = EMERALD_STEEL;
    // Seitenwand Player (ohne Pfeile)
    MainMenu.uMenuScreen[15 * MainMenu.uXdim + 34] = EMERALD_STEEL;
    MainMenu.uMenuScreen[16 * MainMenu.uXdim + 34] = EMERALD_STEEL;
    // Button-Untergrund "CREATE PLAYER"
    MainMenu.uMenuScreen[13 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
    MainMenu.uMenuScreen[13 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
    MainMenu.uMenuScreen[13 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
    MainMenu.uMenuScreen[13 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
    // Button-Untergrund "LEVELEDITOR"
    MainMenu.uMenuScreen[15 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
    MainMenu.uMenuScreen[15 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
    MainMenu.uMenuScreen[15 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
    MainMenu.uMenuScreen[15 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
    // Button-Untergrund "HIGHSCORES"
    MainMenu.uMenuScreen[16 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
    MainMenu.uMenuScreen[16 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
    MainMenu.uMenuScreen[16 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
    MainMenu.uMenuScreen[16 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
}


/*----------------------------------------------------------------------------
Name:           SetDynamicMenuElements
------------------------------------------------------------------------------
Beschreibung: Setzt dynamische Elemente f�r das Emerald-Mine-Hauptmen�.
Parameter
      Eingang: -
      Ausgang: -
R�ckgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Names.x, g_LevelgroupFilesCount, MainMenu.x, Actualplayer.x
------------------------------------------------------------------------------*/
int SetDynamicMenuElements(void) {
    char szText[256];
    int nErrorCode = 0;
    uint32_t I;
    uint32_t uKey;

    sprintf(szText," AVAILABLE LEVELGROUPS:%03u    LEVELS  ",g_LevelgroupFilesCount);
    SetMenuText(MainMenu.uMenuScreen,szText,1,18,EMERALD_FONT_STEEL_BLUE);
    sprintf(szText,"      AVAILABLE PLAYERS:%03u           ",Names.uNameCount);
    SetMenuText(MainMenu.uMenuScreen,szText,1,13,EMERALD_FONT_STEEL_BLUE);
    // Zeigt bis zu 4 Levelgruppen mit Levelanzahl im unteren Bereich an
    for (I = 0; I < 4; I++) {
        if (MainMenu.uLevelgroupList[I] != 0xFF) {
            SetMenuText(MainMenu.uMenuScreen,"                         ",2,19 + I,EMERALD_FONT_BLUE);
            SetMenuText(MainMenu.uMenuScreen,LevelgroupFiles[MainMenu.uLevelgroupList[I]].szLevelgroupname,2,19 + I,EMERALD_FONT_BLUE);
            sprintf(szText,"%04u",LevelgroupFiles[MainMenu.uLevelgroupList[I]].uLevelCount);
            SetMenuText(MainMenu.uMenuScreen,szText,32,19 + I,EMERALD_FONT_BLUE);
        }
    }
    // Hier die Pfeile der Levelgruppenauswahl, wenn mehr als 4 Levelgruppen verf�gbar sind
    if (g_LevelgroupFilesCount > MAX_LEVELGROUPS_IN_LIST) {
        MainMenu.uMenuScreen[19 * MainMenu.uXdim + 28] = EMERALD_STEEL_ARROW_UP;
        MainMenu.uMenuScreen[22 * MainMenu.uXdim + 28] = EMERALD_STEEL_ARROW_DOWN;
    } else {
        MainMenu.uMenuScreen[19 * MainMenu.uXdim + 28] = EMERALD_STEEL;
        MainMenu.uMenuScreen[22 * MainMenu.uXdim + 28] = EMERALD_STEEL;
    }
    // Zeigt bis zu 4 Namen im mittleren Bereich an
    //SetMenuText(MainMenu.uMenuScreen,"1234567890123456789012345678901",2,13,EMERALD_FONT_BLUE);
    for (I = 0; I < MAX_NAMES_IN_LIST; I++) {
        if (MainMenu.uNamesList[I] != 0xFF) {
            SetMenuText(MainMenu.uMenuScreen,"                               ",2,14 + I,EMERALD_FONT_BLUE);
            SetMenuText(MainMenu.uMenuScreen,Names.Name[MainMenu.uNamesList[I]].szName,2,14 + I,EMERALD_FONT_BLUE);
        } else {
            SetMenuText(MainMenu.uMenuScreen,"                               ",2,14 + I,EMERALD_FONT_BLUE);
        }
    }
    if (Names.uNameCount > MAX_NAMES_IN_LIST) {
        MainMenu.uMenuScreen[14 * MainMenu.uXdim + 34] = EMERALD_STEEL_ARROW_UP;
        MainMenu.uMenuScreen[17 * MainMenu.uXdim + 34] = EMERALD_STEEL_ARROW_DOWN;
    } else {
        MainMenu.uMenuScreen[14 * MainMenu.uXdim + 34] = EMERALD_STEEL;
        MainMenu.uMenuScreen[17 * MainMenu.uXdim + 34] = EMERALD_STEEL;
    }
    SetMenuText(MainMenu.uMenuScreen,"-       ",8,6,EMERALD_FONT_GREEN);   // Level
    SetMenuText(MainMenu.uMenuScreen,"-       ",13,7,EMERALD_FONT_GREEN);  // Totalscore
    SetMenuText(MainMenu.uMenuScreen,"-       ",15,8,EMERALD_FONT_GREEN);  // Games Played
    SetMenuText(MainMenu.uMenuScreen,"-                   ",11,9,EMERALD_FONT_GREEN);  // Playtime
    SetMenuText(MainMenu.uMenuScreen,"-       ",12,10,EMERALD_FONT_GREEN); // Games Won
    SetMenuText(MainMenu.uMenuScreen,"-       ",11,11,EMERALD_FONT_GREEN); // Handicap
    SetMenuText(MainMenu.uMenuScreen,"                               ",7,5,EMERALD_FONT_GREEN); // ausgew�hlter Playername
    if (MainMenu.nState == 0) {
        if (Names.uNameCount > 0) {
            if (Actualplayer.bValid) {
                SetMenuText(MainMenu.uMenuScreen,Actualplayer.szPlayername,7,5,EMERALD_FONT_GREEN);
                sprintf(szText,"%u       ",Actualplayer.uLevel);
                SetMenuText(MainMenu.uMenuScreen,szText,8,6,EMERALD_FONT_GREEN);
                //MainMenu.uMenuScreen[6 * MainMenu.uXdim + 8] = EMERALD_FONT_STEEL_ARROW_LEFT;
                //MainMenu.uMenuScreen[6 * MainMenu.uXdim + 12] = EMERALD_FONT_STEEL_ARROW_RIGHT;
                MainMenu.uMenuScreen[6 * MainMenu.uXdim + 11] = EMERALD_STEEL_ARROW_DOWN;
                MainMenu.uMenuScreen[6 * MainMenu.uXdim + 12] = EMERALD_STEEL_ARROW_UP;
                sprintf(szText,"%u       ",Actualplayer.uTotalScore);
                SetMenuText(MainMenu.uMenuScreen,szText,13,7,EMERALD_FONT_GREEN);
                sprintf(szText,"%u       ",Actualplayer.uGamesPlayed);
                SetMenuText(MainMenu.uMenuScreen,szText,15,8,EMERALD_FONT_GREEN);
                sprintf(szText,"%f DAYS      ",(float)Actualplayer.uPlayTimeS / 86400);
                SetMenuText(MainMenu.uMenuScreen,szText,11,9,EMERALD_FONT_GREEN);
                sprintf(szText,"%u       ",Actualplayer.uGamesWon);
                SetMenuText(MainMenu.uMenuScreen,szText,12,10,EMERALD_FONT_GREEN);
                sprintf(szText,"%u       ",Actualplayer.uHandicap);
                SetMenuText(MainMenu.uMenuScreen,szText,11,11,EMERALD_FONT_GREEN);
                SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,true);
                MainMenu.uMenuScreen[14 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
                MainMenu.uMenuScreen[14 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
                MainMenu.uMenuScreen[14 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
                MainMenu.uMenuScreen[14 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
            } else {
                SetMenuText(MainMenu.uMenuScreen,"PLEASE SELECT PLAYER",7,5,EMERALD_FONT_GREEN);
                SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,false);
                MainMenu.uMenuScreen[14 * MainMenu.uXdim + 75] = EMERALD_SPACE;
                MainMenu.uMenuScreen[14 * MainMenu.uXdim + 76] = EMERALD_SPACE;
                MainMenu.uMenuScreen[14 * MainMenu.uXdim + 77] = EMERALD_SPACE;
                MainMenu.uMenuScreen[14 * MainMenu.uXdim + 78] = EMERALD_SPACE;
            }
        } else {
            SetMenuText(MainMenu.uMenuScreen,"PLEASE CREATE A PLAYER",7,5,EMERALD_FONT_GREEN);
        }
    } else if (MainMenu.nState == 1) {
        if (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) {  // Verlassen der Namens-Eingabe
            nErrorCode = SDL_SetTextureColorMod(GetTextureByIndex(71),100,100,100); // Cursor-Stein auf volle Helligkeit stellen
            SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,true);
            MainMenu.uMenuScreen[13 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[13 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[13 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[13 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
            SetButtonActivity(BUTTONLABEL_LEVELEDITOR,true);
            MainMenu.uMenuScreen[15 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[15 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[15 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[15 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
            SetButtonActivity(BUTTONLABEL_HIGHSCORES,true);
            MainMenu.uMenuScreen[16 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[16 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[16 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[16 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
            memset(MainMenu.szTempName,0,sizeof(MainMenu.szTempName));
            MainMenu.uCursorPos = 0;
            MainMenu.nState = 0;
            WaitNoKey();
        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_RETURN]) {
            nErrorCode = SDL_SetTextureColorMod(GetTextureByIndex(71),100,100,100);
            SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,true);
            MainMenu.uMenuScreen[13 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[13 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[13 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[13 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
            SetButtonActivity(BUTTONLABEL_LEVELEDITOR,true);
            MainMenu.uMenuScreen[15 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[15 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[15 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[15 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
            SetButtonActivity(BUTTONLABEL_HIGHSCORES,true);
            MainMenu.uMenuScreen[16 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[16 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[16 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
            MainMenu.uMenuScreen[16 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
            if (InsertNewName(MainMenu.szTempName) == 0) {
                if (ReadNamesFile() == 0) {
                    InitLists();
                    SDL_Log("Inserting name: %s ... OK",MainMenu.szTempName);
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
            MainMenu.uMenuScreen[13 * MainMenu.uXdim + 75] = EMERALD_SPACE;
            MainMenu.uMenuScreen[13 * MainMenu.uXdim + 76] = EMERALD_SPACE;
            MainMenu.uMenuScreen[13 * MainMenu.uXdim + 77] = EMERALD_SPACE;
            MainMenu.uMenuScreen[13 * MainMenu.uXdim + 78] = EMERALD_SPACE;
            // Delete Player
            MainMenu.uMenuScreen[14 * MainMenu.uXdim + 75] = EMERALD_SPACE;
            MainMenu.uMenuScreen[14 * MainMenu.uXdim + 76] = EMERALD_SPACE;
            MainMenu.uMenuScreen[14 * MainMenu.uXdim + 77] = EMERALD_SPACE;
            MainMenu.uMenuScreen[14 * MainMenu.uXdim + 78] = EMERALD_SPACE;
            // Leveleditor
            MainMenu.uMenuScreen[15 * MainMenu.uXdim + 75] = EMERALD_SPACE;
            MainMenu.uMenuScreen[15 * MainMenu.uXdim + 76] = EMERALD_SPACE;
            MainMenu.uMenuScreen[15 * MainMenu.uXdim + 77] = EMERALD_SPACE;
            MainMenu.uMenuScreen[15 * MainMenu.uXdim + 78] = EMERALD_SPACE;
            // Highscores
            MainMenu.uMenuScreen[16 * MainMenu.uXdim + 75] = EMERALD_SPACE;
            MainMenu.uMenuScreen[16 * MainMenu.uXdim + 76] = EMERALD_SPACE;
            MainMenu.uMenuScreen[16 * MainMenu.uXdim + 77] = EMERALD_SPACE;
            MainMenu.uMenuScreen[16 * MainMenu.uXdim + 78] = EMERALD_SPACE;

            // Eingabe eines Zeichens f�r den Spielernamen
            nErrorCode = SDL_SetTextureColorMod(GetTextureByIndex(71),MainMenu.uFlashBrightness[MainMenu.uFlashIndex],MainMenu.uFlashBrightness[MainMenu.uFlashIndex],MainMenu.uFlashBrightness[MainMenu.uFlashIndex]);
            MainMenu.uFlashIndex++;
            if (MainMenu.uFlashIndex > MainMenu.uMaxFlashIndex) {
                MainMenu.uFlashIndex = 0;
            }
            uKey = FilterBigFontKey(GetKey());
            if (uKey != 0) {
               SDL_Log("Key = %u",uKey);
               if (MainMenu.uCursorPos < EMERALD_PLAYERNAME_LEN) {
                    MainMenu.szTempName[MainMenu.uCursorPos] = uKey;
                    MainMenu.uCursorPos++;
               }
            } else  if (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]) {
                 SDL_Log("Key = BACKSPACE");
                 if (MainMenu.uCursorPos > 0) {
                    MainMenu.szTempName[MainMenu.uCursorPos - 1] = 0;
                    MainMenu.uCursorPos--;
                 }
                 do {
                    UpdateInputStates();
                 } while (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]);
            }
            SetMenuText(MainMenu.uMenuScreen,"                                   ",7,5,EMERALD_FONT_GREEN); // l�scht auch alte Cursorposition
            SetMenuText(MainMenu.uMenuScreen,MainMenu.szTempName,7,5,EMERALD_FONT_GREEN);
            MainMenu.uMenuScreen[5 * MainMenu.uXdim + 7 + MainMenu.uCursorPos] = EMERALD_STONE;    // Stein als Cursor setzen
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           EmeraldMineMainMenu
------------------------------------------------------------------------------
Beschreibung: Baut das Hauptmen� auf. Hier kann Levelgruppe, Name und Level ausgew�hlt werden.
              Mit Hauptschleifen-Funktion f�r das Spiel.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -

R�ckgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Playfield.x, InputStates.x, MainMenu.x
               SelectedLevelgroup.x, Config.x, Audioplayer.x, Actualplayer.x
------------------------------------------------------------------------------*/
int EmeraldMineMainMenu(SDL_Renderer *pRenderer) {
    int nErrorCode;
    uint32_t uModVolume;
    uint32_t uHighscoreLevel;
    bool bMenuRun;
    bool bEndGame;
    bool bPrepareExit;
    bool bShowHighscores;
    bool bStartEditor;
    int nColorDimm;
    int nLevelButton;
    int nLastLevelButton;
    int nPlayerlistButton;
    int nLastPlayerlistButton;
    int nLevelgrouplistButton;
    int nLastLevelgrouplistButton;
    int nNewHighscoreIndex;
    char szText[256];

    InitMainMenu();
    // Namen einlesen
    if (ReadNamesFile() != 0) {
        return -1;
    }
    if (SetModMusic(6) != 0) {  // 6. 2kad04.mod  von Maktone
        return -1;
    }
    if (CreateButton(BUTTONLABEL_CREATE_PLAYER,"CREATE PLAYER",MainMenu.uXoffs + 1127,MainMenu.uYoffs + 454,true,false) != 0) {
        return -1;
    }
    if (CreateButton(BUTTONLABEL_DELETE_PLAYER,"DELETE PLAYER",MainMenu.uXoffs + 1127,MainMenu.uYoffs + 486,false,false) != 0) {
        return -1;
    }
    if (CreateButton(BUTTONLABEL_LEVELEDITOR,"LEVELEDITOR",MainMenu.uXoffs + 1135,MainMenu.uYoffs + 518,true,false) != 0) {
        return -1;
    }
    if (CreateButton(BUTTONLABEL_HIGHSCORES,"HIGHSCORES",MainMenu.uXoffs + 1139,MainMenu.uYoffs + 550,true,false) != 0) {
        return -1;
    }
    SDL_PauseAudioDevice(Audioplayer.audio_device, 0);
    SetStaticMenuElements();
    uModVolume = 0;
    SetModVolume(uModVolume);
    nErrorCode = 0;
    nColorDimm = 0;
    bPrepareExit = false;
    bMenuRun = true;
    bEndGame = false;
    bStartEditor =  false;
    bShowHighscores = false;
    nLevelButton = 0;   // 0 = nicht gedr�ckt, 1 = down, 2 = up
    nLastLevelButton = 0;
    nPlayerlistButton = 0;
    nLastPlayerlistButton = 0;
    nLevelgrouplistButton = 0;
    nLastLevelgrouplistButton = 0;
    nNewHighscoreIndex = -1;
    if (GetLevelgroupFiles() == 0) {    // Wenn das nicht funktioniert, kann nicht weitergemacht werden!
        InitLists();
        // ShowAvailableLevelgroups();
        if (SelectAlternativeLevelgroup(Config.uLevelgroupMd5Hash,true) == 0) {
            // ShowSelectedLevelgroup();
            if (SelectName(Config.szPlayername,SelectedLevelgroup.uMd5Hash) != 0) {
                // Falls letzter Name nicht mehr ausw�hlbar ist, diesen aus dem Konfigurationsfile l�schen
                memset(Config.szPlayername,0,sizeof(Config.szPlayername));
                nErrorCode = WriteConfigFile();
            }
            while ((bMenuRun) && (nErrorCode == 0)) {
                UpdateManKey(); // Ruft UpdateInputStates() auf
                if (((InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) || (InputStates.bQuit)) && (nColorDimm == 100) && (MainMenu.nState == 0)) {
                    bPrepareExit = true;
                    bEndGame = true;        // Spiel beenden
                } else if ( (MainMenu.nState == 0) && (Actualplayer.bValid) && (nColorDimm == 100) && (SelectedLevelgroup.bOK) && ((ManKey.bFire) || (InputStates.pKeyboardArray[SDL_SCANCODE_SPACE])) ) {
                    bPrepareExit = true;    // Level starten
                } else if ( ((InputStates.pKeyboardArray[SDL_SCANCODE_H]) || IsButtonPressed(BUTTONLABEL_HIGHSCORES)) && (nColorDimm == 100) && (MainMenu.nState == 0) ) {
                    bPrepareExit = true;    // Highscores zeigen
                    bShowHighscores = true;
                } else if ((MainMenu.nState == 0) && (IsButtonPressed(BUTTONLABEL_DELETE_PLAYER))) {
                    if (Actualplayer.bValid) {
                        SDL_Log("Deleting name :%s",Actualplayer.szPlayername);
                        nErrorCode = DeleteName(Actualplayer.szPlayername);
                        if (nErrorCode == 0) {
                            memset(&Actualplayer,0,sizeof(Actualplayer));
                            InitLists();
                        }
                    }
                } else if ( (MainMenu.nState == 0) && (nColorDimm == 100) && (SelectedLevelgroup.bOK) && (IsButtonPressed(BUTTONLABEL_LEVELEDITOR) )) {
                    bPrepareExit = true;    // Highscores zeigen
                    bStartEditor =  true;
                }
                if (bPrepareExit) {
                    if (nColorDimm > 0) {
                        nColorDimm = nColorDimm - 4;
                        SetAllTextureColors(nColorDimm);
                        uModVolume = uModVolume - 4;
                        SetModVolume(uModVolume);
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
                            SetButtonActivity(BUTTONLABEL_LEVELEDITOR,true);
                            SetButtonActivity(BUTTONLABEL_HIGHSCORES,true);
                            SetStaticMenuElements();
                            bPrepareExit = false;
                            nColorDimm = 0;
                            uModVolume = 0;
                            SetAllTextureColors(nColorDimm);
                            SetModVolume(uModVolume);
                            if (SetModMusic(6) != 0) {
                                return -1;
                            }
                            bShowHighscores = false;
                            WaitNoKey();
                        } else if (bStartEditor) {
                            SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,false);
                            SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,false);
                            SetButtonActivity(BUTTONLABEL_LEVELEDITOR,false);
                            SetButtonActivity(BUTTONLABEL_HIGHSCORES,false);

                            memset(&Actualplayer,0,sizeof(Actualplayer));
                            nErrorCode = PreEditorMenu(pRenderer);
                            if (nErrorCode == 0) {
                                // Eine ggf. ge�nderte Levelgruppe nun mit Highscorefile ausstatten
                                nErrorCode = SelectAlternativeLevelgroup(Config.uLevelgroupMd5Hash,true);
                            }
                            SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,true);
                            if (Actualplayer.bValid) {
                                SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,true);
                            }
                            SetButtonActivity(BUTTONLABEL_LEVELEDITOR,true);
                            SetButtonActivity(BUTTONLABEL_HIGHSCORES,true);
                            nColorDimm = 0;
                            uModVolume = 0;
                            SetStaticMenuElements();
                            bPrepareExit = false;
                            bStartEditor = false;
                            if (SetModMusic(6) != 0) {      // MOD 6, 2kad04
                                return -1;
                            }
                            WaitNoKey();
                        } else if (nErrorCode == 0) {
                            // Ein erster Entwurf f�r Emerald Mine. Das Spielergebnis (Erfolg oder Versagen) kann in Playfield.x abgefragt werden.
                            SDL_Log("Start Game with level %u",Actualplayer.uLevel);
                            nErrorCode = RunGame(pRenderer,Actualplayer.uLevel);
                            SDL_Log("%s: RunGame() ErrorCode: %u",__FUNCTION__,nErrorCode);
                            if (nErrorCode == 0) {
                                nErrorCode = EvaluateGame(&nNewHighscoreIndex,&uHighscoreLevel);
                            }
                            if ((nNewHighscoreIndex >= 0) && (nErrorCode == 0)) {
                                SDL_Log("%s: New highscore at index: %u",__FUNCTION__,nNewHighscoreIndex);
                                SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,false);
                                SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,false);
                                SetButtonActivity(BUTTONLABEL_LEVELEDITOR,false);
                                SetButtonActivity(BUTTONLABEL_HIGHSCORES,false);
                                nErrorCode = ShowHighScores(pRenderer,uHighscoreLevel,nNewHighscoreIndex);
                                SetButtonActivity(BUTTONLABEL_CREATE_PLAYER,true);
                                if (Actualplayer.bValid) {
                                    SetButtonActivity(BUTTONLABEL_DELETE_PLAYER,true);
                                }
                                SetButtonActivity(BUTTONLABEL_LEVELEDITOR,true);
                                SetButtonActivity(BUTTONLABEL_HIGHSCORES,true);
                            }
                            SetStaticMenuElements();
                            bPrepareExit = false;
                            nColorDimm = 0;
                            uModVolume = 0;
                            SetAllTextureColors(nColorDimm);
                            SetModVolume(uModVolume);
                            if (SetModMusic(6) != 0) {      // MOD 6, 2kad04
                                return -1;
                            }
                            WaitNoKey();
                        }
                    }
                }
                // printf("x:%u  y:%u\n",InputStates.nMouseXpos,InputStates.nMouseYpos);
                if (!bPrepareExit) {
                    if (MainMenu.nState == 0) {     // Levelgruppen- und Spielerauswahl nur w�hrend Namenseingabe nicht aktiv
                        nErrorCode = MenuSelectLevelgroup(pRenderer);
                        if (nErrorCode == 0) {
                            nErrorCode = MenuSelectName(pRenderer);
                        }
                        nLastPlayerlistButton = nPlayerlistButton;
                        nPlayerlistButton = GetPlayerListButton();
                        if ((nLastPlayerlistButton == 0) && (nPlayerlistButton != 0)) {
                            ScrollPlayernames(nPlayerlistButton);
                        }

                        nLastLevelgrouplistButton = nLevelgrouplistButton;
                        nLevelgrouplistButton = GetLevelgroupListButton();
                        if ((nLastLevelgrouplistButton == 0) && (nLevelgrouplistButton != 0)) {
                            ScrollLevelGroups(nLevelgrouplistButton);
                        }
                    }
                    SetMenuText(MainMenu.uMenuScreen,"                         ",13,4,EMERALD_FONT_GREEN);
                    SetMenuText(MainMenu.uMenuScreen,SelectedLevelgroup.szLevelgroupname,13,4,EMERALD_FONT_GREEN);
                    SetDynamicMenuElements();
                }
                nLastLevelButton = nLevelButton;
                nLevelButton = GetLevelButton();
                if ((nLastLevelButton == 0) && (Actualplayer.bValid)) {
                    if (nLevelButton == EMERALD_STEEL_ARROW_DOWN_PRESSED) {
                        if (Actualplayer.uLevel > 0) {
                            Actualplayer.uLevel--;
                        }
                    } else if (nLevelButton == EMERALD_STEEL_ARROW_UP_PRESSED) {
                        if (Actualplayer.uLevel < Actualplayer.uHandicap) {
                            Actualplayer.uLevel++;
                        }
                    }
                    if (nLevelButton != 0) {
                        SetMenuText(MainMenu.uMenuScreen,Actualplayer.szPlayername,7,5,EMERALD_FONT_GREEN);
                        sprintf(szText,"%u       ",Actualplayer.uLevel);
                    }
                }
                nLastLevelButton = nLevelButton;
                if (nLevelButton != 0) SDL_Log("LevelButton:%d",nLevelButton);

                if ((!bPrepareExit) && (nColorDimm < 100)) {
                    nColorDimm = nColorDimm + 4;
                    SetAllTextureColors(nColorDimm);
                    uModVolume = uModVolume + 4;
                    SetModVolume(uModVolume);
                }
                PlayMusic();
                if (nErrorCode == 0) {
                    nErrorCode = RenderMenuElements(pRenderer);
                }
                ShowButtons(pRenderer);
                if ((MainMenu.nState == 0) && (IsButtonPressed(BUTTONLABEL_CREATE_PLAYER))) {
                    ActivateInputPlayernameMode();  // Eingabemodus f�r Namenseingabe aktivieren
                }
                SDL_RenderPresent(pRenderer);   // Renderer anzeigen, l�sst Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
                SDL_RenderClear(pRenderer);     // Renderer f�r n�chstes Frame l�schen
                Playfield.uFrameCounter++;
            }
        } else {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Levelgroup problem","Can not select levelgroup!",NULL);
            nErrorCode = -1;
        }
    }
    SetAllTextureColors(100);
    FreeButton(BUTTONLABEL_CREATE_PLAYER);
    FreeButton(BUTTONLABEL_DELETE_PLAYER);
    FreeButton(BUTTONLABEL_LEVELEDITOR);
    FreeButton(BUTTONLABEL_HIGHSCORES);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ShowAuthorAndLevelname
------------------------------------------------------------------------------
Beschreibung: Zeigt vor dem Spiel den Level-Author und den Levelnamen.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               uLevel, uint32_t, Levelnummer

R�ckgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: Playfield.x, InputStates.x, ManKey.x, MainMenu.x
------------------------------------------------------------------------------*/
int ShowAuthorAndLevelname(SDL_Renderer *pRenderer, uint32_t uLevel) {
    int nErrorCode = 0;
    int nColorDimm;
    char szText[EMERALD_AUTHOR_LEN + 128];
    char szNum[16];
    uint32_t I;
    uint32_t uModVolume;
    bool bMenuRun;
    bool bPrepareExit;
    bool bPlayGame;

    bPlayGame = true;
    bMenuRun = true;
    bPrepareExit = false;
    SDL_RenderClear(pRenderer);     // Renderer f�r n�chstes Frame l�schen
    SetMenuBorderAndClear();
    nColorDimm = 0;
    uModVolume = 0;
    SetModVolume(uModVolume);
    SetAllTextureColors(nColorDimm);
    SetMenuText(MainMenu.uMenuScreen,SelectedLevelgroup.szLevelgroupname,-1,1,EMERALD_FONT_BLUE);
    for (I = 1; (I < (MainMenu.uXdim - 1)); I++) {
        MainMenu.uMenuScreen[2 * MainMenu.uXdim + I] = EMERALD_STEEL;
        MainMenu.uMenuScreen[12 * MainMenu.uXdim + I] = EMERALD_STEEL;
    }
    SetMenuText(MainMenu.uMenuScreen,Playfield.szLevelTitle,-1,5,EMERALD_FONT_BLUE);
    sprintf(szNum,"%03d",uLevel);
    strcpy(szText,"LEVEL:");
    strcat(szText,szNum);
    SetMenuText(MainMenu.uMenuScreen,szText,-1,7,EMERALD_FONT_BLUE);
    strcpy(szText,"AUTHOR:");
    strcat(szText,Playfield.szLevelAuthor);
    SetMenuText(MainMenu.uMenuScreen,szText,-1,9,EMERALD_FONT_BLUE);

    MainMenu.uMenuScreen[16 * MainMenu.uXdim + 2] = EMERALD_EMERALD;
    SetMenuText(MainMenu.uMenuScreen,"PRESS FIRE (LEFT CTRL) TO PLAY",4,16,EMERALD_FONT_BLUE);
    MainMenu.uMenuScreen[19 * MainMenu.uXdim + 2] = EMERALD_RUBY;
    SetMenuText(MainMenu.uMenuScreen,"PRESS ESC TO CANCEL",4,19,EMERALD_FONT_BLUE);

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
            } else if ((InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) || InputStates.bQuit) {
                bPrepareExit = true;
                bPlayGame = false;
            }
        }
        if (bPrepareExit) {
            if (nColorDimm > 0) {
                nColorDimm = nColorDimm - 4;
                SetAllTextureColors(nColorDimm);
                uModVolume = uModVolume - 4;
                SetModVolume(uModVolume);
            } else {
                bMenuRun = false;
            }
        }
        SDL_RenderPresent(pRenderer);   // Renderer anzeigen, l�sst Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
        SDL_RenderClear(pRenderer);     // Renderer f�r n�chstes Frame l�schen
        Playfield.uFrameCounter++;
    } while (bMenuRun);
    if ((nErrorCode == 0) && (!bPlayGame)) {
        nErrorCode = 5;
    }
    WaitNoKey();
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

R�ckgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: MainMenu.x
------------------------------------------------------------------------------*/
int DimmMainMenu(SDL_Renderer *pRenderer, bool bDimmUp) {
    int nColorDimm;
    int nErrorCode = 0;
    uint32_t uModVolume;

    if (bDimmUp) {
        nColorDimm = 0;
        uModVolume = 0;
        do {
            nColorDimm = nColorDimm + 2;
            uModVolume = uModVolume + 2;
            SetAllTextureColors(nColorDimm);
            SetModVolume(uModVolume);
            PlayMusic();
            nErrorCode = RenderMenuElements(pRenderer);
            if (nErrorCode == 0) {
                SDL_RenderPresent(pRenderer);   // Renderer anzeigen, l�sst Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
                SDL_RenderClear(pRenderer);     // Renderer f�r n�chstes Frame l�schen
                SDL_Delay(5);
                Playfield.uFrameCounter++;
            }
         } while ((nErrorCode == 0) && (nColorDimm < 100));
    } else {
        nColorDimm = 100;
        uModVolume = 100;
        do {
            nColorDimm = nColorDimm - 2;
            uModVolume = uModVolume - 2;
            SetAllTextureColors(nColorDimm);
            SetModVolume(uModVolume);
            PlayMusic();
            nErrorCode = RenderMenuElements(pRenderer);
            if (nErrorCode == 0) {
                SDL_RenderPresent(pRenderer);   // Renderer anzeigen, l�sst Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
                SDL_RenderClear(pRenderer);     // Renderer f�r n�chstes Frame l�schen
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
               nNewHighScoreIndex, int, Markierung einer Zeile f�r neuen Highscore
                    Falls der Marker negativ oder ung�ltig ist, wird keine Markierung ausgef�hrt.
                    Die Nummerierung wird ab 0 durchgef�hrt, d.h. 0 = erster Platz.

R�ckgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: Playfield.x, InputStates.x, ManKey.x, MainMenu.x, HighscoreFile,
               SelectedLevelgroup.x
------------------------------------------------------------------------------*/
int ShowHighScores(SDL_Renderer *pRenderer, uint32_t uLevel, int nNewHighScoreIndex) {
    int nErrorCode = 0;
    int nColorDimm;
    char szText[1024];
    char szNum[16];
    uint32_t I;
    uint32_t uModVolume;
    uint32_t uRainBowRGB;
    bool bMenuRun;
    bool bPrepareExit;
    bool bPlayGame;
    int nRed,nGreen,nBlue;
    uint8_t uRand;

    nRed = 0;
    nGreen = 0;
    nBlue = 0xFF;
    bPlayGame = true;
    bMenuRun = true;
    bPrepareExit = false;
    SDL_RenderClear(pRenderer);     // Renderer f�r n�chstes Frame l�schen
    SetMenuBorderAndClear();
    nColorDimm = 0;
    uRand = randn(0,10) & 0x01;         // Zufallszahl 0 oder 1
    if (SetModMusic(7 + uRand) != 0) {      // 7. 2kad02.mod (2000AD cracktro02) oder 8. the brewery  keweils von Maktone
        return -1;
    }
    if (CreateButton(BUTTONLABEL_EXIT_HIGHSCORES,"Back to main menu",MainMenu.uXoffs + 1100,MainMenu.uYoffs + 742,true,false) != 0) {
        return -1;
    }
    uModVolume = 0;
    SetModVolume(uModVolume);
    SetAllTextureColors(nColorDimm);
    SetMenuText(MainMenu.uMenuScreen,"TOP TWENTY",-1,0,EMERALD_FONT_STEEL_BLUE);
    sprintf(szText,"%s/LEVEL:%03u",SelectedLevelgroup.szLevelgroupname,uLevel);
    SetMenuText(MainMenu.uMenuScreen,szText,-1,1,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.uMenuScreen,"   NAME                          SCORE",1,2,EMERALD_FONT_STEEL_BLUE);
    for (I = 0; I < EMERALD_HIGHSCORE_LISTLEN; I++) {
        sprintf(szNum,"%02d",I + 1);    // Positionsnummer
        SetMenuText(MainMenu.uMenuScreen,szNum,1,I + 3,EMERALD_FONT_BLUE);  // Positionsnummer
        MainMenu.uMenuScreen[(I + 3) * MainMenu.uXdim + 3] = EMERALD_STEEL; // Senkrechte Abtrennung Pos.Nr zu Name
        MainMenu.uMenuScreen[(I + 3) * MainMenu.uXdim + 33] = EMERALD_STEEL;// Senkrechte Abtrennung Name zu Score
        // Namen und Score
        if (strlen(HighscoreFile.TopTwenty[uLevel].szTopTwenty[I]) > 0) {
            SetMenuText(MainMenu.uMenuScreen,HighscoreFile.TopTwenty[uLevel].szTopTwenty[I],4,I + 3,EMERALD_FONT_BLUE);
        } else {
            SetMenuText(MainMenu.uMenuScreen,"-",4,I + 3,EMERALD_FONT_BLUE);
        }
        sprintf(szNum,"%04u",HighscoreFile.TopTwenty[uLevel].uHighScore[I]);
        SetMenuText(MainMenu.uMenuScreen,szNum,35,I + 3,EMERALD_FONT_BLUE);
    }
    // Markierung eines neuen Highscores durchf�hren
    if ((nNewHighScoreIndex >= 0) && (nNewHighScoreIndex < EMERALD_HIGHSCORE_LISTLEN)) {
        MainMenu.uMenuScreen[(nNewHighScoreIndex + 3) * MainMenu.uXdim + 3] = EMERALD_WHEEL; // Senkrechte Abtrennung Pos.Nr zu Name
        MainMenu.uMenuScreen[(nNewHighScoreIndex + 3) * MainMenu.uXdim + 33] = EMERALD_WHEEL; // Senkrechte Abtrennung Pos.Nr zu Name
    }
    do {
        for (I = 0; I < EMERALD_HIGHSCORE_LISTLEN; I++) {
            uRainBowRGB = GetRainbowColors(I * 5 + Playfield.uFrameCounter);
            nRed = (((uRainBowRGB >> 16) & 0xFF) * nColorDimm) / 100;
            nGreen = (((uRainBowRGB >> 8) & 0xFF) * nColorDimm) / 100;
            nBlue = ((uRainBowRGB & 0xFF) * nColorDimm) / 100;
            SDL_SetRenderDrawColor(pRenderer,nRed,nGreen,nBlue,SDL_ALPHA_OPAQUE);  // Farbe f�r Line setzen
            SDL_RenderDrawLine(pRenderer,MainMenu.uXoffs + 0, MainMenu.uYoffs + ((I + 3) * FONT_H), DEFAULT_WINDOW_W - 1, MainMenu.uYoffs + ((I + 3) * FONT_H));
            SDL_SetRenderDrawColor(pRenderer,0,0,0, SDL_ALPHA_OPAQUE);  // Muss am Ende stehen, damit Hintergrund wieder dunkel wird
        }
        UpdateManKey();
        PlayMusic();
        RenderMenuElements(pRenderer);
        if ((!bPrepareExit) && (nColorDimm < 100)) {
            nColorDimm = nColorDimm + 2;
            SetAllTextureColors(nColorDimm);
            uModVolume = uModVolume + 2;
            SetModVolume(uModVolume);
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
                uModVolume = uModVolume - 2;
                SetModVolume(uModVolume);
            } else {
                bMenuRun = false;
            }
        }
        ShowButtons(pRenderer);
        SDL_RenderPresent(pRenderer);   // Renderer anzeigen, l�sst Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
        SDL_RenderClear(pRenderer);     // Renderer f�r n�chstes Frame l�schen
        Playfield.uFrameCounter++;
    } while (bMenuRun);
    if ((nErrorCode == 0) && (!bPlayGame)) {
        nErrorCode = 5;
    }
    FreeButton(BUTTONLABEL_EXIT_HIGHSCORES);
    WaitNoKey();
    return nErrorCode;
}
