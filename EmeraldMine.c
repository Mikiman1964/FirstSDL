/*
TODO
* doppeltes Rollen ("tanzen") der Elemente vermeiden, wenn diese nicht auf Laufband liegen
* Explosion
    * Explosionen mit Sumpf testen (erster Test sieht gut aus)
* Leveleditor
    * Undo für Editor
* Default-Level-Group: Level 'Shaft' entfernen/anpassen
*/

/**
    News:
    * Teleporter
    * Remote Bomb
    * Hiscores: Won games marked
    * Explosion-Fixes
    * new level
**/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include "alien.h"
#include "beetle.h"
#include "bomb.h"
#include "buttons_checkboxes.h"
#include "crystal.h"
#include "EmeraldMine.h"
#include "EmeraldMineMainMenu.h"
#include "editor.h"
#include "emerald.h"
#include "explosion.h"
#include "GetTextureIndexByElement.h"
#include "greendrop.h"
#include "highscores.h"
#include "KeyboardMouse.h"
#include "levelconverter.h"
#include "loadlevel.h"
#include "lightbarrier.h"
#include "magicwall.h"
#include "man.h"
#include "megabomb.h"
#include "mine.h"
#include "modplay.h"
#include "mole.h"
#include "mySDL.h"
#include "mystd.h"
#include "nut.h"
#include "panel.h"
#include "perl.h"
#include "RenderLevel.h"
#include "replicator.h"
#include "ruby.h"
#include "sand.h"
#include "saphir.h"
#include "smileys.h"
#include "sound.h"
#include "steel_wall_grow.h"
#include "stone.h"
#include "teleporter.h"
#include "yam.h"


PLAYFIELD Playfield;
extern GAMESOUND GameSound;
extern INPUTSTATES InputStates;
extern MANKEY ManKey;
extern uint8_t ge_uLevel[];
extern uint8_t ge_uBeamColors[];
extern uint8_t g_uIntensityProzent;
extern SMILEY Smileys[MAX_SMILEYS];
extern LEVELGROUP SelectedLevelgroup;
extern MAINMENU MainMenu;
extern AUDIOPLAYER Audioplayer;
extern uint32_t ge_uXoffs;             // X-Offset für die Zentrierung von Elementen
extern uint32_t ge_uYoffs;             // X-Offset für die Zentrierung von Elementen

/*----------------------------------------------------------------------------
Name:           Menu
------------------------------------------------------------------------------
Beschreibung: Hauptmenü, um den entsprechenden SDL2-Programmteil aufzurufen.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int, 0 = Level-Editor, 1 = Game, 2 = SDL2-Demo, 3 = Quit
Seiteneffekte: Playfield.x für FrameCounter, Audioplayer.x, MainMenu.x,
               ge_uXoffs, ge_uYoffs
------------------------------------------------------------------------------*/
int Menu(SDL_Renderer *pRenderer) {
    uint32_t uModVolume;
    int nErrorCode;
    int nChoose;
    int nColorDimm;
    float fAngle;
    SDL_Rect DestR;
    uint32_t uFrameCounter;
    uint32_t I;
    uint32_t uTextureIndex;
    uint32_t uPositionsAndElements[] = {// 1. Zeile
        352,64,EMERALD_FONT_W,384,64,EMERALD_FONT_E,416,64,EMERALD_FONT_L,448,64,EMERALD_FONT_C,480,64,EMERALD_FONT_O,512,64,EMERALD_FONT_M,544,64,EMERALD_FONT_E,
        608,64,EMERALD_FONT_T,640,64,EMERALD_FONT_O,
        80,128,EMERALD_FONT_APOSTROPHE,112,128,EMERALD_FONT_M,144,128,EMERALD_FONT_Y,208,128,EMERALD_FONT_F,240,128,EMERALD_FONT_I,272,128,EMERALD_FONT_R,304,128,EMERALD_FONT_S,
        336,128,EMERALD_FONT_T,400,128,EMERALD_FONT_S,432,128,EMERALD_FONT_D,464,128,EMERALD_FONT_L,496,128,EMERALD_FONT_2,560,128,EMERALD_FONT_A,592,128,EMERALD_FONT_P,
        624,128,EMERALD_FONT_P,656,128,EMERALD_FONT_L,688,128,EMERALD_FONT_I,720,128,EMERALD_FONT_C,752,128,EMERALD_FONT_A,784,128,EMERALD_FONT_T,816,128,EMERALD_FONT_I,
        848,128,EMERALD_FONT_O,880,128,EMERALD_FONT_N,912,128,EMERALD_FONT_APOSTROPHE,
        // Obere Stahl-Zeile
        -128,0,EMERALD_STEEL,-96,0,EMERALD_STEEL,-64,0,EMERALD_STEEL,-32,0,EMERALD_STEEL,0,0,EMERALD_STEEL,32,0,EMERALD_STEEL,64,0,EMERALD_STEEL,96,0,EMERALD_STEEL,128,0,EMERALD_STEEL,160,0,EMERALD_STEEL,192,0,EMERALD_STEEL,224,0,EMERALD_STEEL,256,0,EMERALD_STEEL,
        288,0,EMERALD_STEEL,320,0,EMERALD_STEEL,352,0,EMERALD_STEEL,384,0,EMERALD_STEEL,416,0,EMERALD_STEEL,448,0,EMERALD_STEEL,480,0,EMERALD_STEEL,512,0,EMERALD_STEEL,544,0,EMERALD_STEEL,
        576,0,EMERALD_STEEL,608,0,EMERALD_STEEL,640,0,EMERALD_STEEL,672,0,EMERALD_STEEL,704,0,EMERALD_STEEL,736,0,EMERALD_STEEL,768,0,EMERALD_STEEL,800,0,EMERALD_STEEL,832,0,EMERALD_STEEL,
        864,0,EMERALD_STEEL,896,0,EMERALD_STEEL,928,0,EMERALD_STEEL,960,0,EMERALD_STEEL,992,0,EMERALD_STEEL,1024,0,EMERALD_STEEL,1056,0,EMERALD_STEEL,1088,0,EMERALD_STEEL,1120,0,EMERALD_STEEL,
        // Untere Stahl-Zeile
        -128,736,EMERALD_STEEL,-96,736,EMERALD_STEEL,-64,736,EMERALD_STEEL,-32,736,EMERALD_STEEL,0,736,EMERALD_STEEL,32,736,EMERALD_STEEL,64,736,EMERALD_STEEL,96,736,EMERALD_STEEL,128,736,EMERALD_STEEL,160,736,EMERALD_STEEL,192,736,EMERALD_STEEL,224,736,EMERALD_STEEL,
        256,736,EMERALD_STEEL,288,736,EMERALD_STEEL,320,736,EMERALD_STEEL,352,736,EMERALD_STEEL,384,736,EMERALD_STEEL,416,736,EMERALD_STEEL,448,736,EMERALD_STEEL,480,736,EMERALD_STEEL,
        512,736,EMERALD_STEEL,544,736,EMERALD_STEEL,576,736,EMERALD_STEEL,608,736,EMERALD_STEEL,640,736,EMERALD_STEEL,672,736,EMERALD_STEEL,704,736,EMERALD_STEEL,736,736,EMERALD_STEEL,
        768,736,EMERALD_STEEL,800,736,EMERALD_STEEL,832,736,EMERALD_STEEL,864,736,EMERALD_STEEL,896,736,EMERALD_STEEL,928,736,EMERALD_STEEL,960,736,EMERALD_STEEL,992,736,EMERALD_STEEL,
        1024,736,EMERALD_STEEL,1056,736,EMERALD_STEEL,1088,736,EMERALD_STEEL,1120,736,EMERALD_STEEL,
        // Linke Stahl-Zeile
        -128,32,EMERALD_STEEL,-128,64,EMERALD_STEEL,-128,96,EMERALD_STEEL,-128,128,EMERALD_STEEL,-128,160,EMERALD_STEEL,-128,192,EMERALD_STEEL,-128,224,EMERALD_STEEL,-128,256,EMERALD_STEEL,-128,288,EMERALD_STEEL,
        -128,320,EMERALD_STEEL,-128,352,EMERALD_STEEL,-128,384,EMERALD_STEEL,-128,416,EMERALD_STEEL,-128,448,EMERALD_STEEL,-128,480,EMERALD_STEEL,-128,512,EMERALD_STEEL,-128,544,EMERALD_STEEL,-128,576,EMERALD_STEEL,
        -128,608,EMERALD_STEEL,-128,640,EMERALD_STEEL,-128,672,EMERALD_STEEL,-128,704,EMERALD_STEEL,
        // rechte Stahl-Zeile
        1120,32,EMERALD_STEEL,1120,64,EMERALD_STEEL,1120,96,EMERALD_STEEL,1120,128,EMERALD_STEEL,1120,160,EMERALD_STEEL,1120,192,EMERALD_STEEL,1120,224,EMERALD_STEEL,1120,256,EMERALD_STEEL,
        1120,288,EMERALD_STEEL,1120,320,EMERALD_STEEL,1120,352,EMERALD_STEEL,1120,384,EMERALD_STEEL,1120,416,EMERALD_STEEL,1120,448,EMERALD_STEEL,1120,480,EMERALD_STEEL,1120,512,EMERALD_STEEL,
        1120,544,EMERALD_STEEL,1120,576,EMERALD_STEEL,1120,608,EMERALD_STEEL,1120,640,EMERALD_STEEL,1120,672,EMERALD_STEEL,1120,704,EMERALD_STEEL,
        // "Menüpunkte"
        128,224,EMERALD_RUBY,128,288,EMERALD_SAPPHIRE,128,576,EMERALD_STEEL_EXIT,
        // "PROGRAMMED IN 2023"
        144,672,EMERALD_FONT_P,176,672,EMERALD_FONT_R,208,672,EMERALD_FONT_O,240,672,EMERALD_FONT_G,272,672,EMERALD_FONT_R,304,672,EMERALD_FONT_A,336,672,EMERALD_FONT_M,368,672,EMERALD_FONT_M,
        400,672,EMERALD_FONT_E,432,672,EMERALD_FONT_D,496,672,EMERALD_FONT_I,528,672,EMERALD_FONT_N,592,672,EMERALD_FONT_2,624,672,EMERALD_FONT_0,656,672,EMERALD_FONT_2,688,672,EMERALD_FONT_2,
        720,672,EMERALD_FONT_MINUS,752,672,EMERALD_FONT_2,784,672,EMERALD_FONT_0,816,672,EMERALD_FONT_2,848,672,EMERALD_FONT_3
    };

    InitSmileys();
    uFrameCounter = 0;
    nErrorCode = 0;
    nChoose = -1;
    // Buttons erzeugen
    nErrorCode = nErrorCode + CreateButton(BUTTONLABEL_CALL_GAME,"Try the game",320,228,true,false);
    nErrorCode = nErrorCode + CreateButton(BUTTONLABEL_CALL_DEMO,"SDL2 Demo",320,292,true,false);
    nErrorCode = nErrorCode + CreateButton(BUTTONLABEL_CALL_QUIT,"Quit program",320,580,true,false);
    nErrorCode = nErrorCode + SetModMusic(5);
    SDL_PauseAudioDevice(Audioplayer.audio_device, 0);
    uModVolume = 0;
    SetModVolume(uModVolume);
    nColorDimm = 0;
    SetAllTextureColors(nColorDimm);
    while (((nErrorCode == 0) && (nChoose == -1)) || (nColorDimm > 0) ) {
        MoveSmileys(pRenderer);
        UpdateInputStates();
        if ((nChoose == -1) && (nColorDimm < 100)) {
            nColorDimm++;
            SetAllTextureColors(nColorDimm);
            uModVolume++;
            SetModVolume(uModVolume);
        }
        PlayMusic(true);
        for (I = 0; (I < sizeof(uPositionsAndElements) / (sizeof(uint32_t) * 3)) && (nErrorCode == 0); I++) {
            uTextureIndex = GetTextureIndexByElement(uPositionsAndElements[I * 3 + 2],uFrameCounter % 16,&fAngle);
            DestR.x = 128 + ge_uXoffs + uPositionsAndElements[I * 3 + 0];
            DestR.y = ge_uYoffs + uPositionsAndElements[I * 3 + 1];
            DestR.w = FONT_W;
            DestR.h = FONT_H;
            if (nErrorCode == 0) {
                if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle,NULL, SDL_FLIP_NONE) != 0) {
                    nErrorCode = -1;
                    SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                }
            }
        }
        PrintLittleFont(pRenderer,448,232,0,"LEFT CONTROL = FIRE, FIRE CAN BE USED WITH CURSOR DIRECTION.",K_RELATIVE);
        PrintLittleFont(pRenderer,448,296,0,"USE THE FOLLOWING KEYS:",K_RELATIVE);
        PrintLittleFont(pRenderer,468,311,0,"* ESC OR LEFT MOUSEBUTTON TO EXIT",K_RELATIVE);
        PrintLittleFont(pRenderer,468,326,0,"* MOUSEWHEEL TO ZOOM BALLONS",K_RELATIVE);
        PrintLittleFont(pRenderer,468,341,0,"* 'A' / 'B' TO TOGGLE TEXTURE FOR BALLONS, ASTEROIDS AND SMILEYS",K_RELATIVE);
        PrintLittleFont(pRenderer,468,356,0,"* 'D' TO TOGGLE 'DRUNKEN ASTEROIDS' MODE",K_RELATIVE);
        PrintLittleFont(pRenderer,468,371,0,"* '+' / '-' ON KEYPAD TO CHANGE MUSIC VOLUME",K_RELATIVE);
        PrintLittleFont(pRenderer,468,386,0,"* '1' FOR MUSIC 1 -> ECHOING BY BANANA (CHRISTOF M\x63HLAN), 1988",K_RELATIVE);
        PrintLittleFont(pRenderer,468,401,0,"* '2' FOR MUSIC 2 -> CIRCUS TIME 2 VOYCE/DELIGHT, 1993",K_RELATIVE);
        PrintLittleFont(pRenderer,468,416,0,"* '3' FOR MUSIC 3 -> CLASS01 BY MAKTONE (MARTIN NORDELL), 1999",K_RELATIVE);
        PrintLittleFont(pRenderer,468,431,0,"* '4' FOR MUSIC 4 -> GLOBAL TRASH 3 V2 BY JESPER KYD, 1991",K_RELATIVE);
        PrintLittleFont(pRenderer,468,446,0,"* '5' FOR MUSIC 5 -> CLASS11.TIME FLIES BY MAKTONE",K_RELATIVE);
        PrintLittleFont(pRenderer,468,461,0,"* '6' FOR MUSIC 6 -> 2000AD:CRACKTRO:IV BY MAKTONE",K_RELATIVE);
        PrintLittleFont(pRenderer,468,476,0,"* '7' FOR MUSIC 7 -> 2000AD:CRACKTRO02 BY MAKTONE",K_RELATIVE);
        PrintLittleFont(pRenderer,468,491,0,"* '8' FOR MUSIC 8 -> BREWERY BY MAKTONE",K_RELATIVE);
        PrintLittleFont(pRenderer,468,506,0,"* '9' FOR MUSIC 9 -> CLASS05 BY MAKTONE, 1999",K_RELATIVE);
        PrintLittleFont(pRenderer,468,521,0,"* '0' FOR MUSIC 0 -> SOFTWORLD BY OXYGENER/MAKTONE",K_RELATIVE);
        PrintLittleFont(pRenderer,448,584,0,"NUFF SAID",K_RELATIVE);
        nErrorCode = ShowButtons(pRenderer);
        if (IsButtonPressed(BUTTONLABEL_CALL_GAME)) {
            nChoose = 1;
        } else if (IsButtonPressed(BUTTONLABEL_CALL_DEMO)) {
            nChoose = 2;
        } else if ((IsButtonPressed(BUTTONLABEL_CALL_QUIT)) || (InputStates.bQuit) || (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE])) {
            nChoose = 3;
        }
        if ((nChoose != -1) && (nColorDimm > 0)) {
            nColorDimm--;
            SetAllTextureColors(nColorDimm);
            uModVolume--;
            SetModVolume(uModVolume);
        }
        SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
        SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
        uFrameCounter++;
        Playfield.uFrameCounter++;
    }
    // Buttons freigeben
    FreeButton(BUTTONLABEL_CALL_GAME);
    FreeButton(BUTTONLABEL_CALL_DEMO);
    FreeButton(BUTTONLABEL_CALL_QUIT);
    SetAllTextureColors(100);
    return nChoose;
}


/*----------------------------------------------------------------------------
Name:           RunGame
------------------------------------------------------------------------------
Beschreibung: Hauptschleifen-Funktion für das Spielen eines Levels.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               uLevel, uint32_t, Levelnummer
      Ausgang: -

Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: Playfield.x, InputStates.x, ManKey.x, GameSound.x,
------------------------------------------------------------------------------*/
int RunGame(SDL_Renderer *pRenderer, uint32_t uLevel) {
    bool bLevelRun;
    bool bPrepareLevelExit;
    bool bPause;
    int nColorDimm;
    int nCheckLevelCount;
    uint32_t uManDirection = EMERALD_ANIM_STAND;     // Rückgabe von CheckLevel() -> Wohin ist der Man gelaufen?
    uint32_t uKey;
    uint32_t I;
    bool bDimmIn = true;
    bool bDebug = false;
    uint32_t uQuitTime;
    int nRet;

    bPause = false;
    nRet = 0;
    nColorDimm = 0;
    FillCheeseRandomNumbers();
    bLevelRun = (InitialisePlayfield(uLevel) == 0);
    // Renderer mit schwarz löschen
    SDL_SetRenderDrawColor(pRenderer,0,0,0,SDL_ALPHA_OPAQUE);
    SDL_RenderClear(pRenderer);
    SDL_RenderPresent(pRenderer);
    bPrepareLevelExit = false;
    if (bLevelRun) {
        nRet = ShowAuthorAndLevelname(pRenderer,uLevel);
        if (nRet < 0) {
            bLevelRun = false; // Ein Fehler ist aufgetreten
        } else if (nRet > 0) {  // Abbruch durch ESC
            bPrepareLevelExit = true;
        }
    }
    SetAllTextureColors(0);
    Playfield.uPlayTimeStart = SDL_GetTicks();
    uQuitTime = 0xFFFFFFFF;
    nCheckLevelCount = 0;
    ManKey.uLastActiveDirection = MANKEY_NONE;
    ManKey.uLastDirectionFrameCount = 0;
    ManKey.uLastFireFrameCount = 0;
    ManKey.uFireCount = 0;
    ManKey.bExit = false;
    SDL_ShowCursor(SDL_DISABLE);    // Mauspfeil verstecken
    while (bLevelRun) {
        UpdateManKey();
        if ((InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) || (InputStates.bQuit) || (ManKey.bExit)) {
            bPrepareLevelExit = true;
            ManKey.bExit = false; // hier bestätigen
        }
        if ((Playfield.bManDead) || (Playfield.bWellDone)) {
            if (uQuitTime == 0xFFFFFFFF) {
                uQuitTime = SDL_GetTicks();
            }
            if ((uQuitTime != 0xFFFFFFFF) && ((SDL_GetTicks() - uQuitTime) > 2000) && ((ManKey.bFire) || (InputStates.pKeyboardArray[SDL_SCANCODE_SPACE])))   {
                bPrepareLevelExit = true;
            }
        }

        //// DEBUG-Code
        if (InputStates.pKeyboardArray[SDL_SCANCODE_X]) {
            bDebug = !bDebug;
            if (bDebug) {
                SDL_Log("Debugmode = on");
            } else {
                SDL_Log("Debugmode = off");
            }
            WaitNoSpecialKey(SDL_SCANCODE_X);
        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_I]) {
            PrintPlayfieldValues();
        }
        //// DEBUG-Code Ende
        if ((InputStates.pKeyboardArray[SDL_SCANCODE_P]) || ((bPause) && (ManKey.bFire))) {
            bPause = !bPause;
            if (!bPause) {
                bDimmIn = true; // Nach Pause wieder aufdimmen
            }
            do {
                UpdateManKey();
            } while ((InputStates.pKeyboardArray[SDL_SCANCODE_P]) || (ManKey.bFire));
        }
        if ((nCheckLevelCount == 0) && (!bPause)) {
            if ((ManKey.uDirection == MANKEY_NONE) && ((Playfield.uFrameCounter - ManKey.uLastDirectionFrameCount) <= 15)) {
                SDL_Log("%s: use buffered key: dir: %u   dif:%u",__FUNCTION__,ManKey.uLastActiveDirection,Playfield.uFrameCounter - ManKey.uLastDirectionFrameCount);
                uKey = ManKey.uLastActiveDirection;
            } else {
                uKey = ManKey.uDirection;
            }
            uManDirection = ControlGame(uKey);
            if ((Playfield.uEmeraldsToCollect == 0) && (!Playfield.bReadyToGo)) {
                Playfield.bReadyToGo = true;
                PreparePlaySound(SOUND_REPLICATOR_PLOP,Playfield.uManXpos + Playfield.uManYpos * Playfield.uLevel_X_Dimension);
            }
            if (nRet == 0) {   // Falls in ShowAuthorAndLevelname() ESC-Taste gedrückt wurde, Spielsound unterdrücken
                PlayAllSounds();
            }
        }
        if (!bPause) {
            ScrollAndCenterLevel(uManDirection);
            CheckRunningWheel();
            CheckRunningMagicWall();
            CheckLight();
            CheckTimeDoorOpen();
            CheckPlayTime();
        } else {
            // Pause abdimmen
            if (nColorDimm > 20) {
                nColorDimm = nColorDimm - 2;
                SetAllTextureColors(nColorDimm);
            }
        }
        RenderLevel(pRenderer,&Playfield.nTopLeftXpos,&Playfield.nTopLeftYpos,nCheckLevelCount);  // nCheckLevelCount 0 ... 15
        if (bDebug) PrintLittleFont(pRenderer,400,700,0,"DEBUG MODE ON, PRESS X TO TOGGLE",K_ABSOLUTE);
        if (!bPause) {
            nCheckLevelCount++;
        }
        if (nCheckLevelCount == 16) {
            nCheckLevelCount = 0;
        }
        if (bPrepareLevelExit) {
            if (nColorDimm > 0) {
                nColorDimm = nColorDimm - 2;
                if (nColorDimm < 0) {
                    nColorDimm = 0;
                }
                SetAllTextureColors(nColorDimm);
            } else {
                bLevelRun = false;
            }
        }
        if ((bDimmIn) && (!bPrepareLevelExit)) {
            nColorDimm = nColorDimm + 4;
            if (nColorDimm >= 100) {
                nColorDimm = 100;
                bDimmIn = false;;
            }
            SetAllTextureColors(nColorDimm);
        }
        if (Playfield.uShowMessageNo != 0) {
            ConfirmMessage(pRenderer);  // Spiel pausiert hier, bis Nachricht bestätigt wurde
        }
        SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
        SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
        if (bDebug) SDL_Delay(100);

    }
    SDL_ShowCursor(SDL_ENABLE);    // Mauspfeil verstecken
    Playfield.uPlayTimeEnd = SDL_GetTicks();
    SetAllTextureColors(100);           // Farben beim Verlassen wieder auf volle Helligekit
    SAFE_FREE(Playfield.pLevel);
    SAFE_FREE(Playfield.pInvalidElement);
    SAFE_FREE(Playfield.pStatusAnimation);
    SAFE_FREE(Playfield.pLastStatusAnimation);
    SAFE_FREE(Playfield.pPostAnimation);
    SAFE_FREE(Playfield.pLastYamDirection);
    FreeTeleporterCoordinates();
    for (I = 0; I < EMERALD_MAX_MESSAGES; I++) {
        SAFE_FREE(Playfield.pMessage[I]);
    }
    WaitNoKey();
    return 0;
}


/*----------------------------------------------------------------------------
Name:           ControlGame
------------------------------------------------------------------------------
Beschreibung: Alle 16 Frames bzw. alle 16 Aniomationsschritte wird diese Funktion aufgerufen, um das Spiel zu steuern.
Parameter
      Eingang: uDirection, uint32_t, gewünschte Richtung des Man
      Ausgang: -

Rückgabewert:  uint32_t, tatsächliche Richtung des Man (damit wird Levelscrolling gesteuert)
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t ControlGame(uint32_t uDirection) {
    uint32_t uManDirection;
    uint32_t I;
    uint16_t uLevelElement;
    uint16_t uCleanElement;
    uint32_t uAnimStatus;
    uint32_t uCleanStatus;

    uManDirection = EMERALD_ANIM_STAND;
    Playfield.bSwitchRemoteBombLeft = false;
    Playfield.bSwitchRemoteBombRight = false;
    Playfield.bSwitchRemoteBombDown = false;
    Playfield.bSwitchRemoteBombUp = false;
    Playfield.bSwitchRemoteBombIgnition = false;
    Playfield.bRemoteBombMoved = false;
    // Ab hier das Level und die Status für alle Elemente aus voriger Steuerung vorbereiten
    for (I = 0; I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension; I++) {
        // Dieser Block sorgt bei bewegten Objekten dafür, dass diese
        // ihren neuen Platz (invalides Feld) einnehmen.
        if (Playfield.pLevel[I] == EMERALD_INVALID) {
            uCleanStatus = Playfield.pStatusAnimation[I] & 0x00FF0000;
            if (uCleanStatus != EMERALD_ANIM_CLEAN_NOTHING) {   // Wird für grüne Tropfen verwendet
                if (Playfield.pInvalidElement[I] == EMERALD_NONE) {
                    SDL_Log("%s: warning, set element EMERALD_NONE at position %d",__FUNCTION__,I);
                }
                Playfield.pLevel[I] = Playfield.pInvalidElement[I]; // von invalides auf nächstes Element setzen
                Playfield.pInvalidElement[I] = EMERALD_NONE;
                // Neue Element-Koordinaten des Man berechnen
                if (Playfield.pLevel[I] == EMERALD_MAN) {
                    Playfield.uManXpos = I % Playfield.uLevel_X_Dimension;
                    Playfield.uManYpos = I / Playfield.uLevel_X_Dimension;
                }
                if ( (Playfield.pLevel[I] == EMERALD_MOLE_LEFT) ||
                     (Playfield.pLevel[I] == EMERALD_MOLE_RIGHT) ||
                     (Playfield.pLevel[I] == EMERALD_MOLE_UP) ||
                     (Playfield.pLevel[I] == EMERALD_MOLE_DOWN) )  {
                    uCleanElement = EMERALD_SAND_MOLE;
                } else {
                    uCleanElement = EMERALD_SPACE;
                }
                switch (uCleanStatus) {
                    case (EMERALD_ANIM_CLEAN_UP):
                        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = uCleanElement;
                        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                        break;
                    case (EMERALD_ANIM_CLEAN_RIGHT):
                        Playfield.pLevel[I + 1] = uCleanElement;
                        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_STAND;
                        break;
                    case (EMERALD_ANIM_CLEAN_DOWN):
                        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = uCleanElement;
                        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                        break;
                    case (EMERALD_ANIM_CLEAN_LEFT):
                        Playfield.pLevel[I - 1] = uCleanElement;
                        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_STAND;
                        break;
                    case (EMERALD_ANIM_CLEAN_UP_DOUBLESPEED):
                        Playfield.pLevel[I - 2 * Playfield.uLevel_X_Dimension] = uCleanElement;
                        Playfield.pStatusAnimation[I - 2 * Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                        break;
                    case (EMERALD_ANIM_CLEAN_RIGHT_DOUBLESPEED):
                        Playfield.pLevel[I + 2] = uCleanElement;
                        Playfield.pStatusAnimation[I + 2] = EMERALD_ANIM_STAND;
                        break;
                    case (EMERALD_ANIM_CLEAN_DOWN_DOUBLESPEED):
                        Playfield.pLevel[I + 2 * Playfield.uLevel_X_Dimension] = uCleanElement;
                        Playfield.pStatusAnimation[I + 2 * Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                        break;
                    case (EMERALD_ANIM_CLEAN_LEFT_DOUBLESPEED):
                        Playfield.pLevel[I - 2] = uCleanElement;
                        Playfield.pStatusAnimation[I - 2] = EMERALD_ANIM_STAND;
                        break;
                    default:
                        SDL_Log("%s: invalid clean status  %x",__FUNCTION__,uCleanStatus);
                        break;
                }
            }
        } else if (Playfield.pLevel[I] == EMERALD_ACIDPOOL_DESTROY) {
            // EMERALD_ACIDPOOL_DESTROY muss im Vorwege "behandelt" werden, da sonst Elemente auf EMERALD_ACIDPOOL_DESTROY fallen können.
            // Mole down muss beim Säürebecken gesondert behandelt werden, da beim Gang ins Säurebecken noch Sand über dem Becken erzeugt werden muss
            if (Playfield.pInvalidElement[I] == EMERALD_MOLE_DOWN) {
                Playfield.pLevel[I] = EMERALD_SAND_MOLE;
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_MOLE_GOES_ACID;
            } else {
                Playfield.pLevel[I] = EMERALD_SPACE;
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            }
        }
        // Dieser Block sorgt bei drehenden Objekten (Minen+Käfer+Mole) dafür, dass diese
        // nach der Drehung in die richtige Richtung zeigen.
        uAnimStatus = Playfield.pStatusAnimation[I] & 0x0000FFFF00;
        if ((uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_RIGHT) || (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_RIGHT)) {
            // Ab hier auf Mine, Käfer und Mole prüfen
            if ((Playfield.pLevel[I] == EMERALD_MINE_UP) || (Playfield.pLevel[I] == EMERALD_MINE_DOWN)) {
                Playfield.pLevel[I] = EMERALD_MINE_RIGHT;
            } else if ((Playfield.pLevel[I] == EMERALD_BEETLE_UP) || (Playfield.pLevel[I] == EMERALD_BEETLE_DOWN)) {
                Playfield.pLevel[I] = EMERALD_BEETLE_RIGHT;
            } else if ((Playfield.pLevel[I] == EMERALD_MOLE_UP) || (Playfield.pLevel[I] == EMERALD_MOLE_DOWN)) {
                Playfield.pLevel[I] = EMERALD_MOLE_RIGHT;
            }
        } else if ((uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_DOWN) || (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_DOWN)) {
            // Ab hier auf Mine, Käfer und Mole prüfen
            if ((Playfield.pLevel[I] == EMERALD_MINE_RIGHT) || (Playfield.pLevel[I] == EMERALD_MINE_LEFT)) {
                Playfield.pLevel[I] = EMERALD_MINE_DOWN;
            } else if ((Playfield.pLevel[I] == EMERALD_BEETLE_RIGHT) || (Playfield.pLevel[I] == EMERALD_BEETLE_LEFT)) {
                Playfield.pLevel[I] = EMERALD_BEETLE_DOWN;
            } else if ((Playfield.pLevel[I] == EMERALD_MOLE_RIGHT) || (Playfield.pLevel[I] == EMERALD_MOLE_LEFT)) {
                Playfield.pLevel[I] = EMERALD_MOLE_DOWN;
            }
        } else if ((uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_LEFT) || (uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_LEFT)) {
            // Ab hier auf Mine, Käfer und Mole prüfen
            if ((Playfield.pLevel[I] == EMERALD_MINE_DOWN) || (Playfield.pLevel[I] == EMERALD_MINE_UP)) {
                Playfield.pLevel[I] = EMERALD_MINE_LEFT;
            } else if ((Playfield.pLevel[I] == EMERALD_BEETLE_DOWN) || (Playfield.pLevel[I] == EMERALD_BEETLE_UP)) {
                Playfield.pLevel[I] = EMERALD_BEETLE_LEFT;
            } else if ((Playfield.pLevel[I] == EMERALD_MOLE_DOWN) || (Playfield.pLevel[I] == EMERALD_MOLE_UP)) {
                Playfield.pLevel[I] = EMERALD_MOLE_LEFT;
            }
        } else if ((uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_UP) || (uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_UP)) {
            // Ab hier auf Mine, Käfer und Mole prüfen
            if ((Playfield.pLevel[I] == EMERALD_MINE_LEFT) || (Playfield.pLevel[I] == EMERALD_MINE_RIGHT)) {
                Playfield.pLevel[I] = EMERALD_MINE_UP;
            } else if ((Playfield.pLevel[I] == EMERALD_BEETLE_LEFT) || (Playfield.pLevel[I] == EMERALD_BEETLE_RIGHT)) {
                Playfield.pLevel[I] = EMERALD_BEETLE_UP;
            } else if ((Playfield.pLevel[I] == EMERALD_MOLE_LEFT) || (Playfield.pLevel[I] == EMERALD_MOLE_RIGHT)) {
                Playfield.pLevel[I] = EMERALD_MOLE_UP;
            }
        }
        uCleanStatus = Playfield.pStatusAnimation[I] & 0xFF000000;
        // Es muss hier auch mit dem Levelelement UND-Verknüpft werden, da der Man unter Umständen auf das Objekt zuläuft und es aufnimmt
        if (    ((uCleanStatus == EMERALD_ANIM_KEY_RED_SHRINK) && (Playfield.pLevel[I] == EMERALD_KEY_RED)) ||
                ((uCleanStatus == EMERALD_ANIM_KEY_GREEN_SHRINK) && (Playfield.pLevel[I] == EMERALD_KEY_GREEN)) ||
                ((uCleanStatus == EMERALD_ANIM_KEY_BLUE_SHRINK) && (Playfield.pLevel[I] == EMERALD_KEY_BLUE)) ||
                ((uCleanStatus == EMERALD_ANIM_KEY_YELLOW_SHRINK) && (Playfield.pLevel[I] == EMERALD_KEY_YELLOW))  ||
                ((uCleanStatus == EMERALD_ANIM_KEY_WHITE_SHRINK) && (Playfield.pLevel[I] == EMERALD_KEY_WHITE))  ||
                ((uCleanStatus == EMERALD_ANIM_KEY_GENERAL_SHRINK) && (Playfield.pLevel[I] == EMERALD_KEY_GENERAL))  ||
                ((uCleanStatus == EMERALD_ANIM_SAND_SHRINK) && (Playfield.pLevel[I] == EMERALD_SAND)) ||
                ((uCleanStatus == EMERALD_ANIM_SAND_INVISIBLE_SHRINK) && (Playfield.pLevel[I] == EMERALD_SAND_INVISIBLE)) ||
                ((uCleanStatus == EMERALD_ANIM_GRASS_SHRINK) && (Playfield.pLevel[I] == EMERALD_GRASS)) ||
                ((uCleanStatus == EMERALD_ANIM_EMERALD_SHRINK) && (Playfield.pLevel[I] == EMERALD_EMERALD)) ||
                ((uCleanStatus == EMERALD_ANIM_RUBY_SHRINK) && (Playfield.pLevel[I] == EMERALD_RUBY)) ||
                ((uCleanStatus == EMERALD_ANIM_PERL_SHRINK) && (Playfield.pLevel[I] == EMERALD_PERL)) ||
                ((uCleanStatus == EMERALD_ANIM_CRYSTAL_SHRINK) && (Playfield.pLevel[I] == EMERALD_CRYSTAL)) ||
                ((uCleanStatus == EMERALD_ANIM_TIME_COIN_SHRINK) && (Playfield.pLevel[I] == EMERALD_TIME_COIN)) ||
                ((uCleanStatus == EMERALD_ANIM_HAMMER_SHRINK) && (Playfield.pLevel[I] == EMERALD_HAMMER)) ||
                ((uCleanStatus == EMERALD_ANIM_DYNAMITE_SHRINK) && (Playfield.pLevel[I] == EMERALD_DYNAMITE_OFF)) ||
                ((uCleanStatus == EMERALD_ANIM_MESSAGE_SHRINK) && (Playfield.pLevel[I] >= EMERALD_MESSAGE_1) && (Playfield.pLevel[I] <= EMERALD_MESSAGE_8)) ||
                ((uCleanStatus == EMERALD_ANIM_SAPPHIRE_SHRINK) && (Playfield.pLevel[I] == EMERALD_SAPPHIRE)) ) {
            Playfield.pLevel[I] = EMERALD_SPACE;
            Playfield.pStatusAnimation[I] = EMERALD_NO_ADDITIONAL_ANIMSTATUS;
        }
        // "Geboren" wird über 2 Steuerungsrunden, da sonst zu schnell geboren wird
        if (uCleanStatus == EMERALD_ANIM_BORN1) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_BORN2;
        } else if (uCleanStatus == EMERALD_ANIM_BORN2) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            PreparePlaySound(SOUND_REPLICATOR_PLOP,I);
        }
        // "Selbststeuernde" Animationen und Animationsstatus nicht zurücksetzen,
        // Nur Clean-Status zurücksetzen
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
    }
    if (Playfield.uTimeDoorTimeLeft == 0) {
        // Zeit-Tür schließen, da Zeit abgelaufen ist
        Playfield.bTimeDoorOpen = false;
    }
    // Ab hier beginnt eine neue Steuerung für alle Elemente
    ControlPreElements();   // ggf. Spaces für Elemente einsetzen, die sich auflösen, Molen-Sand in normalen Sand wandeln
    // Man als Zweites steuern !
    uManDirection = ControlMan(Playfield.uManYpos * Playfield.uLevel_X_Dimension + Playfield.uManXpos,uDirection);
    ControlLightBarriers();

    if (Playfield.uDynamitePos != 0xFFFFFFFF) {
        ControlManWithDynamiteOn(Playfield.uDynamitePos);
    }

    for (I = 0; I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension; I++) {
        uLevelElement = Playfield.pLevel[I];
        switch (uLevelElement) {
            case (EMERALD_REMOTEBOMB):
                ControlRemoteBomb(I);
                break;
            case (EMERALD_WALL_GROW_LEFT):
                ControlWallGrowLeft(I);
                break;
            case (EMERALD_WALL_GROWING_LEFT):
                ControlWallGrowingLeft(I);
                break;
            case (EMERALD_WALL_GROW_RIGHT):
                ControlWallGrowRight(I);
                break;
            case (EMERALD_WALL_GROWING_RIGHT):
                ControlWallGrowingRight(I);
                break;
            case (EMERALD_WALL_GROW_UP):
                ControlWallGrowUp(I);
                break;
            case (EMERALD_WALL_GROWING_UP):
                ControlWallGrowingUp(I);
                break;
            case (EMERALD_WALL_GROW_DOWN):
                ControlWallGrowDown(I);
                break;
            case (EMERALD_WALL_GROWING_DOWN):
                ControlWallGrowingDown(I);
                break;
            case (EMERALD_WALL_GROW_LEFT_RIGHT):
                ControlWallGrowLeftRight(I);
                break;
            case (EMERALD_WALL_GROW_UP_DOWN):
                ControlWallGrowUpDown(I);
                break;
            case (EMERALD_WALL_GROW_ALL):
                ControlWallGrowAllDirections(I);
                break;
            case (EMERALD_STEEL_GROW_LEFT):
                ControlSteelGrowLeft(I);
                break;
            case (EMERALD_STEEL_GROWING_LEFT):
                ControlSteelGrowingLeft(I);
                break;
            case (EMERALD_STEEL_GROW_RIGHT):
                ControlSteelGrowRight(I);
                break;
            case (EMERALD_STEEL_GROWING_RIGHT):
                ControlSteelGrowingRight(I);
                break;
            case (EMERALD_STEEL_GROW_UP):
                ControlSteelGrowUp(I);
                break;
            case (EMERALD_STEEL_GROWING_UP):
                ControlSteelGrowingUp(I);
                break;
            case (EMERALD_STEEL_GROW_DOWN):
                ControlSteelGrowDown(I);
                break;
            case (EMERALD_STEEL_GROWING_DOWN):
                ControlSteelGrowingDown(I);
                break;
            case (EMERALD_STEEL_GROW_LEFT_RIGHT):
                ControlSteelGrowLeftRight(I);
                break;
            case (EMERALD_STEEL_GROW_UP_DOWN):
                ControlSteelGrowUpDown(I);
                break;
            case (EMERALD_STEEL_GROW_ALL):
                ControlSteelGrowAllDirections(I);
                break;
            case (EMERALD_YAM_KILLS_MAN):
                ControlYamKillsMan(I);
                break;
            case (EMERALD_ALIEN_KILLS_MAN):
                ControlAlienKillsMan(I);
                break;
            case (EMERALD_MAN_DIES):
                ControlManDies(I);
                break;
            case (EMERALD_DYNAMITE_ON):
                ControlDynamiteOn(I);
                break;
            case (EMERALD_CENTRAL_EXPLOSION_BEETLE):
                ControlCentralBeetleExplosion(I);       // 3x3-Käferexplosion
                PreparePlaySound(SOUND_EXPLOSION,I);
                break;
            case (EMERALD_CENTRAL_EXPLOSION):
                ControlCentralExplosion(I);             // Normale 3x3 Explosion
                PreparePlaySound(SOUND_EXPLOSION,I);
                break;
            case (EMERALD_CENTRAL_EXPLOSION_MEGA):
                ControlCentralMegaExplosion(I);         // Mega-Explosion
                PreparePlaySound(SOUND_EXPLOSION,I);
                break;
            case (EMERALD_SWITCHDOOR_OPEN):
                // wird nach der Steuerungsrunde durchgeführt in ControlSwitchDoors();
                break;
            case (EMERALD_SWITCHDOOR_CLOSED):
                // wird nach der Steuerungsrunde durchgeführt in ControlSwitchDoors();
                break;
            case (EMERALD_DOOR_TIME):
                ControlTimeDoor(I);
                break;
            case (EMERALD_WHEEL_TIMEDOOR):
                if (Playfield.bTimeDoorOpen) {
                    PreparePlaySound(SOUND_WHEEL_TIMEDOOR,I);
                }
                break;
            case (EMERALD_GREEN_CHEESE_GOES):
                // wird oben im Vorwege bereits behandelt und kommt hier nicht mehr vor, da bereits in Space gewandelt
                break;
            case (EMERALD_SAND_MOLE):
                // ControlMoleSand(I);
                break;
            case (EMERALD_STANDMINE):
                ControlStandMine(I);
                break;
            case (EMERALD_GREEN_DROP_COMES):
                ControlSpreadCheese(I);
                break;
            case (EMERALD_GREEN_CHEESE):
                ControlGreenCheese(I);
                break;
            case (EMERALD_GREEN_DROP):
                ControlGreenDrop(I);
                break;
            case (EMERALD_YAM):
                ControlYam(I);
                break;
            case(EMERALD_DOOR_END_READY):
                ControlEnddoorReady(I);
                break;
            case(EMERALD_DOOR_END_READY_STEEL):
                ControlEnddoorReadySteel(I);
                break;
            case (EMERALD_DOOR_END_NOT_READY):
                if (Playfield.uEmeraldsToCollect == 0) {
                    Playfield.pLevel[I] = EMERALD_DOOR_END_READY;
                }
                break;
            case (EMERALD_DOOR_END_NOT_READY_STEEL):
                if (Playfield.uEmeraldsToCollect == 0) {
                    Playfield.pLevel[I] = EMERALD_DOOR_END_READY_STEEL;
                }
                break;
            case (EMERALD_EXPLOSION_TO_ELEMENT_1):
            case (EMERALD_EXPLOSION_TO_ELEMENT_2):
                //ControlExplosion(I);
                break;
            case (EMERALD_MAGIC_WALL):
            case (EMERALD_MAGIC_WALL_STEEL):
                ControlMagicWall(I);
                break;
            case (EMERALD_ACIDPOOL_DESTROY):
                // wird oben im Vorwege bereits behandelt und kommt hier nicht mehr vor, da bereits in Space gewandelt
                break;
            case (EMERALD_STONE_SAG):
                ControlStoneSag(I);
                break;
            case (EMERALD_SWAMP_STONE):
                ControlSwampStone(I);
                break;
            case (EMERALD_STONE_SINK):
                ControlStoneSink(I);
                break;
            case (EMERALD_EMERALD):
                ControlEmerald(I);
                break;
            case (EMERALD_RUBY):
                ControlRuby(I);
                break;
            case (EMERALD_PERL):
                ControlPerl(I);
                break;
            case (EMERALD_CRYSTAL):
                ControlCrystal(I);
                break;
            case (EMERALD_SAPPHIRE):
                ControlSaphir(I);
                break;
            case (EMERALD_BOMB):
                ControlBomb(I);
                break;
            case (EMERALD_MEGABOMB):
                ControlMegaBomb(I);
                break;
            case (EMERALD_NUT):
                ControlNut(I);
                break;
            case (EMERALD_STONE):
                ControlStone(I);
                break;
            case (EMERALD_SAND):
                ControlSand(I);
                break;
            case (EMERALD_SAND_INVISIBLE):
                ControlSandInvisible(I);
                break;
            case (EMERALD_SANDMINE):
                ControlSandMine(I);
                break;
            case (EMERALD_GRASS):
                ControlGrass(I);
                break;
            case (EMERALD_GRASS_COMES):
                ControlGrassComes(I);
                break;
            case (EMERALD_REPLICATOR_RED_TOP_MID):
                ControlRedReplicator(I);
                break;
            case (EMERALD_REPLICATOR_YELLOW_TOP_MID):
                ControlYellowReplicator(I);
                break;
            case (EMERALD_REPLICATOR_GREEN_TOP_MID):
                ControlGreenReplicator(I);
                break;
            case (EMERALD_REPLICATOR_BLUE_TOP_MID):
                ControlBlueReplicator(I);
                break;
            case (EMERALD_CONVEYORBELT_RED):
                if (Playfield.uConveybeltRedState != EMERALD_CONVEYBELT_OFF) {
                    PreparePlaySound(SOUND_CONVEYORBELT,I);
                }
                break;
            case (EMERALD_CONVEYORBELT_YELLOW):
                if (Playfield.uConveybeltYellowState != EMERALD_CONVEYBELT_OFF) {
                    PreparePlaySound(SOUND_CONVEYORBELT,I);
                }
                break;
            case (EMERALD_CONVEYORBELT_GREEN):
                if (Playfield.uConveybeltGreenState != EMERALD_CONVEYBELT_OFF) {
                    PreparePlaySound(SOUND_CONVEYORBELT,I);
                }
                break;
            case (EMERALD_CONVEYORBELT_BLUE):
                if (Playfield.uConveybeltBlueState != EMERALD_CONVEYBELT_OFF) {
                    PreparePlaySound(SOUND_CONVEYORBELT,I);
                }
                break;
            case (EMERALD_MAN):
                // Man wird vorab gesteuert, siehe oben
                break;
            case (EMERALD_MOLE_UP):
                ControlMoleUp(I);
                PreparePlaySound(SOUND_MOLE,I);
                break;
            case (EMERALD_MOLE_RIGHT):
                ControlMoleRight(I);
                PreparePlaySound(SOUND_MOLE,I);
                break;
            case (EMERALD_MOLE_DOWN):
                ControlMoleDown(I);
                PreparePlaySound(SOUND_MOLE,I);
                break;
            case (EMERALD_MOLE_LEFT):
                ControlMoleLeft(I);
                PreparePlaySound(SOUND_MOLE,I);
                break;
            case (EMERALD_MINE_UP):
                ControlMineUp(I);
                PreparePlaySound(SOUND_MINE,I);
                break;
            case (EMERALD_MINE_RIGHT):
                ControlMineRight(I);
                PreparePlaySound(SOUND_MINE,I);
                break;
            case (EMERALD_MINE_DOWN):
                ControlMineDown(I);
                PreparePlaySound(SOUND_MINE,I);
                break;
            case (EMERALD_MINE_LEFT):
                ControlMineLeft(I);
                PreparePlaySound(SOUND_MINE,I);
                break;
            case (EMERALD_BEETLE_UP):
                ControlBeetleUp(I);
                PreparePlaySound(SOUND_BEETLE,I);
                break;
            case (EMERALD_BEETLE_RIGHT):
                ControlBeetleRight(I);
                PreparePlaySound(SOUND_BEETLE,I);
                break;
            case (EMERALD_BEETLE_DOWN):
                ControlBeetleDown(I);
                PreparePlaySound(SOUND_BEETLE,I);
                break;
            case (EMERALD_BEETLE_LEFT):
                ControlBeetleLeft(I);
                PreparePlaySound(SOUND_BEETLE,I);
                break;
            case (EMERALD_ALIEN):
                ControlAlien(I);
                break;
            case (EMERALD_WHEEL):
                if (Playfield.bWheelRunning) {
                    PreparePlaySound(SOUND_WHEEL,I);
                }
                break;
        }
    }
    PostControlSwitchDoors();
    if ((Playfield.bSwitchRemoteBombDown) || (Playfield.bSwitchRemoteBombUp) || (Playfield.bSwitchRemoteBombLeft) || (Playfield.bSwitchRemoteBombRight)) {
        if (Playfield.bRemoteBombMoved) {
            PreparePlaySound(SOUND_REMOTE_BOMB,0);
        } else {
            PreparePlaySound(SOUND_SWITCH,0);
        }
    }
    return uManDirection;
}


/*----------------------------------------------------------------------------
Name:           ScrollAndCenterLevel
------------------------------------------------------------------------------
Beschreibung: Scrollt und zentriert das Level, wenn sich der Man bewegt.
Parameter
      Eingang: uManDirection, uint32_t, aktuelle Richtung des Man
      Ausgang: -

Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ScrollAndCenterLevel(uint32_t uManDirection) {
    switch (uManDirection) {
        case (EMERALD_ANIM_UP):
            if (Playfield.uManYpos < (Playfield.uLevel_Y_Dimension - Playfield.uVisibleCenterY)) {
                Playfield.nTopLeftYpos -= 2;
            }
            break;
        case (EMERALD_ANIM_RIGHT):
            if (Playfield.uManXpos >= Playfield.uVisibleCenterX) {
                Playfield.nTopLeftXpos += 2;
            }
            break;
        case (EMERALD_ANIM_DOWN):
            if (Playfield.uManYpos >= Playfield.uVisibleCenterY) {
                Playfield.nTopLeftYpos += 2;
            }
            break;
        case (EMERALD_ANIM_LEFT):
            if (Playfield.uManXpos < (Playfield.uLevel_X_Dimension - Playfield.uVisibleCenterX)) {
                Playfield.nTopLeftXpos -= 2;
            }
            break;
        case (EMERALD_ANIM_UP_DOUBLESPEED):
            if (Playfield.uManYpos < (Playfield.uLevel_Y_Dimension - Playfield.uVisibleCenterY)) {
                Playfield.nTopLeftYpos -= 4;
            }
            break;
        case (EMERALD_ANIM_RIGHT_DOUBLESPEED):
            if (Playfield.uManXpos >= Playfield.uVisibleCenterX) {
                Playfield.nTopLeftXpos += 4;
            }
            break;
        case (EMERALD_ANIM_DOWN_DOUBLESPEED):
            if (Playfield.uManYpos >= Playfield.uVisibleCenterY) {
                Playfield.nTopLeftYpos += 4;
            }
            break;
        case (EMERALD_ANIM_LEFT_DOUBLESPEED):
            if (Playfield.uManXpos < (Playfield.uLevel_X_Dimension - Playfield.uVisibleCenterX)) {
                Playfield.nTopLeftXpos -= 4;
            }
            break;
    }
}


/*----------------------------------------------------------------------------
Name:           GetTextureIndexByElementForAcidPool
------------------------------------------------------------------------------
Beschreibung: Holt den entsprechenden Texture-Index anhand eines Elements und des
              Animations-Schrittes. Die Funktion ist speziell für Elemente, die
              ins Säurebecken fallen können.
Parameter
      Eingang: uElement, uint16_t, Element, z.B. EMERALD_MINE_RIGHT
               nAnimationCount, int, Animationsschritt
      Ausgang: pfAngle, float *, Winkel für Texture;
Rückgabewert:  uint32_t , Texture, wenn keine Texture ermittelt werden kann, wird
                SPACE (EMERALD_SPACE) zurückgegeben.
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t GetTextureIndexByElementForAcidPool(uint16_t uElement,int nAnimationCount, float *pfAngle) {
    uint32_t uTextureIndex;
    uint32_t K;
    float fAngle = 0;

    switch (uElement) {
        case (EMERALD_GREEN_DROP):
            uTextureIndex = 346;
            break;
        case (EMERALD_EMERALD):
            uTextureIndex = 226 + nAnimationCount / 2;     // Emerald, liegend
            break;
        case (EMERALD_RUBY):
            uTextureIndex = 301 + nAnimationCount / 2;     // Rubin, liegend
            break;
        case (EMERALD_CRYSTAL):
            uTextureIndex = 309;                            // Kristall, liegend
            break;
        case (EMERALD_SAPPHIRE):
            uTextureIndex = 248 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
            break;
        case (EMERALD_NUT):
            uTextureIndex = 234;
            break;
        case (EMERALD_STONE):
            uTextureIndex = 71;
            break;
        case (EMERALD_YAM):
            K = Playfield.uFrameCounter % 11;       // Y von 0 bis 10
            if (K <= 5) {                           // 0,1,2,3,4,5
                uTextureIndex = 362 + K;            // 362 - 367
            } else {                                // 6,7,8,9,10
                uTextureIndex = 367 + 5 - K;        // 366 - 362
            }
            break;
        case (EMERALD_ALIEN):
            if ((nAnimationCount >= 4) && (nAnimationCount <= 11)) {
                uTextureIndex = 135;                        // Alien geht 2, Flügel voll ausgebreitet
            } else {
                uTextureIndex = 136;                        // Alien geht 1
            }
            break;
        case (EMERALD_MOLE_UP):
        case (EMERALD_MOLE_RIGHT):
        case (EMERALD_MOLE_DOWN):
        case( EMERALD_MOLE_LEFT):
            fAngle = 270;
            uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
            break;
        case (EMERALD_MINE_UP):
        case (EMERALD_MINE_RIGHT):
        case (EMERALD_MINE_DOWN):
        case( EMERALD_MINE_LEFT):
            fAngle = 270;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = 73;     // Mine links
            } else {
                uTextureIndex = 74;     // Mine links an
            }
            break;
        case (EMERALD_BEETLE_UP):
        case (EMERALD_BEETLE_RIGHT):
        case (EMERALD_BEETLE_DOWN):
        case (EMERALD_BEETLE_LEFT):
            uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
            fAngle = 270;
            break;
        case (EMERALD_MAN):
            uTextureIndex = 119 + nAnimationCount % 8;     // Man runter
            break;
        case (EMERALD_BOMB):
            uTextureIndex = 271 + nAnimationCount % 8;
            break;
        case (EMERALD_MEGABOMB):
            uTextureIndex = 524 + ((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 5;
            break;
        case (EMERALD_PERL):
            uTextureIndex = 436 + nAnimationCount % 8;
            break;
        case (EMERALD_REMOTEBOMB):
            uTextureIndex = 1028;
            fAngle = nAnimationCount * 22.5;
            break;
        default:
            SDL_Log("%s: unknown element: %x",__FUNCTION__,uElement);
            uTextureIndex = 0;     // Space
            break;
    }
    *pfAngle = fAngle;
    return uTextureIndex;
}


/*----------------------------------------------------------------------------
Name:           InitRollUnderground
------------------------------------------------------------------------------
Beschreibung: Initialisiert das Array Playfield.uRollUnderground[256]. Hier ist für
              maximal 65536 Elemente (Element-Index, z.B. EMERALD_SAPPHIRE) abgelegt,
              ob Elemente von einem anderen Element herunterrollen.
              Bit 0 = Emerald, Bit 1 = Saphir, Bit 2 = Stone, Bit 3 = Nut, Bit 4 = Bomb, Bit 5 = Rubin, Bit 6 = Kristall, Bit 7 = Perle, Bit 8 = Megabombe
              Beispiel: Playfield.uRollUnderground[EMERALD_KEY_GREEN] = 0x03.
              -> Emerald (Bit 1) und Saphir (Bit 2) rollt von grünem Schlüssel
              Funktion wird 1-malig zu Programmbeginn aufgerufen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void InitRollUnderground(void) {
    memset(Playfield.uRollUnderground,0,sizeof(Playfield.uRollUnderground));
    Playfield.uRollUnderground[EMERALD_STONE] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_SAPPHIRE] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_BOMB] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_MEGABOMB] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_EMERALD] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_RUBY] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_PERL] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_CRYSTAL] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_NUT] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_WALL_ROUND] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_WALL_ROUND_PIKE] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_STEEL_ROUND_PIKE] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_STEEL] = 0xEB;                           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_GROW_LEFT] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_GROW_RIGHT] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_GROW_UP] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_GROW_DOWN] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_GROW_LEFT_RIGHT] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_GROW_UP_DOWN] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_GROW_ALL] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_GROW_LEFT] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_GROW_RIGHT] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_GROW_UP] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_GROW_DOWN] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_GROW_LEFT_RIGHT] = 0xEB;            // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_GROW_UP_DOWN] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_GROW_ALL] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_CORNERED] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_WALL_INVISIBLE] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_MODERN_LEFT_END] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_MODERN_LEFT_RIGHT] = 0xEB;         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_MODERN_RIGHT_END] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_MODERN_UP_END] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_MODERN_UP_DOWN] = 0xEB;            // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_MODERN_DOWN_END] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_MODERN_MIDDLE] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
    // Bei DC3 rollt nichts von den Schaltern für ferngesteuerte Bombe
    Playfield.uRollUnderground[EMERALD_SWITCH_REMOTEBOMB_UP] = 0xEB;            // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_SWITCH_REMOTEBOMB_DOWN] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_SWITCH_REMOTEBOMB_LEFT] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_SWITCH_REMOTEBOMB_RIGHT] = 0xEB;         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_SWITCH_REMOTEBOMB_IGNITION] = 0xEB;      // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_REMOTEBOMB] = 0x1FF;
    // Bei DC3 rollt nichts vom Schlüssel
    // Playfield.uRollUnderground[EMERALD_KEY_RED] = 0x1FF;                     // nicht bei DC3
    // Playfield.uRollUnderground[EMERALD_KEY_YELLOW] = 0x1FF;                  // nicht bei DC3
    // Playfield.uRollUnderground[EMERALD_KEY_BLUE] = 0x1FF;                    // nicht bei DC3
    // Playfield.uRollUnderground[EMERALD_KEY_GREEN] = 0x1FF;                   // nicht bei DC3
	// Playfield.uRollUnderground[EMERALD_KEY_GENERAL] = 0x1FF;                 // nicht bei DC3
	// Playfield.uRollUnderground[EMERALD_KEY_WHITE] = 0x1FF;                   // nicht bei DC3
    Playfield.uRollUnderground[EMERALD_WHEEL] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_ACIDPOOL_TOP_LEFT] = 0x1FF;              // nicht bei DC3
    Playfield.uRollUnderground[EMERALD_ACIDPOOL_TOP_RIGHT] = 0x1FF;             // nicht bei DC3
    Playfield.uRollUnderground[EMERALD_DOOR_RED] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_YELLOW] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_BLUE] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_GREEN] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_WHITE] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_EMERALD] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_MULTICOLOR] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_RED_WOOD] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_YELLOW_WOOD] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_BLUE_WOOD] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_GREEN_WOOD] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_WHITE_WOOD] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_GREY_RED] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_GREY_YELLOW] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_GREY_GREEN] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_GREY_BLUE] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_DOOR_GREY_WHITE] = 0x1FF;
	Playfield.uRollUnderground[EMERALD_WHEEL_TIMEDOOR] = 0x1FF;
	Playfield.uRollUnderground[EMERALD_DOOR_TIME] = 0x1FF;
	Playfield.uRollUnderground[EMERALD_SWITCHDOOR_OPEN] = 0x1FF;
	Playfield.uRollUnderground[EMERALD_SWITCHDOOR_CLOSED] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_STEEL_ROUND] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_STEEL_WARNING] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_BIOHAZARD] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_DEADEND] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_STOP] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_PARKING] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_FORBIDDEN] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_EXIT] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_RADIOACTIVE] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_EXPLOSION] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_ACID] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_HEART] = 0xEB;                     // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_PLAYERHEAD] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_NO_ENTRY] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_GIVE_WAY] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_YING] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_WHEELCHAIR] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_ARROW_DOWN] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_ARROW_UP] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_ARROW_LEFT] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_ARROW_RIGHT] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_INVISIBLE] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_MARKER_LEFT_UP] = 0xEB;            // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_MARKER_UP] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_MARKER_RIGHT_UP] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_MARKER_LEFT] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_MARKER_RIGHT] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_MARKER_LEFT_BOTTOM] = 0xEB;        // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_MARKER_BOTTOM] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_MARKER_RIGHT_BOTTOM] = 0xEB;       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_GROW_RIGHT] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	// Playfield.uRollUnderground[EMERALD_STEEL_GROWING_RIGHT] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_GROW_LEFT] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
	// Playfield.uRollUnderground[EMERALD_STEEL_GROWING_LEFT] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_GROW_UP] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	// Playfield.uRollUnderground[EMERALD_STEEL_GROWING_UP] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_GROW_DOWN] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    // Playfield.uRollUnderground[EMERALD_STEEL_GROWING_DOWN] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_WALL_GROW_RIGHT] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	// Playfield.uRollUnderground[EMERALD_WALL_GROWING_RIGHT] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_WALL_GROW_LEFT] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
	// Playfield.uRollUnderground[EMERALD_WALL_GROWING_LEFT] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_WALL_GROW_UP] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	// Playfield.uRollUnderground[EMERALD_WALL_GROWING_UP] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_WALL_GROW_DOWN] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    // Playfield.uRollUnderground[EMERALD_WALL_GROWING_DOWN] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_EXCLAMATION] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_ARROW_RIGHT] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_ARROW_UP] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_ARROW_DOWN] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_APOSTROPHE] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_BRACE_OPEN] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_BRACE_CLOSE] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_COPYRIGHT] = 0xEB;            // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_PLUS] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_COMMA] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_MINUS] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_POINT] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_SLASH] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_0] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_1] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_2] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_3] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_4] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_5] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_6] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_7] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_8] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_9] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_DOUBLE_POINT] = 0xEB;         // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_PLATE] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_ARROW_LEFT] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_QUESTION_MARK] = 0xEB;        // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_A] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_B] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_C] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_D] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_E] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_F] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_G] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_H] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_I] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_J] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_K] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_L] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_M] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_N] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_O] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_P] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_Q] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_R] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_S] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_T] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_U] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_V] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_W] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_X] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_Y] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_Z] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_AE] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_OE] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_UE] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_EXCLAMATION] = 0xEB;    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_ARROW_RIGHT] = 0xEB;    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_ARROW_UP] = 0xEB;       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_ARROW_DOWN] = 0xEB;     // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_APOSTROPHE] = 0xEB;     // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_BRACE_OPEN] = 0xEB;     // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_BRACE_CLOSE] = 0xEB;    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_COPYRIGHT] = 0xEB;      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_PLUS] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_COMMA] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_MINUS] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_POINT] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_SLASH] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_0] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_1] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_2] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_3] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_4] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_5] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_6] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_7] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_8] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_9] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_DOUBLE_POINT] = 0xEB;   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_PLATE] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_ARROW_LEFT] = 0xEB;     // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_QUESTION_MARK] = 0xEB;  // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_A] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_B] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_C] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_D] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_E] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_F] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_G] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_H] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_I] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_J] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_K] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_L] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_M] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_N] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_O] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_P] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_Q] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_R] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_S] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_T] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_U] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_V] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_W] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_X] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_Y] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_Z] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_AE] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_OE] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_STEEL_GREEN_UE] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_EMERALD] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_RUBY] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_SAPPHIRE] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_PERL] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_KEY_RED] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_KEY_GREEN] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_KEY_BLUE] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_KEY_YELLOW] = 0xEB;            // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_KEY_WHITE] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_KEY_GENERAL] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_BOMB] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_MEGABOMB] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_STONE] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_NUT] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_WHEEL] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_DYNAMITE] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_ENDDOOR] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_ENDDOOR_READY] = 0xEB;         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_MINE_UP] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_MOLE_UP] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_GREEN_CHEESE] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_BEETLE_UP] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_YAM] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_ALIEN] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_CRYSTAL] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_TIME_COIN] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_RED_TOP_LEFT] = 0xEB;         // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_RED_TOP_RIGHT] = 0xEB;        // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_GREEN_TOP_LEFT] = 0xEB;       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_GREEN_TOP_RIGHT] = 0xEB;      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_YELLOW_TOP_LEFT] = 0xEB;      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_YELLOW_TOP_RIGHT] = 0xEB;     // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_BLUE_TOP_LEFT] = 0xEB;        // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_DOOR_ONLY_UP_STEEL] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_DOOR_ONLY_DOWN_STEEL] = 0xEB;            // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_DOOR_ONLY_LEFT_STEEL] = 0xEB;            // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_DOOR_ONLY_RIGHT_STEEL] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_RED_SWITCH] = 0xEB;         // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_GREEN_SWITCH] = 0xEB;       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_BLUE_SWITCH] = 0xEB;        // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_YELLOW_SWITCH] = 0xEB;      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_RED_SWITCH] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_YELLOW_SWITCH] = 0xEB;        // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_GREEN_SWITCH] = 0xEB;         // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_BLUE_SWITCH] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_LIGHT_SWITCH] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_MAGIC_WALL_SWITCH] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_SWITCH_SWITCHDOOR] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_CONVEYORBELT_SWITCH_RED] = 0xEB;         // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_CONVEYORBELT_SWITCH_GREEN] = 0xEB;       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_CONVEYORBELT_SWITCH_BLUE] = 0xEB;        // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_CONVEYORBELT_SWITCH_YELLOW] = 0xEB;      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_DOOR_END_NOT_READY_STEEL] = 0xEB;        // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_DOOR_END_READY_STEEL] = 0xEB;            // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_DOOR_END_READY] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_DOOR_END_NOT_READY] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_RED_UP] = 0x1FF;            // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_RED_DOWN] = 0x1FF;          // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_RED_LEFT] = 0x1FF;          // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_RED_RIGHT] = 0x1FF;         // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_GREEN_UP] = 0x1FF;          // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_GREEN_DOWN] = 0x1FF;        // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_GREEN_LEFT] = 0x1FF;        // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_GREEN_RIGHT] = 0x1FF;       // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_BLUE_UP] = 0x1FF;           // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_BLUE_DOWN] = 0x1FF;         // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_BLUE_LEFT] = 0x1FF;         // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_BLUE_RIGHT] = 0x1FF;        // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_YELLOW_UP] = 0x1FF;         // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_YELLOW_DOWN] = 0x1FF;       // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_YELLOW_LEFT] = 0x1FF;       // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_YELLOW_RIGHT] = 0x1FF;      // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_TIME_COIN] = 0x1FF;                      // Alles rollt von Münzen
}


/*----------------------------------------------------------------------------
Name:           GetFreeRollDirections
------------------------------------------------------------------------------
Beschreibung: Prüft freie Roll-Richtungen
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  uint8_t, 0 = kann nicht rollen, 1 = kann links rollen, 2 = kann rechts rollen, 3 = kann links und rechts rollen
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint8_t GetFreeRollDirections(uint32_t I) {
    uint8_t uFreeDirs = 0;

    // Kann Element links rollen?
    if ( (Playfield.pLevel[I - 1] == EMERALD_SPACE) &&
         ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension - 1] == EMERALD_SPACE) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension - 1] == EMERALD_ACIDPOOL)) ) {
        uFreeDirs = uFreeDirs | 0x01;
    }
    // Kann Element rechts rollen?
    if ( (Playfield.pLevel[I + 1] == EMERALD_SPACE) &&
        ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension + 1] == EMERALD_SPACE) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension + 1] == EMERALD_ACIDPOOL)) ) {
        uFreeDirs = uFreeDirs | 0x02;
    }
    return uFreeDirs;
}


/*----------------------------------------------------------------------------
Name:           GetTextureIndexByShrink
------------------------------------------------------------------------------
Beschreibung: Ermittelt anhand einer "Shrink-Animation" den Texture-Index
Parameter
      Eingang: uShrinkAnimation, uint32_t, Shrink-Animation, z.B.: EMERALD_ANIM_KEY_RED_SHRINK
Rückgabewert:  uint32_t, TextureIndex, 0 = Space = nicht verfügbar
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t GetTextureIndexByShrink(uint32_t uShrinkAnimation) {
    uint32_t uTextureIndex;

    switch (uShrinkAnimation) {
        case (EMERALD_ANIM_MESSAGE_SHRINK):
            uTextureIndex = 356;
            break;
        case (EMERALD_ANIM_KEY_RED_SHRINK):
            uTextureIndex = 98;
            break;
        case (EMERALD_ANIM_KEY_GREEN_SHRINK):
            uTextureIndex = 99;
            break;
        case (EMERALD_ANIM_KEY_BLUE_SHRINK):
            uTextureIndex = 100;
            break;
        case (EMERALD_ANIM_KEY_YELLOW_SHRINK):
            uTextureIndex = 101;
            break;
        case (EMERALD_ANIM_KEY_WHITE_SHRINK):
            uTextureIndex = 519;
            break;
        case (EMERALD_ANIM_KEY_GENERAL_SHRINK):
            uTextureIndex = 520;
            break;
        case (EMERALD_ANIM_SAND_SHRINK):
            uTextureIndex = 156;
            break;
        case (EMERALD_ANIM_SAND_INVISIBLE_SHRINK):
            uTextureIndex = 783;
            break;
        case (EMERALD_ANIM_GRASS_SHRINK):
            uTextureIndex = 750;
            break;
        case (EMERALD_ANIM_EMERALD_SHRINK):
            uTextureIndex = 226;
            break;
        case (EMERALD_ANIM_RUBY_SHRINK):
            uTextureIndex = 301;
            break;
        case (EMERALD_ANIM_PERL_SHRINK):
            uTextureIndex = 436;
            break;
        case (EMERALD_ANIM_CRYSTAL_SHRINK):
            uTextureIndex = 309;
            break;
        case (EMERALD_ANIM_SAPPHIRE_SHRINK):
            uTextureIndex = 248;
            break;
        case (EMERALD_ANIM_TIME_COIN_SHRINK):
            uTextureIndex = 310;
            break;
        case (EMERALD_ANIM_HAMMER_SHRINK):
            uTextureIndex = 318;
            break;
        case (EMERALD_ANIM_DYNAMITE_SHRINK):
            uTextureIndex = 286;
            break;
        default:
            uTextureIndex = 0; // Space
            SDL_Log("%s: Animation (%d) not found",__FUNCTION__,uShrinkAnimation);
            break;
    }
    return uTextureIndex;
}


/*----------------------------------------------------------------------------
Name:           ControlPreElements
------------------------------------------------------------------------------
Beschreibung: Erzeugt ggf. Spaces für Elemente, die sich auflösen.
              Molen-Sand wird in normalen Sand gewandelt.
              Schaltet Schalttüren ggf. um.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlPreElements(void) {
    uint32_t I;

    for (I = 0; I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension; I++) {
        if (Playfield.pLevel[I] == EMERALD_SPACE) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;     // setzt ggf. EMERALD_ANIM_BLOCK_MAN zurück
        } else if ( (Playfield.pLevel[I] == EMERALD_GREEN_CHEESE_GOES) ||
             ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_SINK_IN_MAGIC_WALL) ||   // Für Elemente, die im Magic Wall eintauchen
             (Playfield.pStatusAnimation[I] == EMERALD_ANIM_PERL_BREAK) ) {                         // Zerbrechende Perle
           Playfield.pLevel[I] = EMERALD_SPACE;           // Enstehenden Tropfen in Tropfen wandeln.
           Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        } else if (Playfield.pLevel[I] == EMERALD_SAND_MOLE) {
            if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_MOLE_GOES_ACID) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            } else {
                Playfield.pLevel[I] = EMERALD_SAND;     // Molen-Sand in normalen Sand wandeln
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            }
        } else if ((Playfield.pLevel[I] == EMERALD_SWITCHDOOR_CLOSED) && (Playfield.pStatusAnimation[I] == EMERALD_ANIM_DOOR_OPEN)) {
            Playfield.pLevel[I] = EMERALD_SWITCHDOOR_OPEN;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        } else if ((Playfield.pLevel[I] == EMERALD_SWITCHDOOR_OPEN) && (Playfield.pStatusAnimation[I] == EMERALD_ANIM_DOOR_CLOSE)) {
            Playfield.pLevel[I] = EMERALD_SWITCHDOOR_CLOSED;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        } else if ((Playfield.pLevel[I] == EMERALD_EXPLOSION_TO_ELEMENT_1) || (Playfield.pLevel[I] == EMERALD_EXPLOSION_TO_ELEMENT_2)) {
            ControlExplosionToElement(I);
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlTimeDoor
------------------------------------------------------------------------------
Beschreibung: Steuert die Zeit-Tür.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlTimeDoor(uint32_t I) {
    bool bActDoorOpenState;     // false = geschlossen, true = geöffnet

    Playfield.pStatusAnimation[I] &= 0x00FFFFFF;    // Selbststeuernden Animationsstatus zurücksetzen

    bActDoorOpenState = ((Playfield.pStatusAnimation[I] & 0xFF00) == EMERALD_STATUS_DOOR_OPEN);
    if (Playfield.bTimeDoorOpen != bActDoorOpenState) {
        if (Playfield.bTimeDoorOpen) {
            // SDL_Log("%s: Time door opens",__FUNCTION__);
            Playfield.pStatusAnimation[I] = EMERALD_STATUS_DOOR_OPEN | EMERALD_ANIM_DOOR_OPEN;
            PreparePlaySound(SOUND_DOOR_OPEN_CLOSE,I);
        } else {
            // SDL_Log("%s: Time door closes",__FUNCTION__);
            Playfield.pStatusAnimation[I] = EMERALD_STATUS_DOOR_CLOSE |EMERALD_ANIM_DOOR_CLOSE;
            PreparePlaySound(SOUND_DOOR_OPEN_CLOSE,I);
        }
    }
}


/*----------------------------------------------------------------------------
Name:           PostControlSwitchDoors
------------------------------------------------------------------------------
Beschreibung: Steuert die Schalttüren unmittelbar nach der Haupt-Steuerungsrunde, wenn
              der Man einen Schalt-Impuls gegeben hat.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void PostControlSwitchDoors(void) {
    uint32_t I;

    if (Playfield.bSwitchDoorImpluse) {
        for (I = 0; I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension; I++) {
            if (Playfield.pLevel[I] == EMERALD_SWITCHDOOR_CLOSED) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOOR_OPEN;     // Öffnen der Schalttür einleiten
                PreparePlaySound(SOUND_DOOR_OPEN_CLOSE,I);
            } else if (Playfield.pLevel[I] == EMERALD_SWITCHDOOR_OPEN) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOOR_CLOSE;    // Schließen der Schalttür einleiten
                PreparePlaySound(SOUND_DOOR_OPEN_CLOSE,I);
            }
        }
        Playfield.bSwitchDoorImpluse = false;
    }
}


/*----------------------------------------------------------------------------
Name:           IsSteel
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Element Stahleigenschaften hat.
Parameter
      Eingang: uElement, uint16_t, Element, das geprüft wird
      Ausgang: -
Rückgabewert:  bool, true = Element gehört zum Stahl.
Seiteneffekte: -
------------------------------------------------------------------------------*/
bool IsSteel(uint16_t uElement) {

    return ((uElement == EMERALD_STEEL) ||
    (uElement == EMERALD_STEEL_ROUND_PIKE) ||
    (uElement == EMERALD_STEEL_ROUND) ||
    (uElement == EMERALD_STEEL_WARNING) ||
    (uElement == EMERALD_STEEL_BIOHAZARD) ||
    (uElement == EMERALD_STEEL_DEADEND) ||
    (uElement == EMERALD_STEEL_STOP) ||
    (uElement == EMERALD_STEEL_PARKING) ||
    (uElement == EMERALD_STEEL_FORBIDDEN) ||
    (uElement == EMERALD_STEEL_EXIT) ||
    (uElement == EMERALD_STEEL_RADIOACTIVE) ||
    (uElement == EMERALD_STEEL_EXPLOSION) ||
    (uElement == EMERALD_STEEL_ACID) ||
    (uElement == EMERALD_STEEL_NOT_ROUND) ||
    (uElement == EMERALD_STEEL_MARKER_LEFT_UP) ||
    (uElement == EMERALD_STEEL_MARKER_UP) ||
    (uElement == EMERALD_STEEL_MARKER_RIGHT_UP) ||
    (uElement == EMERALD_STEEL_MARKER_LEFT) ||
    (uElement == EMERALD_STEEL_MARKER_RIGHT) ||
    (uElement == EMERALD_STEEL_MARKER_LEFT_BOTTOM) ||
    (uElement == EMERALD_STEEL_MARKER_BOTTOM) ||
    (uElement == EMERALD_STEEL_MARKER_RIGHT_BOTTOM) ||
    (uElement == EMERALD_STEEL_HEART) ||
    (uElement == EMERALD_STEEL_PLAYERHEAD) ||
    (uElement == EMERALD_STEEL_NO_ENTRY) ||
    (uElement == EMERALD_STEEL_GIVE_WAY) ||
    (uElement == EMERALD_STEEL_YING) ||
    (uElement == EMERALD_STEEL_WHEELCHAIR) ||
    (uElement == EMERALD_STEEL_ARROW_DOWN) ||
    (uElement == EMERALD_STEEL_ARROW_UP) ||
    (uElement == EMERALD_STEEL_ARROW_LEFT) ||
    (uElement == EMERALD_STEEL_ARROW_RIGHT) ||
    (uElement == EMERALD_STEEL_INVISIBLE) ||
    (uElement == EMERALD_DOOR_END_NOT_READY_STEEL) ||
    (uElement == EMERALD_DOOR_END_READY_STEEL) ||
    (uElement == EMERALD_STEEL_TRASHCAN) ||
    (uElement == EMERALD_STEEL_JOYSTICK) ||
    (uElement == EMERALD_STEEL_EDIT_LEVEL) ||
    (uElement == EMERALD_STEEL_MOVE_LEVEL) ||
    (uElement == EMERALD_STEEL_COPY_LEVEL) ||
    (uElement == EMERALD_STEEL_CLIPBOARD_LEVEL) ||
    (uElement == EMERALD_STEEL_DC3_IMPORT) ||
    (uElement == EMERALD_STEEL_ADD_LEVELGROUP) ||
    (uElement == EMERALD_REPLICATOR_RED_TOP_LEFT) ||
    (uElement == EMERALD_REPLICATOR_RED_TOP_MID) ||
    (uElement == EMERALD_REPLICATOR_RED_TOP_RIGHT) ||
    (uElement == EMERALD_REPLICATOR_RED_BOTTOM_LEFT) ||
    (uElement == EMERALD_REPLICATOR_RED_BOTTOM_RIGHT) ||
    (uElement == EMERALD_REPLICATOR_RED_SWITCH) ||
    (uElement == EMERALD_REPLICATOR_YELLOW_TOP_LEFT) ||
    (uElement == EMERALD_REPLICATOR_YELLOW_TOP_MID) ||
    (uElement == EMERALD_REPLICATOR_YELLOW_TOP_RIGHT) ||
    (uElement == EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT) ||
    (uElement == EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT) ||
    (uElement == EMERALD_REPLICATOR_YELLOW_SWITCH) ||
    (uElement == EMERALD_MAGIC_WALL_SWITCH) ||
    (uElement == EMERALD_MAGIC_WALL_STEEL) ||
    (uElement == EMERALD_LIGHT_SWITCH) ||
    (uElement == EMERALD_REPLICATOR_GREEN_TOP_LEFT) ||
    (uElement == EMERALD_REPLICATOR_GREEN_TOP_MID) ||
    (uElement == EMERALD_REPLICATOR_GREEN_TOP_RIGHT) ||
    (uElement == EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT) ||
    (uElement == EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT) ||
    (uElement == EMERALD_REPLICATOR_GREEN_SWITCH) ||
    (uElement == EMERALD_REPLICATOR_BLUE_TOP_LEFT) ||
    (uElement == EMERALD_REPLICATOR_BLUE_TOP_MID) ||
    (uElement == EMERALD_REPLICATOR_BLUE_TOP_RIGHT) ||
    (uElement == EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT) ||
    (uElement == EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT) ||
    (uElement == EMERALD_REPLICATOR_BLUE_SWITCH) ||
    (uElement == EMERALD_LIGHTBARRIER_RED_UP) ||
    (uElement == EMERALD_LIGHTBARRIER_RED_DOWN) ||
    (uElement == EMERALD_LIGHTBARRIER_RED_LEFT) ||
    (uElement == EMERALD_LIGHTBARRIER_RED_RIGHT) ||
    (uElement == EMERALD_LIGHTBARRIER_GREEN_UP) ||
    (uElement == EMERALD_LIGHTBARRIER_GREEN_DOWN) ||
    (uElement == EMERALD_LIGHTBARRIER_GREEN_LEFT) ||
    (uElement == EMERALD_LIGHTBARRIER_GREEN_RIGHT) ||
    (uElement == EMERALD_LIGHTBARRIER_BLUE_UP) ||
    (uElement == EMERALD_LIGHTBARRIER_BLUE_DOWN) ||
    (uElement == EMERALD_LIGHTBARRIER_BLUE_LEFT) ||
    (uElement == EMERALD_LIGHTBARRIER_BLUE_RIGHT) ||
    (uElement == EMERALD_LIGHTBARRIER_YELLOW_UP) ||
    (uElement == EMERALD_LIGHTBARRIER_YELLOW_DOWN) ||
    (uElement == EMERALD_LIGHTBARRIER_YELLOW_LEFT) ||
    (uElement == EMERALD_LIGHTBARRIER_YELLOW_RIGHT) ||
    (uElement == EMERALD_LIGHTBARRIER_RED_SWITCH) ||
    (uElement == EMERALD_LIGHTBARRIER_GREEN_SWITCH) ||
    (uElement == EMERALD_LIGHTBARRIER_BLUE_SWITCH) ||
    (uElement == EMERALD_LIGHTBARRIER_YELLOW_SWITCH) ||
    (uElement == EMERALD_ACIDPOOL_TOP_LEFT) ||
    (uElement == EMERALD_ACIDPOOL_TOP_RIGHT) ||
    (uElement == EMERALD_ACIDPOOL_BOTTOM_LEFT) ||
    (uElement == EMERALD_ACIDPOOL_BOTTOM_MID) ||
    (uElement == EMERALD_ACIDPOOL_BOTTOM_RIGHT) ||
    (uElement == EMERALD_WHEEL_TIMEDOOR) ||
    (uElement == EMERALD_SWITCH_SWITCHDOOR) ||
    (uElement == EMERALD_CONVEYORBELT_SWITCH_RED) ||
    (uElement == EMERALD_CONVEYORBELT_SWITCH_GREEN) ||
    (uElement == EMERALD_CONVEYORBELT_SWITCH_BLUE) ||
    (uElement == EMERALD_CONVEYORBELT_SWITCH_YELLOW) ||
    (uElement == EMERALD_SWITCH_REMOTEBOMB_UP) ||
    (uElement == EMERALD_SWITCH_REMOTEBOMB_DOWN) ||
    (uElement == EMERALD_SWITCH_REMOTEBOMB_LEFT) ||
    (uElement == EMERALD_SWITCH_REMOTEBOMB_RIGHT) ||
    (uElement == EMERALD_SWITCH_REMOTEBOMB_IGNITION) ||
    (uElement == EMERALD_STEEL_MODERN_LEFT_END) ||
    (uElement == EMERALD_STEEL_MODERN_LEFT_RIGHT) ||
    (uElement == EMERALD_STEEL_MODERN_RIGHT_END) ||
    (uElement == EMERALD_STEEL_MODERN_UP_END) ||
    (uElement == EMERALD_STEEL_MODERN_UP_DOWN) ||
    (uElement == EMERALD_STEEL_MODERN_DOWN_END) ||
    (uElement == EMERALD_STEEL_MODERN_MIDDLE) ||
    ((uElement >= EMERALD_FONT_STEEL_GREEN_EXCLAMATION) && (uElement <= EMERALD_FONT_STEEL_GREEN_UE)) ||
    ((uElement >= EMERALD_FONT_STEEL_EXCLAMATION) && (uElement <= EMERALD_FONT_STEEL_UE))
    );
}


/*----------------------------------------------------------------------------
Name:           CheckGameDirectorys
------------------------------------------------------------------------------
Beschreibung: Prüft, ob alle benötigten Directorys vorhanden sind und legt diese
              ggf. an.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int CheckGameDirectorys(void) {
    int nErrorCode = -1;

    if (CheckHighScoresDir() == 0) {
        if (CheckAndCreateDir(EMERALD_IMPORTDC3_DIRECTORYNAME) == 0) {
            if (CheckAndCreateDir(EMERALD_LEVELGROUPS_DIRECTORYNAME) == 0) {
                nErrorCode = CheckImportLevelFiles();
            }
        }
    }
    if (nErrorCode != 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Directory problem","Can not create directory!\nPlease check write permissions.",NULL);
    }
    return nErrorCode;
}
