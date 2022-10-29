#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include "EmeraldMine.h"
#include "loadlevel.h"
#include "alien.h"
#include "mine.h"
#include "beetle.h"
#include "man.h"
#include "sand.h"
#include "mySDL.h"
#include "mystd.h"
#include "KeyboardMouse.h"


PLAYFIELD Playfield;
extern SDL_DisplayMode ge_DisplayMode;
extern INPUTSTATES InputStates;
extern MANKEY ManKey;
extern uint8_t ge_uLevel[];

/*----------------------------------------------------------------------------
Name:           RunGame
------------------------------------------------------------------------------
Beschreibung: Hauptschleifen-Funktion für das Spielen eines Levels.
Parameter
      Eingang: pWindow, SDL_Window, Fenster-Handle
               pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -

Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: Playfield.x, InputStates, ManKey.x, ge_uLevel[]
------------------------------------------------------------------------------*/
int RunGame(SDL_Window *pWindow, SDL_Renderer *pRenderer) {
    bool bLevelRun;
    bool bPrepareLevelExit;
    int nColorDimm;
    int nCheckLevelCount;
    nCheckLevelCount = 0;
    nColorDimm = 0;
    uint32_t uManDirection = EMERALD_ANIM_STAND;     // Rückgabe von CheckLevel() -> Wohin ist der Man gelaufen?
    uint32_t uKey;
    bool bDimmIn = true;

    SetAllTextureColors(0);
    if (InitialisePlayfield(ge_uLevel) != 0) {
        exit(0);
    }
    // Renderer mit schwarz löschen
    SDL_SetRenderDrawColor(pRenderer,0,0,0,SDL_ALPHA_OPAQUE);
    SDL_RenderClear(pRenderer);
    SDL_RenderPresent(pRenderer);

    bLevelRun = true;
    bPrepareLevelExit = false;

    while (bLevelRun) {
        UpdateManKey();
        if ((InputStates.pKeyboardArray[SDL_SCANCODE_SPACE]) || (InputStates.bQuit) || (Playfield.uTimeToPlay == 0)) {
            bPrepareLevelExit = true;
        }
        if (InputStates.pKeyboardArray[SDL_SCANCODE_P]) {
            PrintPlayfieldValues();
        }
        if (nCheckLevelCount == 0) {
            if ((ManKey.uDirection == MANKEY_NONE) && ((Playfield.uFrameCounter - ManKey.uLastDirectionFrameCount) <= 15)) {
                SDL_Log("%s: use buffered key: dir: %u   dif:%u",__FUNCTION__,ManKey.uLastActiveDirection,Playfield.uFrameCounter - ManKey.uLastDirectionFrameCount);
                uKey = ManKey.uLastActiveDirection;
            } else {
                uKey = ManKey.uDirection;
            }
            uManDirection = ControlGame(uKey);
        }
        ScrollAndCenterLevel(uManDirection);
        RenderLevel(pRenderer,&Playfield.nXpos,&Playfield.nYpos,nCheckLevelCount);  // nCheckLevelCount 0 ... 15
        nCheckLevelCount++;
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
            nColorDimm = nColorDimm + 1;
            if (nColorDimm >= 100) {
                nColorDimm = 100;
                bDimmIn = false;;
            }
            SetAllTextureColors(nColorDimm);
        }
        SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
        SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
        // SDL_Delay(50);
    }
    SetAllTextureColors(100);           // Farben beim Verlassen wieder auf volle Helligekit
    SAFE_FREE(Playfield.pLevel);
    SAFE_FREE(Playfield.pStatusAnimation);

    do {
        UpdateInputStates();
        SDL_Delay(2);
    } while (InputStates.bQuit);


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
    uint8_t uLevelElement;
    uint32_t AnimStatus;
    uint32_t CleanStatus;

    uManDirection = EMERALD_ANIM_STAND;
    for (I = 0; I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension; I++) {
        // Dieser Block sorgt bei bewegten Objekten dafür, dass diese
        // ihren neuen Platz (invalides Feld) einnehmen.
        if (Playfield.pLevel[I] == EMERALD_INVALID) {
            CleanStatus = Playfield.pStatusAnimation[I] & 0x00FF0000;
            Playfield.pLevel[I] = Playfield.pStatusAnimation[I] & 0xFF; // von invalides auf nächstes Element setzen
            // Neue Element-Koordinaten des Man berechnen
            if (Playfield.pLevel[I] == EMERALD_MAN) {
                Playfield.uManXpos = I % Playfield.uLevel_X_Dimension;
                Playfield.uManYpos = I / Playfield.uLevel_X_Dimension;
            }
            switch (CleanStatus) {
                case (EMERALD_ANIM_CLEAN_UP):
                    Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_SPACE;
                    Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = 0x00;
                    break;
                case (EMERALD_ANIM_CLEAN_RIGHT):
                    Playfield.pLevel[I + 1] = EMERALD_SPACE;
                    Playfield.pStatusAnimation[I + 1] = 0x00;
                    break;
                case (EMERALD_ANIM_CLEAN_DOWN):
                    Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_SPACE;
                    Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = 0x00;
                    break;
                case (EMERALD_ANIM_CLEAN_LEFT):
                    Playfield.pLevel[I - 1] = EMERALD_SPACE;
                    Playfield.pStatusAnimation[I - 1] = 0x00;
                    break;
                case (EMERALD_ANIM_CLEAN_UP_DOUBLESPEED):
                    Playfield.pLevel[I - 2 * Playfield.uLevel_X_Dimension] = EMERALD_SPACE;
                    Playfield.pStatusAnimation[I - 2 * Playfield.uLevel_X_Dimension] = 0x00;
                    break;
                case (EMERALD_ANIM_CLEAN_RIGHT_DOUBLESPEED):
                    Playfield.pLevel[I + 2] = EMERALD_SPACE;
                    Playfield.pStatusAnimation[I + 2] = 0x00;
                    break;
                case (EMERALD_ANIM_CLEAN_DOWN_DOUBLESPEED):
                    Playfield.pLevel[I + 2 * Playfield.uLevel_X_Dimension] = EMERALD_SPACE;
                    Playfield.pStatusAnimation[I + 2 * Playfield.uLevel_X_Dimension] = 0x00;
                    break;
                case (EMERALD_ANIM_CLEAN_LEFT_DOUBLESPEED):
                    Playfield.pLevel[I - 2] = EMERALD_SPACE;
                    Playfield.pStatusAnimation[I - 2] = 0x00;
                    break;
                default:
                    SDL_Log("%s: invalid clean status  %x",__FUNCTION__,CleanStatus);
                    break;
            }
        }
        // Dieser Block sorgt bei drehenden Objekten (Minen+Käfer) dafür, dass diese
        // nach der Drehung in die richtige Richtung zeigen.
        AnimStatus = Playfield.pStatusAnimation[I] & 0x0000FF00;
        if ((AnimStatus == EMERALD_ANIM_SPIN_UP_TO_RIGHT) || (AnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_RIGHT)) {
            // Ab hier auf Mine oder Käfer prüfen
            if ((Playfield.pLevel[I] == EMERALD_MINE_UP) || (Playfield.pLevel[I] == EMERALD_MINE_DOWN)) {
                Playfield.pLevel[I] = EMERALD_MINE_RIGHT;
            } else if ((Playfield.pLevel[I] == EMERALD_BEETLE_UP) || (Playfield.pLevel[I] == EMERALD_BEETLE_DOWN)) {
                Playfield.pLevel[I] = EMERALD_BEETLE_RIGHT;
            }
        } else if ((AnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_DOWN) || (AnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_DOWN)) {
            // Ab hier auf Mine oder Käfer prüfen
            if ((Playfield.pLevel[I] == EMERALD_MINE_RIGHT) || (Playfield.pLevel[I] == EMERALD_MINE_LEFT)) {
                Playfield.pLevel[I] = EMERALD_MINE_DOWN;
            } else if ((Playfield.pLevel[I] == EMERALD_BEETLE_RIGHT) || (Playfield.pLevel[I] == EMERALD_BEETLE_LEFT)) {
                Playfield.pLevel[I] = EMERALD_BEETLE_DOWN;
            }

        } else if ((AnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_LEFT) || (AnimStatus == EMERALD_ANIM_SPIN_UP_TO_LEFT)) {
            // Ab hier auf Mine oder Käfer prüfen
            if ((Playfield.pLevel[I] == EMERALD_MINE_DOWN) || (Playfield.pLevel[I] == EMERALD_MINE_UP)) {
                Playfield.pLevel[I] = EMERALD_MINE_LEFT;
            } else if ((Playfield.pLevel[I] == EMERALD_BEETLE_DOWN) || (Playfield.pLevel[I] == EMERALD_BEETLE_UP)) {
                Playfield.pLevel[I] = EMERALD_BEETLE_LEFT;
            }
        } else if ((AnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_UP) || (AnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_UP)) {
            // Ab hier auf Mine oder Käfer prüfen
            if ((Playfield.pLevel[I] == EMERALD_MINE_LEFT) || (Playfield.pLevel[I] == EMERALD_MINE_RIGHT)) {
                Playfield.pLevel[I] = EMERALD_MINE_UP;
            } else if ((Playfield.pLevel[I] == EMERALD_BEETLE_LEFT) || (Playfield.pLevel[I] == EMERALD_BEETLE_RIGHT)) {
                Playfield.pLevel[I] = EMERALD_BEETLE_UP;
            }
        }
        // "Selbststeuernde" Elemente nicht zurückensetzen, aber den Rest
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF000000;
    }
    for (I = 0; I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension; I++) {
        uLevelElement = Playfield.pLevel[I];
        switch (uLevelElement)
        {
            case (EMERALD_SAND):
                ControlSand(I);
                break;
            case (EMERALD_REPLICATOR_RED_TOP_MID):
                ControlRedReplikator(I);
                break;
            case (EMERALD_MAN):
                uManDirection = ControlMan(I,uDirection);
                break;
            case (EMERALD_MINE_UP):
                ControlMineUp(I);
                break;
            case (EMERALD_MINE_RIGHT):
                ControlMineRight(I);
                break;
            case (EMERALD_MINE_DOWN):
                ControlMineDown(I);
                break;
            case (EMERALD_MINE_LEFT):
                ControlMineLeft(I);
                break;
            case (EMERALD_BEETLE_UP):
                ControlBeetleUp(I);
                break;
            case (EMERALD_BEETLE_RIGHT):
                ControlBeetleRight(I);
                break;
            case (EMERALD_BEETLE_DOWN):
                ControlBeetleDown(I);
                break;
            case (EMERALD_BEETLE_LEFT):
                ControlBeetleLeft(I);
                break;
            case (EMERALD_ALIEN):
                ControlAlien(I);
                break;
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
                Playfield.nYpos -= 2;
            }
            break;
        case (EMERALD_ANIM_RIGHT):
            if (Playfield.uManXpos >= Playfield.uVisibleCenterX) {
                Playfield.nXpos += 2;
            }
            break;
        case (EMERALD_ANIM_DOWN):
            if (Playfield.uManYpos >= Playfield.uVisibleCenterY) {
                Playfield.nYpos += 2;
            }
            break;
        case (EMERALD_ANIM_LEFT):
            if (Playfield.uManXpos < (Playfield.uLevel_X_Dimension - Playfield.uVisibleCenterX)) {
                Playfield.nXpos -= 2;
            }
            break;
        case (EMERALD_ANIM_UP_DOUBLESPEED):
            if (Playfield.uManYpos < (Playfield.uLevel_Y_Dimension - Playfield.uVisibleCenterY)) {
                Playfield.nYpos -= 4;
            }
            break;
        case (EMERALD_ANIM_RIGHT_DOUBLESPEED):
            if (Playfield.uManXpos >= Playfield.uVisibleCenterX) {
                Playfield.nXpos += 4;
            }
            break;
        case (EMERALD_ANIM_DOWN_DOUBLESPEED):
            if (Playfield.uManYpos >= Playfield.uVisibleCenterY) {
                Playfield.nYpos += 4;
            }
            break;
        case (EMERALD_ANIM_LEFT_DOUBLESPEED):
            if (Playfield.uManXpos < (Playfield.uLevel_X_Dimension - Playfield.uVisibleCenterX)) {
                Playfield.nXpos -= 4;
            }
            break;
    }
}


/*----------------------------------------------------------------------------
Name:           RenderLevel
------------------------------------------------------------------------------
Beschreibung: Kopiert den sichtbaren Teil des Levels in den Renderer
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               pnXpos, int *, Pixel-Positionierung X (obere linke Ecke des Levelausschnitts)
               pnYpos, int *, Pixel-Positionierung Y (obere linke Ecke des Levelausschnitts)
               nAnimationCount, int, 0 - 15 für Animationsstufe
      Ausgang: pnXpos, int *, ggf. korrigierte Pixel-Positionierung X (obere linke Ecke des Levelausschnitts)
               pnYpos, int *, ggf. korrigierte Pixel-Positionierung Y (obere linke Ecke des Levelausschnitts)
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int RenderLevel(SDL_Renderer *pRenderer, int *pnXpos, int *pnYpos, int nAnimationCount)
{
    int nErrorCode;
    uint32_t uX;
    uint32_t uY;
    int nXoffs;                         // Animations-Offset X
    int nYoffs;                         // Animations-Offset Y
    int nMaxXpos;
    int nMaxYpos;
    uint32_t I;                         // Levelindex
    uint32_t uReplicatorAnimation;      // Animationsschritt für Replikator
    uint32_t uShiftLevelXpix;           // Anzahl Pixel, die das Level nach rechts geshiftet wird, da es kleiner bzw. gleich der Anzeigebreite ist
    uint32_t uShiftLevelYpix;           // Anzahl Pixel, die das Level nach unten geshiftet wird, da es kleiner bzw. gleich der Anzeigehöhe ist
    uint32_t uUpperLeftLevelIndex;      // oberstes linkes Element, welches entweder komplett oder dessen untere rechte Ecke noch gerade sichtbar ist
    uint32_t uTextureIndex;
    uint32_t uAnimStatus;
    uint8_t uLevelElement;
    SDL_Rect DestR;                     // Zum Kopieren in den Renderer
    SDL_RendererFlip Flip;
    float fAngle;
    float fAngleOffs;
    float fScaleW;
    float fScaleH;

    // Die Eingangsparameter "grob" prüfen, damit nichts Schlimmes passiert
    if ((pRenderer == NULL) || (Playfield.pLevel == NULL)) {
        SDL_Log("%s: bad input parameters",__FUNCTION__);
        return -1;
    }
    // Y-Zentrierung des Levels im Anzeigebereich
    if (Playfield.uVisibleY >= Playfield.uLevel_Y_Dimension) {    // Level ist in Y-Richtung kleiner bzw. gleich der Anzeigehöhe (in Y-Richtung wird nicht gescrollt)
        uShiftLevelYpix = ((Playfield.uVisibleY - Playfield.uLevel_Y_Dimension) / 2) * FONT_H;    // Level im Anzeigebereich zentrieren
    } else {
        // Level ist in Y-Richtung größer als Anzeigebereich, daher oben anfangen (es muss ggf. in Y-Richtung gescrollt werden)
        uShiftLevelYpix = 0;
    }
    // X-Zentrierung des Levels im Anzeigebereich
    if (Playfield.uVisibleX >= Playfield.uLevel_X_Dimension) {    // Level ist in X-Richtung kleiner bzw. gleich der Anzeigebreite (in X-Richtung wird nicht gescrollt)
        uShiftLevelXpix = ((Playfield.uVisibleX - Playfield.uLevel_X_Dimension) / 2) * FONT_H;    // Level im Anzeigebereich zentrieren
    } else {
        // Level ist in X-Richtung größer als Anzeigebereich, daher oben anfangen (es muss ggf. in Y-Richtung gescrollt werden)
        uShiftLevelXpix = 0;
    }
    // Positionsüberläufe abfangen
    nMaxXpos = (Playfield.uLevel_X_Dimension * FONT_W) - WINDOW_W - 1;
    if (nMaxXpos < 0) {
        nMaxXpos = 0;
    }
    if (*pnXpos < 0) {
        *pnXpos = 0;
    } else if (*pnXpos > nMaxXpos) {
        *pnXpos = nMaxXpos;
    }
    nMaxYpos = (Playfield.uLevel_Y_Dimension * FONT_H) - WINDOW_H - 1 + PANEL_H;
    if (nMaxYpos < 0) {
        nMaxYpos = 0;
    }
    if (*pnYpos < 0) {
        *pnYpos = 0;
    } else if (*pnYpos > nMaxYpos) {
        *pnYpos = nMaxYpos;
    }
    CheckRunningWheel();
    CheckPlayTime();
    uUpperLeftLevelIndex = (*pnXpos / FONT_W) + (*pnYpos / FONT_H) * Playfield.uLevel_X_Dimension;
    nErrorCode = 0;
    // Den sichtbaren Teil des Levels in den Renderer kopieren-
    for (uY = 0; (uY <= ((WINDOW_H - PANEL_H) / FONT_H)) && (uY < Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); uY++) {
        for (uX = 0; (uX <= (WINDOW_W / FONT_W)) && (uX < Playfield.uLevel_X_Dimension) && (nErrorCode == 0); uX++) {
            nXoffs = 0;
            nYoffs = 0;
            fAngleOffs = 0;
            fScaleW = 1;
            fScaleH = 1;
            // Levelindex berechnen
            I = uUpperLeftLevelIndex + uY * Playfield.uLevel_X_Dimension + uX;
            uLevelElement = Playfield.pLevel[I];
            if (I > ((Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension) - 1)) {
                SDL_Log("%s: Warning: Level-Overflow, V:%u   X:%u   Y:%u  T:%u",__FUNCTION__,I,uX,uY,SDL_GetTicks());
            }
            Flip = SDL_FLIP_NONE;
            uAnimStatus = Playfield.pStatusAnimation[I] & 0x0000FF00;
            uReplicatorAnimation = Playfield.uFrameCounter % 12;
            switch (uLevelElement) {
                case (EMERALD_REPLICATOR_RED_SWITCH):
                    fAngle = 0;
                    if (Playfield.bReplicatorRedOn) {
                        uTextureIndex = 155;     // Replikator-Schalter, rot, an
                    } else {
                        uTextureIndex = 154;     // Replikator-Schalter, rot, aus
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_TOP_LEFT):
                    fAngle = 0;
                    if (Playfield.bReplicatorRedOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 143;     // Replikator, rot, an, oben links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 146;     // Replikator, rot, an, oben links, Animationsschritt 2
                        } else {
                            uTextureIndex = 149;     // Replikator, rot, an, oben links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 138;     // Replikator, rot, aus, oben links
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_TOP_MID):
                    fAngle = 0;
                    if (Playfield.bReplicatorRedOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 144;     // Replikator, rot, an, mitte links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 147;     // Replikator, rot, an, mitte links, Animationsschritt 2
                        } else {
                            uTextureIndex = 150;     // Replikator, rot, an, mitte links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 139;     // Replikator, rot, aus, oben mitte
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_TOP_RIGHT):
                    fAngle = 0;
                    if (Playfield.bReplicatorRedOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 145;     // Replikator, rot, an, oben rechts, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 148;     // Replikator, rot, an, oben rechts, Animationsschritt 2
                        } else {
                            uTextureIndex = 151;     // Replikator, rot, an, oben rechts, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 140;     // Replikator, rot, aus, oben rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_BUTTOM_LEFT):
                    fAngle = 0;
                    if (Playfield.bReplicatorRedOn) {
                        uTextureIndex = 152;     // Replikator, rot, an, unten links, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 141;     // Replikator, rot, aus, unten links
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_BUTTOM_RIGHT):
                    fAngle = 0;
                    if (Playfield.bReplicatorRedOn) {
                        uTextureIndex = 153;     // Replikator, rot, an, unten rechts, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 142;     // Replikator, rot, aus, unten rechts
                    }
                    break;
                case (EMERALD_KEY_RED):
                    fAngle = 0;
                    uTextureIndex = 98;     // roter Schlüssel
                    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_KEY_RED_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                        if (nAnimationCount == 15) {    // Nach Animation den Schlüssel durch Space ersetzen
                            Playfield.pLevel[I] = EMERALD_SPACE;
                            Playfield.pStatusAnimation[I] = EMERALD_NO_ADDITIONAL_ANIMSTATUS;
                        }
                    }
                    break;
                case (EMERALD_KEY_YELLOW):
                    fAngle = 0;
                    uTextureIndex = 101;    // gelber Schlüssel
                    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_KEY_YELLOW_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                        if (nAnimationCount == 15) {    // Nach Animation den Schlüssel durch Space ersetzen
                            Playfield.pLevel[I] = EMERALD_SPACE;
                            Playfield.pStatusAnimation[I] = EMERALD_NO_ADDITIONAL_ANIMSTATUS;
                        }
                    }
                    break;
                case (EMERALD_KEY_BLUE):
                    fAngle = 0;
                    uTextureIndex = 100;    // blauer Schlüssel
                    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_KEY_BLUE_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                        if (nAnimationCount == 15) {    // Nach Animation den Schlüssel durch Space ersetzen
                            Playfield.pLevel[I] = EMERALD_SPACE;
                            Playfield.pStatusAnimation[I] = EMERALD_NO_ADDITIONAL_ANIMSTATUS;
                        }
                    }
                    break;
                case (EMERALD_KEY_GREEN):
                    fAngle = 0;
                    uTextureIndex = 99;     // grüner Schlüssel
                    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_KEY_GREEN_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                        if (nAnimationCount == 15) {    // Nach Animation den Schlüssel durch Space ersetzen
                            Playfield.pLevel[I] = EMERALD_SPACE;
                            Playfield.pStatusAnimation[I] = EMERALD_NO_ADDITIONAL_ANIMSTATUS;
                        }
                    }
                    break;
                case (EMERALD_DOOR_RED):
                    fAngle = 0;
                    uTextureIndex = 94;     // rote Tür
                    break;
                case (EMERALD_DOOR_YELLOW):
                    fAngle = 0;
                    uTextureIndex = 97;     // gelbe Tür
                    break;
                case (EMERALD_DOOR_BLUE):
                    fAngle = 0;
                    uTextureIndex = 96;     // blaue Tür
                    break;
                case (EMERALD_DOOR_GREEN):
                    fAngle = 0;
                    uTextureIndex = 95;     // grüne Tür
                    break;
                case (EMERALD_MAN):
                    fAngle = 0;
                    if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        uTextureIndex = 103 + nAnimationCount % 8;     // Man links
                        nXoffs = -nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        uTextureIndex = 103 + nAnimationCount % 8;     // Man rechts
                        nXoffs = nAnimationCount * 2;
                        Flip = SDL_FLIP_HORIZONTAL;
                    } else if (uAnimStatus == EMERALD_ANIM_UP) {
                        uTextureIndex = 111 + nAnimationCount % 8;     // Man hoch
                        nYoffs = -nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        uTextureIndex = 119 + nAnimationCount % 8;     // Man runter
                        nYoffs = nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT_DOUBLESPEED) {
                        uTextureIndex = 103 + nAnimationCount % 8;     // Man links
                        nXoffs = -nAnimationCount * 4;
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT_DOUBLESPEED) {
                        uTextureIndex = 103 + nAnimationCount % 8;     // Man rechts
                        nXoffs = nAnimationCount * 4;
                        Flip = SDL_FLIP_HORIZONTAL;
                    } else if (uAnimStatus == EMERALD_ANIM_UP_DOUBLESPEED) {
                        uTextureIndex = 111 + nAnimationCount % 8;     // Man hoch
                        nYoffs = -nAnimationCount * 4;
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN_DOUBLESPEED) {
                        uTextureIndex = 119 + nAnimationCount % 8;     // Man runter
                        nYoffs = nAnimationCount * 4;
                    } else {
                        uTextureIndex = 102;                            // Man stehend
                    }
                    break;
                case (EMERALD_ALIEN):
                    fAngle = 0;
                    if (uAnimStatus == EMERALD_ANIM_STAND) {
                        uTextureIndex = 127 + nAnimationCount / 2;     // Alien stehend
                    } else {
                        if ((nAnimationCount >= 4) && (nAnimationCount <= 11)) {
                            uTextureIndex = 135;                        // Alien geht 2, Flügel voll ausgebreitet
                        } else {
                            uTextureIndex = 136;                        // Alien geht 1
                        }
                        if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            nXoffs = -nAnimationCount * 2;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            nXoffs = nAnimationCount * 2;
                        } else if (uAnimStatus == EMERALD_ANIM_UP) {
                            nYoffs = -nAnimationCount * 2;
                        } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs = nAnimationCount * 2;
                        }
                    }
                    break;
                case (EMERALD_WHEEL):
                    uTextureIndex = 137;     // Alien-Rad
                    fAngle = 0;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_WHEEL_RUN) {
                        fAngleOffs = nAnimationCount * 11;
                    } else {
                        fAngleOffs = 0;
                    }
                    break;
                case (EMERALD_HARDWALL):
                    fAngle = 0;
                    uTextureIndex = 72;     // Mauer hart
                    break;
                case (EMERALD_SAND):
                    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_SAND_SHRINK) { // Man mit Richtung und Fire
                        uTextureIndex = 156; // kompl. freier Sand
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 5.5;
                        if (nAnimationCount == 15) {    // Nach Animation den Sand durch Space ersetzen
                            Playfield.pLevel[I] = EMERALD_SPACE;
                            Playfield.pStatusAnimation[I] = EMERALD_NO_ADDITIONAL_ANIMSTATUS;
                        }
                    } else {
                        fAngle = 0;
                        uTextureIndex = 156 + (Playfield.pStatusAnimation[I] & 0xFF);     // Sand mit richtigem Rand aussuchen
                    }
                    break;
                case (EMERALD_SPACE):
                    fAngle = 0;
                    uTextureIndex = 0;      // Space
                    break;
                case (EMERALD_BEETLE_UP):   // Käfer hoch
                    uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
                    fAngle = 90;
                    if (uAnimStatus == EMERALD_ANIM_UP) {
                        nYoffs = -nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_RIGHT) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_LEFT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_BEETLE_RIGHT):// Käfer rechts
                    uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
                    fAngle = 180;
                    if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        nXoffs = nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_DOWN) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_UP) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_BEETLE_DOWN): // Käfer runter
                    uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
                    fAngle = 270;
                    if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        nYoffs = nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_LEFT) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_RIGHT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_BEETLE_LEFT): // Käfer left
                    uTextureIndex = 75 + nAnimationCount % 8;     // Käfer links
                    fAngle = 0;
                    if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        nXoffs = -nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_DOWN) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_UP) {
                        fAngleOffs = nAnimationCount * 5.5; // OK
                    }
                    break;
                case (EMERALD_MINE_UP):     // Mine hoch
                    uTextureIndex = GetTextureIndexByElement(EMERALD_MINE_UP,nAnimationCount);
                    fAngle = 90;
                    if (uAnimStatus == EMERALD_ANIM_UP) {
                        nYoffs = -nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_RIGHT) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_LEFT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_MINE_RIGHT):  // Mine rechts
                    uTextureIndex = GetTextureIndexByElement(EMERALD_MINE_RIGHT,nAnimationCount);
                    fAngle = 180;
                    if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        nXoffs = nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_DOWN) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_UP) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_MINE_DOWN):   // Mine runter (kann über 2 Steuerungsphasen geboren werden)
                    uTextureIndex = GetTextureIndexByElement(EMERALD_MINE_DOWN,nAnimationCount);
                    fAngle = 270;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                        if (nAnimationCount == 15) {
                            Playfield.pStatusAnimation[I] = EMERALD_ANIM_BORN2;
                        }
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                        if (nAnimationCount == 15) {
                            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        nYoffs = nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_LEFT) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_RIGHT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_MINE_LEFT):   // Mine links
                    uTextureIndex = GetTextureIndexByElement(EMERALD_MINE_LEFT,nAnimationCount);
                    fAngle = 0;
                    if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        nXoffs = -nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_DOWN) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_UP) {
                        fAngleOffs = nAnimationCount * 5.5; // OK
                    }
                    break;
                case (EMERALD_INVALID):     // invalides Element gefunden
                    // Es muss sich nur um Elemente gekümmert werden, die sich am Rand des sichtbaren Bereichs befinden
                    // und sich in den sichtbaren Bereich >hinein< bewegen
                    uTextureIndex = GetTextureIndexByElement(Playfield.pStatusAnimation[I] & 0xFF,nAnimationCount);
                    if ((uY == 0) && ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_CLEAN_UP)) {
                        // 1. Element befindet sich am oberen Rand und will nach unten in den sichtbaren Bereich
                        //SDL_Log("%s: invalid element at top found    Anim:%x   T:%u",__FUNCTION__,Playfield.pStatusAnimation[I],SDL_GetTicks());
                        fAngle = 270; // bewegt sich nach unten
                        nYoffs = -FONT_H + nAnimationCount * 2;
                    } else if ((uX == 0) && ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_CLEAN_LEFT)) {
                        // 2. Element befindet sich am linken Rand und will nach rechts in den sichtbaren Bereich
                        //SDL_Log("%s: invalid element at left found    Anim:%x   T:%u",__FUNCTION__,Playfield.pStatusAnimation[I],SDL_GetTicks());
                        fAngle = 180; // bewegt sich nach rechts
                        nXoffs = -FONT_H + nAnimationCount * 2;
                    } else if ((uX == (WINDOW_W / FONT_W)) && ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_CLEAN_RIGHT)) {
                        // 3. Element befindet sich am rechten Rand und will nach links in den sichtbaren Bereich
                        //SDL_Log("%s: invalid element at right found    Anim:%x   T:%u",__FUNCTION__,Playfield.pStatusAnimation[I],SDL_GetTicks());
                        fAngle = 0;
                        nXoffs = FONT_H - nAnimationCount * 2;
                    } else if ( (uY == ((WINDOW_H - PANEL_H) / FONT_H)) && ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_CLEAN_DOWN)) {
                        // 4. Element befindet sich am unteren Rand und will nach oben in den sichtbaren Bereich
                        //SDL_Log("%s: invalid element at bottom found    Anim:%x   T:%u",__FUNCTION__,Playfield.pStatusAnimation[I],SDL_GetTicks());
                        fAngle = 90;
                        nYoffs = FONT_H - nAnimationCount * 2;
                    } else {
                        uTextureIndex = 0;      // Space
                        fAngle = 0;
                    }
                    // TODO: ab hier muss wahrscheinlich später ausgelagert werden
                    uAnimStatus = Playfield.pStatusAnimation[I] & 0xFF000000;
                    switch (uAnimStatus) {
                        case (EMERALD_ANIM_KEY_RED_SHRINK):
                            uTextureIndex = 98; // roter Schlüssel
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            break;
                        case (EMERALD_ANIM_KEY_GREEN_SHRINK):
                            uTextureIndex = 99; // grüner Schlüssel
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            break;
                        case (EMERALD_ANIM_KEY_BLUE_SHRINK):
                            uTextureIndex = 100; // blauer Schlüssel
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            break;
                        case (EMERALD_ANIM_KEY_YELLOW_SHRINK):
                            uTextureIndex = 101; // gelber Schlüssel
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            break;
                        case (EMERALD_ANIM_SAND_SHRINK):
                            uTextureIndex = 156; // kompl. freier Sand
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            //fAngleOffs = nAnimationCount * 5.5;
                            break;
                    }
                    break;
                default:
                    fAngle = 0;
                    if ((uLevelElement > 0xD2) && (uLevelElement != EMERALD_INVALID)) {  // Buchstaben
                        uTextureIndex = uLevelElement - 0xD2 + 27;
                    } else {
                        uTextureIndex = 0;      // Space
                        SDL_Log("%s: default: unknown element[%d] at adress %u found",__FUNCTION__,uLevelElement,I);
                    }
                    break;
            }
            if (uLevelElement != EMERALD_SPACE) {   // Space muss nicht gerendert werden, da nach jedem Bild sowieso Alles gelöscht wird
                // Position innerhalb des Renderers
                DestR.x = uX * FONT_W - (*pnXpos % FONT_W) + nXoffs + uShiftLevelXpix;
                DestR.y = uY * FONT_H - (*pnYpos % FONT_H) + nYoffs + uShiftLevelYpix;
                DestR.w = FONT_W * fScaleW;
                DestR.h = FONT_H * fScaleH;
                if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle + fAngleOffs,NULL, Flip) != 0) {
                    SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                    nErrorCode = -1;
                }
            }
        }
    }
    if (nErrorCode == 0) {
        if (ShowPanel(pRenderer) != 0) {
            SDL_Log("%s: ShowPanel() failed",__FUNCTION__);
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetTextureIndexByElement
------------------------------------------------------------------------------
Beschreibung: Holt den entsprechenden Texture-Index anhand eines Elements und des
              Animations-Schrittes
Parameter
      Eingang: uElement, uint32_t, Element, z.B. EMERALD_MINE_RIGHT
               nAnimationCount, int, Animationsschritt
      Ausgang: -
Rückgabewert:  uint32_t , Texture, wenn keine Texture ermittelt werden kann, wird
                SPACE (EMERALD_SPACE) zurückgegeben.
Seiteneffekte: -
------------------------------------------------------------------------------*/
uint32_t GetTextureIndexByElement(uint32_t uElement,int nAnimationCount) {
    uint32_t uTextureIndex;
    switch (uElement) {
        case (EMERALD_ALIEN):
            uTextureIndex = 73;
            break;
        case (EMERALD_MINE_UP):
        case (EMERALD_MINE_RIGHT):
        case (EMERALD_MINE_DOWN):
        case( EMERALD_MINE_LEFT):
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
            break;
        case (EMERALD_SPACE):
            uTextureIndex = 0;      // Space
            break;
        case (EMERALD_SAND):
            uTextureIndex = 71;     // Sand
            break;
        case (EMERALD_HARDWALL):
            uTextureIndex = 72;     // Mauer hart
            break;
        case (EMERALD_KEY_RED):
            uTextureIndex = 98;     // roter Schlüssel
            break;
        case (EMERALD_KEY_YELLOW):
            uTextureIndex = 101;    // gelber Schlüssel
            break;
        case (EMERALD_KEY_BLUE):
            uTextureIndex = 100;    // blauer Schlüssel
            break;
        case (EMERALD_KEY_GREEN):
            uTextureIndex = 99;     // grüner Schlüssel
            break;
        case (EMERALD_DOOR_RED):
            uTextureIndex = 94;     // rote Tür
            break;
        case (EMERALD_DOOR_YELLOW):
            uTextureIndex = 97;     // gelbe Tür
            break;
        case (EMERALD_DOOR_BLUE):
            uTextureIndex = 96;     // blaue Tür
            break;
        case (EMERALD_DOOR_GREEN):
            uTextureIndex = 95;     // grüne Tür
            break;
        case (EMERALD_MAN):
            uTextureIndex = 102;    // Man
            break;
        default:
            SDL_Log("%s: unknown element: %x",__FUNCTION__,uElement);
            uTextureIndex = 0;     // Space
            break;
    }
    return uTextureIndex;
}


/*----------------------------------------------------------------------------
Name:           ShowPanel
------------------------------------------------------------------------------
Beschreibung: Zeigt das Spielepanel mit den aktuellen Werten wie z.B. Time,
              Score, Emeralds, usw.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int ShowPanel(SDL_Renderer *pRenderer) {
    SDL_Rect DestR;         // Zum Kopieren in den Renderer
    int nErrorCode;
    char szNumber[8];

    // Die folgenden Werte müssen 4-stellig bleiben
    if (Playfield.uTimeToPlay > 9999) {
        Playfield.uTimeToPlay = 9999;
    }
    if (Playfield.uTotalScore > 9999) {
        Playfield.uTotalScore = 9999;
    }
    if (Playfield.uEmeraldsToCollect > 9999) {
        Playfield.uEmeraldsToCollect = 9999;
    }

    // Anzeige-Panel erzeugen
    DestR.x = 0;
    DestR.y = WINDOW_H - FONT_H;
    DestR.w = WINDOW_W;
    DestR.h = PANEL_H;
    if (SDL_RenderCopy(pRenderer,GetTextureByIndex(83),NULL,&DestR) != 0) {
        SDL_Log("%s: SDL_RenderCopy() failed: %s",__FUNCTION__,SDL_GetError());
        return - 1;
    }
    if (Playfield.uTimeToPlay < 10) {
        if ((Playfield.uFrameCounter % 60) == 0) {
            nErrorCode = CopyColorRect(pRenderer,255,0,0,6,614,98,20);
        } else {
            nErrorCode = CopyColorRect(pRenderer,255 - 4*(Playfield.uFrameCounter % 60),0,0,6,614,98,20);
        }
    } else {
        nErrorCode = 0;
    }
    if (nErrorCode == 0) {
        nErrorCode = -1;
        if (WritePanelText(pRenderer,"TIME:",8, WINDOW_H - FONT_H + 8, 16, false) == 0) {
            sprintf(szNumber,"%d",Playfield.uTimeToPlay);
            if (WritePanelText(pRenderer,szNumber,8 + 80, WINDOW_H - FONT_H + 8 , 16, true) == 0) {
                if (WritePanelText(pRenderer,"SCORE:",8 + 176, WINDOW_H - FONT_H + 8, 16, false) == 0) {
                    sprintf(szNumber,"%d",Playfield.uTotalScore);
                    if (WritePanelText(pRenderer,szNumber,8 + 272, WINDOW_H - FONT_H + 8 , 16, true) == 0) {
                        if (WritePanelText(pRenderer,"EMERALDS:",8 + 368, WINDOW_H - FONT_H + 8, 16, false) == 0) {
                            sprintf(szNumber,"%d",Playfield.uEmeraldsToCollect);
                            if (WritePanelText(pRenderer,szNumber,8 + 512, WINDOW_H - FONT_H + 8 , 16, true) == 0) {
                                nErrorCode = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    return nErrorCode;
}

/*----------------------------------------------------------------------------
Name:           WritePanelText
------------------------------------------------------------------------------
Beschreibung: Schreibt Text in den Renderer.

Parameter
      Eingang: SDL_Renderer *, pRenderer, Zeiger auf Renderer
               szText, const char *, Zeiger auf Text
               nXpos, int, X-Position in Pixeln
               nYpos, int, Y-Position in Pixeln
               nFontSize, int, Skalierung für Font (Standard = 32, für Panel = 16)
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int WritePanelText(SDL_Renderer *pRenderer, const char *szText, int nXpos, int nYpos, int nFontSize, bool bGreenNumbers) {
    int nErrorCode;
    int nI;
    SDL_Rect DestR;         // Zum Kopieren in den Renderer

    if ((pRenderer == NULL) && (szText == NULL)) {
        SDL_Log("%s: bad pointers",__FUNCTION__);
        return -1;
    }
    nI = 0;
    nErrorCode = 0;
    while ((nErrorCode == 0) && (szText[nI] != 0)) {
        DestR.x = nXpos;
        DestR.y = nYpos;
        DestR.w = nFontSize;
        DestR.h = nFontSize;
        if ((szText[nI] >= '0') && (szText[nI] <= '9') && bGreenNumbers) {
            // nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(84 - 0x30 + szText[nI]),NULL,&DestR,0,NULL, SDL_FLIP_NONE);
            nErrorCode = SDL_RenderCopy(pRenderer,GetTextureByIndex(84 - 0x30 + szText[nI]),NULL,&DestR);
            if (nErrorCode != 0) {
                SDL_Log("%s[green font]: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                nErrorCode = -1;
            }
        } else {
            // nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(szText[nI] - 32),NULL,&DestR,0,NULL, SDL_FLIP_NONE);
            nErrorCode = SDL_RenderCopy(pRenderer,GetTextureByIndex(szText[nI] - 32),NULL,&DestR);
            if (nErrorCode != 0) {
                SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                nErrorCode = -1;
            }
        }
        nI++;
        nXpos = nXpos + nFontSize;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CheckPlayTime
------------------------------------------------------------------------------
Beschreibung: Prüft, ob noch Spielzeit verfügbar ist und verringert dann nach
              einer gewissen Anzahl Frames (Refreshrate) die Spielzeit um 1.
              Diese Funktion wird aus RenderLevel() aufgerufen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x, ge_DisplayMode.x
------------------------------------------------------------------------------*/
void CheckPlayTime(void) {
    Playfield.uFrameCounter++;
    if ((Playfield.uFrameCounter % ge_DisplayMode.refresh_rate) == 0) {
        if (Playfield.uTimeToPlay > 0) {
            Playfield.uTimeToPlay--;
        }
    }
}
