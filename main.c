#define SDL_MAIN_HANDLED // ansonsten kommt folgende Linker-Fehlermeldung: undefined reference to 'WinMain'
#include <SDL2/SDL.h>
#include <math.h>
#include "asteroids.h"
#include "buttons.h"
#include "copper.h"
#include "editor.h"
#include "EmeraldMine.h"
#include "EmeraldMineMainMenu.h"
#include "highscores.h"
#include "KeyboardMouse.h"
#include "loadlevel.h"
#include "modplay.h"
#include "mySDL.h"
#include "mystd.h"
#include "levelconverter.h"
#include "scroller.h"

SDL_DisplayMode ge_DisplayMode;
extern INPUTSTATES InputStates;
extern CONFIG Config;
extern AUDIOPLAYER Audioplayer;

int main(int argc, char *argv[]) {
    bool bRun;
    bool bPrepareExit;
    bool bDrunkenAsteroids = false;
    bool bHasDisplayMode = false;;
    float fAngle2 = 0;
    float fSin2;
    float fAngle3 = 0;
    float fSin3;
    int nBallonSize = 0;
    SDL_Rect DestR_Ballon;
    bool bShowFDLogo = false;
    float fFDLogoSizeFactor = 0;
    float fFDLogoAngle = 0;
    uint32_t uFDLogoLastShown = SDL_GetTicks();
    SDL_Rect DestR_FDLogo;
    uint8_t szMessage1[] = {"PROGRAMMED BY#MIK\"IN SEPTEMBER 2022 - APRIL 2023. MODPLAYER BY MICHAL PROCHAZKA (WWW.PROCHAZKAML.EU). PLEASE WAIT FOR THE ASTEROIDS. PRESS D TO TOGGLE DRUNKEN ASTEROID MODE ....  \
MOD 1 > ECHOING, BY BANANA (CHRISTOF M}HLAN, 1988)   MOD 2 > RIPPED, BY ?, 1990    MOD 3 > CLASS01, BY MAKTONE (MARTIN NORDELL, 1999)   MOD 4 > GLOBAL TRASH 3 V2, BY JESPER KYD, 1991   MOD 5 > CLASS11.TIME FLIES, BY MAKTONE   MOD 6 > 2000AD:CRACKTRO:IV, BY MAKTONE             "};
    uint8_t szMessage2[] = {"PRESS ESC OR LEFT MOUSEBUTTON TO EXIT !   PRESS 1,2,3,4,5 OR 6 TO CHANGE MUSIC !   CHECK THE MOUSE WHEEL TOO ..... PRESS A / B TO CHANGE TEXTURES ..... FONT AND GAME GFX BY PETER ELZNER ... COPPER-EFFECT INSPIRED BY WORLD OF WONDERS      "};
    uint8_t szMessage3[] = {"HALLO SASCHIMANN, ICH HABE NOCHMAL EIN BISSCHEN AN DIESEM SCH|NEN DEMO WEITER GEBAUT. ES SOLLTE DURCH DEN COPPER-EFFEKT NUN NOCH HYPNOTISCHER AUF DEN ZUSEHER WIRKEN. ICH WERDE IN N{CHSTER\
 ZEIT MAL SCHAUEN, OB ICH DAS SPIEL WEITER PORTIERE, BIS JETZT GEHT ES GANZ GUT VON DER HAND. BIS DENN ERSTMAL     9  8  7  6  5  4  3  2  1  0                                                    "};

    uint32_t uLastKeyTime = 0;
    SDL_Window *pWindow = NULL;
    SDL_Renderer *pRenderer = NULL;
    uint32_t uCurrentTicks;
    SCROLLER Scroller1;
    SCROLLER Scroller2;
    SCROLLER Scroller3;
    bool bScroller1Started = false;
    uint32_t uScroller1Timer;
    bool bScroller2Started = false;
    uint32_t uScroller2Timer;
    bool bScroller3Started = false;
    uint32_t uScroller3Timer;
    bool bScroller3Vor = false;
    float fScroller3AlteSteigung;
    float fScroller3NeueSteigung;
    bool bAsteroidsStarted = false;
    uint32_t uAsteroidsTimer;
    int nWindowPosX;
    int nWindowPosY;
    int nWindowDirection = 0;
    int nWindowSpeedX = 1;
    int nWindowSpeedY = 1;
    int nMenuChoose;
    uint32_t uCopperTimer;
    bool bCopperScoll = false;
    uint8_t uModVolume = 100;
    uint8_t uAsteroidsGfx = 0;
    uint8_t uBallonsGfx = 0;

    InitXorShift();
    if (ReadConfigFile() != 0) {            // Konfigurationsfile wird für Fenstergröße bzw. Bildschirmauflösung früh eingelesen.
        return -1;
    }
    if (CheckGameDirectorys() != 0) {
        return -1;
    }
    if (WriteDefaultLevelgroup() != 0) {    // Stellt sicher, dass mindestens eine Levelgruppe vorhanden ist.
        return -1;
    }
    InitButtons();
    InitLevelgroups();
    InitAsteroidLayer();
    if (InitVisibibleCopperSegments() != 0) {
        return -1;
    }
    if (InitInputStates() != 0) {
        return -1;
    }
    pWindow = InitSDL_Window((int)Config.uResX, Config.uResY, WINDOW_TITLE);     // SDL_Init() und Fenster erzeugen
    if (pWindow == NULL) {
        return -1;
    }
    bHasDisplayMode = ((GetDisplayMode(pWindow,&nWindowPosX, &nWindowPosY)) == 0);
    pRenderer = CreateRenderer(pWindow);        // Renderer für erzeugtes Fenster erstellen
    if (pRenderer == NULL) {
        return -1;
    }
    // Audiostruktur initialisieren und Audiodevice öffnen. darf erst nach InitSDL_Window() aufgerufen werden
    if (InitAudioplayerStruct() != 0) {
        return -1;
    }
    // Renderer mit schwarz löschen
    SDL_SetRenderDrawColor(pRenderer,0,0,0,SDL_ALPHA_OPAQUE);
    SDL_RenderClear(pRenderer);
    SDL_RenderPresent(pRenderer);

    if (LoadTextures(pRenderer) != 0) {         // Für alle Bitmaps Texturen erzeugen
        return -1;
    }
    nMenuChoose = Menu(pRenderer);              // Demomenü aufrufen
    if ((nMenuChoose == 3) || (nMenuChoose == -1)) {
        return nMenuChoose;
    }
    if (nMenuChoose == 1) {
        return EmeraldMineMainMenu(pRenderer);         // Emerald-Mine-Hauptmenü mit Spiel
    }
    if (nMenuChoose == 2) {
        SDL_Log("Start SDL2 Demo");
    }
    if (SetModMusic(3) != 0) {        // Mit MOD 3 class_cracktro#15 starten
        return -1;
    }
    SDL_WarpMouseInWindow(pWindow,10,10);
    do {
        UpdateInputStates();
    }  while(InputStates.bQuit);

    // InitScroller(SCROLLER *pScroller, uint32_t uScrollSpeedPixel, int nYpos, uint8_t *pszScrolltext, float fXfreq, float fYfreq, float fYamplitude, float fScale, bool bSinus, bool bSwellFont)
    uScroller1Timer = SDL_GetTicks();
    if (InitScroller(&Scroller1,2,Config.uResY / 2,szMessage1,0.3,0.02,100,1,true,false) != 0) {
        return -1;
    }
    uScroller2Timer = SDL_GetTicks();
    if (InitScroller(&Scroller2,1,0,szMessage2,0,0,0,1,false,false) != 0) {
        return -1;
    }
    uScroller3Timer = SDL_GetTicks();
    if (InitScroller(&Scroller3,2,0,szMessage3,0,0,0,1,false,false) != 0) {
        return -1;
    }
    uAsteroidsTimer = SDL_GetTicks();
    uCopperTimer = SDL_GetTicks();
    //////////////////////////////////////////////////////////////// Hauptschleife START
    bRun = true;
    bPrepareExit = false;
    while (bRun) {
        UpdateInputStates();            // Zustände für Maus und Tastatur auffrischen
        if ((InputStates.bQuit) || (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) || (InputStates.bLeftMouseButton)) {
            bPrepareExit = true;
        } else if (InputStates.nMouseWheelY > 0) {  // scroll up
            if (nBallonSize < 1440) {
                nBallonSize = nBallonSize + 20;
            }
        } else if (InputStates.nMouseWheelY < 0) {  // scroll down
            if (nBallonSize > 20) {
                nBallonSize = nBallonSize - 20;
            }
        }
        // Drunken-Asteroids Modus toggeln
        if (InputStates.pKeyboardArray[SDL_SCANCODE_D]) {
            if (SDL_GetTicks() - uLastKeyTime > 200) {
                bDrunkenAsteroids = !bDrunkenAsteroids;
                SwitchDrunkenAsteroids(bDrunkenAsteroids);
            }
            uLastKeyTime = SDL_GetTicks();
        }
        // Texture Asteroids/Ballons/Smileys toggeln
        if (InputStates.pKeyboardArray[SDL_SCANCODE_A]) {
            if (SDL_GetTicks() - uLastKeyTime > 200) {
                uAsteroidsGfx++;                        // Möglich ist 0 = Asteroide, 1 = Ballons, 2 = Smileys
                if (uAsteroidsGfx > 2) {
                    uAsteroidsGfx = 0;
                }
            }
            uLastKeyTime = SDL_GetTicks();
        }
        // Mittlere Ballon-Grafik  Asteroids/Ballons/Smileys toggeln
        if (InputStates.pKeyboardArray[SDL_SCANCODE_B]) {
            if (SDL_GetTicks() - uLastKeyTime > 200) {
                uBallonsGfx++;                        // Möglich ist 0 = Ballons, 1 = Asteroid, 2 = Smileys
                if (uBallonsGfx > 2) {
                    uBallonsGfx = 0;
                }
            }
            uLastKeyTime = SDL_GetTicks();
        }
        if (InputStates.pKeyboardArray[SDL_SCANCODE_KP_PLUS]) {
            if (uModVolume < 100) {
                uModVolume++;
                SetModVolume(uModVolume);
            }
            SDL_Log("plus, V:%u",uModVolume);

        }
        if (InputStates.pKeyboardArray[SDL_SCANCODE_KP_MINUS]) {
            if (uModVolume > 0) {
                uModVolume--;
                SetModVolume(uModVolume);
            }
            SDL_Log("minus, V:%u",uModVolume);
        }

        DoCopper(pRenderer,bCopperScoll);
        uCurrentTicks = SDL_GetTicks();
        if (uCurrentTicks - uScroller1Timer > 0) {
            bScroller1Started = true;
        }
        if (uCurrentTicks - uCopperTimer > 7000) {
            bCopperScoll = true;
        }
        if (uCurrentTicks - uScroller2Timer > 12000) {
            bScroller2Started = true;
        }
        if (uCurrentTicks - uScroller3Timer > 20000) {
            bScroller3Started = true;
        }

        if (!bScroller3Vor && bScroller3Started) {
            DoScroller(pRenderer,&Scroller3);
        }
        if (bScroller2Started) {
            DoScroller(pRenderer,&Scroller2);
        }
        if (bScroller1Started) {
            DoScroller(pRenderer,&Scroller1);
        }
        // Scroller2 mit Sinus-Verlauf
        fAngle2 = fAngle2 + 0.01;
        fSin2 = sin(fAngle2) * 365;
        Scroller2.nYpos = 370 + (int)fSin2;

        // Scroller3 mit Sinus-Verlauf
        // Steigungsumkehr des sinus 3 für Scroller 3 erkennen -> Steigung für Sinus ist erste Ableitung, also Cosinus
        // hier ist fWinkel3 noch der bisherige Wert
        fScroller3AlteSteigung = cos(fAngle3);
        fAngle3 = fAngle3 + 0.02;
        fScroller3NeueSteigung = cos(fAngle3);
        fSin3 = sin(fAngle3) * 410;
        Scroller3.nYpos = 370 + (int)fSin3;
        if ( ((fScroller3AlteSteigung > 0) && (fScroller3NeueSteigung < 0)) || ((fScroller3AlteSteigung < 0) && (fScroller3NeueSteigung > 0)) ) {
            bScroller3Vor = !bScroller3Vor;     // Scroller 3 umkreist den Rest
        }
        if (uCurrentTicks - uAsteroidsTimer > 35000)
            bAsteroidsStarted = true;
        if (bAsteroidsStarted) {
            if (uAsteroidsGfx == 1) {
                MoveAsteroids(pRenderer,GetTextureByIndex(68));     // Ballons
            } else if (uAsteroidsGfx == 2) {
                MoveAsteroids(pRenderer,GetTextureByIndex(718));    // Smileys
            } else {    // 0
                MoveAsteroids(pRenderer,GetTextureByIndex(69));     // Asteroide
            }
        }
        // Ballons anzeigen
        DestR_Ballon.x = Config.uResX / 2 - nBallonSize / 2;    // Position muss abhängig von Größe
        DestR_Ballon.y = Config.uResY / 2 - nBallonSize / 2;    // sein, damit mittig angezeigt wird
        DestR_Ballon.w = nBallonSize; //360;
        DestR_Ballon.h = nBallonSize; // 360;

        if (uBallonsGfx == 1) {
            SDL_RenderCopy(pRenderer,GetTextureByIndex(69),NULL,&DestR_Ballon); // Asteroide
        } else if (uBallonsGfx == 2) {
            SDL_RenderCopy(pRenderer,GetTextureByIndex(718),NULL,&DestR_Ballon); // Smileys
        } else {
            SDL_RenderCopy(pRenderer,GetTextureByIndex(68),NULL,&DestR_Ballon); // Ballons
        }

        if (bScroller3Vor && bScroller3Started) {
            DoScroller(pRenderer,&Scroller3);
        }

        // Nachdem Scroller2 gestarted ist, die Ballons langsam aufblasen
        if ((bScroller2Started) && (nBallonSize < 360)) {
                nBallonSize++;
        }

        if (bShowFDLogo) {
            uFDLogoLastShown = SDL_GetTicks();
            //                                              Addition sorgt dafür, dass Logo langsam nach rechts diftet
            DestR_FDLogo.x = 500 - fFDLogoSizeFactor * 15 + fFDLogoSizeFactor * 6;    // Position muss abhängig von Größe
            DestR_FDLogo.y = 330 - fFDLogoSizeFactor * 15;    // sein, damit mittig angezeigt wird
            DestR_FDLogo.w = 69 * fFDLogoSizeFactor; // 695
            DestR_FDLogo.h = 31 * fFDLogoSizeFactor; // 313;
            SDL_RenderCopyEx(pRenderer, GetTextureByIndex(70), NULL, &DestR_FDLogo, fFDLogoAngle, NULL, SDL_FLIP_NONE);
            fFDLogoSizeFactor = fFDLogoSizeFactor + 0.03;
            if (fFDLogoSizeFactor > 3.0) {
                fFDLogoAngle = fFDLogoAngle + 0.2;
            }
            if (fFDLogoSizeFactor > 44) {
                bShowFDLogo = false;
                fFDLogoSizeFactor = 0;
                fFDLogoAngle = 0;
            }
            SDL_Log("FDLogoSizeFactor: %f",fFDLogoSizeFactor);
        }
        if ((Audioplayer.nMusicIndex == 3) && (!bShowFDLogo) && (SDL_GetTicks() - uFDLogoLastShown > 60000)) {
            // Bei Mod 4 (Indexierung ab 0, daher == 3) alle 60 Sekunden FD-Logo neu zeigen
            bShowFDLogo = true;
        }

        SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
        SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
        if (bPrepareExit) {
            nBallonSize = nBallonSize + 16; // Ballons bis fast zum Platzen aufblasen
            if (nBallonSize > 1600) {
                bRun = false;
            }
        }

        // MOD wechseln
        CheckMusicSwitch(InputStates.pKeyboardArray);
        if (Audioplayer.nNextMusicIndex != 0) {
            if (Audioplayer.nNextMusicIndex == 4) {     // Bei MOD 4 FD-Logo starten
                bShowFDLogo = true;
            }
            if (SDL_GetQueuedAudioSize(Audioplayer.audio_device) == 0) {    // Etwas warten, bis Callback-Funktion nicht mehr aufgerufen wird
                if (SetModMusic(Audioplayer.nNextMusicIndex) != 0) {
                    return -1;
                }
                uModVolume = 100;
            }
        } else  {
            PlayMusic();
        }
        // Fenster-Verschiebung im Drunken-Asteroids Modus
        if ((bHasDisplayMode) && (bDrunkenAsteroids)) {
            SDL_GetWindowPosition(pWindow,&nWindowPosX,&nWindowPosY);
            if (nWindowDirection == 0) {    // Rechts runter
                nWindowPosX = nWindowPosX + nWindowSpeedX;
                nWindowPosY = nWindowPosY + nWindowSpeedY;
                if (nWindowPosX > ge_DisplayMode.w - Config.uResX - 40) {    // Fenster stößt rechts an (- 40 wegen Fensterrahmen)
                    SDL_Log("%s: [rechts/runter] -> Fenster stoesst rechts an",__FUNCTION__);
                    nWindowDirection = 3;
                    nWindowSpeedX = (int)randf(1.1,5.1);
                    nWindowSpeedY = (int)randf(1.1,5.1);
                } else if (nWindowPosY > ge_DisplayMode.h - Config.uResY - 40) { // Fenster stößt unten an, -40 wegen Fensterrhamen
                    SDL_Log("%s: [rechts/runter] -> Fenster stoesst unten an",__FUNCTION__);
                    nWindowDirection = 1;
                    nWindowSpeedX = (int)randf(1.1,5.1);
                    nWindowSpeedY = (int)randf(1.1,5.1);
                }
            } else if (nWindowDirection == 1) { // Rechts hoch
                nWindowPosX = nWindowPosX + nWindowSpeedX;
                nWindowPosY = nWindowPosY - nWindowSpeedY;
                if (nWindowPosX > ge_DisplayMode.w - Config.uResX - 40) {   // Fenster stößt rechts an, - 40 wegen Fensterrahmen
                    SDL_Log("%s: [rechts/hoch] -> Fenster stoesst rechts an",__FUNCTION__);
                    nWindowDirection = 2;
                    nWindowSpeedX = (int)randf(1.1,5.1);
                    nWindowSpeedY = (int)randf(1.1,5.1);
                } else if  (nWindowPosY < 30) {      // Fenster stößt oben an, 30 statt 0 wegen Fensterrahmen
                    SDL_Log("%s: [rechts/hoch] -> Fenster stoesst oben an",__FUNCTION__);
                    nWindowDirection = 0;
                    nWindowSpeedX = (int)randf(1.1,5.1);
                    nWindowSpeedY = (int)randf(1.1,5.1);
                }
            } else if (nWindowDirection == 2) { // Links hoch
                nWindowPosX = nWindowPosX - nWindowSpeedX;
                nWindowPosY = nWindowPosY - nWindowSpeedY;
                if (nWindowPosX < 60) {                // Fenster stößt links an, 60 statt 0 wegen Fensterrahmen
                    SDL_Log("%s: [links/hoch] -> Fenster stoesst links an",__FUNCTION__);
                    nWindowDirection = 1;
                    nWindowSpeedX = (int)randf(1.1,5.1);
                    nWindowSpeedY = (int)randf(1.1,5.1);
                } else if (nWindowPosY < 30) {       // Fenster stößt oben an, 30 statt 0 wegen Fensterrahmen
                    SDL_Log("%s: [links/hoch] -> Fenster stoesst oben an",__FUNCTION__);
                    nWindowDirection = 3;
                    nWindowSpeedX = (int)randf(1.1,5.1);
                    nWindowSpeedY = (int)randf(1.1,5.1);
                }
            } else {                            // Links runter
                nWindowPosX = nWindowPosX - nWindowSpeedX;
                nWindowPosY = nWindowPosY + nWindowSpeedY;
                if (nWindowPosX < 60) {              // Fenster stößt links an, 60 statt 0 wegen Fensterrahmen
                    SDL_Log("%s: [links/runter] -> Fenster stoesst links an",__FUNCTION__);
                    nWindowDirection = 0;
                    nWindowSpeedX = (int)randf(1.1,5.1);
                    nWindowSpeedY = (int)randf(1.1,5.1);
                } else if (nWindowPosY > ge_DisplayMode.h - Config.uResY - 40) { // Fenster stößt unten an, - 40 wegen Fensterrahmen
                    SDL_Log("%s: [links/runter] -> Fenster stoesst unten an",__FUNCTION__);
                    nWindowDirection = 2;
                    nWindowSpeedX = (int)randf(1.1,5.1);
                    nWindowSpeedY = (int)randf(1.1,5.1);
                }
            }
            SDL_SetWindowPosition(pWindow,nWindowPosX,nWindowPosY);
        }
    }
    //////////////////////////////////////////////////////////////// Hauptschleife ENDE
    SDL_CloseAudioDevice(Audioplayer.audio_device);
    SAFE_FREE(Audioplayer.pTheMusic);
    FreeTextures();
    FreeScroller(&Scroller1);
    FreeScroller(&Scroller2);
    FreeScroller(&Scroller3);
    FreeCopper();
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
    return 0;
}
