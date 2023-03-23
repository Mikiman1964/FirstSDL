#include <ctype.h>
#include "buttons.h"
#include "EmeraldMineMainMenu.h"
#include "GetTextureIndexByElement.h"
#include "KeyboardMouse.h"
#include "loadlevel.h"
#include "man.h"
#include "modplay.h"
#include "mySDL.h"
#include "mystd.H"


extern INPUTSTATES InputStates;
extern MANKEY ManKey;
extern PLAYFIELD Playfield;
extern uint32_t g_LevelgroupFilesCount;
extern NAMES Names;
extern CONFIG Config;
extern LEVELGROUPFILE LevelgroupFiles[EMERALD_MAX_LEVELGROUPFILES];
extern LEVELGROUP SelectedLevelgroup;
extern ACTUALPLAYER Actualplayer;
MAINMENU MainMenu;


/*----------------------------------------------------------------------------
Name:           GetLevelButton
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Levelbutton (down/up) gedrückt ist.

Parameter
      Eingang: -
      Ausgang: -

Rückgabewert:  int, 0 = kein Button gedrückz, 1 = down Button, 2 = up Button
Seiteneffekte: InputStates.x, MainMenu.x
------------------------------------------------------------------------------*/
int GetLevelButton(void) {
    int nButton;

    nButton = 0;
    if (InputStates.bLeftMouseButton) {
        // Beide Buttons auf selber Y-Position
        if ((InputStates.nMouseYpos >= (192 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (224 + MainMenu.uYoffs))) {
            if ((InputStates.nMouseXpos >= (352 + MainMenu.uXoffs)) && (InputStates.nMouseXpos < (384 + MainMenu.uXoffs))) {
                nButton = 1;
            } else if ((InputStates.nMouseXpos >= (384 + MainMenu.uXoffs)) && (InputStates.nMouseXpos < (416 + MainMenu.uXoffs))) {
                nButton = 2;
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
Beschreibung: Bewertet das zurückliegende Spiel und speichert neue Werte
              (Totalscore, Handicap usw.) für den aktuellen Spieler.


Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: Playfield.x, Actualplayer.x, SelectedLevelgroup.x
------------------------------------------------------------------------------*/
int EvaluateGame(void) {
    int nErrorCode;

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
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           MenuSelectLevelgroup
------------------------------------------------------------------------------
Beschreibung: Erledigt die Auswahl einer Levelgruppe aus dem Hauptmenü heraus und
              blendet die Balken für die Auswahl einer Levelgruppe an, wenn sich
              der Mauspfeil über den Namen einer Gruppe befindet.

Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: MainMenu.x, Actualplayer.x
------------------------------------------------------------------------------*/
int MenuSelectLevelgroup(SDL_Renderer *pRenderer) {
    int nErrorCode = 0;
    uint32_t uBeamPosition;
    char szPlayername[EMERALD_PLAYERNAME_LEN + 1];

    uBeamPosition = GetLevelgroupBeamPosition();
    if (uBeamPosition != 0xFFFFFFFF) {
        nErrorCode = DrawBeam(pRenderer,MainMenu.uXoffs + FONT_W, MainMenu.uYoffs + 608 + FONT_H * uBeamPosition, Config.uResX - 2 * FONT_W, FONT_H, 0x20,0x20,0xF0,0xC0);
        if ((InputStates.bLeftMouseButton) && (nErrorCode == 0)) {
            if (SelectAlternativeLevelgroup(LevelgroupFiles[uBeamPosition].uMd5Hash) == 0) {
                SDL_Log("Select %s, OK",SelectedLevelgroup.szLevelgroupname);
                if (Actualplayer.bValid) {  // Der aktuelle Name wird nochmals ausgewählt, damit dieser ggf. den Levelgruppen-Hash bekommt
                    strcpy(szPlayername,Actualplayer.szPlayername); // Muss kopiert werden, da Selectname die Struktur Actualplayer.x löscht
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
Beschreibung: Erledigt die Auswahl eines Namens aus dem Hauptmenü heraus und
              blendet die Balken für die Auswahl eines Namens an, wenn sich
              der Mauspfeil über einem Namen befindet.

Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: MainMenu.x, Names.x
------------------------------------------------------------------------------*/
int MenuSelectName(SDL_Renderer *pRenderer) {
    int nErrorCode = 0;
    uint32_t uBeamPosition;

    uBeamPosition = GetNamesBeamPosition();
    if (uBeamPosition != 0xFFFFFFFF) {
        nErrorCode = DrawBeam(pRenderer,MainMenu.uXoffs + FONT_W, MainMenu.uYoffs + 448 + FONT_H * uBeamPosition, Config.uResX - 7 * FONT_W, FONT_H, 0x20,0x20,0xF0,0xC0);
        if ((InputStates.bLeftMouseButton) && (nErrorCode == 0)) {
            if (SelectName(Names.Name[uBeamPosition].szName,SelectedLevelgroup.uMd5Hash) == 0) {
                SDL_Log("select name: %s, OK",Names.Name[uBeamPosition].szName);
                strcpy(Config.szPlayername,Names.Name[uBeamPosition].szName);
                memcpy(Config.uLevelgroupMd5Hash,SelectedLevelgroup.uMd5Hash,16);
                nErrorCode = WriteConfigFile();
            } else {
                SDL_Log("select name: %s, ERROR",Names.Name[uBeamPosition].szName);
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
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: MainMenu.x
------------------------------------------------------------------------------*/
void InitLists(void) {
    uint32_t I;

    // Levelgruppenliste vorbereiten
    memset(MainMenu.uLevelgroupList,0xFF,sizeof(MainMenu.uLevelgroupList));
    for (I = 0; (I < 4) && (I < g_LevelgroupFilesCount); I++) {
        MainMenu.uLevelgroupList[I] = I;
    }
    // Namensliste vorbereiten
    memset(MainMenu.uNamesList,0xFF,sizeof(MainMenu.uNamesList));
    for (I = 0; (I < 4) && (I < Names.uNameCount); I++) {
        MainMenu.uNamesList[I] = I;
    }
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

Rückgabewert:  -
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
    memcpy(MainMenu.uFlashBrightness,"\x00\10\x14\x19\x1E\x28\x32\x4B\x5A\x5F\x64\x5F\x5A\x4B\x32\x28\x1E\x19\x14\x0A",sizeof(MainMenu.uFlashBrightness));// Für den Stein-Cursor
}


/*----------------------------------------------------------------------------
Name:           ScrolllevelGroups
------------------------------------------------------------------------------
Beschreibung: Alle Menü-Elemente im Menü-Screen uMenuScreen werden in den Renderer geschrieben.
Parameter
      Eingang: -
      Ausgang: -

Rückgabewert:  -
Seiteneffekte: InputStates.x, MainMenu.x, g_LevelgroupFilesCount
------------------------------------------------------------------------------*/
void ScrollLevelGroups(void) {
    uint32_t I;

    if (g_LevelgroupFilesCount > 5) {
        if (InputStates.bLeftMouseButton) {
            // Button Levelgruppen Pfeil hoch
            if  ((InputStates.nMouseXpos >= (896 + MainMenu.uXoffs)) && (InputStates.nMouseXpos < (896 + FONT_W + MainMenu.uXoffs))) {
                if ( (InputStates.nMouseYpos >= (608 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (608 + FONT_H + MainMenu.uYoffs))) {
                    if (MainMenu.uLevelgroupList[0] > 0) {
                        for (I = 0; I < 4; I++) {
                            if (MainMenu.uLevelgroupList[I] != 0xFF) {
                                MainMenu.uLevelgroupList[I]--;
                            }
                        }
                    }
                    WaitNoKey();
                } else if ( (InputStates.nMouseYpos >= (704 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (704 + FONT_H + MainMenu.uYoffs))) {
                    if (MainMenu.uLevelgroupList[3] < (g_LevelgroupFilesCount - 1)) {
                        for (I = 0; I < 4; I++) {
                            if (MainMenu.uLevelgroupList[I] != 0xFF) {
                                MainMenu.uLevelgroupList[I]++;
                            }
                        }
                    }
                    WaitNoKey();
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           DrawBeam
------------------------------------------------------------------------------
Beschreibung: Zeichnet ein Rechteck / einen Balken für die Levelgruppen- und Namen-Auswahl.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               uXpos, uint32_t, X-Position für Balken
               uYpos, uint32_t, X-Position für Balken
               uWidth, uint32_t, Breite des Balkens
               uHeight, uint32_t, Höhe des Balkens
               uRed, uint8_t, Rot-Anteil für Farbe des Balkens
               uGreen, uint8_t, Grün-Anteil für Farbe des Balkens
               uBlue, uint8_t, Blau-Anteil für Farbe des Balkens
               uTransp, uint8_t, Transparenz der Farbe
      Ausgang: -

Rückgabewert:  int, 0 = Alles OK, sonst Fehler
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
    if (SDL_SetRenderDrawColor(pRenderer,uRed,uGreen,uBlue,uTransp) == 0) {   // dunkelblaue, halbtransparente Fensterfläche
        if (SDL_RenderFillRect(pRenderer,&DestR) == 0) {
            nErrorCode = SDL_SetRenderDrawColor(pRenderer,0,0,0,SDL_ALPHA_OPAQUE);
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           RenderMenuElements
------------------------------------------------------------------------------
Beschreibung: Alle Menü-Elemente im Menü-Screen uMenuScreen werden in den Renderer geschrieben.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -

Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Playfield.x, Mainmenu.x
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
Beschreibung: Ermittelt, ob und wo ein Balken eingeblendet werden muss, wenn der Mauspfeil sich über
              der Liste der Spieler-Namen befindet.
Parameter
      Eingang: -
      Ausgang: -

Rückgabewert:  uint32_t, 0xFFFFFFFF = keine Einblendung, sonst 0 bis 3 für einen der 4 Spieler Namen
Seiteneffekte: InputStates.x, MainMenu.x
------------------------------------------------------------------------------*/
uint32_t GetNamesBeamPosition(void) {
    uint32_t uBeamPosition = 0xFFFFFFFF;

    // Prüfen, ob Mauspfeil über Spielernamen-Liste steht und ggf. Balken einblenden
    if ((InputStates.nMouseXpos >= (FONT_W + MainMenu.uXoffs)) && (InputStates.nMouseXpos < (1088 + MainMenu.uXoffs))) {
        if ((InputStates.nMouseYpos >= (448 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (480 + MainMenu.uYoffs))) {
            if (MainMenu.uNamesList[0] != 0xFF) {
                uBeamPosition = 0;
            }
        } else if ((InputStates.nMouseYpos >= (480 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (512 + MainMenu.uYoffs))) {
            if (MainMenu.uNamesList[1] != 0xFF) {
                uBeamPosition = 1;
            }
        } else if ((InputStates.nMouseYpos >= (512 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (544 + MainMenu.uYoffs))) {
            if (MainMenu.uNamesList[2] != 0xFF) {
                uBeamPosition = 2;
            }
        } else if ((InputStates.nMouseYpos >= (544 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (576 + MainMenu.uYoffs))) {
            if (MainMenu.uNamesList[3] != 0xFF) {
                uBeamPosition = 3;
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

Rückgabewert:  uint32_t, 0xFFFFFFFF = keine Einblendung, sonst 0 bis 3 für einen der 4 Levelgruppen-Namen
Seiteneffekte: InputStates.x, MainMenu.x
------------------------------------------------------------------------------*/
uint32_t GetLevelgroupBeamPosition(void) {
    uint32_t uBeamPosition = 0xFFFFFFFF;

    // Prüfen, ob Mauspfeil über Levelgruppenliste steht und ggf. Balken einblenden
    if ((InputStates.nMouseXpos >= (FONT_W + MainMenu.uXoffs)) && (InputStates.nMouseXpos < (896 + MainMenu.uXoffs))) {
        if ((InputStates.nMouseYpos >= (608 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (640 + MainMenu.uYoffs))) {
            if (MainMenu.uLevelgroupList[0] != 0xFF) {
                uBeamPosition = 0;
            }
        } else if ((InputStates.nMouseYpos >= (640 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (672 + MainMenu.uYoffs))) {
            if (MainMenu.uLevelgroupList[1] != 0xFF) {
                uBeamPosition = 1;
            }
        } else if ((InputStates.nMouseYpos >= (672 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (704 + MainMenu.uYoffs))) {
            if (MainMenu.uLevelgroupList[2] != 0xFF) {
                uBeamPosition = 2;
            }
        } else if ((InputStates.nMouseYpos >= (704 + MainMenu.uYoffs)) && (InputStates.nMouseYpos < (736 + MainMenu.uYoffs))) {
            if (MainMenu.uLevelgroupList[3] != 0xFF) {
                uBeamPosition = 3;
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
Beschreibung: Setzt statische Elemente für das Emerald-Mine-Hauptmenü.

Parameter
      Eingang: -
      Ausgang: -

Rückgabewert:  -
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
    // Button-Untergrund
    MainMenu.uMenuScreen[13 * MainMenu.uXdim + 75] = EMERALD_MAGIC_WALL;
    MainMenu.uMenuScreen[13 * MainMenu.uXdim + 76] = EMERALD_MAGIC_WALL;
    MainMenu.uMenuScreen[13 * MainMenu.uXdim + 77] = EMERALD_MAGIC_WALL;
    MainMenu.uMenuScreen[13 * MainMenu.uXdim + 78] = EMERALD_MAGIC_WALL;
/*
    SetMenuText(MainMenu.uMenuScreen,"START",6,18,EMERALD_FONT_BLUE);
*/
}


/*----------------------------------------------------------------------------
Name:           SetDynamicMenuElements
------------------------------------------------------------------------------
Beschreibung: Setzt dynamische Elemente für das Emerald-Mine-Hauptmenü.

Parameter
      Eingang: -
      Ausgang: -

Rückgabewert:  int, 0 = Alles OK, sonst Fehler
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
    // Hier die Pfeile der Levelgruppenauswahl, wenn mehr als 4 Levelgruppen verfügbar sind
    if (g_LevelgroupFilesCount > 4) {
        MainMenu.uMenuScreen[19 * MainMenu.uXdim + 28] = EMERALD_STEEL_ARROW_UP;
        MainMenu.uMenuScreen[22 * MainMenu.uXdim + 28] = EMERALD_STEEL_ARROW_DOWN;
    } else {
        MainMenu.uMenuScreen[19 * MainMenu.uXdim + 28] = EMERALD_STEEL;
        MainMenu.uMenuScreen[22 * MainMenu.uXdim + 28] = EMERALD_STEEL;
    }
    // Zeigt bis zu 4 Namen im mittleren Bereich an
    //SetMenuText(MainMenu.uMenuScreen,"1234567890123456789012345678901",2,13,EMERALD_FONT_BLUE);
    for (I = 0; I < 4; I++) {
        if (MainMenu.uNamesList[I] != 0xFF) {
            SetMenuText(MainMenu.uMenuScreen,"                               ",2,14 + I,EMERALD_FONT_BLUE);
            SetMenuText(MainMenu.uMenuScreen,Names.Name[I].szName,2,14 + I,EMERALD_FONT_BLUE);
        }
    }
    if (Names.uNameCount > 4) {
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
    SetMenuText(MainMenu.uMenuScreen,"                               ",7,5,EMERALD_FONT_GREEN); // ausgewählter Playername

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
            } else {
                SetMenuText(MainMenu.uMenuScreen,"PLEASE SELECT PLAYER",7,5,EMERALD_FONT_GREEN);
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
            // Eingabe eines Zeichens für den Spielernamen
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
            SetMenuText(MainMenu.uMenuScreen,"                                   ",7,5,EMERALD_FONT_GREEN); // löscht auch alte Cursorposition
            SetMenuText(MainMenu.uMenuScreen,MainMenu.szTempName,7,5,EMERALD_FONT_GREEN);
            MainMenu.uMenuScreen[5 * MainMenu.uXdim + 7 + MainMenu.uCursorPos] = EMERALD_STONE;    // Stein als Cursor setzen
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
               SelectedLevelgroup.x, Config.x
------------------------------------------------------------------------------*/
int EmeraldMineMainMenu(SDL_Renderer *pRenderer) {
    int nErrorCode;
    AUDIOPLAYER Audioplayer;
    SDL_AudioDeviceID audio_device;
    uint32_t uModVolume;
    bool bMenuRun;
    bool bEndGame;
    bool bPrepareExit;
    int nColorDimm;
    int nLevelButton;
    int nLastLevelButton;
    char szText[256];

    InitMainMenu();
    // Namen einlesen
    if (ReadNamesFile() != 0) {
        return -1;
    }
    memset(&Audioplayer,0,sizeof(AUDIOPLAYER));
    // Audiostruktur initialisieren
    InitAudioplayerStruct(&Audioplayer,6);      // MOD 6, 2kad04
    // Modfile initialisieren
    if (!InitMOD(Audioplayer.pTheMusic, Audioplayer.sdl_audio.freq)) {
        SDL_Log("%s: invalid mod file, data size: %d",__FUNCTION__,Audioplayer.nMusicSize);
        return -1;
    }
    audio_device = SDL_OpenAudioDevice(NULL, 0, &Audioplayer.sdl_audio, NULL, 0);
    if (audio_device == 0) {
        SDL_Log("%s: SDL_OpenAudioDevice() failed: %s",__FUNCTION__,SDL_GetError());
        return -1;
    }
    if (CreateButton(BUTTONLABEL_CREATE_PLAYER,"Create Player",MainMenu.uXoffs + 1127,MainMenu.uYoffs + 453,true,true) != 0) {
        return -1;
    }
    SDL_PauseAudioDevice(audio_device, 0);
    SetStaticMenuElements();
    uModVolume = 0;
    SetModVolume(uModVolume);
    nErrorCode = 0;
    nColorDimm = 0;
    bPrepareExit = false;
    bMenuRun = true;
    bEndGame = false;
    nLevelButton = 0;   // 0 = nicht gedrückt, 1 = down, 2 = up
    nLastLevelButton = 0;
    if (GetLevelgroupFiles() == 0) {    // Wenn das nicht funktioniert, kann nicht weitergemacht werden!
        InitLists();
        // ShowAvailableLevelgroups();
        if (SelectAlternativeLevelgroup(Config.uLevelgroupMd5Hash) == 0) {
            // ShowSelectedLevelgroup();
            if (SelectName(Config.szPlayername,SelectedLevelgroup.uMd5Hash) != 0) {
                // Falls letzter Name nicht mehr auswählbar ist, diesen aus dem Konfigurationsfile löschen
                memset(Config.szPlayername,0,sizeof(Config.szPlayername));
                nErrorCode = WriteConfigFile();
            }
            while ((bMenuRun) && (nErrorCode == 0)) {
                UpdateManKey(); // Ruft UpdateInputStates() auf
                if ((InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) && (nColorDimm == 100) && (MainMenu.nState == 0)) {
                    bPrepareExit = true;
                    bEndGame = true;
                } else if ( (Actualplayer.bValid) && (SelectedLevelgroup.bOK) && ((ManKey.bFire) || (InputStates.pKeyboardArray[SDL_SCANCODE_SPACE])) ) {
                    bPrepareExit = true;
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
                        } else if (nErrorCode == 0) {
                            // Ein erster Entwurf für Emerald Mine. Das Spielergebnis (Erfolg oder Versagen) kann in Playfield.x abgefragt werden.
                            SDL_Log("Start Game with level %u",Actualplayer.uLevel);
                            nErrorCode = RunGame(pRenderer,Actualplayer.uLevel);
                            SDL_Log("%s: RunGame() ErrorCode: %u",__FUNCTION__,nErrorCode);
                            if (nErrorCode == 0) {
                                nErrorCode = EvaluateGame();
                            }
                            SetStaticMenuElements();
                            bPrepareExit = false;
                            nColorDimm = 0;
                            uModVolume = 0;
                            SetAllTextureColors(nColorDimm);
                            SetModVolume(uModVolume);
                            InitAudioplayerStruct(&Audioplayer,6);      // MOD 6, 2kad04
                            // Modfile initialisieren
                            if (!InitMOD(Audioplayer.pTheMusic, Audioplayer.sdl_audio.freq)) {
                                SDL_Log("%s: invalid mod file, data size: %d",__FUNCTION__,Audioplayer.nMusicSize);
                                return -1;
                            }
                            WaitNoKey();
                        }
                    }
                }
                // printf("x:%u  y:%u\n",InputStates.nMouseXpos,InputStates.nMouseYpos);
                nErrorCode = MenuSelectLevelgroup(pRenderer);
                if (nErrorCode == 0) {
                    nErrorCode = MenuSelectName(pRenderer);
                }
                SetMenuText(MainMenu.uMenuScreen,"                         ",13,4,EMERALD_FONT_GREEN);
                SetMenuText(MainMenu.uMenuScreen,SelectedLevelgroup.szLevelgroupname,13,4,EMERALD_FONT_GREEN);
                ScrollLevelGroups();
                SetDynamicMenuElements();


                nLastLevelButton = nLevelButton;
                nLevelButton = GetLevelButton();
                if ((nLastLevelButton == 0) && (Actualplayer.bValid)) {
                    if (nLevelButton == 1) {
                        if (Actualplayer.uLevel > 0) {
                            Actualplayer.uLevel--;
                        }
                    } else if (nLevelButton == 2) {
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
                    nColorDimm++;
                    SetAllTextureColors(nColorDimm);
                    uModVolume++;
                    SetModVolume(uModVolume);
                }

                if (InputStates.pKeyboardArray[SDL_SCANCODE_A]) { //raus
                    ShowActualPlayer();

                }

                if (SDL_GetQueuedAudioSize(audio_device) < (Audioplayer.sdl_audio.samples * 4)) {
                    RenderMOD(Audioplayer.audiobuffer, Audioplayer.sdl_audio.samples);
                    SDL_QueueAudio(audio_device,Audioplayer. audiobuffer, Audioplayer.sdl_audio.samples * 4); // 2 channels, 2 bytes/sample
                }

                if (nErrorCode == 0) {
                    nErrorCode = RenderMenuElements(pRenderer);
                }
                ShowButtons(pRenderer);
                if ((MainMenu.nState == 0) && (IsButtonPressed(BUTTONLABEL_CREATE_PLAYER))) {
                    ActivateInputPlayernameMode();  // Eingabemodus für Namenseingabe aktivieren
                }
                SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
                SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
                Playfield.uFrameCounter++;
            }
        } else {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Levelgroup problem","Can not select levelgroup!",NULL);
            nErrorCode = -1;
        }
    }
    SetAllTextureColors(100);
    SAFE_FREE(Audioplayer.pTheMusic);
    FreeButton(BUTTONLABEL_CREATE_PLAYER);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ShowAuthorAndLevelname
------------------------------------------------------------------------------
Beschreibung: Zeigt vor dem Spiel den Level-Author und den Levelnamen.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               uLevel, uint32_t, Levelnummer

Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
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
    SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
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

    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 2] = EMERALD_EMERALD;
    SetMenuText(MainMenu.uMenuScreen,"PRESS FIRE (LEFT CTRL) TO PLAY",4,14,EMERALD_FONT_BLUE);
    MainMenu.uMenuScreen[16 * MainMenu.uXdim + 2] = EMERALD_RUBY;
    SetMenuText(MainMenu.uMenuScreen,"PRESS ESC TO CANCEL",4,16,EMERALD_FONT_BLUE);
    MainMenu.uMenuScreen[18 * MainMenu.uXdim + 2] = EMERALD_SAPPHIRE;
    SetMenuText(MainMenu.uMenuScreen,"PRESS H FOR HIGHSCORES",4,18,EMERALD_FONT_BLUE);

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
            } else if (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) {
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
        SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
        SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
        Playfield.uFrameCounter++;
    } while (bMenuRun);
    if ((nErrorCode == 0) && (!bPlayGame)) {
        nErrorCode = 5;
    }
    WaitNoKey();
    return nErrorCode;
}
