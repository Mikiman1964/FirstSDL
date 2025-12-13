/*
TODO
* doppeltes Rollen ("tanzen") der Elemente vermeiden, wenn diese nicht auf Laufband liegen, DC3 hat gleiches Problem
* Leveleditor
    * Undo für Editor
* Tasten für Gamespeed und Quicksave konfigurierbar
* Fehler bei Quicksave/Quickload? Nach Load hatte Spieler plötzlich 2 weiße Schlüssel

Für V 1.13
* Hint im Leveleditor wird nach 1 statt 3 Sekunden angezeigt
* Fehler beim Quick-Load/Save im Zusammenhang mit Pipes gefixt
* 1 Frame mehr Zeit für EMERALD_WHEEL_TIMEDOOR -> Playfield.uTimeDoorTimeLeft =  Playfield.uTimeDoorTime + 1 in man.c
* Anzeige für Anzahl YAMS + 1
* Leveleditor:Maschinen: Animierte Replikatoren und Laufbänder
* Leveleditor: Hinweis bei Spielzeit = 0 = INFINITE
* Variablennamen: Conveybelt -> Conveyorbelt
* Gesamtanzahl Levels werden im Hauptmenü angezeigt
* Quicksave/Quicload funktionierte auch, wenn das im Level nicht erlaubt ist.
* Quicksave verhindern, wenn Man tot.
* Bessere Lesbarkeit in Statuszeile durch größere Ziffern/Beschriftung (Arcade font)
* Datenstruktur POSTANIMATION aufgelöst
*/

#include "gfx/textures.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include "alien.h"
#include "beetle.h"
#include "bomb.h"
#include "buttons_checkboxes.h"
#include "crystal.h"
#include "drop.h"
#include "earth.h"
#include "EmeraldMine.h"
#include "EmeraldMineMainMenu.h"
#include "editor.h"
#include "emerald.h"
#include "explosion.h"
#include "GetTextureIndexByElement.h"
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
#include "quicksaveloadgame.h"
#include "RenderLevel.h"
#include "replicator.h"
#include "ruby.h"
#include "sapphire.h"
#include "smileys.h"
#include "slime.h"
#include "sound.h"
#include "steel_wall_grow.h"
#include "stone.h"
#include "teleporter.h"
#include "yam.h"

PLAYFIELD Playfield;
int32_t g_nGameSpeed;
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
extern VIDEO Video;
extern FPS Fps;


/*----------------------------------------------------------------------------
Name:           Menu
------------------------------------------------------------------------------
Beschreibung: Hauptmenü, um den entsprechenden SDL2-Programmteil aufzurufen.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int32_t, 1 = Game, 2 = SDL2-Demo, 3 = Quit
Seiteneffekte: Playfield.x für FrameCounter, Audioplayer.x, MainMenu.x,
               Video.x, Fps.x, ManKey.x
------------------------------------------------------------------------------*/
int32_t Menu(SDL_Renderer *pRenderer) {
    uint8_t uMusicVolume;
    int32_t nErrorCode;
    int32_t nChoose;
    int32_t nColorDimm;
    float fAngle;
    SDL_Rect DestR;
    uint32_t uFrameCounter;
    uint32_t I;
    uint32_t uTextureIndex;
    uint32_t uPositionsAndElements[] = {// 1. Zeile
        352,64,EMERALD_FONT_BLUE_W,384,64,EMERALD_FONT_BLUE_E,416,64,EMERALD_FONT_BLUE_L,448,64,EMERALD_FONT_BLUE_C,480,64,EMERALD_FONT_BLUE_O,512,64,EMERALD_FONT_BLUE_M,544,64,EMERALD_FONT_BLUE_E,
        608,64,EMERALD_FONT_BLUE_T,640,64,EMERALD_FONT_BLUE_O,
        80,128,EMERALD_FONT_BLUE_APOSTROPHE,112,128,EMERALD_FONT_BLUE_M,144,128,EMERALD_FONT_BLUE_Y,208,128,EMERALD_FONT_BLUE_F,240,128,EMERALD_FONT_BLUE_I,272,128,EMERALD_FONT_BLUE_R,304,128,EMERALD_FONT_BLUE_S,
        336,128,EMERALD_FONT_BLUE_T,400,128,EMERALD_FONT_BLUE_S,432,128,EMERALD_FONT_BLUE_D,464,128,EMERALD_FONT_BLUE_L,496,128,EMERALD_FONT_BLUE_2,560,128,EMERALD_FONT_BLUE_A,592,128,EMERALD_FONT_BLUE_P,
        624,128,EMERALD_FONT_BLUE_P,656,128,EMERALD_FONT_BLUE_L,688,128,EMERALD_FONT_BLUE_I,720,128,EMERALD_FONT_BLUE_C,752,128,EMERALD_FONT_BLUE_A,784,128,EMERALD_FONT_BLUE_T,816,128,EMERALD_FONT_BLUE_I,
        848,128,EMERALD_FONT_BLUE_O,880,128,EMERALD_FONT_BLUE_N,912,128,EMERALD_FONT_BLUE_APOSTROPHE,
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
        144,672,EMERALD_FONT_BLUE_P,176,672,EMERALD_FONT_BLUE_R,208,672,EMERALD_FONT_BLUE_O,240,672,EMERALD_FONT_BLUE_G,272,672,EMERALD_FONT_BLUE_R,304,672,EMERALD_FONT_BLUE_A,336,672,EMERALD_FONT_BLUE_M,368,672,EMERALD_FONT_BLUE_M,
        400,672,EMERALD_FONT_BLUE_E,432,672,EMERALD_FONT_BLUE_D,496,672,EMERALD_FONT_BLUE_I,528,672,EMERALD_FONT_BLUE_N,592,672,EMERALD_FONT_BLUE_2,624,672,EMERALD_FONT_BLUE_0,656,672,EMERALD_FONT_BLUE_2,688,672,EMERALD_FONT_BLUE_2,
        720,672,EMERALD_FONT_BLUE_MINUS,752,672,EMERALD_FONT_BLUE_2,784,672,EMERALD_FONT_BLUE_0,816,672,EMERALD_FONT_BLUE_2,848,672,EMERALD_FONT_BLUE_5
    };

    InitSmileys();
    uFrameCounter = 0;
    nErrorCode = 0;
    nChoose = -1;
    // Buttons erzeugen
    nErrorCode = nErrorCode + CreateButton(BUTTONLABEL_CALL_GAME,"Try the game",320,228,true,false);
    nErrorCode = nErrorCode + CreateButton(BUTTONLABEL_CALL_DEMO,"SDL2 Demo",320,292,true,false);
    nErrorCode = nErrorCode + CreateButton(BUTTONLABEL_CALL_QUIT,"Quit program",320,580,true,false);
    nErrorCode = nErrorCode + SetMusic(5);
    SDL_PauseAudioDevice(Audioplayer.audio_device, 0);
    uMusicVolume = 0;
    SetMusicVolume(uMusicVolume);
    nColorDimm = 0;
    SetAllTextureColors(nColorDimm);

    memset(&Fps,0,sizeof(Fps));
    while (((nErrorCode == 0) && (nChoose == -1)) || (nColorDimm > 0) ) {
        MeasureFps();
        MoveSmileys(pRenderer);
        UpdateManKey();
        if ((nChoose == -1) && (nColorDimm < 100)) {
            nColorDimm++;
            SetAllTextureColors(nColorDimm);
            uMusicVolume++;
            SetMusicVolume(uMusicVolume);
        }
        PlayMusic(true);
        for (I = 0; (I < sizeof(uPositionsAndElements) / (sizeof(uint32_t) * 3)) && (nErrorCode == 0); I++) {
            uTextureIndex = GetTextureIndexByElement(uPositionsAndElements[I * 3 + 2],uFrameCounter % 16,&fAngle,false,EMERALD_CONVEYORBELT_OFF);
            DestR.x = 128 + Video.uXoffs + uPositionsAndElements[I * 3 + 0];
            DestR.y = Video.uYoffs + uPositionsAndElements[I * 3 + 1];
            DestR.w = FONT_W;
            DestR.h = FONT_H;
            if (nErrorCode == 0) {
                if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle,NULL, SDL_FLIP_NONE) != 0) {
                    nErrorCode = -1;
                    SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                }
            }
        }
        PrintLittleFont(pRenderer,448,232,0,"LEFT CONTROL = FIRE, FIRE CAN BE USED WITH CURSOR DIRECTION.",K_RELATIVE,1);
        PrintLittleFont(pRenderer,448,247,0,"F1 = NORMAL SPEED, F2 = FAST SPEED, F9 = QUICK SAVE, F10 = QUICK LOAD",K_RELATIVE,1);
        PrintLittleFont(pRenderer,448,296,0,"USE THE FOLLOWING KEYS:",K_RELATIVE,1);
        PrintLittleFont(pRenderer,468,311,0,"* ESC OR LEFT MOUSEBUTTON TO EXIT",K_RELATIVE,1);
        PrintLittleFont(pRenderer,468,326,0,"* MOUSEWHEEL TO ZOOM BALLONS",K_RELATIVE,1);
        PrintLittleFont(pRenderer,468,341,0,"* 'A' / 'B' TO TOGGLE TEXTURE FOR BALLONS, ASTEROIDS AND SMILEYS",K_RELATIVE,1);
        PrintLittleFont(pRenderer,468,356,0,"* 'D' TO TOGGLE 'DRUNKEN ASTEROIDS' MODE",K_RELATIVE,1);
        PrintLittleFont(pRenderer,468,371,0,"* '1' FOR MUSIC 1 -> ECHOING BY BANANA (CHRISTOF M\x63HLAN), 1988",K_RELATIVE,1);
        PrintLittleFont(pRenderer,468,386,0,"* '2' FOR MUSIC 2 -> CIRCUS TIME 2 VOYCE/DELIGHT, 1993",K_RELATIVE,1);
        PrintLittleFont(pRenderer,468,401,0,"* '3' FOR MUSIC 3 -> CLASS15 BY MAKTONE (MARTIN NORDELL), 1999",K_RELATIVE,1);
        PrintLittleFont(pRenderer,468,416,0,"* '4' FOR MUSIC 4 -> GLOBAL TRASH 3 V2 BY JESPER KYD, 1991",K_RELATIVE,1);
        PrintLittleFont(pRenderer,468,431,0,"* '5' FOR MUSIC 5 -> CLASS11.TIME FLIES BY MAKTONE",K_RELATIVE,1);
        PrintLittleFont(pRenderer,468,446,0,"* '6' FOR MUSIC 6 -> 2000AD:CRACKTRO:IV BY MAKTONE",K_RELATIVE,1);
        PrintLittleFont(pRenderer,468,461,0,"* '7' FOR MUSIC 7 -> 2000AD:CRACKTRO02 BY MAKTONE",K_RELATIVE,1);
        PrintLittleFont(pRenderer,468,476,0,"* '8' FOR MUSIC 8 -> BREWERY BY MAKTONE",K_RELATIVE,1);
        PrintLittleFont(pRenderer,468,491,0,"* '9' FOR MUSIC 9 -> CLASS05 BY MAKTONE, 1999",K_RELATIVE,1);
        PrintLittleFont(pRenderer,468,506,0,"* '0' FOR MUSIC 0 -> SOFTWORLD BY OXYGENER/MAKTONE",K_RELATIVE,1);
        PrintLittleFont(pRenderer,448,584,0,"NUFF SAID",K_RELATIVE,1);
        // Framerate anzeigen
        if (strlen(Fps.szFrameaPerSecond) > 0) {
            PrintLittleFont(pRenderer,32,744,0,Fps.szFrameaPerSecond,K_RELATIVE,1);
            PrintLittleFont(pRenderer,33,745,3,Fps.szFrameaPerSecond,K_RELATIVE,1);
        }
        nErrorCode = ShowButtons(pRenderer,K_RELATIVE);
        if (IsButtonPressed(BUTTONLABEL_CALL_GAME) || (ManKey.bFire)) {
            nChoose = 1;
        } else if (IsButtonPressed(BUTTONLABEL_CALL_DEMO)) {
            nChoose = 2;
        } else if ((IsButtonPressed(BUTTONLABEL_CALL_QUIT)) || (InputStates.bQuit) || (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE])) {
            nChoose = 3;
        }
        if ((nChoose != -1) && (nColorDimm > 0)) {
            nColorDimm--;
            SetAllTextureColors(nColorDimm);
            uMusicVolume--;
            SetMusicVolume(uMusicVolume);
        }
        RenderPresentAndClear(pRenderer);
        uFrameCounter++;
        Playfield.uFrameCounter++;
    }
    // Buttons freigeben
    FreeButton(BUTTONLABEL_CALL_GAME);
    FreeButton(BUTTONLABEL_CALL_DEMO);
    FreeButton(BUTTONLABEL_CALL_QUIT);
    SetAllTextureColors(100);
    SetMusicVolume(100);
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

Rückgabewert:  int32_t, 0 = kein Fehler, sonst Fehler
Seiteneffekte: Playfield.x, InputStates.x, ManKey.x, GameSound.x, g_nGameSpeed
------------------------------------------------------------------------------*/
int32_t RunGame(SDL_Renderer *pRenderer, uint32_t uLevel) {
    bool bLevelRun;
    bool bPrepareLevelExit;
    bool bPause;
    bool bDelay;
    int32_t nColorDimm;
    int32_t nCheckLevelCount;
    uint32_t uManDirection = EMERALD_ANIM_STAND;     // Rückgabe von CheckLevel() -> Wohin ist der Man gelaufen?
    uint32_t uKey;
    uint32_t I;
    bool bDimmIn = true;
    uint32_t uQuitTime;
    int32_t nRet;
    uint32_t uShowQuickSaveMessageTicks;

    uShowQuickSaveMessageTicks = 0xFFFFFFFF;
    bPause = false;
    bDelay = false;
    nRet = 0;
    nColorDimm = 0;
    FillCheeseRandomNumbers();
    bLevelRun = (InitialisePlayfield(uLevel) == 0);
    // Renderer mit schwarz löschen
    SDL_SetRenderDrawColor(pRenderer,0,0,0,SDL_ALPHA_OPAQUE);
    RenderPresentAndClear(pRenderer);
    bPrepareLevelExit = false;
    if (bLevelRun) {
        SetPipeLevel();
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
    SetTreasureChestStart();
    ClearFkeys();
    // Playfield.bQuicksaveAllowed = true;
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
            if ((uQuitTime != 0xFFFFFFFF) && ((SDL_GetTicks() - uQuitTime) > 1000) && ((ManKey.bFire) || (InputStates.pKeyboardArray[SDL_SCANCODE_SPACE])))   {
                bPrepareLevelExit = true;
            }
        }
        GetFkeys();

        if ((InputStates.pKeyboardArray[SDL_SCANCODE_P]) || ((bPause) && (ManKey.bFire))) {
            bPause = !bPause;
            if (!bPause) {
                bDimmIn = true; // Nach Pause wieder aufdimmen
            }
            WaitNoSpecialKey(SDL_SCANCODE_P);   // warten, dass "P" wieder losgelassen wird
        }

        if ((nCheckLevelCount == 0) && (!bPause)) {
            if (InputStates.bFkey[1]) {
                SetGameSpeed(GAMESPEED_NORMAL);
                InputStates.bFkey[1] = false;
                bDelay = false;
            } else if (InputStates.bFkey[2]) {
                SetGameSpeed(GAMESPEED_FAST);
                InputStates.bFkey[2] = false;
                bDelay = false;
            } else if (InputStates.bFkey[3]) {
                SetGameSpeed(GAMESPEED_NORMAL);
                InputStates.bFkey[3] = false;
                bDelay = true;
            } else if (InputStates.bFkey[9]) {
                if ((Playfield.bQuicksaveAllowed) && (!Playfield.bManDead)) {
                    QuickSaveGame();
                }
                InputStates.bFkey[9] = false;
                uShowQuickSaveMessageTicks = SDL_GetTicks();
            } else if (InputStates.bFkey[10]) {
                if (IsQuickLoadGameAvailable()) {
                    InputStates.bFkey[10] = false;
                    // Aktuelles Playfield abdimmen
                    while (nColorDimm > 0) {
                        UpdateManKey();
                        if (nColorDimm > 2) {
                            nColorDimm = nColorDimm - 2;
                        } else {
                            nColorDimm = 0;
                        }
                        SetAllTextureColors(nColorDimm);
                        RenderLevel(pRenderer,&Playfield.nTopLeftXpos,&Playfield.nTopLeftYpos,nCheckLevelCount);
                        RenderPresentAndClear(pRenderer);
                    }
                    QuickLoadGame();
                    // Neues Playfield aufdimmen
                    while (nColorDimm < 100) {
                        UpdateManKey();
                        if (nColorDimm < 98) {
                            nColorDimm = nColorDimm + 2;
                        } else {
                            nColorDimm = 100;
                        }
                        SetAllTextureColors(nColorDimm);
                        RenderLevel(pRenderer,&Playfield.nTopLeftXpos,&Playfield.nTopLeftYpos,nCheckLevelCount);
                        RenderPresentAndClear(pRenderer);
                    }
                }
            }
            if ((ManKey.uDirection == MANKEY_NONE) && ((Playfield.uFrameCounter - ManKey.uLastDirectionFrameCount) <= (15 * (g_nGameSpeed + 1)))) {
                // SDL_Log("%s: use buffered key: dir: %u   dif:%u",__FUNCTION__,ManKey.uLastActiveDirection,Playfield.uFrameCounter - ManKey.uLastDirectionFrameCount);
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
            // SDL_Log("Y: %u",Playfield.uManYpos);
            for (I = 0; I < (g_nGameSpeed + 1); I++) {
                ScrollAndCenterLevel(uManDirection);
                CheckRunningWheel();
                CheckRunningMagicWall();
                CheckLight();
                CheckTimeDoorOpen();
                CheckPlayTime();    // erhöht auch Playfield.uFrameCounter
            }
        } else {
            // Pause abdimmen
            if (nColorDimm > 20) {
                nColorDimm = nColorDimm - 2;
                SetAllTextureColors(nColorDimm);
            }
        }
        RenderLevel(pRenderer,&Playfield.nTopLeftXpos,&Playfield.nTopLeftYpos,nCheckLevelCount);  // nCheckLevelCount 0 ... 15
        if (uShowQuickSaveMessageTicks != 0xFFFFFFFF) {
            if (SDL_GetTicks() - uShowQuickSaveMessageTicks < 5000) {
                if (Playfield.bQuicksaveAllowed) {
                    if (Playfield.bManDead) {
                        CreateMessageWindow(pRenderer, 10,10, 0, "YOU ARE DEAD! GAME NOT SAVED",NULL,NULL);
                    } else {
                        CreateMessageWindow(pRenderer, 10,10, 0, "GAME SAVED",NULL,NULL);
                    }
                } else {
                    CreateMessageWindow(pRenderer, 10,10, 0, "QUICKSAVE NOT ALLOWED",NULL,NULL);
                }
            } else {
                uShowQuickSaveMessageTicks = 0xFFFFFFFF;
            }
        }
        if (!bPause) {
            if (g_nGameSpeed == GAMESPEED_FAST) {
                nCheckLevelCount = nCheckLevelCount + 2;
            } else {
                nCheckLevelCount++;
            }
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
        RenderPresentAndClear(pRenderer);
        if (bDelay) {
            SDL_Delay(24);
        }
    }
    SDL_ShowCursor(SDL_ENABLE);    // Mauspfeil verstecken
    Playfield.uPlayTimeEnd = SDL_GetTicks();
    SetAllTextureColors(100);           // Farben beim Verlassen wieder auf volle Helligekit
    SAFE_FREE(Playfield.pLevel);
    SAFE_FREE(Playfield.pPipeLevel);
    SAFE_FREE(Playfield.pInvalidElement);
    SAFE_FREE(Playfield.pSlimeElement);
    SAFE_FREE(Playfield.pStatusAnimation);
    SAFE_FREE(Playfield.pLastStatusAnimation);
    SAFE_FREE(Playfield.pLastYamSlimeDirection);
    FreeTeleporterCoordinates();
    for (I = 0; I < EMERALD_MAX_MESSAGES; I++) {
        SAFE_FREE(Playfield.pMessage[I]);
    }
    WaitNoKey();
    return 0;
}


/*----------------------------------------------------------------------------
Name:           SetElementToNextPosition
------------------------------------------------------------------------------
Beschreibung: Setzt ein Element auf die nächste Position. Die nächste Position ist abhängig von uStatusAnimation.
Parameter
      Eingang: I, uint32_t, aktuelle Position/lineare Koordinate
               uStatusAnimation, uint32_t, Animation/Status
               uNextStatusAnimation, uint32_t, Animation-, Clean-, Self-Status für die nächste Position
               uInvalidElement, uint16_t, Element, das auf die nächste Position verschoben werden soll
      Ausgang: -

Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void SetElementToNextPosition(uint32_t I,uint32_t uStatusAnimation, uint32_t uNextStatusAnimation, uint16_t uInvalidElement) {
    uint32_t uNextPos;

    switch (uStatusAnimation) {
        case (EMERALD_ANIM_RIGHT):
            uNextPos = I + 1;
            break;
        case (EMERALD_ANIM_LEFT):
            uNextPos = I - 1;
            break;
        case (EMERALD_ANIM_UP):
            uNextPos = I - Playfield.uLevel_X_Dimension;
            break;
        case (EMERALD_ANIM_DOWN):
            uNextPos = I + Playfield.uLevel_X_Dimension;
            break;
        default:
            SDL_Log("%s: WARNING: bad Status/Animation at position %u with element : 0x%04x",__FUNCTION__,I,uInvalidElement);
            return;     // Nichts machen, schwerer Fehler!
    }
    // Animation für aktuelles Element setzen
    Playfield.pStatusAnimation[I] = uStatusAnimation;
    // neuen Platz mit ungültigem Element besetzen
    Playfield.pLevel[uNextPos] = EMERALD_INVALID;
    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
    Playfield.pInvalidElement[uNextPos] = uInvalidElement;
    Playfield.pLastStatusAnimation[uNextPos] = uStatusAnimation;
    Playfield.pStatusAnimation[uNextPos] = uNextStatusAnimation;
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
    bool     bSlime;

    uManDirection = EMERALD_ANIM_STAND;
    Playfield.bSwitchRemoteBombLeft = false;
    Playfield.bSwitchRemoteBombRight = false;
    Playfield.bSwitchRemoteBombDown = false;
    Playfield.bSwitchRemoteBombUp = false;
    Playfield.bSwitchRemoteBombIgnition = false;
    Playfield.bRemoteBombMoved = false;
    // Ab hier das Level und den Status für alle Elemente aus voriger Steuerung vorbereiten
    for (I = 0; I < Playfield.uLevel_XY_Dimension; I++) {
        // Dieser Block sorgt bei bewegten Objekten dafür, dass diese
        // ihren neuen Platz (invalides Feld) einnehmen.
        bSlime = (Playfield.pInvalidElement[I] == EMERALD_SLIME);
        uAnimStatus = Playfield.pStatusAnimation[I] & 0x0000FFFF00;
        if (Playfield.pLevel[I] == EMERALD_INVALID) {
            uCleanStatus = Playfield.pStatusAnimation[I] & 0x00FF0000;
             // Wird für grüne Tropfen und Schleim verwendet
            if (uCleanStatus != EMERALD_ANIM_CLEAN_NOTHING) {
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
                    uCleanElement = EMERALD_EARTH_MOLE;
                } else {
                    uCleanElement = EMERALD_SPACE;
                }
                switch (uCleanStatus) {
                    case (EMERALD_ANIM_CLEAN_UP):
                        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = uCleanElement;
                        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                        if (bSlime) {
                            Playfield.pSlimeElement[I - Playfield.uLevel_X_Dimension] = EMERALD_NONE;
                        }
                        break;
                    case (EMERALD_ANIM_CLEAN_RIGHT):
                        Playfield.pLevel[I + 1] = uCleanElement;
                        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_STAND;
                        if (bSlime) {
                            Playfield.pSlimeElement[I + 1] = EMERALD_NONE;
                        }
                        break;
                    case (EMERALD_ANIM_CLEAN_DOWN):
                        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = uCleanElement;
                        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                        if (bSlime) {
                            Playfield.pSlimeElement[I + Playfield.uLevel_X_Dimension] = EMERALD_NONE;
                        }
                        break;
                    case (EMERALD_ANIM_CLEAN_LEFT):
                        Playfield.pLevel[I - 1] = uCleanElement;
                        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_STAND;
                        if (bSlime) {
                            Playfield.pSlimeElement[I - 1] = EMERALD_NONE;
                        }
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
                        SDL_Log("%s: invalid clean status: %x, animstatus: %x, bSlime: %d",__FUNCTION__,uCleanStatus,Playfield.pStatusAnimation[I],bSlime);
                        break;
                }
            }
        } else if (Playfield.pLevel[I] == EMERALD_ACIDPOOL_DESTROY) {
            // EMERALD_ACIDPOOL_DESTROY muss im Vorwege "behandelt" werden, da sonst Elemente auf EMERALD_ACIDPOOL_DESTROY fallen können.
            // Mole down muss beim Säürebecken gesondert behandelt werden, da beim Gang ins Säurebecken noch Erde über dem Becken erzeugt werden muss
            if (Playfield.pInvalidElement[I] == EMERALD_MOLE_DOWN) {
                Playfield.pLevel[I] = EMERALD_EARTH_MOLE;
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_MOLE_GOES_ACID;
            } else {
                Playfield.pLevel[I] = EMERALD_SPACE;
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            }
        }
        // Dieser Block sorgt bei drehenden Objekten (Minen+Käfer+Mole) dafür, dass diese
        // nach der Drehung in die richtige Richtung zeigen.
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
                ((uCleanStatus == EMERALD_ANIM_EARTH_SHRINK) && (Playfield.pLevel[I] == EMERALD_EARTH)) ||
                ((uCleanStatus == EMERALD_ANIM_EARTH_INVISIBLE_SHRINK) && (Playfield.pLevel[I] == EMERALD_EARTH_INVISIBLE)) ||
                ((uCleanStatus == EMERALD_ANIM_GRASS_SHRINK) && (Playfield.pLevel[I] == EMERALD_GRASS)) ||
                ((uCleanStatus == EMERALD_ANIM_EMERALD_SHRINK) && (Playfield.pLevel[I] == EMERALD_EMERALD)) ||
                ((uCleanStatus == EMERALD_ANIM_RUBY_SHRINK) && (Playfield.pLevel[I] == EMERALD_RUBY)) ||
                ((uCleanStatus == EMERALD_ANIM_PERL_SHRINK) && (Playfield.pLevel[I] == EMERALD_PERL)) ||
                ((uCleanStatus == EMERALD_ANIM_CRYSTAL_SHRINK) && (Playfield.pLevel[I] == EMERALD_CRYSTAL)) ||
                ((uCleanStatus == EMERALD_ANIM_TIME_COIN_SHRINK) && (Playfield.pLevel[I] == EMERALD_TIME_COIN)) ||
                ((uCleanStatus == EMERALD_ANIM_SHIELD_COIN_SHRINK) && (Playfield.pLevel[I] == EMERALD_SHIELD_COIN)) ||
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
            Playfield.pLastStatusAnimation[I] = EMERALD_ANIM_STAND;
            Playfield.pLastYamSlimeDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
            Playfield.pSlimeElement[I] = EMERALD_NONE;
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
    ControlPreElements();   // ggf. Spaces für Elemente einsetzen, die sich auflösen, Molen-Erde in normale Erde wandeln
    // Man als Zweites steuern !
    uManDirection = ControlMan(Playfield.uManYpos * Playfield.uLevel_X_Dimension + Playfield.uManXpos,uDirection);
    //SDL_Log("Man protected: %d",Playfield.bManProtected);
    ControlLightBarriers();
    if (Playfield.uDynamitePos != 0xFFFFFFFF) {
        ControlManWithDynamiteOn(Playfield.uDynamitePos);
    }
    for (I = 0; I < Playfield.uLevel_XY_Dimension; I++) {
        uLevelElement = Playfield.pLevel[I];
        switch (uLevelElement) {
            case (EMERALD_SLIME):
                ControlSlime(I);
                break;
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
                ControlCentralExplosion(I,EMERALD_SPACE);             // Normale 3x3 Explosion
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
            case (EMERALD_EARTH_MOLE):
                // Wird vorher in ControlPreElements() durchgeführt
                break;
            case (EMERALD_MINE_CONTACT):
                ControlContactMine(I);
                break;
            case (EMERALD_GREEN_DROP_COMES):
            case (EMERALD_YELLOW_DROP_COMES):
                ControlSpreadCheese(I);
                break;
            case (EMERALD_GREEN_CHEESE):
            case (EMERALD_YELLOW_CHEESE):
                ControlCheese(I);
                break;
            case (EMERALD_GREEN_DROP):
            case (EMERALD_YELLOW_DROP):
                ControlDrop(I);
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
            case (EMERALD_QUICKSAND_STONE):
                ControlQuicksandStone(I);
                break;
            case (EMERALD_STONE_SINK):
                ControlStoneSink(I);
                break;
            case (EMERALD_STONE_SAG_SLOW):
                ControlStoneSagSlow(I);
                break;
            case (EMERALD_QUICKSAND_STONE_SLOW):
                ControlQuicksandStoneSlow(I);
                break;
            case (EMERALD_STONE_SINK_SLOW):
                ControlStoneSinkSlow(I);
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
            case (EMERALD_EARTH):
                ControlEarth(I);
                break;
            case (EMERALD_EARTH_INVISIBLE):
                ControlEarthInvisible(I);
                break;
            case (EMERALD_MINE_EARTH):
                ControlEarthMine(I);
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
                if (Playfield.uConveyorbeltRedState != EMERALD_CONVEYORBELT_OFF) {
                    PreparePlaySound(SOUND_CONVEYORBELT,I);
                }
                break;
            case (EMERALD_CONVEYORBELT_YELLOW):
                if (Playfield.uConveyorbeltYellowState != EMERALD_CONVEYORBELT_OFF) {
                    PreparePlaySound(SOUND_CONVEYORBELT,I);
                }
                break;
            case (EMERALD_CONVEYORBELT_GREEN):
                if (Playfield.uConveyorbeltGreenState != EMERALD_CONVEYORBELT_OFF) {
                    PreparePlaySound(SOUND_CONVEYORBELT,I);
                }
                break;
            case (EMERALD_CONVEYORBELT_BLUE):
                if (Playfield.uConveyorbeltBlueState != EMERALD_CONVEYORBELT_OFF) {
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
    if (Playfield.uShieldCoinTimeLeft > 0) {
        if (Playfield.uShieldCoinTimeLeft == 1) {
            PreparePlaySound(SOUND_SHIELD_END,0);
        }
        Playfield.uShieldCoinTimeLeft--;
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
               nAnimationCount, int32_t, Animationsschritt
      Ausgang: pfAngle, float *, Winkel für Texture;
Rückgabewert:  uint32_t , Texture, wenn keine Texture ermittelt werden kann, wird
                SPACE (EMERALD_SPACE) zurückgegeben.
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t GetTextureIndexByElementForAcidPool(uint16_t uElement,int32_t nAnimationCount, float *pfAngle) {
    uint32_t uTextureIndex;
    uint32_t K;
    float fAngle = 0;

    switch (uElement) {
        case (EMERALD_GREEN_DROP):
            uTextureIndex = TEX_DROP_GREEN;
            break;
        case (EMERALD_YELLOW_DROP):
            uTextureIndex = TEX_DROP_YELLOW;
            break;
        case (EMERALD_EMERALD):
            uTextureIndex = TEX_EMERALD_1 + nAnimationCount / 2;     // Emerald, liegend
            break;
        case (EMERALD_RUBY):
            uTextureIndex = TEX_RUBY_1 + nAnimationCount / 2;     // Rubin, liegend
            break;
        case (EMERALD_CRYSTAL):
            uTextureIndex = TEX_CRYSTAL;                            // Kristall, liegend
            break;
        case (EMERALD_SAPPHIRE):
            uTextureIndex = TEX_SAPPHIRE_1 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
            break;
        case (EMERALD_NUT):
            uTextureIndex = TEX_NUT;
            break;
        case (EMERALD_STONE):
            uTextureIndex = TEX_STONE;
            break;
        case (EMERALD_YAM):
            K = Playfield.uFrameCounter % 11;               // Y von 0 bis 10
            if (K <= 5) {                                   // 0,1,2,3,4,5
                uTextureIndex = TEX_YAM_MOVE_1 + K;         // 362 - 367
            } else {                                        // 6,7,8,9,10
                uTextureIndex = TEX_YAM_MOVE_6 + 5 - K;     // 366 - 362
            }
            break;
        case (EMERALD_SLIME):
            uTextureIndex = TEX_SLIME;
            break;
        case (EMERALD_ALIEN):
            if ((nAnimationCount >= 4) && (nAnimationCount <= 11)) {
                uTextureIndex = TEX_ALIEN_MOVE_1;                        // Alien geht 1
            } else {
                uTextureIndex = TEX_ALIEN_MOVE_2;                        // Alien geht 2, Flügel voll ausgebreitet
            }
            break;
        case (EMERALD_MOLE_UP):
        case (EMERALD_MOLE_RIGHT):
        case (EMERALD_MOLE_DOWN):
        case( EMERALD_MOLE_LEFT):
            fAngle = 270;
            uTextureIndex = TEX_MOLE_01 + Playfield.uFrameCounter % 11;     // Mole links
            break;
        case (EMERALD_MINE_UP):
        case (EMERALD_MINE_RIGHT):
        case (EMERALD_MINE_DOWN):
        case( EMERALD_MINE_LEFT):
            fAngle = 270;
            if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                uTextureIndex = TEX_MINE_LEFT_OFF;     // Mine links
            } else {
                uTextureIndex = TEX_MINE_LEFT_ON;     // Mine links an
            }
            break;
        case (EMERALD_BEETLE_UP):
        case (EMERALD_BEETLE_RIGHT):
        case (EMERALD_BEETLE_DOWN):
        case (EMERALD_BEETLE_LEFT):
            uTextureIndex = TEX_BEETLE_LEFT_1 + nAnimationCount % 8;     // Käfer links
            fAngle = 270;
            break;
        case (EMERALD_MAN):
            uTextureIndex = TEX_MAN_DOWN_1 + nAnimationCount % 8;     // Man runter
            break;
        case (EMERALD_BOMB):
            uTextureIndex = TEX_BOMB_1 + nAnimationCount % 8;
            break;
        case (EMERALD_MEGABOMB):
            uTextureIndex = TEX_BOMB_MEGA_1 + ((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 5;
            break;
        case (EMERALD_PERL):
            uTextureIndex = TEX_PERL_1 + nAnimationCount % 8;
            break;
        case (EMERALD_REMOTEBOMB):
            uTextureIndex = TEX_BOMB_REMOTE;
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
    Playfield.uRollUnderground[EMERALD_STEEL] = 0xEB;                                   // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_GROW_LEFT] = 0xEB;                         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_GROW_RIGHT] = 0xEB;                        // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_GROW_UP] = 0xEB;                           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_GROW_DOWN] = 0xEB;                         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_GROW_LEFT_RIGHT] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_GROW_UP_DOWN] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_GROW_ALL] = 0xEB;                          // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_GROW_LEFT] = 0xEB;                          // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_GROW_RIGHT] = 0xEB;                         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_GROW_UP] = 0xEB;                            // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_GROW_DOWN] = 0xEB;                          // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_GROW_LEFT_RIGHT] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_GROW_UP_DOWN] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_GROW_ALL] = 0xEB;                           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_CORNERED] = 0xEB;                           // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_WALL_INVISIBLE] = 0xEB;                          // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_MODERN_LEFT_END] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_MODERN_LEFT_RIGHT] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_MODERN_RIGHT_END] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_MODERN_UP_END] = 0xEB;                     // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_MODERN_UP_DOWN] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_MODERN_DOWN_END] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_MODERN_MIDDLE] = 0xEB;                     // Nur Steine und Bomben rollen hier nicht herunter
    // Bei DC3 rollt nichts von den Schaltern für ferngesteuerte Bombe
    Playfield.uRollUnderground[EMERALD_SWITCH_REMOTEBOMB_UP] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_SWITCH_REMOTEBOMB_DOWN] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_SWITCH_REMOTEBOMB_LEFT] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_SWITCH_REMOTEBOMB_RIGHT] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_SWITCH_REMOTEBOMB_IGNITION] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_REMOTEBOMB] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_WHEEL] = 0x1FF;
    Playfield.uRollUnderground[EMERALD_ACIDPOOL_TOP_LEFT] = 0x1FF;                      // nicht bei DC3
    Playfield.uRollUnderground[EMERALD_ACIDPOOL_TOP_RIGHT] = 0x1FF;                     // nicht bei DC3
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
    Playfield.uRollUnderground[EMERALD_STEEL_WARNING] = 0xEB;                           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_BIOHAZARD] = 0xEB;                         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_DEADEND] = 0xEB;                           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_STOP] = 0xEB;                              // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_PARKING] = 0xEB;                           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_FORBIDDEN] = 0xEB;                         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_EXIT] = 0xEB;                              // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_RADIOACTIVE] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_EXPLOSION] = 0xEB;                         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_ACID] = 0xEB;                              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_HEART] = 0xEB;                             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_PLAYERHEAD] = 0xEB;                        // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_PLAYERHEAD_2] = 0xEB;                        // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_NO_ENTRY] = 0xEB;                          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_GIVE_WAY] = 0xEB;                          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_YING] = 0xEB;                              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_WHEELCHAIR] = 0xEB;                        // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_ARROW_DOWN] = 0xEB;                        // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_ARROW_UP] = 0xEB;                          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_ARROW_LEFT] = 0xEB;                        // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_ARROW_RIGHT] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_INVISIBLE] = 0xEB;                         // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_STRIPE_LEFT_TOP] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_STRIPE_TOP] = 0xEB;                        // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_STRIPE_RIGHT_TOP] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_STRIPE_LEFT] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_STRIPE_RIGHT] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_STRIPE_LEFT_BOTTOM] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_STRIPE_BOTTOM] = 0xEB;                     // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_STRIPE_RIGHT_BOTTOM] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_GROW_RIGHT] = 0xEB;                        // Nur Steine und Bomben rollen hier nicht herunter
	// Playfield.uRollUnderground[EMERALD_STEEL_GROWING_RIGHT] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_GROW_LEFT] = 0xEB;                         // Nur Steine und Bomben rollen hier nicht herunter
	// Playfield.uRollUnderground[EMERALD_STEEL_GROWING_LEFT] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_GROW_UP] = 0xEB;                           // Nur Steine und Bomben rollen hier nicht herunter
	// Playfield.uRollUnderground[EMERALD_STEEL_GROWING_UP] = 0xEB;                     // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_STEEL_GROW_DOWN] = 0xEB;                         // Nur Steine und Bomben rollen hier nicht herunter
    // Playfield.uRollUnderground[EMERALD_STEEL_GROWING_DOWN] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_WALL_GROW_RIGHT] = 0xEB;                         // Nur Steine und Bomben rollen hier nicht herunter
	// Playfield.uRollUnderground[EMERALD_WALL_GROWING_RIGHT] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_WALL_GROW_LEFT] = 0xEB;                          // Nur Steine und Bomben rollen hier nicht herunter
	// Playfield.uRollUnderground[EMERALD_WALL_GROWING_LEFT] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_WALL_GROW_UP] = 0xEB;                            // Nur Steine und Bomben rollen hier nicht herunter
	// Playfield.uRollUnderground[EMERALD_WALL_GROWING_UP] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_WALL_GROW_DOWN] = 0xEB;                          // Nur Steine und Bomben rollen hier nicht herunter
    // Playfield.uRollUnderground[EMERALD_WALL_GROWING_DOWN] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_EXCLAMATION] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_ARROW_RIGHT] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_ARROW_UP] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_ARROW_DOWN] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_APOSTROPHE] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_BRACE_OPEN] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_BRACE_CLOSE] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_COPYRIGHT] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_PLUS] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_COMMA] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_MINUS] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_DOT] = 0xEB;                     // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_SLASH] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_0] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_1] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_2] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_3] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_4] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_5] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_6] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_7] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_8] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_9] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_DOUBLE_DOT] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_PLATE] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_ARROW_LEFT] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_QUESTION_MARK] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_A] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_B] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_C] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_D] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_E] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_F] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_G] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_H] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_I] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_J] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_K] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_L] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_M] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_N] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_O] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_P] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_Q] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_R] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_S] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_T] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_U] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_V] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_W] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_X] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_Y] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_Z] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_AE] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_OE] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_BLUE_STEEL_UE] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_EXCLAMATION] = 0xEB;            // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_ARROW_RIGHT] = 0xEB;            // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_ARROW_UP] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_ARROW_DOWN] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_APOSTROPHE] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_BRACE_OPEN] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_BRACE_CLOSE] = 0xEB;            // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_COPYRIGHT] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_PLUS] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_COMMA] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_MINUS] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_DOT] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_SLASH] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_0] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_1] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_2] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_3] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_4] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_5] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_6] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_7] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_8] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_9] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_DOUBLE_DOT] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_PLATE] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_ARROW_LEFT] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_QUESTION_MARK] = 0xEB;          // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_A] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_B] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_C] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_D] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_E] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_F] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_G] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_H] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_I] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_J] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_K] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_L] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_M] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_N] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_O] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_P] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_Q] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_R] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_S] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_T] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_U] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_V] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_W] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_X] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_Y] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_Z] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_AE] = 0xEB;                     // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_OE] = 0xEB;                     // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_FONT_GREEN_STEEL_UE] = 0xEB;                     // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_EMERALD] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_RUBY] = 0xEB;                          // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_SAPPHIRE] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_PERL] = 0xEB;                          // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_KEY_RED] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_KEY_GREEN] = 0xEB;                     // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_KEY_BLUE] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_KEY_YELLOW] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_KEY_WHITE] = 0xEB;                     // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_KEY_GENERAL] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_BOMB] = 0xEB;                          // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_MEGABOMB] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_STONE] = 0xEB;                         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_NUT] = 0xEB;                           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_WHEEL] = 0xEB;                         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_DYNAMITE] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_ENDDOOR] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_ENDDOOR_READY] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_MINE_UP] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_MOLE_UP] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_GREEN_CHEESE] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_YELLOW_CHEESE] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_BEETLE_UP] = 0xEB;                     // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_YAM] = 0xEB;                           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_ALIEN] = 0xEB;                         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_CRYSTAL] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_TIME_COIN] = 0xEB;                     // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_WALL_WITH_SHIELD_COIN] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_RED_TOP_LEFT] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_RED_TOP_RIGHT] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_GREEN_TOP_LEFT] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_GREEN_TOP_RIGHT] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_YELLOW_TOP_LEFT] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_YELLOW_TOP_RIGHT] = 0xEB;             // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_BLUE_TOP_LEFT] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_DOOR_ONLY_UP_STEEL] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_DOOR_ONLY_DOWN_STEEL] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_DOOR_ONLY_LEFT_STEEL] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_DOOR_ONLY_RIGHT_STEEL] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_RED_SWITCH] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_GREEN_SWITCH] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_BLUE_SWITCH] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_YELLOW_SWITCH] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_RED_SWITCH] = 0xEB;                   // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_YELLOW_SWITCH] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_GREEN_SWITCH] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_REPLICATOR_BLUE_SWITCH] = 0xEB;                  // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_LIGHT_SWITCH] = 0xEB;                            // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_MAGIC_WALL_SWITCH] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_SWITCH_SWITCHDOOR] = 0xEB;                       // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_CONVEYORBELT_SWITCH_RED] = 0xEB;                 // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_CONVEYORBELT_SWITCH_GREEN] = 0xEB;               // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_CONVEYORBELT_SWITCH_BLUE] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_CONVEYORBELT_SWITCH_YELLOW] = 0xEB;              // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_DOOR_END_NOT_READY_STEEL] = 0xEB;                // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_DOOR_END_READY_STEEL] = 0xEB;                    // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_DOOR_END_READY] = 0xEB;                          // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_DOOR_END_NOT_READY] = 0xEB;                      // Nur Steine und Bomben rollen hier nicht herunter
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_RED_UP] = 0x1FF;                    // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_RED_DOWN] = 0x1FF;                  // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_RED_LEFT] = 0x1FF;                  // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_RED_RIGHT] = 0x1FF;                 // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_GREEN_UP] = 0x1FF;                  // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_GREEN_DOWN] = 0x1FF;                // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_GREEN_LEFT] = 0x1FF;                // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_GREEN_RIGHT] = 0x1FF;               // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_BLUE_UP] = 0x1FF;                   // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_BLUE_DOWN] = 0x1FF;                 // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_BLUE_LEFT] = 0x1FF;                 // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_BLUE_RIGHT] = 0x1FF;                // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_YELLOW_UP] = 0x1FF;                 // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_YELLOW_DOWN] = 0x1FF;               // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_YELLOW_LEFT] = 0x1FF;               // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_LIGHTBARRIER_YELLOW_RIGHT] = 0x1FF;              // Alles rollt von Lichtschranken
	Playfield.uRollUnderground[EMERALD_TIME_COIN] = 0x1FF;                              // Alles rollt von Münzen
	Playfield.uRollUnderground[EMERALD_SHIELD_COIN] = 0x1FF;                            // Alles rollt von Münzen
	Playfield.uRollUnderground[EMERALD_TREASURECHEST_1] = 0x1FF;                        // Alles rollt von Schatztruhen
	Playfield.uRollUnderground[EMERALD_TREASURECHEST_2] = 0x1FF;                        // Alles rollt von Schatztruhen
	Playfield.uRollUnderground[EMERALD_TREASURECHEST_3] = 0x1FF;                        // Alles rollt von Schatztruhen
	Playfield.uRollUnderground[EMERALD_TREASURECHEST_4] = 0x1FF;                        // Alles rollt von Schatztruhen
	Playfield.uRollUnderground[EMERALD_TREASURECHEST_5] = 0x1FF;                        // Alles rollt von Schatztruhen
	Playfield.uRollUnderground[EMERALD_TREASURECHEST_6] = 0x1FF;                        // Alles rollt von Schatztruhen
	Playfield.uRollUnderground[EMERALD_TREASURECHEST_7] = 0x1FF;                        // Alles rollt von Schatztruhen
	Playfield.uRollUnderground[EMERALD_TREASURECHEST_8] = 0x1FF;                        // Alles rollt von Schatztruhen
    Playfield.uRollUnderground[EMERALD_STEEL_STRIPE_CORNER_LEFT_TOP] = 0xEB;            // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_STRIPE_CORNER_RIGHT_TOP] = 0xEB;           // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_STRIPE_CORNER_LEFT_BOTTOM] = 0xEB;         // Nur Steine und Bomben rollen hier nicht herunter
    Playfield.uRollUnderground[EMERALD_STEEL_STRIPE_CORNER_RIGHT_BOTTOM] = 0xEB;        // Nur Steine und Bomben rollen hier nicht herunter
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
    if (IS_SPACE(I - 1) && (IS_SPACE(I + Playfield.uLevel_X_Dimension - 1) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension - 1] == EMERALD_ACIDPOOL)) ) {
        uFreeDirs = uFreeDirs | 0x01;
    }
    // Kann Element rechts rollen?
    if (IS_SPACE(I + 1) && (IS_SPACE(I + Playfield.uLevel_X_Dimension + 1) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension + 1] == EMERALD_ACIDPOOL)) ) {
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
            uTextureIndex = TEX_MESSAGE;
            break;
        case (EMERALD_ANIM_KEY_RED_SHRINK):
            uTextureIndex = TEX_KEY_RED;
            break;
        case (EMERALD_ANIM_KEY_GREEN_SHRINK):
            uTextureIndex = TEX_KEY_GREEN;
            break;
        case (EMERALD_ANIM_KEY_BLUE_SHRINK):
            uTextureIndex = TEX_KEY_BLUE;
            break;
        case (EMERALD_ANIM_KEY_YELLOW_SHRINK):
            uTextureIndex = TEX_KEY_YELLOW;
            break;
        case (EMERALD_ANIM_KEY_WHITE_SHRINK):
            uTextureIndex = TEX_KEY_WHITE;
            break;
        case (EMERALD_ANIM_KEY_GENERAL_SHRINK):
            uTextureIndex = TEX_KEY_GENERAL;
            break;
        case (EMERALD_ANIM_EARTH_SHRINK):
            uTextureIndex = TEX_EARTH_LEFT_RIGHT_TOP_BOTTOM;
            break;
        case (EMERALD_ANIM_EARTH_INVISIBLE_SHRINK):
            uTextureIndex = TEX_EARTH_INVISIBLE_LEFT_RIGHT_TOP_BOTTOM;
            break;
        case (EMERALD_ANIM_GRASS_SHRINK):
            uTextureIndex = TEX_GRASS_LEFT_RIGHT_TOP_BOTTOM;
            break;
        case (EMERALD_ANIM_EMERALD_SHRINK):
            uTextureIndex = TEX_EMERALD_1;
            break;
        case (EMERALD_ANIM_RUBY_SHRINK):
            uTextureIndex = TEX_RUBY_1;
            break;
        case (EMERALD_ANIM_PERL_SHRINK):
            uTextureIndex = TEX_PERL_1;
            break;
        case (EMERALD_ANIM_CRYSTAL_SHRINK):
            uTextureIndex = TEX_CRYSTAL;
            break;
        case (EMERALD_ANIM_SAPPHIRE_SHRINK):
            uTextureIndex = TEX_SAPPHIRE_1;
            break;
        case (EMERALD_ANIM_TIME_COIN_SHRINK):
            uTextureIndex = TEX_COIN_TIME_1;
            break;
        case (EMERALD_ANIM_SHIELD_COIN_SHRINK):
            uTextureIndex = TEX_COIN_SHIELD_1;
            break;
        case (EMERALD_ANIM_HAMMER_SHRINK):
            uTextureIndex = TEX_HAMMER;
            break;
        case (EMERALD_ANIM_DYNAMITE_SHRINK):
            uTextureIndex = TEX_DYNAMITE_OFF;
            break;
        default:
            uTextureIndex = TEX_SPACE; // Space
            SDL_Log("%s: Animation (%d) not found",__FUNCTION__,uShrinkAnimation);
            break;
    }
    return uTextureIndex;
}


/*----------------------------------------------------------------------------
Name:           ControlPreElements
------------------------------------------------------------------------------
Beschreibung: Erzeugt ggf. Spaces für Elemente, die sich auflösen.
              Molen-Erde wird in normale Erde gewandelt.
              Schaltet Schalttüren ggf. um.
              Geknackte Nüsse in Emerald wandeln
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlPreElements(void) {
    uint32_t I;

    for (I = 0; I < Playfield.uLevel_XY_Dimension; I++) {
        if (IS_SPACE(I)) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;     // setzt ggf. EMERALD_ANIM_BLOCK_MAN zurück
        } else if ( (Playfield.pLevel[I] == EMERALD_GREEN_CHEESE_GOES) || (Playfield.pLevel[I] == EMERALD_YELLOW_CHEESE_GOES) ||
             ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_SINK_IN_MAGIC_WALL) ||   // Für Elemente, die im Magic Wall eintauchen
             (Playfield.pStatusAnimation[I] == EMERALD_ANIM_PERL_BREAK) ) {                         // Zerbrechende Perle
           Playfield.pLevel[I] = EMERALD_SPACE;           // Enstehenden Tropfen in Tropfen wandeln.
           Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        } else if (Playfield.pLevel[I] == EMERALD_EARTH_MOLE) {
            if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_MOLE_GOES_ACID) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            } else {
                Playfield.pLevel[I] = EMERALD_EARTH;     // Molen-Erde in normale Erde wandeln
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
        } else if ((Playfield.pLevel[I] == EMERALD_NUT) && ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_NUT_CRACK2)) {
            Playfield.pLevel[I] = EMERALD_EMERALD;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        } else if (((Playfield.pLevel[I]) == EMERALD_SLIME) && ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_SLIME_CLEAN)) {
            Playfield.pLevel[I] = EMERALD_SPACE;           // Schleim, der in der 2. Phase in eine Explosion gelaufen ist.
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
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
        for (I = 0; I < Playfield.uLevel_XY_Dimension; I++) {
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
Name:           CheckGameDirectorys
------------------------------------------------------------------------------
Beschreibung: Prüft, ob alle benötigten Directorys vorhanden sind und legt diese
              ggf. an.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int32_t, 0 = Alles OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int32_t CheckGameDirectorys(void) {
    int32_t nErrorCode = -1;

    if (CheckHighScoresDir() == 0) {
        if (CheckAndCreateDir(EMERALD_IMPORTDC3_DIRECTORYNAME) == 0) {
            nErrorCode = CheckAndCreateDir(EMERALD_LEVELGROUPS_DIRECTORYNAME);
        }
    }
    if (nErrorCode != 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Directory problem","Can not create directory!\nPlease check write permissions.",NULL);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           SetGameSpeed
------------------------------------------------------------------------------
Beschreibung: Setzt die Spielgeschwindigkeit. Die Geschwindigkeit sollte/darf nur
              außerhalb eines Animationsablaufes geändert werden.
Parameter
      Eingang: nGameSpeed, int32_t, zur Zeit sind folgende Geschwindigkeiten möglich:
                    GAMESPEED_NORMAL (2 Pixel / Frame)
                    GAMESPEED_FAST   (4 Pixel / Frame)
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: g_nGameSpeed
------------------------------------------------------------------------------*/
void SetGameSpeed(int32_t nGameSpeed) {
    g_nGameSpeed = nGameSpeed;
}
