#include "config.h"
#include "EmeraldMine.h"
#include "GetTextureIndexByElement.h"
#include "loadlevel.h"
#include "magicwall.h"
#include "man.h"
#include "mySDL.h"
#include "mystd.h"
#include "panel.h"
#include "RenderLevel.h"

extern PLAYFIELD Playfield;
extern uint8_t ge_uBeamColors[];
extern CONFIG Config;
uint8_t g_uCheeseRandom[MAX_CHEESE_RANDOM_NUMBERS];


/*----------------------------------------------------------------------------
Name:           FillCheeseRandomNumbers
------------------------------------------------------------------------------
Beschreibung: Füllt einen Speicherbereich mit Zufallszahlen (0,1,2 oder 3), damit die
              Funktion RenderLevel ein zufälligeres Käsemuster erzeugen kann.
              Es reicht, dass diese Funktion einmalig aufgerufen wird.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: g_uCheeseRandom[]
------------------------------------------------------------------------------*/
void FillCheeseRandomNumbers(void) {
    uint32_t I;

    for (I = 0; I < MAX_CHEESE_RANDOM_NUMBERS; I++) {
        g_uCheeseRandom[I] = randn(0,3);
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
Seiteneffekte: Playfield.x, Config.x
------------------------------------------------------------------------------*/
int RenderLevel(SDL_Renderer *pRenderer, int *pnXpos, int *pnYpos, int nAnimationCount)
{
    int nErrorCode;
    uint32_t uX;
    uint32_t uY;
    uint32_t I;                         // Levelindex
    uint32_t Y;                         // Für YAM-Animation und Post-Animation
    uint32_t uReplicatorAnimation;      // Animationsschritt für Replikator
    uint32_t uUpperLeftLevelIndex;      // oberstes linkes Element, welches entweder komplett oder dessen untere rechte Ecke noch gerade sichtbar ist
    uint32_t uPostAnimationIndex;       // Zeiger auf Playfield.pPostAnimation für Post-Animationen
    uint32_t uAnimStatus;
    uint32_t uSelfStatus;
    uint32_t uNewMagicElement;
    SDL_Rect DestR;                     // Zum Kopieren in den Renderer
    /////// Standard
    uint32_t uTextureIndex;
    uint16_t uLevelElement;
    int nXoffs;                         // Animations-Offset X
    int nYoffs;                         // Animations-Offset Y
    SDL_RendererFlip Flip;
    float fAngle;
    float fAngleOffs;
    float fScaleW;
    float fScaleH;
    /////// Erweitert: dieses Element wird >zuerst<, also unter das Standard-Element auf derselben Position gezeichnet
    uint32_t uTextureIndex_0;
    bool bExtendedElement;
    int nXoffs_0;                        // Animations-Offset X
    int nYoffs_0;                        // Animations-Offset Y
    SDL_RendererFlip Flip_0;
    float fAngle_0;
    float fAngleOffs_0;
    float fScaleW_0;
    float fScaleH_0;
    ///////
    uint8_t uVerticalBeamColor;
    uint8_t uHorizontalBeamColor;
    uint8_t uCheeseRandom;
    uint32_t uResX, uResY;

    // Sichtbare Fläche aufrunden statt abrunden
    uResX = ((Config.uResX + FONT_W) / FONT_W) * FONT_W;
    uResY = ((Config.uResY + FONT_H) / FONT_H) * FONT_H;

    uPostAnimationIndex = 0;
    // Die Eingangsparameter "grob" prüfen, damit nichts Schlimmes passiert
    if ((pRenderer == NULL) || (Playfield.pLevel == NULL)) {
        SDL_Log("%s: bad input parameters",__FUNCTION__);
        return -1;
    }
    // Positionsüberläufe abfangen
    if (*pnXpos < 0) {
        *pnXpos = 0;
    } else if (*pnXpos > Playfield.nMaxXpos) {
        *pnXpos = Playfield.nMaxXpos;
    }
    if (*pnYpos < 0) {
        *pnYpos = 0;
    } else if (*pnYpos > Playfield.nMaxYpos) {
        *pnYpos = Playfield.nMaxYpos;
    }
    uUpperLeftLevelIndex = (*pnXpos / FONT_W) + (*pnYpos / FONT_H) * Playfield.uLevel_X_Dimension;
    nErrorCode = 0;
    // Den sichtbaren Teil des Levels in den Renderer kopieren.
    for (uY = 0; (uY <= ((uResY - PANEL_H) / FONT_H)) && (uY < Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); uY++) {
        for (uX = 0; (uX <= (uResX / FONT_W)) && (uX < Playfield.uLevel_X_Dimension) && (nErrorCode == 0); uX++) {
            // Levelindex berechnen
            I = uUpperLeftLevelIndex + uY * Playfield.uLevel_X_Dimension + uX;
            if (I > ((Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension) - 1)) {
                // SDL_Log("%s: Warning: Level-Overflow, V:%u   X:%u   Y:%u  T:%u",__FUNCTION__,I,uX,uY,SDL_GetTicks());
                break;
            }
            uLevelElement = Playfield.pLevel[I];
            // Standard
            fAngle = 0;
            nXoffs = 0;
            nYoffs = 0;
            fAngleOffs = 0;
            fScaleW = 1;
            fScaleH = 1;
            Flip = SDL_FLIP_NONE;
            // Erweitert
            uTextureIndex_0 = 0; // Space
            bExtendedElement = false;
            fAngle_0 = 0;
            fAngleOffs_0 = 0;
            nXoffs_0 = 0;
            nYoffs_0 = 0;
            fAngleOffs_0 = 0;
            fScaleW_0 = 1;
            fScaleH_0 = 1;
            Flip_0 = SDL_FLIP_NONE;

            uAnimStatus = Playfield.pStatusAnimation[I] & 0x0000FF00;
            uSelfStatus = Playfield.pStatusAnimation[I] & 0xFF000000;
            uReplicatorAnimation = Playfield.uFrameCounter % 12;
            switch (uLevelElement) {
                case (EMERALD_STEEL_MODERN_LEFT_END):
                    uTextureIndex = 1029;
                    break;
                case (EMERALD_STEEL_MODERN_LEFT_RIGHT):
                    uTextureIndex = 1030;
                    break;
                case (EMERALD_STEEL_MODERN_RIGHT_END):
                    uTextureIndex = 1031;
                    break;
                case (EMERALD_STEEL_MODERN_UP_END):
                    uTextureIndex = 1032;
                    break;
                case (EMERALD_STEEL_MODERN_UP_DOWN):
                    uTextureIndex = 1033;
                    break;
                case (EMERALD_STEEL_MODERN_DOWN_END):
                    uTextureIndex = 1034;
                    break;
                case (EMERALD_STEEL_MODERN_MIDDLE):
                    uTextureIndex = 1035;
                    break;
                case (EMERALD_REMOTEBOMB):
                    uTextureIndex = 1028;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        nXoffs = -nAnimationCount * 2;
                        fAngleOffs = -nAnimationCount * 22.5;
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        nXoffs = nAnimationCount * 2;
                        fAngleOffs = nAnimationCount * 22.5;
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs = nAnimationCount * 2;
                            fAngleOffs = nAnimationCount * 22.5;
                    } else if (uAnimStatus == EMERALD_ANIM_UP) {
                            nYoffs = -nAnimationCount * 2;
                            fAngleOffs = -nAnimationCount * 22.5;
                    }
                    break;
                case (EMERALD_SWITCH_REMOTEBOMB_UP):
                    if (Playfield.bSwitchRemoteBombUp) {
                        uTextureIndex = 1018 + (((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) & 1);
                    } else {
                        uTextureIndex = 1019;
                    }
                    break;
                case (EMERALD_SWITCH_REMOTEBOMB_DOWN):
                    if (Playfield.bSwitchRemoteBombDown) {
                        uTextureIndex = 1020 + (((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) & 1);
                    } else {
                        uTextureIndex = 1021;
                    }
                    break;
                case (EMERALD_SWITCH_REMOTEBOMB_LEFT):
                    if (Playfield.bSwitchRemoteBombLeft) {
                        uTextureIndex = 1022 + (((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) & 1);
                    } else {
                        uTextureIndex = 1023;
                    }
                    break;
                case (EMERALD_SWITCH_REMOTEBOMB_RIGHT):
                    if (Playfield.bSwitchRemoteBombRight) {
                        uTextureIndex = 1024 + (((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) & 1);
                    } else {
                        uTextureIndex = 1025;
                    }
                    break;
                case (EMERALD_SWITCH_REMOTEBOMB_IGNITION):
                    if (Playfield.bSwitchRemoteBombIgnition) {
                        uTextureIndex = 1026 + (((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) & 1);
                    } else {
                        uTextureIndex = 1027;
                    }
                    break;
                case (EMERALD_TELEPORTER_RED):
                    uTextureIndex_0 = 3;    // Stahl
                    bExtendedElement = true;
                    uTextureIndex = 966 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, rot
                    break;
                case (EMERALD_TELEPORTER_YELLOW):
                    uTextureIndex_0 = 3;    // Stahl
                    bExtendedElement = true;
                    uTextureIndex = 979 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, gelb
                    break;
                case (EMERALD_TELEPORTER_GREEN):
                    uTextureIndex_0 = 3;    // Stahl
                    bExtendedElement = true;
                    uTextureIndex = 992 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, grün
                    break;
                case (EMERALD_TELEPORTER_BLUE):
                    uTextureIndex_0 = 3;    // Stahl
                    bExtendedElement = true;
                    uTextureIndex = 1005 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, blau
                    break;
                case (EMERALD_STEEL_GROW_DOWN):
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    }
                    // kein break
                case (EMERALD_STEEL_GROW_LEFT):
                case (EMERALD_STEEL_GROW_RIGHT):
                case (EMERALD_STEEL_GROW_UP):
                case (EMERALD_STEEL_GROW_LEFT_RIGHT):
                case (EMERALD_STEEL_GROW_UP_DOWN):
                case (EMERALD_STEEL_GROW_ALL):
                    uTextureIndex = 72;     // Mauer hart
                    break;
                case (EMERALD_WALL_GROW_DOWN):
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    }
                    // kein break;
                case (EMERALD_WALL_GROW_LEFT):
                case (EMERALD_WALL_GROW_RIGHT):
                case (EMERALD_WALL_GROW_UP):
                case (EMERALD_WALL_GROW_LEFT_RIGHT):
                case (EMERALD_WALL_GROW_UP_DOWN):
                case (EMERALD_WALL_GROW_ALL):
                    uTextureIndex = 316;     // Mauer eckig
                    break;
                case (EMERALD_STEEL_GROWING_LEFT):
                    uTextureIndex = 868 + nAnimationCount;
                    break;
                case (EMERALD_STEEL_GROWING_RIGHT):
                    uTextureIndex = 852 + nAnimationCount;
                    break;
                case (EMERALD_STEEL_GROWING_UP):
                    uTextureIndex = 820 + nAnimationCount;
                    break;
                case (EMERALD_STEEL_GROWING_DOWN):
                    uTextureIndex = 836 + nAnimationCount;
                    break;
                case (EMERALD_WALL_GROWING_LEFT):
                    uTextureIndex = 939 + nAnimationCount;
                    break;
                case (EMERALD_WALL_GROWING_RIGHT):
                    uTextureIndex = 923 + nAnimationCount;
                    break;
                case (EMERALD_WALL_GROWING_UP):
                    uTextureIndex = 891 + nAnimationCount;
                    break;
                case (EMERALD_WALL_GROWING_DOWN):
                    uTextureIndex = 907 + nAnimationCount;
                    break;
                case (EMERALD_YAM_KILLS_MAN):
                    Y = Playfield.uFrameCounter % 11;       // Y von 0 bis 10
                    if (Y <= 5) {                           // 0,1,2,3,4,5
                        uTextureIndex = 362 + Y;            // 362 - 367
                    } else {                                // 6,7,8,9,10
                        uTextureIndex = 367 + 5 - Y;        // 366 - 362
                    }
                    if (uSelfStatus == EMERALD_ANIM_MONSTER_KILLS_LEFT) {
                        nXoffs = -nAnimationCount * 2;
                    } else if (uSelfStatus == EMERALD_ANIM_MONSTER_KILLS_RIGHT) {
                        nXoffs = nAnimationCount * 2;
                    } else if (uSelfStatus == EMERALD_ANIM_MONSTER_KILLS_UP) {
                        nYoffs = -nAnimationCount * 2;
                    } else if (uSelfStatus == EMERALD_ANIM_MONSTER_KILLS_DOWN) {
                        nYoffs = nAnimationCount * 2;
                    } else {
                      SDL_Log("%s: [EMERALD_YAM_KILLS_MAN]: Warning: unhandled Status: %x",__FUNCTION__,uSelfStatus);
                    }
                    break;
                case (EMERALD_ALIEN_KILLS_MAN):
                    if ((nAnimationCount >= 4) && (nAnimationCount <= 11)) {
                        uTextureIndex = 135;                        // Alien geht 2, Flügel voll ausgebreitet
                    } else {
                        uTextureIndex = 136;                        // Alien geht 1
                    }
                    if (uSelfStatus == EMERALD_ANIM_MONSTER_KILLS_LEFT) {
                        nXoffs = -nAnimationCount * 2;
                    } else if (uSelfStatus == EMERALD_ANIM_MONSTER_KILLS_RIGHT) {
                        nXoffs = nAnimationCount * 2;
                    } else if (uSelfStatus == EMERALD_ANIM_MONSTER_KILLS_UP) {
                        nYoffs = -nAnimationCount * 2;
                    } else if (uSelfStatus == EMERALD_ANIM_MONSTER_KILLS_DOWN) {
                        nYoffs = nAnimationCount * 2;
                    } else {
                        SDL_Log("%s: [EMERALD_ALIEN_KILLS_MAN]: Warning: unhandled Status: %x",__FUNCTION__,uSelfStatus);
                    }
                    break;
                case (EMERALD_CONVEYORBELT_SWITCH_RED):
                    if (Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_LEFT) {
                        uTextureIndex = 801;    // links
                    } else if (Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_RIGHT) {
                        uTextureIndex = 802;    // rechts
                    } else {
                        uTextureIndex = 800;    // aus
                    }
                    break;
                case (EMERALD_CONVEYORBELT_RED):
                    uTextureIndex = 799;
                    if (Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_LEFT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * -11.25;   // links drehen
                    } else if (Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_RIGHT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * 11.25;    // rechts drehen
                    }
                    break;
                case (EMERALD_CONVEYORBELT_SWITCH_GREEN):
                    if (Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_LEFT) {
                        uTextureIndex = 805;    // links
                    } else if (Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_RIGHT) {
                        uTextureIndex = 806;    // rechts
                    } else {
                        uTextureIndex = 804;    // aus
                    }
                    break;
                case (EMERALD_CONVEYORBELT_GREEN):
                    uTextureIndex = 803;
                    if (Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_LEFT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * -11.25;   // links drehen
                    } else if (Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_RIGHT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * 11.25;    // rechts drehen
                    }
                    break;
                case (EMERALD_CONVEYORBELT_SWITCH_BLUE):
                    if (Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_LEFT) {
                        uTextureIndex = 809;    // links
                    } else if (Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_RIGHT) {
                        uTextureIndex = 810;    // rechts
                    } else {
                        uTextureIndex = 808;    // aus
                    }
                    break;
                case (EMERALD_CONVEYORBELT_BLUE):
                    uTextureIndex = 807;
                    if (Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_LEFT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * -11.25;   // links drehen
                    } else if (Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_RIGHT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * 11.25;    // rechts drehen
                    }
                    break;
                case (EMERALD_CONVEYORBELT_SWITCH_YELLOW):
                    if (Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_LEFT) {
                        uTextureIndex = 813;    // links
                    } else if (Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_RIGHT) {
                        uTextureIndex = 814;    // rechts
                    } else {
                        uTextureIndex = 812;    // aus
                    }
                    break;
                case (EMERALD_CONVEYORBELT_YELLOW):
                    uTextureIndex = 811;
                    if (Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_LEFT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * -11.25;   // links drehen
                    } else if (Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_RIGHT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * 11.25;    // rechts drehen
                    }
                    break;
                case (EMERALD_LEVELEDITOR_MESSAGE_1_4):
                    uTextureIndex = 815;
                    break;
                case (EMERALD_LEVELEDITOR_MESSAGE_2_4):
                    uTextureIndex = 816;
                    break;
                case (EMERALD_LEVELEDITOR_MESSAGE_3_4):
                    uTextureIndex = 817;
                    break;
                case (EMERALD_LEVELEDITOR_MESSAGE_4_4):
                    uTextureIndex = 818;
                    break;
                case (EMERALD_MAN_DIES):
                    if (uSelfStatus == EMERALD_ANIM_MAN_DIES_P1) {
                        uTextureIndex = 719 + nAnimationCount / 2;
                    } else {
                        uTextureIndex = 719 + 8 + nAnimationCount / 2; // bis 734
                    }
                    break;
                case (EMERALD_DYNAMITE_ON):
                    switch (uSelfStatus) {
                        case (EMERALD_ANIM_DYNAMITE_ON_P1):
                            uTextureIndex = 555;
                            break;
                        case (EMERALD_ANIM_DYNAMITE_ON_P2):
                            uTextureIndex = 556;
                            break;
                        case (EMERALD_ANIM_DYNAMITE_ON_P3):
                            uTextureIndex = 557;
                            break;
                        case (EMERALD_ANIM_DYNAMITE_ON_P4):
                            uTextureIndex = 558;
                            break;
                        default:
                            uTextureIndex = 555;
                            break;
                    }
                    break;
                case (EMERALD_WALL_WITH_TIME_COIN):
                    uTextureIndex = 735;
                    break;
                case (EMERALD_WALL_WITH_CRYSTAL):
                    uTextureIndex = 533;
                    break;
                case (EMERALD_WALL_WITH_KEY_RED):
                    uTextureIndex = 534;
                    break;
                case (EMERALD_WALL_WITH_KEY_GREEN):
                    uTextureIndex = 535;
                    break;
                case (EMERALD_WALL_WITH_KEY_BLUE):
                    uTextureIndex = 536;
                    break;
                case (EMERALD_WALL_WITH_KEY_YELLOW):
                    uTextureIndex = 537;
                    break;
                case (EMERALD_WALL_WITH_KEY_WHITE):
                    uTextureIndex = 538;
                    break;
                case (EMERALD_WALL_WITH_KEY_GENERAL):
                    uTextureIndex = 539;
                    break;
                case (EMERALD_WALL_WITH_BOMB):
                    uTextureIndex = 540;
                    break;
                case (EMERALD_WALL_WITH_MEGABOMB):
                    uTextureIndex = 541;
                    break;
                case (EMERALD_WALL_WITH_STONE):
                    uTextureIndex = 542;
                    break;
                case (EMERALD_WALL_WITH_NUT):
                    uTextureIndex = 543;
                    break;
                case (EMERALD_WALL_WITH_WHEEL):
                    uTextureIndex = 544;
                    break;
                case (EMERALD_WALL_WITH_DYNAMITE):
                    uTextureIndex = 545;
                    break;
                case (EMERALD_WALL_WITH_ENDDOOR):
                    uTextureIndex = 546;
                    break;
                case (EMERALD_WALL_WITH_ENDDOOR_READY):
                    uTextureIndex = 547;
                    break;
                case (EMERALD_WALL_WITH_MINE_UP):
                    uTextureIndex = 548;
                    break;
                case (EMERALD_WALL_WITH_BEETLE_UP):
                    uTextureIndex = 549;
                    break;
                case (EMERALD_WALL_WITH_YAM):
                    uTextureIndex = 550;
                    break;
                case (EMERALD_WALL_WITH_ALIEN):
                    uTextureIndex = 551;
                    break;
                case (EMERALD_WALL_WITH_MOLE_UP):
                    uTextureIndex = 552;
                    break;
                case (EMERALD_WALL_WITH_GREEN_CHEESE):
                    uTextureIndex = 553;
                    break;
                case (EMERALD_WALL_WITH_EMERALD):
                    uTextureIndex = 529;
                    break;
                case (EMERALD_WALL_WITH_RUBY):
                    uTextureIndex = 530;
                    break;
                case (EMERALD_WALL_WITH_SAPPHIRE):
                    uTextureIndex = 531;
                    break;
                case (EMERALD_WALL_WITH_PERL):
                    uTextureIndex = 532;
                    break;
                case (EMERALD_SWITCHDOOR_OPEN):
                    if (uSelfStatus == EMERALD_ANIM_DOOR_CLOSE) {   // Soll Tür sich schließen?
                        if (nAnimationCount < 15) {
                            uTextureIndex = 506 - nAnimationCount / 3;
                        } else {
                            uTextureIndex = 502;    // Tür geschlossen
                        }
                    } else {
                        uTextureIndex = 506;        // Schalttür offen
                    }
                    break;
                case (EMERALD_SWITCHDOOR_CLOSED):
                    if (uSelfStatus == EMERALD_ANIM_DOOR_OPEN) {   // Soll Tür sich öffnen?
                        if (nAnimationCount < 15) {
                            uTextureIndex = 502 + nAnimationCount / 3;
                        } else {
                            uTextureIndex = 506;    // Tür offen
                        }
                    } else {
                        uTextureIndex = 502;        // Schalttür geschlossen
                    }
                    break;
               case (EMERALD_SWITCH_SWITCHDOOR):
                    if (Playfield.bSwitchDoorState) {
                        uTextureIndex = 507;
                    } else {
                        uTextureIndex = 508;
                    }
                    break;
                case (EMERALD_DOOR_TIME):
                    if (uSelfStatus == EMERALD_ANIM_DOOR_OPEN) {   // Tür öffnet sich
                        if (nAnimationCount < 15) {
                            uTextureIndex = 497 + nAnimationCount / 3;
                        } else {
                                uTextureIndex = 501;
                        }
                    } else if (uSelfStatus == EMERALD_ANIM_DOOR_CLOSE) {    // Tür schließt sich
                        if (nAnimationCount < 15) {
                            uTextureIndex = 501 - nAnimationCount / 3;
                        } else {
                            uTextureIndex = 497;    // Tür geschlossen
                        }
                    } else if (Playfield.bTimeDoorOpen) {
                        uTextureIndex = 501;    // Tür offen
                    } else {
                        uTextureIndex = 497;    // Tür geschlossen
                    }
                    break;
                case (EMERALD_WHEEL_TIMEDOOR):
                    if (Playfield.bTimeDoorOpen) {
                        uTextureIndex = 493 + nAnimationCount / 4;
                    } else {
                        uTextureIndex = 493;
                    }
                    break;
                case (EMERALD_DOOR_EMERALD):
                    uTextureIndex = 487;
                    break;
                case (EMERALD_DOOR_MULTICOLOR):
                    uTextureIndex = 488;
                    break;
                case (EMERALD_DOOR_ONLY_UP_STEEL):
                    uTextureIndex = 489;
                    break;
                case (EMERALD_DOOR_ONLY_DOWN_STEEL):
                    uTextureIndex = 490;
                    break;
                case (EMERALD_DOOR_ONLY_LEFT_STEEL):
                    uTextureIndex = 491;
                    break;
                case (EMERALD_DOOR_ONLY_RIGHT_STEEL):
                    uTextureIndex = 492;
                    break;
                case (EMERALD_DOOR_ONLY_UP_WALL):
                    uTextureIndex = 964;
                    break;
                case (EMERALD_DOOR_ONLY_DOWN_WALL):
                    uTextureIndex = 965;
                    break;
                case (EMERALD_DOOR_ONLY_LEFT_WALL):
                    uTextureIndex = 962;
                    break;
                case (EMERALD_DOOR_ONLY_RIGHT_WALL):
                    uTextureIndex = 963;
                    break;
                case (EMERALD_STEEL_INVISIBLE):
                    if (Playfield.bLightOn) {
                        uTextureIndex = 484;
                    } else {
                        uTextureIndex = 0;
                    }
                    break;
                case (EMERALD_WALL_INVISIBLE):
                    if (Playfield.bLightOn) {
                        uTextureIndex = 485;
                    } else {
                        uTextureIndex = 0;
                    }
                    break;
                case (EMERALD_SAND_MOLE):
                    uTextureIndex = 171;        // Sand
                    nXoffs = 15 - nAnimationCount;
                    nYoffs = 15 - nAnimationCount;
                    fScaleW = nAnimationCount * 0.06;
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_LIGHT_SWITCH):
                    uTextureIndex = 486;
                    break;
                case (EMERALD_GREEN_CHEESE_GOES):
                    uTextureIndex = 343 + I % 3;
                    nXoffs = nAnimationCount;
                    nYoffs = nAnimationCount;
                    fScaleW = 1 - nAnimationCount * 0.06;
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_STANDMINE):
                    uTextureIndex = 447;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if ((((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 32) == 0) {
                        uTextureIndex = 448;
                    } else if ((((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 16) == 0) {
                        uTextureIndex = 449;
                    } else {
                        uTextureIndex = 447;
                    }
                    break;
                case (EMERALD_MAGIC_WALL_SWITCH):
                    uTextureIndex = 435;
                    break;
                case (EMERALD_LIGHTBARRIER_RED_UP):
                    if (Playfield.bLightBarrierRedOn) {
                        uTextureIndex = 423;
                    } else {
                        uTextureIndex = 419;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_RED_DOWN):
                    if (Playfield.bLightBarrierRedOn) {
                        uTextureIndex = 424;
                    } else {
                        uTextureIndex = 420;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_RED_LEFT):
                    if (Playfield.bLightBarrierRedOn) {
                        uTextureIndex = 425;
                    } else {
                        uTextureIndex = 421;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_RED_RIGHT):
                    if (Playfield.bLightBarrierRedOn) {
                        uTextureIndex = 426;
                    } else {
                        uTextureIndex = 422;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_UP):
                    if (Playfield.bLightBarrierGreenOn) {
                        uTextureIndex = 399;
                    } else {
                        uTextureIndex = 395;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_DOWN):
                    if (Playfield.bLightBarrierGreenOn) {
                        uTextureIndex = 400;
                    } else {
                        uTextureIndex = 396;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_LEFT):
                    if (Playfield.bLightBarrierGreenOn) {
                        uTextureIndex = 401;
                    } else {
                        uTextureIndex = 397;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_RIGHT):
                    if (Playfield.bLightBarrierGreenOn) {
                        uTextureIndex = 402;
                    } else {
                        uTextureIndex = 398;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_UP):
                    if (Playfield.bLightBarrierBlueOn) {
                        uTextureIndex = 407;
                    } else {
                        uTextureIndex = 403;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_DOWN):
                    if (Playfield.bLightBarrierBlueOn) {
                        uTextureIndex = 408;
                    } else {
                        uTextureIndex = 404;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_LEFT):
                    if (Playfield.bLightBarrierBlueOn) {
                        uTextureIndex = 409;
                    } else {
                        uTextureIndex = 405;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_RIGHT):
                    if (Playfield.bLightBarrierBlueOn) {
                        uTextureIndex = 410;
                    } else {
                        uTextureIndex = 406;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_UP):
                    if (Playfield.bLightBarrierYellowOn) {
                        uTextureIndex = 415;
                    } else {
                        uTextureIndex = 411;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_DOWN):
                    if (Playfield.bLightBarrierYellowOn) {
                        uTextureIndex = 416;
                    } else {
                        uTextureIndex = 412;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_LEFT):
                    if (Playfield.bLightBarrierYellowOn) {
                        uTextureIndex = 417;
                    } else {
                        uTextureIndex = 413;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_RIGHT):
                    if (Playfield.bLightBarrierYellowOn) {
                        uTextureIndex = 418;
                    } else {
                        uTextureIndex = 414;
                    }
                    break;
                case (EMERALD_BEAM_RED_VERTICAL):
                    uTextureIndex = 433;
                    break;
                case (EMERALD_BEAM_RED_HORIZONTAL):
                    uTextureIndex = 434;
                    break;
                case (EMERALD_BEAM_GREEN_VERTICAL):
                    uTextureIndex = 427;
                    break;
                case (EMERALD_BEAM_GREEN_HORIZONTAL):
                    uTextureIndex = 428;
                    break;
                case (EMERALD_BEAM_BLUE_VERTICAL):
                    uTextureIndex = 429;
                    break;
                case (EMERALD_BEAM_BLUE_HORIZONTAL):
                    uTextureIndex = 430;
                    break;
                case (EMERALD_BEAM_YELLOW_VERTICAL):
                    uTextureIndex = 431;
                    break;
                case (EMERALD_BEAM_YELLOW_HORIZONTAL):
                    uTextureIndex = 432;
                    break;
                case (EMERALD_BEAM_CROSS):
                    uTextureIndex = 0;
                    bExtendedElement = false;
                    uVerticalBeamColor = (Playfield.pStatusAnimation[I] >> 4) & 0x3;
                    uHorizontalBeamColor = Playfield.pStatusAnimation[I] & 0x03;
                    // Horizontale Linien zeichnen
                    SDL_SetRenderDrawColor(pRenderer,
                                           ge_uBeamColors[uHorizontalBeamColor * 3 + 0],        // rot
                                           ge_uBeamColors[uHorizontalBeamColor * 3 + 1],        // grün
                                           ge_uBeamColors[uHorizontalBeamColor * 3 + 2],        // blau
                                           SDL_ALPHA_OPAQUE); // Farbe für Line setzen
                    SDL_RenderDrawLine(pRenderer,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix + 2,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix + FONT_W,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix + 2
                                        );
                    SDL_RenderDrawLine(pRenderer,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix + FONT_H - 3,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix + FONT_W,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix + FONT_H - 3
                                        );
                    // Vertikale Linien zeichnen
                    SDL_SetRenderDrawColor(pRenderer,
                                           ge_uBeamColors[uVerticalBeamColor * 3 + 0],        // rot
                                           ge_uBeamColors[uVerticalBeamColor * 3 + 1],        // grün
                                           ge_uBeamColors[uVerticalBeamColor * 3 + 2],        // blau
                                           SDL_ALPHA_OPAQUE); // Farbe für Line setzen
                    SDL_RenderDrawLine(pRenderer,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix + 2,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix + 2,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix + FONT_H
                                        );
                    SDL_RenderDrawLine(pRenderer,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix + FONT_W - 3,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix,
                                       uX * FONT_W - (*pnXpos % FONT_W) + Playfield.uShiftLevelXpix + FONT_W - 3,
                                       uY * FONT_H - (*pnYpos % FONT_H) + Playfield.uShiftLevelYpix + FONT_H
                                        );
                    SDL_SetRenderDrawColor(pRenderer,0,0,0, SDL_ALPHA_OPAQUE); // Zum Schluss wieder schwarz setzen
                    break;
                case (EMERALD_LIGHTBARRIER_RED_SWITCH):
                    if (Playfield.bLightBarrierRedOn) {
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 384;
                        } else {
                            uTextureIndex = 385;
                        }
                    } else {
                        uTextureIndex = 383;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_SWITCH):
                    if (Playfield.bLightBarrierGreenOn) {
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 390;
                        } else {
                            uTextureIndex = 391;
                        }
                    } else {
                        uTextureIndex = 389;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_SWITCH):
                    if (Playfield.bLightBarrierBlueOn) {
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 393;
                        } else {
                            uTextureIndex = 394;
                        }
                    } else {
                        uTextureIndex = 392;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_SWITCH):
                    if (Playfield.bLightBarrierYellowOn) {
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 387;
                        } else {
                            uTextureIndex = 388;
                        }
                    } else {
                        uTextureIndex = 386;
                    }
                    break;
                case (EMERALD_YAM):
                    uTextureIndex = 357;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_STAND) {
                        if (nAnimationCount <= 9) {
                            uTextureIndex = 357 + (nAnimationCount / 2);
                        } else if (nAnimationCount <= 11) { // 10,11
                            uTextureIndex = 360;
                        } else if (nAnimationCount <= 13) { // 12,13
                            uTextureIndex = 359;
                        } else { // 14,15
                            uTextureIndex = 358;
                        }
                    } else {
                        Y = Playfield.uFrameCounter % 11;       // Y von 0 bis 10
                        if (Y <= 5) {                           // 0,1,2,3,4,5
                            uTextureIndex = 362 + Y;            // 362 - 367
                        } else {                                // 6,7,8,9,10
                            uTextureIndex = 367 + 5 - Y;        // 366 - 362
                        }
                        if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            nXoffs = -nAnimationCount * 2;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            nXoffs = nAnimationCount * 2;
                        } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs = nAnimationCount * 2;
                        } else if (uAnimStatus == EMERALD_ANIM_UP) {
                            nYoffs = -nAnimationCount * 2;
                        }
                    }
                    break;
                case (EMERALD_MESSAGE_1):
                case (EMERALD_MESSAGE_2):
                case (EMERALD_MESSAGE_3):
                case (EMERALD_MESSAGE_4):
                case (EMERALD_MESSAGE_5):
                case (EMERALD_MESSAGE_6):
                case (EMERALD_MESSAGE_7):
                case (EMERALD_MESSAGE_8):
                    uTextureIndex = 356;
                    if (uSelfStatus == EMERALD_ANIM_MESSAGE_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_STEEL_MARKER_LEFT_UP):
                    uTextureIndex = 348;
                    break;
                case (EMERALD_STEEL_MARKER_UP):
                    uTextureIndex = 349;
                    break;
                case (EMERALD_STEEL_MARKER_RIGHT_UP):
                    uTextureIndex = 350;
                    break;
                case (EMERALD_STEEL_MARKER_LEFT):
                    uTextureIndex = 351;
                    break;
                case (EMERALD_STEEL_MARKER_RIGHT):
                    uTextureIndex = 352;
                    break;
                case (EMERALD_STEEL_MARKER_LEFT_BOTTOM):
                    uTextureIndex = 353;
                    break;
                case (EMERALD_STEEL_MARKER_BOTTOM):
                    uTextureIndex = 354;
                    break;
                case (EMERALD_STEEL_MARKER_RIGHT_BOTTOM):
                    uTextureIndex = 355;
                    break;
                case (EMERALD_STEEL_HEART):
                    uTextureIndex = 474;
                    break;
                case (EMERALD_STEEL_PLAYERHEAD):
                    uTextureIndex = 475;
                    break;
                case (EMERALD_STEEL_NO_ENTRY):
                    uTextureIndex = 476;
                    break;
                case (EMERALD_STEEL_GIVE_WAY):
                    uTextureIndex = 477;
                    break;
                case (EMERALD_STEEL_YING):
                    uTextureIndex = 478;
                    break;
                case (EMERALD_STEEL_WHEELCHAIR):
                    uTextureIndex = 479;
                    break;
                case (EMERALD_STEEL_ARROW_DOWN):
                    uTextureIndex = 480;
                    break;
                case (EMERALD_STEEL_ARROW_UP):
                    uTextureIndex = 481;
                    break;
                case (EMERALD_STEEL_ARROW_LEFT):
                    uTextureIndex = 482;
                    break;
                case (EMERALD_STEEL_ARROW_RIGHT):
                    uTextureIndex = 483;
                    break;
                case (EMERALD_STEEL_TRASHCAN):
                    uTextureIndex = 736;
                    break;
                case (EMERALD_STEEL_JOYSTICK):
                    uTextureIndex = 737;
                    break;
                case (EMERALD_STEEL_EDIT_LEVEL):
                    uTextureIndex = 738;
                    break;
                case (EMERALD_STEEL_MOVE_LEVEL):
                    uTextureIndex = 739;
                    break;
                case (EMERALD_STEEL_ADD_LEVELGROUP):
                    uTextureIndex = 740;
                    break;
                case (EMERALD_STEEL_COPY_LEVEL):
                    uTextureIndex = 741;
                    break;
                case (EMERALD_STEEL_CLIPBOARD_LEVEL):
                    uTextureIndex = 742;
                    break;
                case (EMERALD_STEEL_DC3_IMPORT):
                    uTextureIndex = 743;
                    break;
                case (EMERALD_STEEL_RENAME_LEVELGROUP):
                    uTextureIndex = 744;
                    break;
                case (EMERALD_STEEL_PASSWORD):
                    uTextureIndex = 745;
                    break;
                case (EMERALD_GREEN_DROP_COMES):
                    uTextureIndex = 346;
                    nXoffs = 16 - nAnimationCount;
                    nYoffs = nXoffs;
                    fScaleW = nAnimationCount * 0.06;
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_GREEN_DROP):
                    uTextureIndex = 346;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else {
                        if (nAnimationCount <= 7) { // 0 ... 7
                            fScaleW = 1 - nAnimationCount * 0.02;   // 1 ..... 0.86
                            nXoffs = nAnimationCount / 2;           // max. 7 / 2 = 3 Pixel zur Mitte
                        } else {                    // 8 ... 15
                            fScaleW = 0.85 + (nAnimationCount - 8) * 0.02;
                            nXoffs = 3 + 4 - nAnimationCount / 2;
                        }
                        if (uSelfStatus == EMERALD_ANIM_GREEN_DROP_1) {
                            nYoffs = nAnimationCount;
                        } else if (uSelfStatus == EMERALD_ANIM_GREEN_DROP_2) {
                            nYoffs = (FONT_H / 2) + nAnimationCount;
                        } else {
                            SDL_Log("%s: green drop, something wrong!  uSelfStatus = %x",__FUNCTION__,uSelfStatus);
                        }
                    }
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_GREEN_CHEESE):
                    uCheeseRandom = g_uCheeseRandom[I & (MAX_CHEESE_RANDOM_NUMBERS - 1)];
                    if (uCheeseRandom < 3) {
                        uTextureIndex = 343 + uCheeseRandom;
                    } else {
                        uTextureIndex = 819;    // Wurde leider erst später hinzugefügt
                    }
                    break;
                case (EMERALD_STEEL_FORBIDDEN):
                    uTextureIndex = 334;
                    break;
                case (EMERALD_STEEL_EXIT):
                    uTextureIndex = 332;
                    break;
                case (EMERALD_STEEL_RADIOACTIVE):
                    uTextureIndex = 337;
                    break;
                case (EMERALD_STEEL_EXPLOSION):
                    uTextureIndex = 333;
                    break;
                case (EMERALD_STEEL_ACID):
                    uTextureIndex = 329;
                    break;
                case (EMERALD_STEEL_NOT_ROUND):
                    uTextureIndex = 335;
                    break;
                case (EMERALD_WALL_NOT_ROUND):
                    uTextureIndex = 342;
                    break;
                case (EMERALD_STEEL_PARKING):
                    uTextureIndex = 336;
                    break;
                case (EMERALD_STEEL_STOP):
                    uTextureIndex = 340;
                    break;
                case (EMERALD_STEEL_DEADEND):
                    uTextureIndex = 331;
                    break;
                case (EMERALD_STEEL_BIOHAZARD):
                    uTextureIndex = 330;
                    break;
                case (EMERALD_STEEL_WARNING):
                    uTextureIndex = 341;
                    break;
                case (EMERALD_STEEL_ROUND):
                    uTextureIndex = 338;
                    break;
                case (EMERALD_STEEL_ROUND_PIKE):
                    uTextureIndex = 339;
                    break;
                case (EMERALD_WALL_ROUND_PIKE):
                    uTextureIndex = 328;
                    break;
                case (EMERALD_DOOR_END_NOT_READY):
                    uTextureIndex = 287;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    }
                    break;
                case (EMERALD_DOOR_END_READY_STEEL):
                    uTextureIndex = 510 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8; // Stahl-Endtür, blinkend
                    if (uSelfStatus == EMERALD_ANIM_MAN_GOES_ENDDOOR) {
                        uTextureIndex_0 = 510 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8; // Endtür, blinkend
                        uTextureIndex = EMERALD_SPACE;
                        bExtendedElement = true;
                        switch (uAnimStatus) {
                            case(EMERALD_ANIM_LEFT):
                                uTextureIndex = 103 + nAnimationCount % 8;     // Man geht links
                                nXoffs = FONT_W - nAnimationCount;
                                nYoffs = nAnimationCount * 1;
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                break;
                            case(EMERALD_ANIM_UP):
                                uTextureIndex = 111 + nAnimationCount % 8;     // Man geht hoch
                                nXoffs = nAnimationCount / 2;
                                nYoffs = FONT_H - nAnimationCount;        // Man steht bereits auf neuer Position, daher + FONT_H
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                break;
                            case(EMERALD_ANIM_RIGHT):
                                uTextureIndex = 103 + nAnimationCount % 8;     // Man geht rechts
                                nXoffs = - FONT_W + nAnimationCount * 3;    // Durch das Verkleinern des Mans wird dieser zusätzlich nach links gezogen
                                nYoffs = nAnimationCount;
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                Flip = SDL_FLIP_HORIZONTAL;
                                break;
                            case(EMERALD_ANIM_DOWN):
                                uTextureIndex = 119 + nAnimationCount % 8;     // Man geht runter
                                nXoffs = nAnimationCount / 2;
                                nYoffs = -FONT_H + nAnimationCount * 3;     // Durch das Verkleinern des Mans wird dieser zusätzlich nach oben gezogen
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                break;
                        }
                    } else if (uSelfStatus == EMERALD_ANIM_DOOR_READY_SHRINK) { // Shrink-Animation wird hier für Stahl-Kopf-Überblendung verwendet
                        bExtendedElement = true;
                        uTextureIndex_0 = 510 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8; // Endtür, blinkend
                        uTextureIndex = 475;   // Stahl mit Kopf
                        fScaleW = nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        nXoffs = 16 - nAnimationCount;
                        nYoffs = nXoffs;
                    }
                    break;
                case (EMERALD_DOOR_END_READY):
                    if (uSelfStatus == EMERALD_ANIM_MAN_GOES_ENDDOOR) {
                        uTextureIndex_0 = 319 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8; // Endtür, blinkend
                        uTextureIndex = EMERALD_SPACE;
                        bExtendedElement = true;
                        switch (uAnimStatus) {
                            case(EMERALD_ANIM_LEFT):
                                uTextureIndex = 103 + nAnimationCount % 8;     // Man geht links
                                nXoffs = FONT_W - nAnimationCount;
                                nYoffs = nAnimationCount * 1;
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                break;
                            case(EMERALD_ANIM_UP):
                                uTextureIndex = 111 + nAnimationCount % 8;     // Man geht hoch
                                nXoffs = nAnimationCount / 2;
                                nYoffs = FONT_H - nAnimationCount;        // Man steht bereits auf neuer Position, daher + FONT_H
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                break;
                            case(EMERALD_ANIM_RIGHT):
                                uTextureIndex = 103 + nAnimationCount % 8;     // Man geht rechts
                                nXoffs = - FONT_W + nAnimationCount * 3;    // Durch das Verkleinern des Mans wird dieser zusätzlich nach links gezogen
                                nYoffs = nAnimationCount;
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                Flip = SDL_FLIP_HORIZONTAL;
                                break;
                            case(EMERALD_ANIM_DOWN):
                                uTextureIndex = 119 + nAnimationCount % 8;     // Man geht runter
                                nXoffs = nAnimationCount / 2;
                                nYoffs = -FONT_H + nAnimationCount * 3;     // Durch das Verkleinern des Mans wird dieser zusätzlich nach oben gezogen
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                break;
                        }
                    } else if (uSelfStatus == EMERALD_ANIM_DOOR_READY_SHRINK) {
                        uTextureIndex = 319 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8; // Endtür, blinkend
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount * 2;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        uTextureIndex = 319 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8; // Endtür, blinkend
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        uTextureIndex = 319 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8; // Endtür, blinkend
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else {
                        uTextureIndex = 319 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8; // Endtür, blinkend
                    }
                    break;
                case (EMERALD_DOOR_END_NOT_READY_STEEL):
                    uTextureIndex = 509;
                    break;

                case (EMERALD_CENTRAL_EXPLOSION):
                case (EMERALD_CENTRAL_EXPLOSION_MEGA):
                case (EMERALD_CENTRAL_EXPLOSION_BEETLE):
                    if (nAnimationCount < 14) {
                        uTextureIndex = 279 + nAnimationCount / 2;
                    } else {
                        uTextureIndex = 279 + 5;
                    }
                    break;

                case (EMERALD_EXPLOSION_TO_ELEMENT_1):
                    uTextureIndex = 279 + nAnimationCount / 3; // bei (nAnimationCount = 15) -> 284
                    break;
                case (EMERALD_EXPLOSION_TO_ELEMENT_2):
                    switch (nAnimationCount) {
                        case (0):
                            uTextureIndex = 284;
                            break;
                        case (1):
                        case (2):
                            uTextureIndex = 285;
                            break;
                        case (3):
                        case (4):
                            uTextureIndex = 284;
                            break;
                        case (5):
                        case (6):
                            uTextureIndex = 283;
                            break;
                        case (7):
                        case (8):
                            uTextureIndex = 282;
                            break;
                        case (9):
                        case (10):
                            uTextureIndex = 281;
                            break;
                        case (11):
                        case (12):
                        case (13):
                            uTextureIndex = 280;
                            break;
                        default:    // 14 und 15
                            uTextureIndex = 279;
                            break;
                    }
                    break;
                case (EMERALD_MAGIC_WALL):
                case (EMERALD_MAGIC_WALL_STEEL):
                    if (uSelfStatus == EMERALD_ANIM_SAG_OUT_MAGIC_WALL) {
                        bExtendedElement = true;
                        uNewMagicElement = Playfield.pInvalidElement[I];
                        // SDL_Log("%s: element %x comes from blue wall",__FUNCTION__,uNewMagicElement);
                        nYoffs_0 = nAnimationCount * 2;
                        switch (uNewMagicElement) {
                            case (EMERALD_EMERALD):      // Stein -> Emerald
                                uTextureIndex_0 = 226 + nAnimationCount / 2;     // Emerald, fallend
                                break;
                            case (EMERALD_SAPPHIRE):     // Emerald -> Saphir
                                uTextureIndex_0 = 248 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
                                break;
                            case (EMERALD_STONE):        // Saphir -> Stein
                                uTextureIndex_0 = 71;
                                break;
                            case (EMERALD_BOMB):        // Perle -> Bombe
                                uTextureIndex_0 = 271 + nAnimationCount % 8;
                                break;
                            case (EMERALD_CRYSTAL):     // Kristall -> Kristall
                                uTextureIndex_0 = 309;
                                break;
                            case (EMERALD_MEGABOMB):    // Rubin -> Megabombe
                                uTextureIndex_0 = 524 + ((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 5;
                                break;
                            default:
                                SDL_Log("%s: warning: new unhandled magic element: %x at %d",__FUNCTION__,uNewMagicElement,I);
                                break;
                        }
                    }
                    // Magic Wall muss Element beim Austritt (teilweise) übermalen
                    nYoffs = 0; //- FONT_H;
                    if (Playfield.bMagicWallRunning) {
                        if (uLevelElement == EMERALD_MAGIC_WALL) {
                            uTextureIndex = 293 + nAnimationCount / 2;
                        } else {    // Steel-Magic-Wall
                            uTextureIndex = 466 + nAnimationCount / 2;
                        }
                    } else {
                        if (uLevelElement == EMERALD_MAGIC_WALL) {
                            uTextureIndex = 293;
                        } else {    // Steel-Magic-Wall
                            uTextureIndex = 466;
                        }
                    }
                    break;
                case(EMERALD_STONE_SINK):
                    uTextureIndex = 71;
                    if (uSelfStatus == EMERALD_ANIM_STONE_SWAMP1) {
                        nYoffs = nAnimationCount;
                    } else if (uSelfStatus == EMERALD_ANIM_STONE_SWAMP2){
                        nYoffs = (FONT_H / 2) + nAnimationCount;
                    }
                    break;
                case(EMERALD_STONE_SAG):
                    bExtendedElement = true;
                    // Zu übermalendes Element
                    uTextureIndex_0 = 71;   // Stein
                    nXoffs_0 = 0;
                    fScaleW_0 = 1;
                    fScaleH_0 = 1;
                    if (uSelfStatus == EMERALD_ANIM_STONE_SWAMP1) {
                        nYoffs_0 = nAnimationCount - FONT_H;
                    } else if (uSelfStatus == EMERALD_ANIM_STONE_SWAMP2){
                        nYoffs_0 = (FONT_H / 2) + nAnimationCount - FONT_H;
                    }
                    // Überdeckendes Element
                    nYoffs = -FONT_H;
                    uTextureIndex = 266;    // Sumpf
                    break;
                case (EMERALD_SWAMP):
                case (EMERALD_SWAMP_STONE):
                    uTextureIndex = 266;
                    break;
                case (EMERALD_ACIDPOOL_DESTROY):
                    // Splash rechts
                    Playfield.pPostAnimation[uPostAnimationIndex].uTextureIndex = 368;  // AcidSplash 1/15
                    Playfield.pPostAnimation[uPostAnimationIndex].uX = uX + 1;
                    Playfield.pPostAnimation[uPostAnimationIndex].uY = uY;
                    Playfield.pPostAnimation[uPostAnimationIndex].bFlipHorizontal = false;
                    Playfield.pPostAnimation[uPostAnimationIndex].bFlipVertical = false;
                    uPostAnimationIndex++;
                    // Splash links
                    Playfield.pPostAnimation[uPostAnimationIndex].uTextureIndex = 368;  // AcidSplash 1/15
                    Playfield.pPostAnimation[uPostAnimationIndex].uX = uX - 1;
                    Playfield.pPostAnimation[uPostAnimationIndex].uY = uY;
                    Playfield.pPostAnimation[uPostAnimationIndex].bFlipHorizontal = true;
                    Playfield.pPostAnimation[uPostAnimationIndex].bFlipVertical = false;
                    uPostAnimationIndex++;
                    uTextureIndex = GetTextureIndexByElementForAcidPool(Playfield.pInvalidElement[I],nAnimationCount,&fAngle);
                    nYoffs = nAnimationCount * 2;
                    break;
                case (EMERALD_ACIDPOOL_TOP_LEFT):
                    uTextureIndex = 257;        // Säurebecken, oben links
                    break;
                case (EMERALD_ACIDPOOL_TOP_MID):
                    uTextureIndex = 262 + (Playfield.uFrameCounter / 13) % 4;        // Säurebecken, oben mitte (aktives Feld), verlangsamte Animation (13 Frames für eine Animations-Phase)
                    break;
                case (EMERALD_ACIDPOOL_TOP_RIGHT):
                    uTextureIndex = 259;        // Säurebecken, oben rechts
                    break;
                case (EMERALD_ACIDPOOL_BOTTOM_LEFT):
                    uTextureIndex = 258;        // Säurebecken, unten links
                    break;
                case (EMERALD_ACIDPOOL_BOTTOM_MID):
                    uTextureIndex = 261;        // Säurebecken, unten mitte
                    break;
                case (EMERALD_ACIDPOOL_BOTTOM_RIGHT):
                    uTextureIndex = 260;        // Säurebecken, unten rechts
                    break;
                case (EMERALD_REPLICATOR_RED_SWITCH):
                    if (Playfield.bReplicatorRedOn) {
                        uTextureIndex = 155;     // Replikator-Schalter, rot, an
                    } else {
                        uTextureIndex = 154;     // Replikator-Schalter, rot, aus
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_SWITCH):
                    if (Playfield.bReplicatorYellowOn) {
                        uTextureIndex = 207;     // Replikator-Schalter, gelb, an
                    } else {
                        uTextureIndex = 206;     // Replikator-Schalter, gelb, aus
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_SWITCH):
                    if (Playfield.bReplicatorGreenOn) {
                        uTextureIndex = 189;     // Replikator-Schalter, grün, an
                    } else {
                        uTextureIndex = 188;     // Replikator-Schalter, grün, aus
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_SWITCH):
                    if (Playfield.bReplicatorBlueOn) {
                        uTextureIndex = 225;     // Replikator-Schalter, blau, an
                    } else {
                        uTextureIndex = 224;     // Replikator-Schalter, blau, aus
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_TOP_LEFT):
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
                case (EMERALD_REPLICATOR_RED_BOTTOM_LEFT):
                    if (Playfield.bReplicatorRedOn) {
                        uTextureIndex = 152;     // Replikator, rot, an, unten links, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 141;     // Replikator, rot, aus, unten links
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_BOTTOM_RIGHT):
                    if (Playfield.bReplicatorRedOn) {
                        uTextureIndex = 153;     // Replikator, rot, an, unten rechts, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 142;     // Replikator, rot, aus, unten rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_TOP_LEFT):
                    if (Playfield.bReplicatorGreenOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 177;     // Replikator, grün, an, oben links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 180;     // Replikator, grün, an, oben links, Animationsschritt 2
                        } else {
                            uTextureIndex = 183;     // Replikator, grün, an, oben links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 172;     // Replikator, grün, aus, oben links
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_TOP_MID):
                    if (Playfield.bReplicatorGreenOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 178;     // Replikator, grün, an, mitte links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 181;     // Replikator, grün, an, mitte links, Animationsschritt 2
                        } else {
                            uTextureIndex = 184;     // Replikator, grün, an, mitte links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 173;     // Replikator, grün, aus, oben mitte
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_TOP_RIGHT):
                    if (Playfield.bReplicatorGreenOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 179;     // Replikator, grün, an, oben rechts, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 182;     // Replikator, grün, an, oben rechts, Animationsschritt 2
                        } else {
                            uTextureIndex = 185;     // Replikator, grün, an, oben rechts, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 174;     // Replikator, grün, aus, oben rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT):
                    if (Playfield.bReplicatorGreenOn) {
                        uTextureIndex = 186;     // Replikator, grün, an, unten links, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 175;     // Replikator, grün, aus, unten links
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT):
                    if (Playfield.bReplicatorGreenOn) {
                        uTextureIndex = 187;     // Replikator, grün, an, unten rechts, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 176;     // Replikator, grün, aus, unten rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_TOP_LEFT):
                    if (Playfield.bReplicatorBlueOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 213;     // Replikator, blau, an, oben links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 216;     // Replikator, blau, an, oben links, Animationsschritt 2
                        } else {
                            uTextureIndex = 219;     // Replikator, blau, an, oben links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 208;     // Replikator, blau, aus, oben links
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_TOP_MID):
                    if (Playfield.bReplicatorBlueOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 214;     // Replikator, blau, an, mitte links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 217;     // Replikator, blau, an, mitte links, Animationsschritt 2
                        } else {
                            uTextureIndex = 220;     // Replikator, blau, an, mitte links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 209;     // Replikator, blau, aus, oben mitte
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_TOP_RIGHT):
                    if (Playfield.bReplicatorBlueOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 215;     // Replikator, blau, an, oben rechts, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 218;     // Replikator, blau, an, oben rechts, Animationsschritt 2
                        } else {
                            uTextureIndex = 221;     // Replikator, blau, an, oben rechts, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 210;     // Replikator, blau, aus, oben rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT):
                    if (Playfield.bReplicatorBlueOn) {
                        uTextureIndex = 222;     // Replikator, blau, an, unten links, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 211;     // Replikator, blau, aus, unten links
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT):
                    if (Playfield.bReplicatorBlueOn) {
                        uTextureIndex = 223;     // Replikator, blau, an, unten rechts, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 212;     // Replikator, blau, aus, unten rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_TOP_LEFT):
                    if (Playfield.bReplicatorYellowOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 195;     // Replikator, gelb, an, oben links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 198;     // Replikator, gelb, an, oben links, Animationsschritt 2
                        } else {
                            uTextureIndex = 201;     // Replikator, gelb, an, oben links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 190;     // Replikator, gelb, aus, oben links
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_TOP_MID):
                    if (Playfield.bReplicatorYellowOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 196;     // Replikator, gelb, an, mitte links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 199;     // Replikator, gelb, an, mitte links, Animationsschritt 2
                        } else {
                            uTextureIndex = 202;     // Replikator, gelb, an, mitte links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 191;     // Replikator, gelb, aus, oben mitte
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_TOP_RIGHT):
                    if (Playfield.bReplicatorYellowOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = 197;     // Replikator, gelb, an, oben rechts, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = 200;     // Replikator, gelb, an, oben rechts, Animationsschritt 2
                        } else {
                            uTextureIndex = 203;     // Replikator, gelb, an, oben rechts, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = 192;     // Replikator, gelb, aus, oben rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT):
                    if (Playfield.bReplicatorYellowOn) {
                        uTextureIndex = 204;     // Replikator, gelb, an, unten links, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 193;     // Replikator, gelb, aus, unten links
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT):
                    if (Playfield.bReplicatorYellowOn) {
                        uTextureIndex = 205;     // Replikator, gelb, an, unten rechts, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = 194;     // Replikator, gelb, aus, unten rechts
                    }
                    break;
                case (EMERALD_KEY_WHITE):
                    uTextureIndex = 519;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_KEY_WHITE_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_KEY_GENERAL):
                    uTextureIndex = 520;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_KEY_GENERAL_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_KEY_RED):
                    uTextureIndex = 98;     // roter Schlüssel
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_KEY_RED_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_KEY_YELLOW):
                    uTextureIndex = 101;    // gelber Schlüssel
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_KEY_YELLOW_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_KEY_BLUE):
                    uTextureIndex = 100;    // blauer Schlüssel
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_KEY_BLUE_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_KEY_GREEN):
                    uTextureIndex = 99;     // grüner Schlüssel
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_KEY_GREEN_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_DOOR_WHITE):
                    uTextureIndex = 521;
                    break;
                case (EMERALD_DOOR_WHITE_WOOD):
                    uTextureIndex = 522;
                    break;
                case (EMERALD_DOOR_RED_WOOD):     // rote Holztür
                    uTextureIndex = 462;
                    break;
                case (EMERALD_DOOR_YELLOW_WOOD):  // gelbe Holztür
                    uTextureIndex = 465;
                    break;
                case (EMERALD_DOOR_BLUE_WOOD):    // blaue Holztür
                    uTextureIndex = 464;
                    break;
                case (EMERALD_DOOR_GREEN_WOOD):   // grüne Holztür
                    uTextureIndex = 463;
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
                case (EMERALD_DOOR_GREY_RED):
                    if (Playfield.bLightOn) {
                        uTextureIndex = 289;     // Geheimtür
                    } else {
                        uTextureIndex = 288;     // Geheimtür
                    }
                    break;
                case (EMERALD_DOOR_GREY_GREEN):
                    if (Playfield.bLightOn) {
                        uTextureIndex = 290;     // Geheimtür
                    } else {
                        uTextureIndex = 288;     // Geheimtür
                    }
                    break;
                case (EMERALD_DOOR_GREY_BLUE):
                    if (Playfield.bLightOn) {
                        uTextureIndex = 291;     // Geheimtür
                    } else {
                        uTextureIndex = 288;     // Geheimtür
                    }
                    break;
                case (EMERALD_DOOR_GREY_YELLOW):
                    if (Playfield.bLightOn) {
                        uTextureIndex = 292;     // Geheimtür
                    } else {
                        uTextureIndex = 288;     // Geheimtür
                    }
                    break;
                case (EMERALD_DOOR_GREY_WHITE):
                    if (Playfield.bLightOn) {
                        uTextureIndex = 523;     // Geheimtür
                    } else {
                        uTextureIndex = 288;     // Geheimtür
                    }
                    break;
                case (EMERALD_MAN):
                    // Nimmt Man Gegenstand auf, d.h. läuft er in das Element hinein?
                    if ( (uSelfStatus == EMERALD_ANIM_KEY_RED_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_KEY_GREEN_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_KEY_BLUE_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_KEY_YELLOW_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_KEY_WHITE_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_KEY_GENERAL_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_EMERALD_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_RUBY_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_PERL_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_CRYSTAL_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_TIME_COIN_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_HAMMER_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_DYNAMITE_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_MESSAGE_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_SAPPHIRE_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_GRASS_SHRINK) ||
                         ((uSelfStatus == EMERALD_ANIM_SAND_INVISIBLE_SHRINK) && (Playfield.bLightOn)) ||
                         (uSelfStatus == EMERALD_ANIM_SAND_SHRINK) ) {
                            bExtendedElement = true;
                            uTextureIndex_0 = GetTextureIndexByShrink(uSelfStatus);
                            nXoffs_0 = nAnimationCount;
                            nYoffs_0 = nAnimationCount;
                            fScaleW_0 = 1 - nAnimationCount * 0.06;
                            fScaleH_0 = fScaleW_0;
                    }
                    if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        nXoffs = FONT_W - nAnimationCount * 2;         // Man steht bereits auf neuer Position, daher + FONT_W
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_LEFT) {
                            Flip = SDL_FLIP_HORIZONTAL;
                            if (nAnimationCount <= 9) {
                                uTextureIndex = 243 + nAnimationCount / 2;     // Man links schiebend
                            } else {
                                uTextureIndex = 246 + 5 - nAnimationCount / 2;
                            }
                        } else {
                            if (uSelfStatus == EMERALD_ANIM_MAN_BLOCKED_LEFT) {
                                nXoffs = 0;
                            }
                            uTextureIndex = 103 + nAnimationCount % 8;     // Man links
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        nXoffs = nAnimationCount * 2 - FONT_W;      // Man steht bereits auf neuer Position, daher - FONT_W
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT) {
                            if (nAnimationCount <= 9) {
                                uTextureIndex = 243 + nAnimationCount / 2;     // Man rechts schiebend
                            } else {
                                uTextureIndex = 246 + 5 - nAnimationCount / 2;
                            }
                        } else {
                            if (uSelfStatus == EMERALD_ANIM_MAN_BLOCKED_RIGHT) {
                                nXoffs = 0;
                            }
                            uTextureIndex = 103 + nAnimationCount % 8; // Man rechts
                            Flip = SDL_FLIP_HORIZONTAL;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_UP) {
                        uTextureIndex = 111 + nAnimationCount % 8;     // Man hoch
                        if (uSelfStatus == EMERALD_ANIM_MAN_BLOCKED_UP) {
                            nYoffs = 0;
                        } else {
                            nYoffs = -nAnimationCount * 2 + FONT_H;        // Man steht bereits auf neuer Position, daher + FONT_H
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        uTextureIndex = 119 + nAnimationCount % 8;     // Man runter

                        if (uSelfStatus == EMERALD_ANIM_MAN_BLOCKED_DOWN) {
                            nYoffs = 0;
                        } else {
                          nYoffs = nAnimationCount * 2 - FONT_H;         // Man steht bereits auf neuer Position, daher - FONT_H
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT_DOUBLESPEED) {
                        uTextureIndex = 103 + nAnimationCount % 8;     // Man links
                        nXoffs = -nAnimationCount * 4 + FONT_W * 2;    // Man steht bereits auf neuer Position, daher + FONT_W * 2 (DoubleSpeed)
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT_DOUBLESPEED) {
                        uTextureIndex = 103 + nAnimationCount % 8;     // Man rechts
                        nXoffs = nAnimationCount * 4 - FONT_W * 2;     // Man steht bereits auf neuer Position, daher - FONT_W * 2 (DoubleSpeed)
                        Flip = SDL_FLIP_HORIZONTAL;
                    } else if (uAnimStatus == EMERALD_ANIM_UP_DOUBLESPEED) {
                        uTextureIndex = 111 + nAnimationCount % 8;     // Man hoch
                        nYoffs = -nAnimationCount * 4 + FONT_H * 2;    // Man steht bereits auf neuer Position, daher + FONT_H * 2 (DoubleSpeed)
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN_DOUBLESPEED) {
                        uTextureIndex = 119 + nAnimationCount % 8;     // Man runter
                        nYoffs = nAnimationCount * 4 - FONT_H * 2;    // Man steht bereits auf neuer Position, daher - FONT_H * 2 (DoubleSpeed)
                    } else {
                        if (uSelfStatus == EMERALD_ANIM_MAN_LEFT_ARM) {
                            uTextureIndex = 240;                        // Man stehend, linker Arm
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_RIGHT_ARM) {
                            uTextureIndex = 241;                        // Man stehend, rechter Arm
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_UP_ARM) {
                            uTextureIndex = 242;                        // Man stehend, Arm hoch
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_DOWN_ARM) {
                            uTextureIndex = 239;                        // Man stehend, Arm runter
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT) {
                            if (nAnimationCount <= 9) {
                                uTextureIndex = 243 + nAnimationCount / 2;     // Man rechts schiebend
                            } else {
                                uTextureIndex = 246 + 5 - nAnimationCount / 2;
                            }
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_LEFT) {
                            Flip = SDL_FLIP_HORIZONTAL;
                            if (nAnimationCount <= 9) {
                                uTextureIndex = 243 + nAnimationCount / 2;     // Man rechts schiebend
                            } else {
                                uTextureIndex = 246 + 5 - nAnimationCount / 2;
                            }
                        } else {
                            // Steht Man noch auf selbst gezündeten Dynamit?
                            if ((Playfield.uManXpos + Playfield.uManYpos * Playfield.uLevel_X_Dimension) == Playfield.uDynamitePos) {
                                bExtendedElement = true;
                                uTextureIndex_0 = 102;                            // Man stehend, unter Dynamit
                                switch (Playfield.uDynamiteStatusAnim) {
                                    case (EMERALD_ANIM_DYNAMITE_ON_P1):
                                        uTextureIndex = 555;
                                        break;
                                    case (EMERALD_ANIM_DYNAMITE_ON_P2):
                                        uTextureIndex = 556;
                                        break;
                                    case (EMERALD_ANIM_DYNAMITE_ON_P3):
                                        uTextureIndex = 557;
                                        break;
                                    case (EMERALD_ANIM_DYNAMITE_ON_P4):
                                        uTextureIndex = 558;
                                        break;
                                    default:
                                        uTextureIndex = 555;
                                        break;
                                }
                            } else {
                                uTextureIndex = 102;                            // Man stehend
                            }
                        }
                    }
                    break;
                case (EMERALD_BOMB):
                    uTextureIndex = 271;     // Bomb, liegend
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        uTextureIndex = 271 + nAnimationCount % 8;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_LEFT2) {
                            nXoffs = -nAnimationCount * 2 + FONT_W;
                        } else {
                            nXoffs = -nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        uTextureIndex = 271 + nAnimationCount % 8;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT2) {
                            nXoffs = nAnimationCount * 2 - FONT_W;
                        } else {
                            nXoffs = nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        nYoffs = nAnimationCount * 2;
                        uTextureIndex = 271 + nAnimationCount % 8;
                    }
                    break;
                case (EMERALD_MEGABOMB):
                    uTextureIndex = 524;     // Mega-Bomb, liegend
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        fAngleOffs = -nAnimationCount * 22.5;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_LEFT2) {
                            nXoffs = -nAnimationCount * 2 + FONT_W;
                        } else {
                            nXoffs = -nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        fAngleOffs = nAnimationCount * 22.5;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT2) {
                            nXoffs = nAnimationCount * 2 - FONT_W;
                        } else {
                            nXoffs = nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        uTextureIndex = 524 + ((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 5;
                        nYoffs = nAnimationCount * 2;
                    }
                    break;
                case (EMERALD_PERL):
                    uTextureIndex = 436;     // Perle, liegend
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_PERL_SHRINK) { // Man mit Richtung und Fire
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            fAngleOffs = nAnimationCount * 10;
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        uTextureIndex = 436 + nAnimationCount % 8;
                        nXoffs = -nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        uTextureIndex = 436 + nAnimationCount % 8;
                        nXoffs = nAnimationCount * 2;
                    } else if ((uAnimStatus == EMERALD_ANIM_DOWN) || (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL)) {
                        nYoffs = nAnimationCount * 2;
                        uTextureIndex = 436 + nAnimationCount % 8;
                    } else if (uSelfStatus == EMERALD_ANIM_PERL_BREAK) {
                        if (nAnimationCount <= 2) {   // 0, 1, 2
                            uTextureIndex = 444;
                        } else if (nAnimationCount <= 5) { // 3, 4, 5
                            uTextureIndex = 445;
                        } else if (nAnimationCount <= 8) { // 6, 7, 8
                            uTextureIndex = 446;
                        } else {
                            uTextureIndex = 0;  // Space
                        }
                    }
                    break;
                case (EMERALD_STONE):
                    uTextureIndex = 71;     // Stein, liegend
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        fAngleOffs = -nAnimationCount * 22.5;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_LEFT2) {
                            nXoffs = -nAnimationCount * 2 + FONT_W;
                        } else {
                            nXoffs = -nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        fAngleOffs = nAnimationCount * 22.5;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT2) {
                            nXoffs = nAnimationCount * 2 - FONT_W;
                        } else {
                            nXoffs = nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        nYoffs = nAnimationCount * 2;
                        if (uSelfStatus == EMERALD_ANIM_SAPPHIRE_SQUEAK) {
                            nYoffs = nAnimationCount * 2 - FONT_H;
                            bExtendedElement = true;
                            uTextureIndex_0 = 248; // Saphir, liegend
                            nXoffs_0 = 0;
                            nYoffs_0 = nAnimationCount * 2;
                            fScaleW_0 = 1;
                            fScaleH_0 = 1 - nAnimationCount * 0.06;
                        }
                    } else if (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL) {
                        nYoffs = nAnimationCount * 2;
                    }
                    break;
                case (EMERALD_NUT):
                    uTextureIndex = 234;     // Nut, liegend
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_NUT_CRACK2) {
                        uTextureIndex = 235 + nAnimationCount / 4;     // Nut, liegend
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        fAngleOffs = -nAnimationCount * 22.5;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_LEFT2) {
                            nXoffs = -nAnimationCount * 2 + FONT_W;
                        } else {
                            nXoffs = -nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        fAngleOffs = nAnimationCount * 22.5;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT2) {
                            nXoffs = nAnimationCount * 2 - FONT_W;
                        } else {
                            nXoffs = nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        nYoffs = nAnimationCount * 2;
                    }
                    break;
                case (EMERALD_RUBY):
                    // Soll Rubin blitzen ?
                    if ((nAnimationCount == 0) && (Playfield.pStatusAnimation[I] == EMERALD_ANIM_STAND)) {
                        if (randn(0,100) < 10) {
                            Playfield.pStatusAnimation[I] = EMERALD_ANIM_BLITZ;
                        }
                    }
                    if (Playfield.pStatusAnimation[I] != EMERALD_ANIM_BLITZ) {
                        uTextureIndex = 301;     // Rubin, liegend
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs = 15 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs = 7 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = 0.5 + nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (uSelfStatus == EMERALD_ANIM_RUBY_SHRINK) { // Man mit Richtung und Fire
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            fAngleOffs = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            uTextureIndex = 301 + nAnimationCount / 2;     // Rubin, liegend
                            nXoffs = -nAnimationCount * 2;
                            fAngleOffs = -nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            uTextureIndex = 301 + nAnimationCount / 2;     // Rubin, liegend
                            nXoffs = nAnimationCount * 2;
                            fAngleOffs = nAnimationCount * 22.5;
                        } else if ((uAnimStatus == EMERALD_ANIM_DOWN) || (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL)) {
                            nYoffs = nAnimationCount * 2;
                            uTextureIndex = 301 + nAnimationCount / 2;     // Rubin, liegend
                        }
                    } else {
                        bExtendedElement = true;
                        fAngle_0 = 0;
                        uTextureIndex_0 = 301;     // Rubin, liegend
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs_0 = 15 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs_0 = 7 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = 0.5 + nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (uSelfStatus == EMERALD_ANIM_RUBY_SHRINK) { // Man mit Richtung und Fire
                            nXoffs_0 = nAnimationCount;
                            nYoffs_0 = nAnimationCount;
                            fScaleW_0 = 1 - nAnimationCount * 0.06;
                            fScaleH_0 = fScaleW_0;
                            fAngleOffs_0 = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            uTextureIndex_0 = 301 + nAnimationCount / 2;     // Rubin, liegend
                            nXoffs_0 = -nAnimationCount * 2;
                            fAngleOffs_0 = -nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            uTextureIndex_0 = 301 + nAnimationCount / 2;     // Rubin, liegend
                            nXoffs_0 = nAnimationCount * 2;
                            fAngleOffs_0 = nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs_0 = nAnimationCount * 2;
                            uTextureIndex_0 = 301 + nAnimationCount / 2;     // Rubin, liegend
                        }
                        // Blitz-Animation über Rubin
                        nXoffs = 0;
                        nYoffs = 0;
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 267 + nAnimationCount / 2;
                        } else {
                            uTextureIndex = 270 - (nAnimationCount - 8) / 2;
                        }
                    }
                    break;
                case (EMERALD_CRYSTAL):
                    // Soll Kristall blitzen ?
                    uTextureIndex = 309;     // Kristall, liegend
                    uTextureIndex_0 = 309;     // Kristall, liegend
                    if ((nAnimationCount == 0) && (Playfield.pStatusAnimation[I] == EMERALD_ANIM_STAND)) {
                        if (randn(0,100) < 10) {
                            Playfield.pStatusAnimation[I] = EMERALD_ANIM_BLITZ;
                        }
                    }
                    if (Playfield.pStatusAnimation[I] != EMERALD_ANIM_BLITZ) {
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs = 15 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs = 7 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = 0.5 + nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (uSelfStatus == EMERALD_ANIM_CRYSTAL_SHRINK) { // Man mit Richtung und Fire
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            fAngleOffs = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            nXoffs = -nAnimationCount * 2;
                            fAngleOffs = -nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            nXoffs = nAnimationCount * 2;
                            fAngleOffs = nAnimationCount * 22.5;
                        } else if ((uAnimStatus == EMERALD_ANIM_DOWN) || (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL)) {
                            nYoffs = nAnimationCount * 2;
                        }
                    } else {
                        bExtendedElement = true;
                        fAngle_0 = 0;
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs_0 = 15 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs_0 = 7 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = 0.5 + nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (uSelfStatus == EMERALD_ANIM_CRYSTAL_SHRINK) { // Man mit Richtung und Fire
                            nXoffs_0 = nAnimationCount;
                            nYoffs_0 = nAnimationCount;
                            fScaleW_0 = 1 - nAnimationCount * 0.06;
                            fScaleH_0 = fScaleW_0;
                            fAngleOffs_0 = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            nXoffs_0 = -nAnimationCount * 2;
                            fAngleOffs_0 = -nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            nXoffs_0 = nAnimationCount * 2;
                            fAngleOffs_0 = nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs_0 = nAnimationCount * 2;
                        }
                        // Blitz-Animation über Kristall
                        nXoffs = 0;
                        nYoffs = 0;
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 267 + nAnimationCount / 2;
                        } else {
                            uTextureIndex = 270 - (nAnimationCount - 8) / 2;
                        }
                    }
                    break;
                case (EMERALD_EMERALD):
                    // Soll Emerald blitzen ?
                    if ((nAnimationCount == 0) && (Playfield.pStatusAnimation[I] == EMERALD_ANIM_STAND)) {
                        if (randn(0,100) < 10) {
                            Playfield.pStatusAnimation[I] = EMERALD_ANIM_BLITZ;
                        }
                    }
                    if (Playfield.pStatusAnimation[I] != EMERALD_ANIM_BLITZ) {
                        uTextureIndex = 226;     // Emerald, liegend
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs = 15 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs = 7 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = 0.5 + nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (uSelfStatus == EMERALD_ANIM_EMERALD_SHRINK) { // Man mit Richtung und Fire
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            fAngleOffs = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            uTextureIndex = 226 + nAnimationCount / 2;     // Emerald, liegend
                            nXoffs = -nAnimationCount * 2;
                            fAngleOffs = -nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            uTextureIndex = 226 + nAnimationCount / 2;     // Emerald, liegend
                            nXoffs = nAnimationCount * 2;
                            fAngleOffs = nAnimationCount * 22.5;
                        } else if ((uAnimStatus == EMERALD_ANIM_DOWN) || (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL)) {
                            nYoffs = nAnimationCount * 2;
                            uTextureIndex = 226 + nAnimationCount / 2;     // Emerald, liegend
                        }
                    } else {
                        bExtendedElement = true;
                        fAngle_0 = 0;
                        uTextureIndex_0 = 226;     // Emerald, liegend
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs_0 = 15 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs_0 = 7 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = 0.5 + nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (uSelfStatus == EMERALD_ANIM_EMERALD_SHRINK) { // Man mit Richtung und Fire
                            nXoffs_0 = nAnimationCount;
                            nYoffs_0 = nAnimationCount;
                            fScaleW_0 = 1 - nAnimationCount * 0.06;
                            fScaleH_0 = fScaleW_0;
                            fAngleOffs_0 = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            uTextureIndex_0 = 226 + nAnimationCount / 2;     // Emerald, liegend
                            nXoffs_0 = -nAnimationCount * 2;
                            fAngleOffs_0 = -nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            uTextureIndex_0 = 226 + nAnimationCount / 2;     // Emerald, liegend
                            nXoffs_0 = nAnimationCount * 2;
                            fAngleOffs_0 = nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs_0 = nAnimationCount * 2;
                            uTextureIndex_0 = 226 + nAnimationCount / 2;     // Emerald, liegend
                        }
                        // Blitz-Animation über Emerald
                        nXoffs = 0;
                        nYoffs = 0;
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 267 + nAnimationCount / 2;
                        } else {
                            uTextureIndex = 270 - (nAnimationCount - 8) / 2;
                        }
                    }
                    break;
                case (EMERALD_SAPPHIRE):
                    // Soll Saphir blitzen ?
                    if ((nAnimationCount == 0) && (Playfield.pStatusAnimation[I] == EMERALD_ANIM_STAND)) {
                        if (randn(0,100) < 10) {
                            Playfield.pStatusAnimation[I] = EMERALD_ANIM_BLITZ;
                        }
                    }
                    if (Playfield.pStatusAnimation[I] != EMERALD_ANIM_BLITZ) {
                        uTextureIndex = 248;
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs = 15 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs = 7 - nAnimationCount / 2;
                            nYoffs = nXoffs;
                            fScaleW = 0.5 + nAnimationCount * 0.031;
                            fScaleH = fScaleW;
                        } else if (uSelfStatus == EMERALD_ANIM_SAPPHIRE_SHRINK) { // Man mit Richtung und Fire
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            fAngleOffs = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            nXoffs = -nAnimationCount * 2;
                            fAngleOffs = -nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            nXoffs = nAnimationCount * 2;
                            fAngleOffs = nAnimationCount * 22.5;
                        } else if ((uAnimStatus == EMERALD_ANIM_DOWN) || (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL)) {
                            nYoffs = nAnimationCount * 2;
                            uTextureIndex = 248 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
                        }
                    } else {
                        bExtendedElement = true;
                        fAngle_0 = 0;
                        uTextureIndex_0 = 248;     // Saphir, liegend
                        if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                            nXoffs_0 = 15 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                            nXoffs_0 = 7 - nAnimationCount / 2;
                            nYoffs_0 = nXoffs_0;
                            fScaleW_0 = 0.5 + nAnimationCount * 0.031;
                            fScaleH_0 = fScaleW_0;
                        } else if (uSelfStatus == EMERALD_ANIM_SAPPHIRE_SHRINK) { // Man mit Richtung und Fire
                            nXoffs_0 = nAnimationCount;
                            nYoffs_0 = nAnimationCount;
                            fScaleW_0 = 1 - nAnimationCount * 0.06;
                            fScaleH_0 = fScaleW_0;
                            fAngleOffs_0 = nAnimationCount * 10;
                        } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                            nXoffs_0 = -nAnimationCount * 2;
                            fAngleOffs_0 = -nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            nXoffs_0 = nAnimationCount * 2;
                            fAngleOffs_0 = nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs_0 = nAnimationCount * 2;
                            uTextureIndex_0 = 248 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
                        }
                        // Blitz-Animation über Saphir
                        nXoffs = 0;
                        nYoffs = 0;
                        if (nAnimationCount <= 7) {
                            uTextureIndex = 267 + nAnimationCount / 2;
                        } else {
                            uTextureIndex = 270 - (nAnimationCount - 8) / 2;
                        }
                    }
                    break;
                case (EMERALD_TIME_COIN):
                    uTextureIndex = 310 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 6; // Zeitmünze, drehend
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_TIME_COIN_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_HAMMER):
                    uTextureIndex = 318;                             // Hammer
                    if (uSelfStatus == EMERALD_ANIM_HAMMER_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    }
                    break;
                case (EMERALD_DYNAMITE_OFF):
                    uTextureIndex = 286;                                // Dynamit
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uSelfStatus == EMERALD_ANIM_DYNAMITE_SHRINK) {  // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_ALIEN):
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        uTextureIndex = 127 + nAnimationCount / 2;     // Alien stehend
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        uTextureIndex = 127 + nAnimationCount / 2;     // Alien stehend
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_STAND) {
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
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_WHEEL_RUN) {
                        fAngleOffs = nAnimationCount * 11;
                    } else {
                        fAngleOffs = 0;
                    }
                    break;
                case (EMERALD_STEEL):
                    uTextureIndex = 72;     // Mauer hart
                    break;
                case (EMERALD_WALL_CORNERED):
                    uTextureIndex = 316;     // Mauer eckig
                    break;
                case (EMERALD_WALL_ROUND):
                    uTextureIndex = 317;     // Mauer rund
                    break;
                case (EMERALD_SANDMINE):
                    // Hinweis: Die äußeren Klammern sind wichtig !
                    uTextureIndex = 766 + (((Playfield.pStatusAnimation[I] & 0xFF00) - EMERALD_ANIM_SAND_0) >> 8);    // Sandmine mit richtigem Rand aussuchen
                    break;
                case (EMERALD_SAND):
                    if (uSelfStatus == EMERALD_ANIM_SAND_SHRINK) { // Man mit Richtung und Fire
                        uTextureIndex = 156; // kompl. freier Sand
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 22.5;
                    } else {
                        // Hinweis: Die äußeren Klammern sind wichtig !
                        uTextureIndex = 156 + (((Playfield.pStatusAnimation[I] & 0xFF00) - EMERALD_ANIM_SAND_0) >> 8);    // Sand mit richtigem Rand aussuchen
                    }
                    break;
                case (EMERALD_SAND_INVISIBLE):
                    if (Playfield.bLightOn) {
                        if (uSelfStatus == EMERALD_ANIM_SAND_INVISIBLE_SHRINK) { // Man mit Richtung und Fire
                            uTextureIndex = 783; // kompl. freier Sand
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            fAngleOffs = nAnimationCount * 22.5;
                        } else {
                            // Hinweis: Die äußeren Klammern sind wichtig !
                            uTextureIndex = 783 + (((Playfield.pStatusAnimation[I] & 0xFF00) - EMERALD_ANIM_SAND_0) >> 8);    // Sand mit richtigem Rand aussuchen
                        }
                    } else {
                        uTextureIndex = 0;
                    }
                    break;
                case (EMERALD_GRASS):
                    if (uSelfStatus == EMERALD_ANIM_GRASS_SHRINK) { // Man mit Richtung und Fire
                        uTextureIndex = 750; // kompl. freies Gras
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 22.5;
                    } else {
                        // Hinweis: Die äußeren Klammern sind wichtig !
                        uTextureIndex = 750 + (((Playfield.pStatusAnimation[I] & 0xFF00) - EMERALD_ANIM_SAND_0) >> 8);    // Gras mit richtigem Rand aussuchen
                    }
                    break;
                case (EMERALD_GRASS_COMES):
                    uTextureIndex = 750; // kompl. freies Gras
                    nXoffs = (FONT_W / 2) - nAnimationCount;
                    nYoffs = (FONT_H / 2) - nAnimationCount;
                    fScaleW = nAnimationCount * 0.06;
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_SPACE):
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
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
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
                case (EMERALD_MOLE_UP):     // Mole hoch
                    fAngle = 90;
                     uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
                    if (uAnimStatus == EMERALD_ANIM_UP) {
                        nYoffs = - nAnimationCount * 2;
                        // Molen-Sand
                        bExtendedElement = true;
                        uTextureIndex_0 = 171;        // Sand
                        nXoffs_0 = 15 - nAnimationCount;;
                        nYoffs_0 = 15 - nAnimationCount;
                        fScaleW_0 = nAnimationCount * 0.06;
                        fScaleH_0 = fScaleW_0;
                    } else if (uAnimStatus == EMERALD_ANIM_MOLE_STAND) {
                        nYoffs = 0;         // läuft auf der Stelle
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_RIGHT) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_UP_TO_LEFT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_MOLE_RIGHT):  // Mole rechts
                    fAngle = 180;
                     uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
                    if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        nXoffs = nAnimationCount * 2;
                        // Molen-Sand
                        bExtendedElement = true;
                        uTextureIndex_0 = 171;        // Sand
                        nXoffs_0 = 15 - nAnimationCount;;
                        nYoffs_0 = 15 - nAnimationCount;
                        fScaleW_0 = nAnimationCount * 0.06;
                        fScaleH_0 = fScaleW_0;
                    } else if (uAnimStatus == EMERALD_ANIM_MOLE_STAND) {
                        nXoffs = 0;         // läuft auf der Stelle
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_DOWN) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_RIGHT_TO_UP) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                     break;
                case (EMERALD_MOLE_DOWN):   // Mole runter (kann über 2 Steuerungsphasen geboren werden)
                    fAngle = 270;
                    uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        nYoffs = nAnimationCount * 2;
                        // Molen-Sand
                        bExtendedElement = true;
                        uTextureIndex_0 = 171;        // Sand
                        nXoffs_0 = 15 - nAnimationCount;;
                        nYoffs_0 = 15 - nAnimationCount;
                        fScaleW_0 = nAnimationCount * 0.06;
                        fScaleH_0 = fScaleW_0;
                    } else if (uAnimStatus == EMERALD_ANIM_MOLE_STAND) {
                        nYoffs = 0;         // läuft auf der Stelle
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_LEFT) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_RIGHT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_MOLE_LEFT):
                    fAngle = 0;
                     uTextureIndex = 450 + Playfield.uFrameCounter % 11;     // Mole links
                    if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        nXoffs = -nAnimationCount * 2;
                        // Molen-Sand
                        bExtendedElement = true;
                        uTextureIndex_0 = 171;        // Sand
                        nXoffs_0 = 15 - nAnimationCount;;
                        nYoffs_0 = 15 - nAnimationCount;
                        fScaleW_0 = nAnimationCount * 0.06;
                        fScaleH_0 = fScaleW_0;
                    } else if (uAnimStatus == EMERALD_ANIM_MOLE_STAND) {
                        nXoffs = 0;         // läuft auf der Stelle
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_DOWN) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_UP) {
                        fAngleOffs = nAnimationCount * 5.5; // OK
                    } else {
                        SDL_Log("%s: left mole unhandled status: %u",__FUNCTION__,uAnimStatus);
                    }
                    break;
                case (EMERALD_MINE_UP):     // Mine hoch
                    if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                        uTextureIndex = 73;     // Mine links
                    } else {
                        uTextureIndex = 74;     // Mine links an
                    }
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
                    if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                        uTextureIndex = 73;     // Mine links
                    } else {
                        uTextureIndex = 74;     // Mine links an
                    }
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
                    if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                        uTextureIndex = 73;     // Mine links
                    } else {
                        uTextureIndex = 74;     // Mine links an
                    }
                    fAngle = 270;
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        nYoffs = nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_LEFT) {
                        fAngleOffs = nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_DOWN_TO_RIGHT) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    }
                    break;
                case (EMERALD_MINE_LEFT):   // Mine links
                    if ( ((nAnimationCount >= 0) && (nAnimationCount <= 3)) || ((nAnimationCount >= 8) && (nAnimationCount <=11)) ) {
                        uTextureIndex = 73;     // Mine links
                    } else {
                        uTextureIndex = 74;     // Mine links an
                    }
                    fAngle = 0;
                    if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        nXoffs = -nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_DOWN) {
                        fAngleOffs = -nAnimationCount * 5.5;
                    } else if (uAnimStatus == EMERALD_ANIM_SPIN_LEFT_TO_UP) {
                        fAngleOffs = nAnimationCount * 5.5; // OK
                    }
                    break;
                case (EMERALD_FONT_EXCLAMATION):
                    uTextureIndex = 1;
                    break;
                case (EMERALD_FONT_ARROW_RIGHT):
                    uTextureIndex = 4;
                    break;
                case (EMERALD_FONT_ARROW_UP):
                    uTextureIndex = 5;
                    break;
                case (EMERALD_FONT_ARROW_DOWN):
                    uTextureIndex = 6;
                    break;
                case (EMERALD_FONT_APOSTROPHE):
                    uTextureIndex = 7;
                    break;
                case (EMERALD_FONT_BRACE_OPEN):
                    uTextureIndex = 8;
                    break;
                case (EMERALD_FONT_BRACE_CLOSE):
                    uTextureIndex = 9;
                    break;
                case (EMERALD_FONT_COPYRIGHT):
                    uTextureIndex = 10;
                    break;
                case (EMERALD_FONT_PLUS):
                    uTextureIndex = 11;
                    break;
                case (EMERALD_FONT_COMMA):
                    uTextureIndex = 12;
                    break;
                case (EMERALD_FONT_MINUS):
                    uTextureIndex = 13;
                    break;
                case (EMERALD_FONT_POINT):
                    uTextureIndex = 14;
                    break;
                case (EMERALD_FONT_SLASH):
                    uTextureIndex = 15;
                    break;
                case (EMERALD_FONT_0):
                    uTextureIndex = 16;
                    break;
                case (EMERALD_FONT_1):
                    uTextureIndex = 17;
                    break;
                case (EMERALD_FONT_2):
                    uTextureIndex = 18;
                    break;
                case (EMERALD_FONT_3):
                    uTextureIndex = 19;
                    break;
                case (EMERALD_FONT_4):
                    uTextureIndex = 20;
                    break;
                case (EMERALD_FONT_5):
                    uTextureIndex = 21;
                    break;
                case (EMERALD_FONT_6):
                    uTextureIndex = 22;
                    break;
                case (EMERALD_FONT_7):
                    uTextureIndex = 23;
                    break;
                case (EMERALD_FONT_8):
                    uTextureIndex = 24;
                    break;
                case (EMERALD_FONT_9):
                    uTextureIndex = 25;
                    break;
                case (EMERALD_FONT_DOUBLE_POINT):
                    uTextureIndex = 26;
                    break;
                case (EMERALD_FONT_PLATE):
                    uTextureIndex = 27;
                    break;
                case (EMERALD_FONT_ARROW_LEFT):
                    uTextureIndex = 28;
                    break;
                case (EMERALD_FONT_PAFF):
                    uTextureIndex = 29;
                    break;
                case (EMERALD_FONT_QUESTION_MARK):
                    uTextureIndex = 31;
                    break;
                case (EMERALD_FONT_A):
                    uTextureIndex = 33;
                    break;
                case (EMERALD_FONT_B):
                    uTextureIndex = 34;
                    break;
                case (EMERALD_FONT_C):
                    uTextureIndex = 35;
                    break;
                case (EMERALD_FONT_D):
                    uTextureIndex = 36;
                    break;
                case (EMERALD_FONT_E):
                    uTextureIndex = 37;
                    break;
                case (EMERALD_FONT_F):
                    uTextureIndex = 38;
                    break;
                case (EMERALD_FONT_G):
                    uTextureIndex = 39;
                    break;
                case (EMERALD_FONT_H):
                    uTextureIndex = 40;
                    break;
                case (EMERALD_FONT_I):
                    uTextureIndex = 41;
                    break;
                case (EMERALD_FONT_J):
                    uTextureIndex = 42;
                    break;
                case (EMERALD_FONT_K):
                    uTextureIndex = 43;
                    break;
                case (EMERALD_FONT_L):
                    uTextureIndex = 44;
                    break;
                case (EMERALD_FONT_M):
                    uTextureIndex = 45;
                    break;
                case (EMERALD_FONT_N):
                    uTextureIndex = 46;
                    break;
                case (EMERALD_FONT_O):
                    uTextureIndex = 47;
                    break;
                case (EMERALD_FONT_P):
                    uTextureIndex = 48;
                    break;
                case (EMERALD_FONT_Q):
                    uTextureIndex = 49;
                    break;
                case (EMERALD_FONT_R):
                    uTextureIndex = 50;
                    break;
                case (EMERALD_FONT_S):
                    uTextureIndex = 51;
                    break;
                case (EMERALD_FONT_T):
                    uTextureIndex = 52;
                    break;
                case (EMERALD_FONT_U):
                    uTextureIndex = 53;
                    break;
                case (EMERALD_FONT_V):
                    uTextureIndex = 54;
                    break;
                case (EMERALD_FONT_W):
                    uTextureIndex = 55;
                    break;
                case (EMERALD_FONT_X):
                    uTextureIndex = 56;
                    break;
                case (EMERALD_FONT_Y):
                    uTextureIndex = 57;
                    break;
                case (EMERALD_FONT_Z):
                    uTextureIndex = 58;
                    break;
                case (EMERALD_FONT_AE):
                    uTextureIndex = 65;
                    break;
                case (EMERALD_FONT_OE):
                    uTextureIndex = 66;
                    break;
                case (EMERALD_FONT_UE):
                    uTextureIndex = 67;
                    break;
                case (EMERALD_FONT_STEEL_EXCLAMATION):
                    uTextureIndex = 674;
                    break;
                case (EMERALD_FONT_STEEL_ARROW_RIGHT):
                    uTextureIndex = 700;
                    break;
                case (EMERALD_FONT_STEEL_ARROW_UP):
                    uTextureIndex = 698;
                    break;
                case (EMERALD_FONT_STEEL_ARROW_DOWN):
                    uTextureIndex = 701;
                    break;
                case (EMERALD_FONT_STEEL_APOSTROPHE):
                    uTextureIndex = 673;
                    break;
                case (EMERALD_FONT_STEEL_BRACE_OPEN):
                    uTextureIndex = 688;
                    break;
                case (EMERALD_FONT_STEEL_BRACE_CLOSE):
                    uTextureIndex = 689;
                    break;
                case (EMERALD_FONT_STEEL_COPYRIGHT):
                    uTextureIndex = 677;
                    break;
                case (EMERALD_FONT_STEEL_PLUS):
                    uTextureIndex = 702;
                    break;
                case (EMERALD_FONT_STEEL_COMMA):
                    uTextureIndex = 690;
                    break;
                case (EMERALD_FONT_STEEL_MINUS):
                    uTextureIndex = 693;
                    break;
                case (EMERALD_FONT_STEEL_POINT):
                    uTextureIndex = 703;
                    break;
                case (EMERALD_FONT_STEEL_SLASH):
                    uTextureIndex = 707;
                    break;
                case (EMERALD_FONT_STEEL_0):
                    uTextureIndex = 662;
                    break;
                case (EMERALD_FONT_STEEL_1):
                    uTextureIndex = 663;
                    break;
                case (EMERALD_FONT_STEEL_2):
                    uTextureIndex = 664;
                    break;
                case (EMERALD_FONT_STEEL_3):
                    uTextureIndex = 665;
                    break;
                case (EMERALD_FONT_STEEL_4):
                    uTextureIndex = 666;
                    break;
                case (EMERALD_FONT_STEEL_5):
                    uTextureIndex = 667;
                    break;
                case (EMERALD_FONT_STEEL_6):
                    uTextureIndex = 668;
                    break;
                case (EMERALD_FONT_STEEL_7):
                    uTextureIndex = 669;
                    break;
                case (EMERALD_FONT_STEEL_8):
                    uTextureIndex = 670;
                    break;
                case (EMERALD_FONT_STEEL_9):
                    uTextureIndex = 671;
                    break;
                case (EMERALD_FONT_STEEL_DOUBLE_POINT):
                    uTextureIndex = 679;
                    break;
                case (EMERALD_FONT_STEEL_PLATE):
                    uTextureIndex = 709;
                    break;
                case (EMERALD_FONT_STEEL_ARROW_LEFT):
                    uTextureIndex = 699;
                    break;
                case (EMERALD_FONT_STEEL_QUESTION_MARK):
                    uTextureIndex = 682;
                    break;
                case (EMERALD_FONT_STEEL_A):
                    uTextureIndex = 675;
                    break;
                case (EMERALD_FONT_STEEL_B):
                    uTextureIndex = 676;
                    break;
                case (EMERALD_FONT_STEEL_C):
                    uTextureIndex = 678;
                    break;
                case (EMERALD_FONT_STEEL_D):
                    uTextureIndex = 680;
                    break;
                case (EMERALD_FONT_STEEL_E):
                    uTextureIndex = 681;
                    break;
                case (EMERALD_FONT_STEEL_F):
                    uTextureIndex = 683;
                    break;
                case (EMERALD_FONT_STEEL_G):
                    uTextureIndex = 684;
                    break;
                case (EMERALD_FONT_STEEL_H):
                    uTextureIndex = 685;
                    break;
                case (EMERALD_FONT_STEEL_I):
                    uTextureIndex = 686;
                    break;
                case (EMERALD_FONT_STEEL_J):
                    uTextureIndex = 687;
                    break;
                case (EMERALD_FONT_STEEL_K):
                    uTextureIndex = 691;
                    break;
                case (EMERALD_FONT_STEEL_L):
                    uTextureIndex = 692;
                    break;
                case (EMERALD_FONT_STEEL_M):
                    uTextureIndex = 694;
                    break;
                case (EMERALD_FONT_STEEL_N):
                    uTextureIndex = 695;
                    break;
                case (EMERALD_FONT_STEEL_O):
                    uTextureIndex = 697;
                    break;
                case (EMERALD_FONT_STEEL_P):
                    uTextureIndex = 704;
                    break;
                case (EMERALD_FONT_STEEL_Q):
                    uTextureIndex = 705;
                    break;
                case (EMERALD_FONT_STEEL_R):
                    uTextureIndex = 706;
                    break;
                case (EMERALD_FONT_STEEL_S):
                    uTextureIndex = 708;
                    break;
                case (EMERALD_FONT_STEEL_T):
                    uTextureIndex = 710;
                    break;
                case (EMERALD_FONT_STEEL_U):
                    uTextureIndex = 712;
                    break;
                case (EMERALD_FONT_STEEL_V):
                    uTextureIndex = 713;
                    break;
                case (EMERALD_FONT_STEEL_W):
                    uTextureIndex = 714;
                    break;
                case (EMERALD_FONT_STEEL_X):
                    uTextureIndex = 715;
                    break;
                case (EMERALD_FONT_STEEL_Y):
                    uTextureIndex = 716;
                    break;
                case (EMERALD_FONT_STEEL_Z):
                    uTextureIndex = 717;
                    break;
                case (EMERALD_FONT_STEEL_AE):
                    uTextureIndex = 672;
                    break;
                case (EMERALD_FONT_STEEL_OE):
                    uTextureIndex = 696;
                    break;
                case (EMERALD_FONT_STEEL_UE):
                    uTextureIndex = 711;
                    break;
                case (EMERALD_FONT_GREEN_EXCLAMATION):
                    uTextureIndex = 562;
                    break;
                case (EMERALD_FONT_GREEN_ARROW_RIGHT):
                    uTextureIndex = 588;
                    break;
                case (EMERALD_FONT_GREEN_ARROW_UP):
                    uTextureIndex = 586;
                    break;
                case (EMERALD_FONT_GREEN_ARROW_DOWN):
                    uTextureIndex = 589;
                    break;
                case (EMERALD_FONT_GREEN_APOSTROPHE):
                    uTextureIndex = 561;
                    break;
                case (EMERALD_FONT_GREEN_BRACE_OPEN):
                    uTextureIndex = 576;
                    break;
                case (EMERALD_FONT_GREEN_BRACE_CLOSE):
                    uTextureIndex = 577;
                    break;
                case (EMERALD_FONT_GREEN_COPYRIGHT):
                    uTextureIndex = 565;
                    break;
                case (EMERALD_FONT_GREEN_PLUS):
                    uTextureIndex = 590;
                    break;
                case (EMERALD_FONT_GREEN_COMMA):
                    uTextureIndex = 578;
                    break;
                case (EMERALD_FONT_GREEN_MINUS):
                    uTextureIndex = 581;
                    break;
                case (EMERALD_FONT_GREEN_POINT):
                    uTextureIndex = 591;
                    break;
                case (EMERALD_FONT_GREEN_SLASH):
                    uTextureIndex = 595;
                    break;
                case (EMERALD_FONT_GREEN_0):
                    uTextureIndex = 84;
                    break;
                case (EMERALD_FONT_GREEN_1):
                    uTextureIndex = 85;
                    break;
                case (EMERALD_FONT_GREEN_2):
                    uTextureIndex = 86;
                    break;
                case (EMERALD_FONT_GREEN_3):
                    uTextureIndex = 87;
                    break;
                case (EMERALD_FONT_GREEN_4):
                    uTextureIndex = 88;
                    break;
                case (EMERALD_FONT_GREEN_5):
                    uTextureIndex = 89;
                    break;
                case (EMERALD_FONT_GREEN_6):
                    uTextureIndex = 90;
                    break;
                case (EMERALD_FONT_GREEN_7):
                    uTextureIndex = 91;
                    break;
                case (EMERALD_FONT_GREEN_8):
                    uTextureIndex = 92;
                    break;
                case (EMERALD_FONT_GREEN_9):
                    uTextureIndex = 93;
                    break;
                case (EMERALD_FONT_GREEN_DOUBLE_POINT):
                    uTextureIndex = 567;
                    break;
                case (EMERALD_FONT_GREEN_PLATE):
                    uTextureIndex = 597;
                    break;
                case (EMERALD_FONT_GREEN_ARROW_LEFT):
                    uTextureIndex = 587;
                    break;
                case (EMERALD_FONT_GREEN_QUESTION_MARK):
                    uTextureIndex = 570;
                    break;
                case (EMERALD_FONT_GREEN_A):
                    uTextureIndex = 563;
                    break;
                case (EMERALD_FONT_GREEN_B):
                    uTextureIndex = 564;
                    break;
                case (EMERALD_FONT_GREEN_C):
                    uTextureIndex = 566;
                    break;
                case (EMERALD_FONT_GREEN_D):
                    uTextureIndex = 568;
                    break;
                case (EMERALD_FONT_GREEN_E):
                    uTextureIndex = 569;
                    break;
                case (EMERALD_FONT_GREEN_F):
                    uTextureIndex = 571;
                    break;
                case (EMERALD_FONT_GREEN_G):
                    uTextureIndex = 572;
                    break;
                case (EMERALD_FONT_GREEN_H):
                    uTextureIndex = 573;
                    break;
                case (EMERALD_FONT_GREEN_I):
                    uTextureIndex = 574;
                    break;
                case (EMERALD_FONT_GREEN_J):
                    uTextureIndex = 575;
                    break;
                case (EMERALD_FONT_GREEN_K):
                    uTextureIndex = 579;
                    break;
                case (EMERALD_FONT_GREEN_L):
                    uTextureIndex = 580;
                    break;
                case (EMERALD_FONT_GREEN_M):
                    uTextureIndex = 582;
                    break;
                case (EMERALD_FONT_GREEN_N):
                    uTextureIndex = 583;
                    break;
                case (EMERALD_FONT_GREEN_O):
                    uTextureIndex = 585;
                    break;
                case (EMERALD_FONT_GREEN_P):
                    uTextureIndex = 592;
                    break;
                case (EMERALD_FONT_GREEN_Q):
                    uTextureIndex = 593;
                    break;
                case (EMERALD_FONT_GREEN_R):
                    uTextureIndex = 594;
                    break;
                case (EMERALD_FONT_GREEN_S):
                    uTextureIndex = 596;
                    break;
                case (EMERALD_FONT_GREEN_T):
                    uTextureIndex = 598;
                    break;
                case (EMERALD_FONT_GREEN_U):
                    uTextureIndex = 600;
                    break;
                case (EMERALD_FONT_GREEN_V):
                    uTextureIndex = 601;
                    break;
                case (EMERALD_FONT_GREEN_W):
                    uTextureIndex = 602;
                    break;
                case (EMERALD_FONT_GREEN_X):
                    uTextureIndex = 603;
                    break;
                case (EMERALD_FONT_GREEN_Y):
                    uTextureIndex = 604;
                    break;
                case (EMERALD_FONT_GREEN_Z):
                    uTextureIndex = 605;
                    break;
                case (EMERALD_FONT_GREEN_AE):
                    uTextureIndex = 560;
                    break;
                case (EMERALD_FONT_GREEN_OE):
                    uTextureIndex = 584;
                    break;
                case (EMERALD_FONT_GREEN_UE):
                    uTextureIndex = 599;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_EXCLAMATION):
                    uTextureIndex = 618;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_ARROW_RIGHT):
                    uTextureIndex = 644;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_ARROW_UP):
                    uTextureIndex = 642;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_ARROW_DOWN):
                    uTextureIndex = 645;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_APOSTROPHE):
                    uTextureIndex = 617;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_BRACE_OPEN):
                    uTextureIndex = 632;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_BRACE_CLOSE):
                    uTextureIndex = 633;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_COPYRIGHT):
                    uTextureIndex = 621;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_PLUS):
                    uTextureIndex = 646;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_COMMA):
                    uTextureIndex = 634;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_MINUS):
                    uTextureIndex = 637;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_POINT):
                    uTextureIndex = 647;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_SLASH):
                    uTextureIndex = 651;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_0):
                    uTextureIndex = 606;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_1):
                    uTextureIndex = 607;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_2):
                    uTextureIndex = 608;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_3):
                    uTextureIndex = 609;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_4):
                    uTextureIndex = 610;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_5):
                    uTextureIndex = 611;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_6):
                    uTextureIndex = 612;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_7):
                    uTextureIndex = 613;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_8):
                    uTextureIndex = 614;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_9):
                    uTextureIndex = 615;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_DOUBLE_POINT):
                    uTextureIndex = 623;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_PLATE):
                    uTextureIndex = 653;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_ARROW_LEFT):
                    uTextureIndex = 643;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_QUESTION_MARK):
                    uTextureIndex = 626;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_A):
                    uTextureIndex = 619;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_B):
                    uTextureIndex = 620;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_C):
                    uTextureIndex = 622;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_D):
                    uTextureIndex = 624;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_E):
                    uTextureIndex = 625;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_F):
                    uTextureIndex = 627;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_G):
                    uTextureIndex = 628;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_H):
                    uTextureIndex = 629;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_I):
                    uTextureIndex = 630;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_J):
                    uTextureIndex = 631;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_K):
                    uTextureIndex = 635;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_L):
                    uTextureIndex = 636;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_M):
                    uTextureIndex = 638;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_N):
                    uTextureIndex = 639;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_O):
                    uTextureIndex = 641;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_P):
                    uTextureIndex = 648;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_Q):
                    uTextureIndex = 649;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_R):
                    uTextureIndex = 650;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_S):
                    uTextureIndex = 652;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_T):
                    uTextureIndex = 654;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_U):
                    uTextureIndex = 656;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_V):
                    uTextureIndex = 657;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_W):
                    uTextureIndex = 658;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_X):
                    uTextureIndex = 659;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_Y):
                    uTextureIndex = 660;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_Z):
                    uTextureIndex = 661;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_AE):
                    uTextureIndex = 616;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_OE):
                    uTextureIndex = 640;
                    break;
                case (EMERALD_FONT_STEEL_GREEN_UE):
                    uTextureIndex = 655;
                    break;
                case (EMERALD_INVALID):     // invalides Element gefunden
                    // Es muss sich nur um Elemente gekümmert werden, die sich am Rand des sichtbaren Bereichs befinden
                    // und sich in den sichtbaren Bereich >hinein< bewegen
                    if (Playfield.pInvalidElement[I] != EMERALD_NONE) {   // Bei grünen Tropfen in Phase 1
                        uTextureIndex = GetTextureIndexByElement(Playfield.pInvalidElement[I],nAnimationCount,&fAngle);
                        if ((uY == 0) && ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_CLEAN_UP)) {
                            // 1. Element befindet sich am oberen Rand und will nach unten in den sichtbaren Bereich
                            //SDL_Log("%s: invalid element at top found    Anim:%x   T:%u",__FUNCTION__,Playfield.pStatusAnimation[I],SDL_GetTicks());
                            nYoffs = -FONT_H + nAnimationCount * 2;
                        } else if ((uX == 0) && ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_CLEAN_LEFT)) {
                            // 2. Element befindet sich am linken Rand und will nach rechts in den sichtbaren Bereich
                            //SDL_Log("%s: invalid element at left found    Anim:%x   T:%u",__FUNCTION__,Playfield.pStatusAnimation[I],SDL_GetTicks());
                            nXoffs = -FONT_H + nAnimationCount * 2;
                        } else if ((uX == (uResX / FONT_W)) && ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_CLEAN_RIGHT)) {
                            // 3. Element befindet sich am rechten Rand und will nach links in den sichtbaren Bereich
                            //SDL_Log("%s: invalid element at right found    Anim:%x   T:%u",__FUNCTION__,Playfield.pStatusAnimation[I],SDL_GetTicks());
                            nXoffs = FONT_H - nAnimationCount * 2;
                        } else if ( (uY == ((uResY - PANEL_H) / FONT_H)) && ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_CLEAN_DOWN)) {
                            // 4. Element befindet sich am unteren Rand und will nach oben in den sichtbaren Bereich
                            //SDL_Log("%s: invalid element at bottom found    Anim:%x   T:%u",__FUNCTION__,Playfield.pStatusAnimation[I],SDL_GetTicks());
                            nYoffs = FONT_H - nAnimationCount * 2;
                        } else {
                            uTextureIndex = 0;      // Space
                            fAngle = 0;
                        }
                    } else {
                        uTextureIndex = 0;      // Space
                        fAngle = 0;
                    }
                    break;
                default:
                    fAngle = 0;
                    uTextureIndex = 0;      // Space
                    SDL_Log("%s: default: unknown/invalid element[%d] at adress %u found",__FUNCTION__,uLevelElement,I);
                    break;
            }
            // Erweitertes Element
            if (bExtendedElement) {
                // Position innerhalb des Renderers
                DestR.x = uX * FONT_W - (*pnXpos % FONT_W) + nXoffs_0 + Playfield.uShiftLevelXpix;
                DestR.y = uY * FONT_H - (*pnYpos % FONT_H) + nYoffs_0 + Playfield.uShiftLevelYpix;
                DestR.w = FONT_W * fScaleW_0;
                DestR.h = FONT_H * fScaleH_0;
                if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex_0),NULL,&DestR,fAngle_0 + fAngleOffs_0,NULL, Flip_0) != 0) {
                    SDL_Log("%s: SDL_RenderCopyEx(extended) failed: %s",__FUNCTION__,SDL_GetError());
                    nErrorCode = -1;
                }
            }
            // Standardelement
            if ((uLevelElement != EMERALD_SPACE) && (uLevelElement != EMERALD_BEAM_CROSS) && (nErrorCode == 0)) {   // Space muss nicht gerendert werden, da nach jedem Bild sowieso Alles gelöscht wird
                // Position innerhalb des Renderers
                DestR.x = uX * FONT_W - (*pnXpos % FONT_W) + nXoffs + Playfield.uShiftLevelXpix;
                DestR.y = uY * FONT_H - (*pnYpos % FONT_H) + nYoffs + Playfield.uShiftLevelYpix;
                DestR.w = FONT_W * fScaleW;
                DestR.h = FONT_H * fScaleH;
                if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle + fAngleOffs,NULL, Flip) != 0) {
                    SDL_Log("%s: SDL_RenderCopyEx(standard) failed: %s",__FUNCTION__,SDL_GetError());
                    nErrorCode = -1;
                }
            }
        } // for (uX = 0; (uX <= (WINDOW_W / FONT_W)) && (uX < Playfield.uLevel_X_Dimension) && (nErrorCode == 0); uX++)
    } // for (uY = 0; (uY <= ((WINDOW_H - PANEL_H) / FONT_H)) && (uY < Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); uY++)
    // Ab hier Post-Animationen. Diese sind keinem bestimmten Element zugeordnet.
    for (Y = 0; (Y < uPostAnimationIndex) & (nErrorCode == 0); Y++) {
        uTextureIndex = Playfield.pPostAnimation[Y].uTextureIndex;
        uX = Playfield.pPostAnimation[Y].uX;
        uY = Playfield.pPostAnimation[Y].uY;
        // Es gibt z.Z. noch keine unterschiedlichen Animationsformeln
        if (nAnimationCount < 15) {
            uTextureIndex = uTextureIndex + nAnimationCount;    // Animationsformel 0
        } else {
            uTextureIndex = 0;
        }
        nXoffs = 0;
        nYoffs = 0;
        fAngle = 0;
        fAngleOffs = 0;
        fScaleW = 1;
        fScaleH = 1;
        Flip = SDL_FLIP_NONE;
        if (Playfield.pPostAnimation[Y].bFlipHorizontal) {
            Flip = (SDL_RendererFlip)(Flip | SDL_FLIP_HORIZONTAL);
        }
        if (Playfield.pPostAnimation[Y].bFlipVertical) {
            Flip = (SDL_RendererFlip)(Flip | SDL_FLIP_VERTICAL);
        }
        // Position innerhalb des Renderers
        DestR.x = uX * FONT_W - (*pnXpos % FONT_W) + nXoffs + Playfield.uShiftLevelXpix;
        DestR.y = uY * FONT_H - (*pnYpos % FONT_H) + nYoffs + Playfield.uShiftLevelYpix;
        DestR.w = FONT_W * fScaleW;
        DestR.h = FONT_H * fScaleH;
        if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle + fAngleOffs,NULL, Flip) != 0) {
            SDL_Log("%s: SDL_RenderCopyEx(post animation) failed: %s, TextureIndex: %u",__FUNCTION__,SDL_GetError(),uTextureIndex);
            nErrorCode = -1;
        }
    }
    if (nErrorCode == 0) {
        if (ShowPanel(pRenderer) != 0) {
            SDL_Log("%s: ShowPanel() failed",__FUNCTION__);
        }
    }
    return nErrorCode;
}
