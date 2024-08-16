#include "gfx/textures.h"
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
uint8_t ge_ExitDoorSequence[8] = {0,1,2,3,4,3,2,1};
uint32_t g_TreasureChestTicks = 0; // Zum Animieren der Schatztruhen
uint32_t g_TreasureChestOpenTicks = 0; // Zum Öffnen der Schatztruhen
bool g_bTreasureChestOpen = false;


/*----------------------------------------------------------------------------
Name:           SetTreasureChestStart
------------------------------------------------------------------------------
Beschreibung: Setzt die Variable g_TreasureChestTicks auf den aktuellen
              Tick-Counter.


Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: g_TreasureChestTicks
------------------------------------------------------------------------------*/
void SetTreasureChestStart(void) {
    g_TreasureChestTicks = SDL_GetTicks();
}


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
Name:           RenderPipeElement
------------------------------------------------------------------------------
Beschreibung: Rendert Pipe-Elemente.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               uPipeElement, uint16_t, Röhren-Element
               uX, uint32_t, X-Position im Level
               uY, uint32_t, Y-Position im Level
               nXpos, int , Pixel-Positionierung X (obere linke Ecke des Levelausschnitts)
               nYpos, int , Pixel-Positionierung Y (obere linke Ecke des Levelausschnitts)

Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x, ge_ExitDoorSequence[], g_uCheeseRandom[]
------------------------------------------------------------------------------*/
int RenderPipeElement(SDL_Renderer *pRenderer, uint16_t uPipeElement, uint32_t uX, uint32_t uY, int nXpos, int nYpos) {
    int nErrorCode;
    uint32_t uTextureIndex;
    SDL_Rect DestR;                     // Zum Kopieren in den Renderer

    switch (uPipeElement) {
        case (EMERALD_PIPE_UP_DOWN):
            uTextureIndex = TEX_PIPE_UP_DOWN;
            break;
        case (EMERALD_PIPE_LEFT_RIGHT):
            uTextureIndex = TEX_PIPE_LEFT_RIGHT;
            break;
        case (EMERALD_PIPE_LEFT_UP):
            uTextureIndex = TEX_PIPE_LEFT_UP;
            break;
        case (EMERALD_PIPE_LEFT_DOWN):
            uTextureIndex = TEX_PIPE_LEFT_DOWN;
            break;
        case (EMERALD_PIPE_RIGHT_UP):
            uTextureIndex = TEX_PIPE_RIGHT_UP;
            break;
        case (EMERALD_PIPE_RIGHT_DOWN):
            uTextureIndex = TEX_PIPE_RIGHT_DOWN;
            break;
        case (EMERALD_PIPE_LEFT_UP_DOWN):
            uTextureIndex = TEX_PIPE_LEFT_UP_DOWN;
            break;
        case (EMERALD_PIPE_RIGHT_UP_DOWN):
            uTextureIndex = TEX_PIPE_RIGHT_UP_DOWN;
            break;
        case (EMERALD_PIPE_LEFT_RIGHT_UP):
            uTextureIndex = TEX_PIPE_LEFT_RIGHT_UP;
            break;
        case (EMERALD_PIPE_LEFT_RIGHT_DOWN):
            uTextureIndex = TEX_PIPE_LEFT_RIGHT_DOWN;
            break;
        case (EMERALD_PIPE_LEFT_RIGHT_UP_DOWN):
            uTextureIndex = TEX_PIPE_LEFT_RIGHT_UP_DOWN;
            break;
        default:
            uTextureIndex = TEX_SPACE;
            SDL_Log("%s: unknown pipe element: 0X%X",__FUNCTION__,uPipeElement);
            break;
    }
    // Position innerhalb des Renderers
    DestR.x = uX * FONT_W - (nXpos % FONT_W) + Playfield.uShiftLevelXpix;
    DestR.y = uY * FONT_H - (nYpos % FONT_H) + Playfield.uShiftLevelYpix;
    DestR.w = FONT_W;
    DestR.h = FONT_H;
    nErrorCode = SDL_RenderCopy(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR);
    if (nErrorCode != 0) {
        SDL_Log("%s: SDL_RenderCopyEx(standard) failed: %s",__FUNCTION__,SDL_GetError());
    }
    return nErrorCode;
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
Seiteneffekte: Playfield.x, Config.x, g_TreasureChestTicks, g_TreasureChestOpenTicks, g_bTreasureChestOpen

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
    uint32_t uTicks;

    uTicks = SDL_GetTicks();

    if (uTicks - g_TreasureChestTicks > TREASURECHEST_TIMER) {
        g_TreasureChestTicks = uTicks;
        g_TreasureChestOpenTicks = uTicks;
        g_bTreasureChestOpen = true;
    }
    if ((g_bTreasureChestOpen) && (uTicks - g_TreasureChestOpenTicks > TREASURECHEST_OPEN_TIMER)) {
        g_bTreasureChestOpen = false;
    }

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
    // Röhren-Elemente ggf. vorher rendern
    for (uY = 0; (uY <= ((uResY - PANEL_H) / FONT_H)) && (uY < Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); uY++) {
        for (uX = 0; (uX <= (uResX / FONT_W)) && (uX < Playfield.uLevel_X_Dimension) && (nErrorCode == 0); uX++) {
            I = uUpperLeftLevelIndex + uY * Playfield.uLevel_X_Dimension + uX;
            if (I < (Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension)) {
                if (Playfield.pPipeLevel[I] != EMERALD_SPACE) {
                    nErrorCode = RenderPipeElement(pRenderer,Playfield.pPipeLevel[I],uX,uY,*pnXpos,*pnYpos);
                }
            }
        }
    }
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
            uTextureIndex_0 = TEX_SPACE; // Space
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
                case (EMERALD_REMOTEBOMB):
                    uTextureIndex = TEX_BOMB_REMOTE;
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
                        uTextureIndex = TEX_SWITCH_BOMB_REMOTE_UP_ON + (((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) & 1);
                    } else {
                        uTextureIndex = TEX_SWITCH_BOMB_REMOTE_UP_OFF;
                    }
                    break;
                case (EMERALD_SWITCH_REMOTEBOMB_DOWN):
                    if (Playfield.bSwitchRemoteBombDown) {
                        uTextureIndex = TEX_SWITCH_BOMB_REMOTE_DOWN_ON + (((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) & 1);
                    } else {
                        uTextureIndex = TEX_SWITCH_BOMB_REMOTE_DOWN_OFF;
                    }
                    break;
                case (EMERALD_SWITCH_REMOTEBOMB_LEFT):
                    if (Playfield.bSwitchRemoteBombLeft) {
                        uTextureIndex = TEX_SWITCH_BOMB_REMOTE_LEFT_ON + (((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) & 1);
                    } else {
                        uTextureIndex = TEX_SWITCH_BOMB_REMOTE_LEFT_OFF;
                    }
                    break;
                case (EMERALD_SWITCH_REMOTEBOMB_RIGHT):
                    if (Playfield.bSwitchRemoteBombRight) {
                        uTextureIndex = TEX_SWITCH_BOMB_REMOTE_RIGHT_ON + (((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) & 1);
                    } else {
                        uTextureIndex = TEX_SWITCH_BOMB_REMOTE_RIGHT_OFF;
                    }
                    break;
                case (EMERALD_SWITCH_REMOTEBOMB_IGNITION):
                    if (Playfield.bSwitchRemoteBombIgnition) {
                        uTextureIndex = TEX_SWITCH_BOMB_REMOTE_IGNITION_ON + (((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) & 1);
                    } else {
                        uTextureIndex = TEX_SWITCH_BOMB_REMOTE_IGNITION_OFF;
                    }
                    break;
                case (EMERALD_TELEPORTER_RED):
                    uTextureIndex_0 = TEX_STEEL;    // Stahl
                    bExtendedElement = true;
                    uTextureIndex = TEX_TELEPORTER_RED_01 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, rot
                    break;
                case (EMERALD_TELEPORTER_YELLOW):
                    uTextureIndex_0 = TEX_STEEL;    // Stahl
                    bExtendedElement = true;
                    uTextureIndex = TEX_TELEPORTER_YELLOW_01 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, gelb
                    break;
                case (EMERALD_TELEPORTER_GREEN):
                    uTextureIndex_0 = TEX_STEEL;    // Stahl
                    bExtendedElement = true;
                    uTextureIndex = TEX_TELEPORTER_GREEN_01 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, grün
                    break;
                case (EMERALD_TELEPORTER_BLUE):
                    uTextureIndex_0 = TEX_STEEL;    // Stahl
                    bExtendedElement = true;
                    uTextureIndex = TEX_TELEPORTER_BLUE_01 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 13; // Teleporter, blau
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
                    uTextureIndex = TEX_STEEL;     // Stahl
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
                    uTextureIndex = TEX_WALL;     // Mauer eckig
                    break;
                case (EMERALD_STEEL_GROWING_LEFT):
                    uTextureIndex = TEX_STEEL_GROW_LEFT_01 + nAnimationCount;
                    break;
                case (EMERALD_STEEL_GROWING_RIGHT):
                    uTextureIndex = TEX_STEEL_GROW_RIGHT_01 + nAnimationCount;
                    break;
                case (EMERALD_STEEL_GROWING_UP):
                    uTextureIndex = TEX_STEEL_GROW_UP_01 + nAnimationCount;
                    break;
                case (EMERALD_STEEL_GROWING_DOWN):
                    uTextureIndex = TEX_STEEL_GROW_DOWN_01 + nAnimationCount;
                    break;
                case (EMERALD_WALL_GROWING_LEFT):
                    uTextureIndex = TEX_WALL_GROW_LEFT_01 + nAnimationCount;
                    break;
                case (EMERALD_WALL_GROWING_RIGHT):
                    uTextureIndex = TEX_WALL_GROW_RIGHT_01 + nAnimationCount;
                    break;
                case (EMERALD_WALL_GROWING_UP):
                    uTextureIndex = TEX_WALL_GROW_UP_01 + nAnimationCount;
                    break;
                case (EMERALD_WALL_GROWING_DOWN):
                    uTextureIndex = TEX_WALL_GROW_DOWN_01 + nAnimationCount;
                    break;
                case (EMERALD_YAM_KILLS_MAN):
                    Y = Playfield.uFrameCounter % 11;       // Y von 0 bis 10
                    if (Y <= 5) {                           // 0,1,2,3,4,5
                        uTextureIndex = TEX_YAM_MOVE_1 + Y;            // 362 - 367
                    } else {                                // 6,7,8,9,10
                        uTextureIndex = TEX_YAM_MOVE_6 + 5 - Y;        // 366 - 362
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
                        uTextureIndex = TEX_ALIEN_MOVE_1;                        // Alien geht 1
                    } else {
                        uTextureIndex = TEX_ALIEN_MOVE_2;                        // Alien geht 2, Flügel voll ausgebreitet
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
                        uTextureIndex = TEX_SWITCH_CONVEYOR_RED_LEFT;    // links
                    } else if (Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_RIGHT) {
                        uTextureIndex = TEX_SWITCH_CONVEYOR_RED_RIGHT;    // rechts
                    } else {
                        uTextureIndex = TEX_SWITCH_CONVEYOR_RED_OFF;    // aus
                    }
                    break;
                case (EMERALD_CONVEYORBELT_RED):
                    uTextureIndex = TEX_CONVEYOR_RED;
                    if (Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_LEFT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * -11.25;   // links drehen
                    } else if (Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_RIGHT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * 11.25;    // rechts drehen
                    }
                    break;
                case (EMERALD_CONVEYORBELT_SWITCH_GREEN):
                    if (Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_LEFT) {
                        uTextureIndex = TEX_SWITCH_CONVEYOR_GREEN_LEFT;    // links
                    } else if (Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_RIGHT) {
                        uTextureIndex = TEX_SWITCH_CONVEYOR_GREEN_RIGHT;    // rechts
                    } else {
                        uTextureIndex = TEX_SWITCH_CONVEYOR_GREEN_OFF;    // aus
                    }
                    break;
                case (EMERALD_CONVEYORBELT_GREEN):
                    uTextureIndex = TEX_CONVEYOR_GREEN;
                    if (Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_LEFT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * -11.25;   // links drehen
                    } else if (Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_RIGHT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * 11.25;    // rechts drehen
                    }
                    break;
                case (EMERALD_CONVEYORBELT_SWITCH_BLUE):
                    if (Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_LEFT) {
                        uTextureIndex = TEX_SWITCH_CONVEYOR_BLUE_LEFT;    // links
                    } else if (Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_RIGHT) {
                        uTextureIndex = TEX_SWITCH_CONVEYOR_BLUE_RIGHT;    // rechts
                    } else {
                        uTextureIndex = TEX_SWITCH_CONVEYOR_BLUE_OFF;    // aus
                    }
                    break;
                case (EMERALD_CONVEYORBELT_BLUE):
                    uTextureIndex = TEX_CONVEYOR_BLUE;
                    if (Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_LEFT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * -11.25;   // links drehen
                    } else if (Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_RIGHT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * 11.25;    // rechts drehen
                    }
                    break;
                case (EMERALD_CONVEYORBELT_SWITCH_YELLOW):
                    if (Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_LEFT) {
                        uTextureIndex = TEX_SWITCH_CONVEYOR_YELLOW_LEFT;    // links
                    } else if (Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_RIGHT) {
                        uTextureIndex = TEX_SWITCH_CONVEYOR_YELLOW_RIGHT;    // rechts
                    } else {
                        uTextureIndex = TEX_SWITCH_CONVEYOR_YELLOW_OFF;    // aus
                    }
                    break;
                case (EMERALD_CONVEYORBELT_YELLOW):
                    uTextureIndex = TEX_CONVEYOR_YELLOW;
                    if (Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_LEFT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * -11.25;   // links drehen
                    } else if (Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_RIGHT) {
                        fAngleOffs = (((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) & 0x1F) * 11.25;    // rechts drehen
                    }
                    break;
                case (EMERALD_MAN_DIES):
                    if (uSelfStatus == EMERALD_ANIM_MAN_DIES_P1) {
                        uTextureIndex = TEX_MAN_DIE_01 + nAnimationCount / 2;
                    } else {
                        uTextureIndex = TEX_MAN_DIE_01 + 8 + nAnimationCount / 2;
                    }
                    break;
                case (EMERALD_DYNAMITE_ON):
                    switch (uSelfStatus) {
                        case (EMERALD_ANIM_DYNAMITE_ON_P1):
                            uTextureIndex = TEX_DYNAMITE_ON_1;
                            break;
                        case (EMERALD_ANIM_DYNAMITE_ON_P2):
                            uTextureIndex = TEX_DYNAMITE_ON_2;
                            break;
                        case (EMERALD_ANIM_DYNAMITE_ON_P3):
                            uTextureIndex = TEX_DYNAMITE_ON_3;
                            break;
                        case (EMERALD_ANIM_DYNAMITE_ON_P4):
                            uTextureIndex = TEX_DYNAMITE_ON_4;
                            break;
                        default:
                            uTextureIndex = TEX_DYNAMITE_ON_1;
                            break;
                    }
                    break;
                case (EMERALD_SWITCHDOOR_OPEN):
                    if (uSelfStatus == EMERALD_ANIM_DOOR_CLOSE) {   // Soll Tür sich schließen?
                        if (nAnimationCount < 15) {
                            uTextureIndex = TEX_DOOR_SWITCHED_5 - nAnimationCount / 3;
                        } else {
                            uTextureIndex = TEX_DOOR_SWITCHED_1;    // Tür geschlossen
                        }
                    } else {
                        uTextureIndex = TEX_DOOR_SWITCHED_5;        // Schalttür offen
                    }
                    break;
                case (EMERALD_SWITCHDOOR_CLOSED):
                    if (uSelfStatus == EMERALD_ANIM_DOOR_OPEN) {   // Soll Tür sich öffnen?
                        if (nAnimationCount < 15) {
                            uTextureIndex = TEX_DOOR_SWITCHED_1 + nAnimationCount / 3;
                        } else {
                            uTextureIndex = TEX_DOOR_SWITCHED_5;    // Tür offen
                        }
                    } else {
                        uTextureIndex = TEX_DOOR_SWITCHED_1;        // Schalttür geschlossen
                    }
                    break;
               case (EMERALD_SWITCH_SWITCHDOOR):
                    if (Playfield.bSwitchDoorState) {
                        uTextureIndex = TEX_SWITCH_DOOR_SWITCHED_1;
                    } else {
                        uTextureIndex = TEX_SWITCH_DOOR_SWITCHED_2;
                    }
                    break;
                case (EMERALD_DOOR_TIME):
                    if (uSelfStatus == EMERALD_ANIM_DOOR_OPEN) {   // Tür öffnet sich
                        if (nAnimationCount < 15) {
                            uTextureIndex = TEX_DOOR_TIME_1 + nAnimationCount / 3;
                        } else {
                            uTextureIndex = TEX_DOOR_TIME_5;
                        }
                    } else if (uSelfStatus == EMERALD_ANIM_DOOR_CLOSE) {    // Tür schließt sich
                        if (nAnimationCount < 15) {
                            uTextureIndex = TEX_DOOR_TIME_5 - nAnimationCount / 3;
                        } else {
                            uTextureIndex = TEX_DOOR_TIME_1;    // Tür geschlossen
                        }
                    } else if (Playfield.bTimeDoorOpen) {
                        uTextureIndex = TEX_DOOR_TIME_5;    // Tür offen
                    } else {
                        uTextureIndex = TEX_DOOR_TIME_1;    // Tür geschlossen
                    }
                    break;
                case (EMERALD_WHEEL_TIMEDOOR):
                    if (Playfield.bTimeDoorOpen) {
                        uTextureIndex = TEX_WHEEL_DOOR_TIME_1 + nAnimationCount / 4;
                    } else {
                        uTextureIndex = TEX_WHEEL_DOOR_TIME_1;
                    }
                    break;
                case (EMERALD_STEEL_INVISIBLE):
                    if (Playfield.bLightOn) {
                        uTextureIndex = TEX_STEEL_INVISIBLE;
                    } else {
                        uTextureIndex = TEX_SPACE;
                    }
                    break;
                case (EMERALD_WALL_INVISIBLE):
                    if (Playfield.bLightOn) {
                        uTextureIndex = TEX_WALL_INVISIBLE;
                    } else {
                        uTextureIndex = TEX_SPACE;
                    }
                    break;
                case (EMERALD_EARTH_MOLE):
                    uTextureIndex = TEX_EARTH;        // Erde
                    nXoffs = 15 - nAnimationCount;
                    nYoffs = 15 - nAnimationCount;
                    fScaleW = nAnimationCount * 0.06;
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_GREEN_CHEESE_GOES):
                    uTextureIndex = TEX_CHEESE_GREEN_1 + I % 3;
                    nXoffs = nAnimationCount;
                    nYoffs = nAnimationCount;
                    fScaleW = 1 - nAnimationCount * 0.06;
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_YELLOW_CHEESE_GOES):
                    uTextureIndex = TEX_CHEESE_YELLOW_1 + I % 3;
                    nXoffs = nAnimationCount;
                    nYoffs = nAnimationCount;
                    fScaleW = 1 - nAnimationCount * 0.06;
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_MINE_CONTACT):
                    uTextureIndex = TEX_MINE_CONTACT_OFF;
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
                        uTextureIndex = TEX_MINE_CONTACT_ON_1;
                    } else if ((((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 16) == 0) {
                        uTextureIndex = TEX_MINE_CONTACT_ON_2;
                    } else {
                        uTextureIndex = TEX_MINE_CONTACT_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_RED_UP):
                    if (Playfield.bLightBarrierRedOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_RED_UP_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_RED_UP_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_RED_DOWN):
                    if (Playfield.bLightBarrierRedOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_RED_DOWN_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_RED_DOWN_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_RED_LEFT):
                    if (Playfield.bLightBarrierRedOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_RED_LEFT_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_RED_LEFT_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_RED_RIGHT):
                    if (Playfield.bLightBarrierRedOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_RED_RIGHT_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_RED_RIGHT_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_UP):
                    if (Playfield.bLightBarrierGreenOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_GREEN_UP_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_GREEN_UP_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_DOWN):
                    if (Playfield.bLightBarrierGreenOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_GREEN_DOWN_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_GREEN_DOWN_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_LEFT):
                    if (Playfield.bLightBarrierGreenOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_GREEN_LEFT_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_GREEN_LEFT_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_RIGHT):
                    if (Playfield.bLightBarrierGreenOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_GREEN_RIGHT_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_GREEN_RIGHT_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_UP):
                    if (Playfield.bLightBarrierBlueOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_BLUE_UP_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_BLUE_UP_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_DOWN):
                    if (Playfield.bLightBarrierBlueOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_BLUE_DOWN_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_BLUE_DOWN_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_LEFT):
                    if (Playfield.bLightBarrierBlueOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_BLUE_LEFT_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_BLUE_LEFT_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_RIGHT):
                    if (Playfield.bLightBarrierBlueOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_BLUE_RIGHT_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_BLUE_RIGHT_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_UP):
                    if (Playfield.bLightBarrierYellowOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_UP_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_UP_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_DOWN):
                    if (Playfield.bLightBarrierYellowOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_DOWN_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_DOWN_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_LEFT):
                    if (Playfield.bLightBarrierYellowOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_LEFT_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_LEFT_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_RIGHT):
                    if (Playfield.bLightBarrierYellowOn) {
                        uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_RIGHT_ON;
                    } else {
                        uTextureIndex = TEX_LIGHT_BARRIER_YELLOW_RIGHT_OFF;
                    }
                    break;
                case (EMERALD_BEAM_CROSS):
                    uTextureIndex = TEX_SPACE;
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
                            uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_RED_ON_1;
                        } else {
                            uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_RED_ON_2;
                        }
                    } else {
                        uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_RED_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_GREEN_SWITCH):
                    if (Playfield.bLightBarrierGreenOn) {
                        if (nAnimationCount <= 7) {
                            uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_GREEN_ON_1;
                        } else {
                            uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_GREEN_ON_2;
                        }
                    } else {
                        uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_GREEN_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_BLUE_SWITCH):
                    if (Playfield.bLightBarrierBlueOn) {
                        if (nAnimationCount <= 7) {
                            uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_BLUE_ON_1;
                        } else {
                            uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_BLUE_ON_2;
                        }
                    } else {
                        uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_BLUE_OFF;
                    }
                    break;
                case (EMERALD_LIGHTBARRIER_YELLOW_SWITCH):
                    if (Playfield.bLightBarrierYellowOn) {
                        if (nAnimationCount <= 7) {
                            uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_YELLOW_ON_1;
                        } else {
                            uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_YELLOW_ON_2;
                        }
                    } else {
                        uTextureIndex = TEX_SWITCH_LIGHT_BARRIER_YELLOW_OFF;
                    }
                    break;
                case (EMERALD_YAM):
                    uTextureIndex = TEX_YAM_STAND_1;
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
                            uTextureIndex = TEX_YAM_STAND_1 + (nAnimationCount / 2);
                        } else if (nAnimationCount <= 11) { // 10,11
                            uTextureIndex = TEX_YAM_STAND_4;
                        } else if (nAnimationCount <= 13) { // 12,13
                            uTextureIndex = TEX_YAM_STAND_3;
                        } else { // 14,15
                            uTextureIndex = TEX_YAM_STAND_2;
                        }
                    } else {
                        Y = Playfield.uFrameCounter % 11;       // Y von 0 bis 10
                        if (Y <= 5) {                           // 0,1,2,3,4,5
                            uTextureIndex = TEX_YAM_MOVE_1 + Y;            // 362 - 367
                        } else {                                // 6,7,8,9,10
                            uTextureIndex = TEX_YAM_MOVE_6 + 5 - Y;        // 366 - 362
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
                case (EMERALD_SLIME):
                    if (Playfield.pSlimeElement[I] == EMERALD_NONE) {
                        uTextureIndex = TEX_SLIME;   // Schleim ohne Inhalt
                    } else {
                        uTextureIndex = TEX_SLIME_FULL;   // Schleim mit Inhalt
                    }
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
                        // Phasen:
                        // 1: Schleim blockiert in Phase 1 (horizontal zusammenziehen)
                        // 2: Schleim blockiert in Phase 2 (horizontal ausdehnen)
                        // 3: Schleim blockiert in Phase 3 (vertikal zusammenziehen)
                        // 4: Schleim blockiert in Phase 4 (vertikal ausdehnen)
                        switch (uSelfStatus) {
                            case (EMERALD_ANIM_SLIME_STAND_PHASE1):
                                fScaleW = 1 - nAnimationCount * 0.03;   // 1 ..... 0.55
                                nXoffs = nAnimationCount / 2;           // max. 15/2 = 7 Pixel zur Mitte
                                break;
                            case (EMERALD_ANIM_SLIME_STAND_PHASE2):
                                fScaleW = 0.55 + (nAnimationCount) * 0.03;      // 0.55 ..... 1
                                nXoffs = 7 - nAnimationCount / 2;
                                break;
                            case (EMERALD_ANIM_SLIME_STAND_PHASE3):
                                fScaleH = 1 - nAnimationCount * 0.03;   // 1 ..... 0.55
                                nYoffs = nAnimationCount / 2;               // max. 15/2 = 7 Pixel zur Mitte
                                break;
                            case (EMERALD_ANIM_SLIME_STAND_PHASE4):
                                fScaleH = 0.55 + (nAnimationCount) * 0.03;      // 0.55 ..... 1
                                nYoffs = 7 - nAnimationCount / 2;
                                break;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_SLIME_GO_LEFT_1) {
                        // Eigenbewegung in vertikaler Richtung
                        fScaleH = 1 - nAnimationCount * 0.03;       // 1 ..... 0.55
                        nYoffs = nAnimationCount / 2;               // max. 15/2 = 7 Pixel zur Mitte
                        // horizontale Bewegung nach links, Phase 1
                        nXoffs = -nAnimationCount;
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        // Eigenbewegung in vertikaler Richtung
                        fScaleH = 0.55 + (nAnimationCount) * 0.03;      // 0.55 ..... 1
                        nYoffs = 7 - nAnimationCount / 2;
                        // horizontale Bewegung nach links, Phase 2
                        nXoffs = -16 - nAnimationCount;

                    } else if (uAnimStatus == EMERALD_ANIM_SLIME_GO_RIGHT_1) {
                        // Eigenbewegung in vertikaler Richtung
                        fScaleH = 1 - nAnimationCount * 0.03;       // 1 ..... 0.55
                        nYoffs = nAnimationCount / 2;               // max. 15/2 = 7 Pixel zur Mitte
                        // horizontale Bewegung nach rechts, Phase 1
                        nXoffs = nAnimationCount;
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        // Eigenbewegung in vertikaler Richtung
                        fScaleH = 0.55 + (nAnimationCount) * 0.03;      // 0.55 ..... 1
                        nYoffs = 7 - nAnimationCount / 2;
                        // horizontale Bewegung nach links, Phase 2
                        nXoffs = 16 + nAnimationCount;
                    } else if (uAnimStatus == EMERALD_ANIM_SLIME_GO_UP_1) {
                        // Eigenbewegung in vertikaler Richtung
                        fScaleW = 1 - nAnimationCount * 0.03;   // 1 ..... 0.55
                        nXoffs = nAnimationCount / 2;           // max. 15/2 = 7 Pixel zur Mitte
                        // horizontale Bewegung nach rechts, Phase 1
                        nYoffs = -nAnimationCount;
                    } else if (uAnimStatus == EMERALD_ANIM_UP) {
                        // Eigenbewegung in vertikaler Richtung
                        fScaleW = 0.55 + (nAnimationCount) * 0.03;      // 0.55 ..... 1
                        nXoffs = 7 - nAnimationCount / 2;
                        // horizontale Bewegung nach links, Phase 2
                        nYoffs = -16 - nAnimationCount;
                    } else if (uAnimStatus == EMERALD_ANIM_SLIME_GO_DOWN_1) {
                        // Eigenbewegung in vertikaler Richtung
                        fScaleW = 1 - nAnimationCount * 0.03;   // 1 ..... 0.55
                        nXoffs = nAnimationCount / 2;           // max. 15/2 = 7 Pixel zur Mitte
                        // horizontale Bewegung nach rechts, Phase 1
                        nYoffs = nAnimationCount;
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        // Eigenbewegung in vertikaler Richtung
                        fScaleW = 0.55 + (nAnimationCount) * 0.03;      // 0.55 ..... 1
                        nXoffs = 7 - nAnimationCount / 2;
                        // horizontale Bewegung nach links, Phase 2
                        nYoffs = 16 + nAnimationCount;
                    } else {
                        SDL_Log("%s: slime: unknown Animstatus: %x",__FUNCTION__,uAnimStatus);
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
                    uTextureIndex = TEX_MESSAGE;
                    if (uSelfStatus == EMERALD_ANIM_MESSAGE_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_TREASURECHEST_1):
                case (EMERALD_TREASURECHEST_2):
                case (EMERALD_TREASURECHEST_3):
                case (EMERALD_TREASURECHEST_4):
                case (EMERALD_TREASURECHEST_5):
                case (EMERALD_TREASURECHEST_6):
                case (EMERALD_TREASURECHEST_7):
                case (EMERALD_TREASURECHEST_8):
                    if ((g_bTreasureChestOpen) && (Playfield.pStatusAnimation[I] != EMERALD_ANIM_STAND)) {
                        uTextureIndex = TEX_TREASURECHEST_OPENED;
                    } else {
                        uTextureIndex = TEX_TREASURECHEST_CLOSED;
                    }
                    break;
                case (EMERALD_TREASURECHEST_OPEN):
                    uTextureIndex = TEX_TREASURECHEST_OPENED;
                    break;
                case (EMERALD_GREEN_DROP_COMES):
                    uTextureIndex = TEX_DROP_GREEN;
                    nXoffs = 16 - nAnimationCount;
                    nYoffs = nXoffs;
                    fScaleW = nAnimationCount * 0.06;
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_YELLOW_DROP_COMES):
                    uTextureIndex = TEX_DROP_YELLOW;
                    nXoffs = 16 - nAnimationCount;
                    nYoffs = nXoffs;
                    fScaleW = nAnimationCount * 0.06;
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_GREEN_DROP):
                    uTextureIndex = TEX_DROP_GREEN;
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
                        if (uSelfStatus == EMERALD_ANIM_DROP_1) {
                            nYoffs = nAnimationCount;
                        } else if (uSelfStatus == EMERALD_ANIM_DROP_2) {
                            nYoffs = (FONT_H / 2) + nAnimationCount;
                        } else {
                            SDL_Log("%s: green drop, something wrong!  uSelfStatus = %x",__FUNCTION__,uSelfStatus);
                        }
                    }
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_YELLOW_DROP):
                    uTextureIndex = TEX_DROP_YELLOW;
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
                        if (uSelfStatus == EMERALD_ANIM_DROP_1) {
                            nYoffs = nAnimationCount;
                        } else if (uSelfStatus == EMERALD_ANIM_DROP_2) {
                            nYoffs = (FONT_H / 2) + nAnimationCount;
                        } else {
                            SDL_Log("%s: yellow drop, something wrong!  uSelfStatus = %x",__FUNCTION__,uSelfStatus);
                        }
                    }
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_GREEN_CHEESE):
                    uCheeseRandom = g_uCheeseRandom[I & (MAX_CHEESE_RANDOM_NUMBERS - 1)];
                    uTextureIndex = TEX_CHEESE_GREEN_1 + uCheeseRandom;
                    break;
                case (EMERALD_YELLOW_CHEESE):
                    uCheeseRandom = g_uCheeseRandom[I & (MAX_CHEESE_RANDOM_NUMBERS - 1)];
                    uTextureIndex = TEX_CHEESE_YELLOW_1 + uCheeseRandom;
                    break;
                case (EMERALD_DOOR_END_NOT_READY):
                    uTextureIndex = TEX_DOOR_EXIT;
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
                case (EMERALD_DOOR_END_READY):
                    if (uSelfStatus == EMERALD_ANIM_MAN_GOES_ENDDOOR) {
                        uTextureIndex_0 = TEX_DOOR_EXIT_1 + ge_ExitDoorSequence[((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8]; // Endtür, blinkend
                        uTextureIndex = TEX_SPACE;
                        bExtendedElement = true;
                        switch (uAnimStatus) {
                            case(EMERALD_ANIM_LEFT):
                                uTextureIndex = TEX_MAN_LEFT_1 + nAnimationCount % 8;     // Man geht links
                                nXoffs = FONT_W - nAnimationCount;
                                nYoffs = nAnimationCount * 1;
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                break;
                            case(EMERALD_ANIM_UP):
                                uTextureIndex = TEX_MAN_UP_1 + nAnimationCount % 8;     // Man geht hoch
                                nXoffs = nAnimationCount / 2;
                                nYoffs = FONT_H - nAnimationCount;        // Man steht bereits auf neuer Position, daher + FONT_H
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                break;
                            case(EMERALD_ANIM_RIGHT):
                                uTextureIndex = TEX_MAN_LEFT_1 + nAnimationCount % 8;     // Man geht rechts
                                nXoffs = - FONT_W + nAnimationCount * 3;    // Durch das Verkleinern des Mans wird dieser zusätzlich nach links gezogen
                                nYoffs = nAnimationCount;
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                Flip = SDL_FLIP_HORIZONTAL;
                                break;
                            case(EMERALD_ANIM_DOWN):
                                uTextureIndex = TEX_MAN_DOWN_1 + nAnimationCount % 8;     // Man geht runter
                                nXoffs = nAnimationCount / 2;
                                nYoffs = -FONT_H + nAnimationCount * 3;     // Durch das Verkleinern des Mans wird dieser zusätzlich nach oben gezogen
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                break;
                        }
                    } else if (uSelfStatus == EMERALD_ANIM_DOOR_READY_SHRINK) {
                        uTextureIndex = TEX_DOOR_EXIT_1 + ge_ExitDoorSequence[((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8]; // Endtür, blinkend
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount * 2;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN1) {
                        uTextureIndex = TEX_DOOR_EXIT_1 + ge_ExitDoorSequence[((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8]; // Endtür, blinkend
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        uTextureIndex = TEX_DOOR_EXIT_1 + ge_ExitDoorSequence[((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8]; // Endtür, blinkend
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else {
                        uTextureIndex = TEX_DOOR_EXIT_1 + ge_ExitDoorSequence[((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8]; // Endtür, blinkend
                    }
                    break;
                case (EMERALD_DOOR_END_READY_STEEL):
                    uTextureIndex = TEX_DOOR_EXIT_STEEL_1 + ge_ExitDoorSequence[((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8]; // Stahl-Endtür, blinkend
                    if (uSelfStatus == EMERALD_ANIM_MAN_GOES_ENDDOOR) {
                        uTextureIndex_0 = TEX_DOOR_EXIT_STEEL_1 + ge_ExitDoorSequence[((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8]; // Stahl-Endtür, blinkend
                        uTextureIndex = TEX_SPACE;
                        bExtendedElement = true;
                        switch (uAnimStatus) {
                            case(EMERALD_ANIM_LEFT):
                                uTextureIndex = TEX_MAN_LEFT_1 + nAnimationCount % 8;     // Man geht links
                                nXoffs = FONT_W - nAnimationCount;
                                nYoffs = nAnimationCount * 1;
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                break;
                            case(EMERALD_ANIM_UP):
                                uTextureIndex = TEX_MAN_UP_1 + nAnimationCount % 8;     // Man geht hoch
                                nXoffs = nAnimationCount / 2;
                                nYoffs = FONT_H - nAnimationCount;        // Man steht bereits auf neuer Position, daher + FONT_H
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                break;
                            case(EMERALD_ANIM_RIGHT):
                                uTextureIndex = TEX_MAN_LEFT_1 + nAnimationCount % 8;     // Man geht rechts
                                nXoffs = - FONT_W + nAnimationCount * 3;    // Durch das Verkleinern des Mans wird dieser zusätzlich nach links gezogen
                                nYoffs = nAnimationCount;
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                Flip = SDL_FLIP_HORIZONTAL;
                                break;
                            case(EMERALD_ANIM_DOWN):
                                uTextureIndex = TEX_MAN_DOWN_1 + nAnimationCount % 8;     // Man geht runter
                                nXoffs = nAnimationCount / 2;
                                nYoffs = -FONT_H + nAnimationCount * 3;     // Durch das Verkleinern des Mans wird dieser zusätzlich nach oben gezogen
                                fScaleW = 1 - nAnimationCount * 0.03;
                                fScaleH = fScaleW;
                                break;
                        }
                    } else if (uSelfStatus == EMERALD_ANIM_DOOR_READY_SHRINK) { // Shrink-Animation wird hier für Stahl-Kopf-Überblendung verwendet
                        bExtendedElement = true;
                        uTextureIndex_0 = TEX_DOOR_EXIT_STEEL_1 + ge_ExitDoorSequence[((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 8]; // Stahl-Endtür, blinkend
                        uTextureIndex = TEX_STEEL_PLAYER;   // Stahl mit Kopf
                        fScaleW = nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        nXoffs = 16 - nAnimationCount;
                        nYoffs = nXoffs;
                    }
                    break;
                case (EMERALD_CENTRAL_EXPLOSION):
                case (EMERALD_CENTRAL_EXPLOSION_MEGA):
                case (EMERALD_CENTRAL_EXPLOSION_BEETLE):
                    if (nAnimationCount < 14) {
                        uTextureIndex = TEX_EXPLOSION_1 + nAnimationCount / 2;
                    } else {
                        uTextureIndex = TEX_EXPLOSION_1 + 5;
                    }
                    break;
                case (EMERALD_EXPLOSION_TO_ELEMENT_1):
                    uTextureIndex = TEX_EXPLOSION_1 + nAnimationCount / 3; // bei (nAnimationCount = 15) -> TEX_EXPLOSION_6
                    break;
                case (EMERALD_EXPLOSION_TO_ELEMENT_2):
                    switch (nAnimationCount) {
                        case (0):
                            uTextureIndex = TEX_EXPLOSION_6;
                            break;
                        case (1):
                        case (2):
                            uTextureIndex = TEX_EXPLOSION_7;
                            break;
                        case (3):
                        case (4):
                            uTextureIndex = TEX_EXPLOSION_6;
                            break;
                        case (5):
                        case (6):
                            uTextureIndex = TEX_EXPLOSION_5;
                            break;
                        case (7):
                        case (8):
                            uTextureIndex = TEX_EXPLOSION_4;
                            break;
                        case (9):
                        case (10):
                            uTextureIndex = TEX_EXPLOSION_3;
                            break;
                        case (11):
                        case (12):
                        case (13):
                            uTextureIndex = TEX_EXPLOSION_2;
                            break;
                        default:    // 14 und 15
                            uTextureIndex = TEX_EXPLOSION_1;
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
                                uTextureIndex_0 = TEX_EMERALD_1 + nAnimationCount / 2;     // Emerald, fallend
                                break;
                            case (EMERALD_SAPPHIRE):     // Emerald -> Saphir
                                uTextureIndex_0 = TEX_SAPPHIRE_1 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
                                break;
                            case (EMERALD_STONE):        // Saphir -> Stein
                                uTextureIndex_0 = TEX_STONE;
                                break;
                            case (EMERALD_BOMB):        // Perle -> Bombe
                                uTextureIndex_0 = TEX_BOMB_1 + nAnimationCount % 8;
                                break;
                            case (EMERALD_CRYSTAL):     // Kristall -> Kristall
                                uTextureIndex_0 = TEX_CRYSTAL;
                                break;
                            case (EMERALD_MEGABOMB):    // Rubin -> Megabombe
                                uTextureIndex_0 = TEX_BOMB_MEGA_1 + ((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 5;
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
                            uTextureIndex = TEX_MAGIC_WALL_1 + nAnimationCount / 2;
                        } else {    // Steel-Magic-Wall
                            uTextureIndex = TEX_MAGIC_WALL_STEEL_1 + nAnimationCount / 2;
                        }
                    } else {
                        if (uLevelElement == EMERALD_MAGIC_WALL) {
                            uTextureIndex = TEX_MAGIC_WALL_1;
                        } else {    // Steel-Magic-Wall
                            uTextureIndex = TEX_MAGIC_WALL_STEEL_1;
                        }
                    }
                    break;
                case (EMERALD_STONE_SINK):
                    uTextureIndex = TEX_STONE;
                    if (uSelfStatus == EMERALD_ANIM_STONE_QUICKSAND1) {
                        nYoffs = nAnimationCount;
                    } else if (uSelfStatus == EMERALD_ANIM_STONE_QUICKSAND2){
                        nYoffs = (FONT_H / 2) + nAnimationCount;
                    }
                    break;
                case (EMERALD_STONE_SAG):
                    bExtendedElement = true;
                    // Zu übermalendes Element
                    uTextureIndex_0 = TEX_STONE;   // Stein
                    nXoffs_0 = 0;
                    fScaleW_0 = 1;
                    fScaleH_0 = 1;
                    if (uSelfStatus == EMERALD_ANIM_STONE_QUICKSAND1) {
                        nYoffs_0 = nAnimationCount - FONT_H;
                    } else if (uSelfStatus == EMERALD_ANIM_STONE_QUICKSAND2){
                        nYoffs_0 = (FONT_H / 2) + nAnimationCount - FONT_H;
                    }
                    // Überdeckendes Element
                    nYoffs = -FONT_H;
                    uTextureIndex = TEX_QUICKSAND;    // Treibsand
                    break;
                case (EMERALD_QUICKSAND):
                case (EMERALD_QUICKSAND_STONE):
                    uTextureIndex = TEX_QUICKSAND;
                    break;
                case (EMERALD_QUICKSAND_SLOW):
                case (EMERALD_QUICKSAND_STONE_SLOW):
                    uTextureIndex = TEX_QUICKSAND_SLOW;
                    break;
                case (EMERALD_STONE_SINK_SLOW):
                    uTextureIndex = TEX_STONE;
                    if (uSelfStatus == EMERALD_ANIM_STONE_SLOW_QUICKSAND1) {
                        nYoffs = nAnimationCount / 2;                                   // 0 ... 7
                    } else if (uSelfStatus == EMERALD_ANIM_STONE_SLOW_QUICKSAND2) {
                        nYoffs = (FONT_H / 4) + nAnimationCount / 2;                    // 8 ... 15
                    } else if (uSelfStatus == EMERALD_ANIM_STONE_SLOW_QUICKSAND3) {
                        nYoffs = (FONT_H / 2) + nAnimationCount / 2;                    // 16 ... 23
                    } else if (uSelfStatus == EMERALD_ANIM_STONE_SLOW_QUICKSAND4) {
                        nYoffs = (FONT_H / 2) + (FONT_H / 4) + nAnimationCount / 2;     // 24 ... 31
                    }
                    break;
                case (EMERALD_STONE_SAG_SLOW):
                    bExtendedElement = true;
                    // Zu übermalendes Element
                    uTextureIndex_0 = TEX_STONE;   // Stein
                    nXoffs_0 = 0;
                    fScaleW_0 = 1;
                    fScaleH_0 = 1;
                    if (uSelfStatus == EMERALD_ANIM_STONE_SLOW_QUICKSAND1) {
                        nYoffs_0 = nAnimationCount / 2 - FONT_H;
                    } else if (uSelfStatus == EMERALD_ANIM_STONE_SLOW_QUICKSAND2) {
                        nYoffs_0 = (FONT_H / 4) + nAnimationCount / 2 - FONT_H;
                    } else if (uSelfStatus == EMERALD_ANIM_STONE_SLOW_QUICKSAND3) {
                        nYoffs_0 = (FONT_H / 2) + nAnimationCount / 2 - FONT_H;
                    } else if (uSelfStatus == EMERALD_ANIM_STONE_SLOW_QUICKSAND4) {
                        nYoffs_0 = (FONT_H / 2) + (FONT_H / 4) + nAnimationCount / 2 - FONT_H;
                    }
                    // Überdeckendes Element
                    nYoffs = -FONT_H;
                    uTextureIndex = TEX_QUICKSAND_SLOW;    // Treibsand
                    break;
                case (EMERALD_ACIDPOOL_DESTROY):
                    // Splash rechts
                    Playfield.pPostAnimation[uPostAnimationIndex].uTextureIndex = TEX_ACID_SPLASH_01;  // AcidSplash 1/15
                    Playfield.pPostAnimation[uPostAnimationIndex].uX = uX + 1;
                    Playfield.pPostAnimation[uPostAnimationIndex].uY = uY;
                    Playfield.pPostAnimation[uPostAnimationIndex].bFlipHorizontal = false;
                    Playfield.pPostAnimation[uPostAnimationIndex].bFlipVertical = false;
                    uPostAnimationIndex++;
                    // Splash links
                    Playfield.pPostAnimation[uPostAnimationIndex].uTextureIndex = TEX_ACID_SPLASH_01;  // AcidSplash 1/15
                    Playfield.pPostAnimation[uPostAnimationIndex].uX = uX - 1;
                    Playfield.pPostAnimation[uPostAnimationIndex].uY = uY;
                    Playfield.pPostAnimation[uPostAnimationIndex].bFlipHorizontal = true;
                    Playfield.pPostAnimation[uPostAnimationIndex].bFlipVertical = false;
                    uPostAnimationIndex++;
                    uTextureIndex = GetTextureIndexByElementForAcidPool(Playfield.pInvalidElement[I],nAnimationCount,&fAngle);
                    nYoffs = nAnimationCount * 2;
                    break;
                case (EMERALD_ACIDPOOL_TOP_MID):
                    uTextureIndex = TEX_ACID_1 + (Playfield.uFrameCounter / 13) % 4;        // Säurebecken, oben mitte (aktives Feld), verlangsamte Animation (13 Frames für eine Animations-Phase)
                    break;
                case (EMERALD_REPLICATOR_RED_SWITCH):
                    if (Playfield.bReplicatorRedOn) {
                        uTextureIndex = TEX_SWITCH_REPLICATOR_RED_ON;     // Replikator-Schalter, rot, an
                    } else {
                        uTextureIndex = TEX_SWITCH_REPLICATOR_RED_OFF;     // Replikator-Schalter, rot, aus
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_SWITCH):
                    if (Playfield.bReplicatorYellowOn) {
                        uTextureIndex = TEX_SWITCH_REPLICATOR_YELLOW_ON;     // Replikator-Schalter, gelb, an
                    } else {
                        uTextureIndex = TEX_SWITCH_REPLICATOR_YELLOW_OFF;     // Replikator-Schalter, gelb, aus
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_SWITCH):
                    if (Playfield.bReplicatorGreenOn) {
                        uTextureIndex = TEX_SWITCH_REPLICATOR_GREEN_ON;     // Replikator-Schalter, grün, an
                    } else {
                        uTextureIndex = TEX_SWITCH_REPLICATOR_GREEN_OFF;     // Replikator-Schalter, grün, aus
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_SWITCH):
                    if (Playfield.bReplicatorBlueOn) {
                        uTextureIndex = TEX_SWITCH_REPLICATOR_BLUE_ON;     // Replikator-Schalter, blau, an
                    } else {
                        uTextureIndex = TEX_SWITCH_REPLICATOR_BLUE_OFF;     // Replikator-Schalter, blau, aus
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_TOP_LEFT):
                    if (Playfield.bReplicatorRedOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = TEX_REPLICATOR_RED_LEFT_TOP_ON_1;     // Replikator, rot, an, oben links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = TEX_REPLICATOR_RED_LEFT_TOP_ON_2;     // Replikator, rot, an, oben links, Animationsschritt 2
                        } else {
                            uTextureIndex = TEX_REPLICATOR_RED_LEFT_TOP_ON_3;     // Replikator, rot, an, oben links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = TEX_REPLICATOR_RED_LEFT_TOP_OFF;     // Replikator, rot, aus, oben links
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_TOP_MID):
                    if (Playfield.bReplicatorRedOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = TEX_REPLICATOR_RED_MIDDLE_TOP_ON_1;     // Replikator, rot, an, mitte, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = TEX_REPLICATOR_RED_MIDDLE_TOP_ON_2;     // Replikator, rot, an, mitte, Animationsschritt 2
                        } else {
                            uTextureIndex = TEX_REPLICATOR_RED_MIDDLE_TOP_ON_3;     // Replikator, rot, an, mitte, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = TEX_REPLICATOR_RED_MIDDLE_TOP_OFF;     // Replikator, rot, aus, oben mitte
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_TOP_RIGHT):
                    if (Playfield.bReplicatorRedOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = TEX_REPLICATOR_RED_RIGHT_TOP_ON_1;     // Replikator, rot, an, oben rechts, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = TEX_REPLICATOR_RED_RIGHT_TOP_ON_2;     // Replikator, rot, an, oben rechts, Animationsschritt 2
                        } else {
                            uTextureIndex = TEX_REPLICATOR_RED_RIGHT_TOP_ON_3;     // Replikator, rot, an, oben rechts, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = TEX_REPLICATOR_RED_RIGHT_TOP_OFF;     // Replikator, rot, aus, oben rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_BOTTOM_LEFT):
                    if (Playfield.bReplicatorRedOn) {
                        uTextureIndex = TEX_REPLICATOR_RED_LEFT_BOTTOM_ON;     // Replikator, rot, an, unten links, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = TEX_REPLICATOR_RED_LEFT_BOTTOM_OFF;    // Replikator, rot, aus, unten links
                    }
                    break;
                case (EMERALD_REPLICATOR_RED_BOTTOM_RIGHT):
                    if (Playfield.bReplicatorRedOn) {
                        uTextureIndex = TEX_REPLICATOR_RED_RIGHT_BOTTOM_ON;     // Replikator, rot, an, unten rechts, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = TEX_REPLICATOR_RED_RIGHT_BOTTOM_OFF;   // Replikator, rot, aus, unten rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_TOP_LEFT):
                    if (Playfield.bReplicatorGreenOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = TEX_REPLICATOR_GREEN_LEFT_TOP_ON_1;     // Replikator, grün, an, oben links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = TEX_REPLICATOR_GREEN_LEFT_TOP_ON_2;     // Replikator, grün, an, oben links, Animationsschritt 2
                        } else {
                            uTextureIndex = TEX_REPLICATOR_GREEN_LEFT_TOP_ON_3;     // Replikator, grün, an, oben links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = TEX_REPLICATOR_GREEN_LEFT_TOP_OFF;     // Replikator, grün, aus, oben links
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_TOP_MID):
                    if (Playfield.bReplicatorGreenOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = TEX_REPLICATOR_GREEN_MIDDLE_TOP_ON_1;     // Replikator, grün, an, mitte, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = TEX_REPLICATOR_GREEN_MIDDLE_TOP_ON_2;     // Replikator, grün, an, mitte, Animationsschritt 2
                        } else {
                            uTextureIndex = TEX_REPLICATOR_GREEN_MIDDLE_TOP_ON_3;     // Replikator, grün, an, mitte, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = TEX_REPLICATOR_GREEN_MIDDLE_TOP_OFF;     // Replikator, grün, aus, oben mitte
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_TOP_RIGHT):
                    if (Playfield.bReplicatorGreenOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = TEX_REPLICATOR_GREEN_RIGHT_TOP_ON_1;     // Replikator, grün, an, oben rechts, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = TEX_REPLICATOR_GREEN_RIGHT_TOP_ON_2;     // Replikator, grün, an, oben rechts, Animationsschritt 2
                        } else {
                            uTextureIndex = TEX_REPLICATOR_GREEN_RIGHT_TOP_ON_3;     // Replikator, grün, an, oben rechts, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = TEX_REPLICATOR_GREEN_RIGHT_TOP_OFF;     // Replikator, grün, aus, oben rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT):
                    if (Playfield.bReplicatorGreenOn) {
                        uTextureIndex = TEX_REPLICATOR_GREEN_LEFT_BOTTOM_ON;     // Replikator, grün, an, unten links, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = TEX_REPLICATOR_GREEN_LEFT_BOTTOM_OFF;     // Replikator, grün, aus, unten links
                    }
                    break;
                case (EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT):
                    if (Playfield.bReplicatorGreenOn) {
                        uTextureIndex = TEX_REPLICATOR_GREEN_RIGHT_BOTTOM_ON;     // Replikator, grün, an, unten rechts, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = TEX_REPLICATOR_GREEN_RIGHT_BOTTOM_OFF;     // Replikator, grün, aus, unten rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_TOP_LEFT):
                    if (Playfield.bReplicatorBlueOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = TEX_REPLICATOR_BLUE_LEFT_TOP_ON_1;     // Replikator, blau, an, oben links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = TEX_REPLICATOR_BLUE_LEFT_TOP_ON_2;     // Replikator, blau, an, oben links, Animationsschritt 2
                        } else {
                            uTextureIndex = TEX_REPLICATOR_BLUE_LEFT_TOP_ON_3;     // Replikator, blau, an, oben links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = TEX_REPLICATOR_BLUE_LEFT_TOP_OFF;     // Replikator, blau, aus, oben links
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_TOP_MID):
                    if (Playfield.bReplicatorBlueOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = TEX_REPLICATOR_BLUE_MIDDLE_TOP_ON_1;     // Replikator, blau, an, mitte, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = TEX_REPLICATOR_BLUE_MIDDLE_TOP_ON_2;     // Replikator, blau, an, mitte, Animationsschritt 2
                        } else {
                            uTextureIndex = TEX_REPLICATOR_BLUE_MIDDLE_TOP_ON_3;     // Replikator, blau, an, mitte, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = TEX_REPLICATOR_BLUE_MIDDLE_TOP_OFF;     // Replikator, blau, aus, oben mitte
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_TOP_RIGHT):
                    if (Playfield.bReplicatorBlueOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = TEX_REPLICATOR_BLUE_RIGHT_TOP_ON_1;     // Replikator, blau, an, oben rechts, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = TEX_REPLICATOR_BLUE_RIGHT_TOP_ON_2;     // Replikator, blau, an, oben rechts, Animationsschritt 2
                        } else {
                            uTextureIndex = TEX_REPLICATOR_BLUE_RIGHT_TOP_ON_3;     // Replikator, blau, an, oben rechts, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = TEX_REPLICATOR_BLUE_RIGHT_TOP_OFF;     // Replikator, blau, aus, oben rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT):
                    if (Playfield.bReplicatorBlueOn) {
                        uTextureIndex = TEX_REPLICATOR_BLUE_LEFT_BOTTOM_ON;     // Replikator, blau, an, unten links, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = TEX_REPLICATOR_BLUE_LEFT_BOTTOM_OFF;     // Replikator, blau, aus, unten links
                    }
                    break;
                case (EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT):
                    if (Playfield.bReplicatorBlueOn) {
                        uTextureIndex = TEX_REPLICATOR_BLUE_RIGHT_BOTTOM_ON;     // Replikator, blau, an, unten rechts, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = TEX_REPLICATOR_BLUE_RIGHT_BOTTOM_OFF;     // Replikator, blau, aus, unten rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_TOP_LEFT):
                    if (Playfield.bReplicatorYellowOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = TEX_REPLICATOR_YELLOW_LEFT_TOP_ON_1;     // Replikator, gelb, an, oben links, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = TEX_REPLICATOR_YELLOW_LEFT_TOP_ON_2;     // Replikator, gelb, an, oben links, Animationsschritt 2
                        } else {
                            uTextureIndex = TEX_REPLICATOR_YELLOW_LEFT_TOP_ON_3;     // Replikator, gelb, an, oben links, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = TEX_REPLICATOR_YELLOW_LEFT_TOP_OFF;     // Replikator, gelb, aus, oben links
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_TOP_MID):
                    if (Playfield.bReplicatorYellowOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = TEX_REPLICATOR_YELLOW_MIDDLE_TOP_ON_1;     // Replikator, gelb, an, mitte, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = TEX_REPLICATOR_YELLOW_MIDDLE_TOP_ON_2;     // Replikator, gelb, an, mitte, Animationsschritt 2
                        } else {
                            uTextureIndex = TEX_REPLICATOR_YELLOW_MIDDLE_TOP_ON_3;     // Replikator, gelb, an, mitte, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = TEX_REPLICATOR_YELLOW_MIDDLE_TOP_OFF;     // Replikator, gelb, aus, oben mitte
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_TOP_RIGHT):
                    if (Playfield.bReplicatorYellowOn) {
                        if ((uReplicatorAnimation >= 0) && (uReplicatorAnimation <= 3)) {
                            uTextureIndex = TEX_REPLICATOR_YELLOW_RIGHT_TOP_ON_1;     // Replikator, gelb, an, oben rechts, Animationsschritt 1
                        } else if ((uReplicatorAnimation >= 4) && (uReplicatorAnimation <= 7)) {
                            uTextureIndex = TEX_REPLICATOR_YELLOW_RIGHT_TOP_ON_2;     // Replikator, gelb, an, oben rechts, Animationsschritt 2
                        } else {
                            uTextureIndex = TEX_REPLICATOR_YELLOW_RIGHT_TOP_ON_3;     // Replikator, gelb, an, oben rechts, Animationsschritt 3
                        }
                    } else {
                        uTextureIndex = TEX_REPLICATOR_YELLOW_RIGHT_TOP_OFF;     // Replikator, gelb, aus, oben rechts
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT):
                    if (Playfield.bReplicatorYellowOn) {
                        uTextureIndex = TEX_REPLICATOR_YELLOW_LEFT_BOTTOM_ON;     // Replikator, gelb, an, unten links, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = TEX_REPLICATOR_YELLOW_LEFT_BOTTOM_OFF;     // Replikator, gelb, aus, unten links
                    }
                    break;
                case (EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT):
                    if (Playfield.bReplicatorYellowOn) {
                        uTextureIndex = TEX_REPLICATOR_YELLOW_RIGHT_BOTTOM_ON;     // Replikator, gelb, an, unten rechts, Animationsschritt 1,2 u. 3
                    } else {
                        uTextureIndex = TEX_REPLICATOR_YELLOW_RIGHT_BOTTOM_OFF;     // Replikator, gelb, aus, unten rechts
                    }
                    break;
                case (EMERALD_KEY_WHITE):
                    uTextureIndex = TEX_KEY_WHITE;
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
                    uTextureIndex = TEX_KEY_GENERAL;
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
                    uTextureIndex = TEX_KEY_RED;     // roter Schlüssel
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
                    uTextureIndex = TEX_KEY_YELLOW;    // gelber Schlüssel
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
                    uTextureIndex = TEX_KEY_BLUE;    // blauer Schlüssel
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
                    uTextureIndex = TEX_KEY_GREEN;     // grüner Schlüssel
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
                case (EMERALD_DOOR_GREY_RED):
                    if (Playfield.bLightOn) {
                        uTextureIndex = TEX_DOOR_GREY_RED;     // Geheimtür
                    } else {
                        uTextureIndex = TEX_DOOR_GREY;     // Geheimtür
                    }
                    break;
                case (EMERALD_DOOR_GREY_GREEN):
                    if (Playfield.bLightOn) {
                        uTextureIndex = TEX_DOOR_GREY_GREEN;     // Geheimtür
                    } else {
                        uTextureIndex = TEX_DOOR_GREY;     // Geheimtür
                    }
                    break;
                case (EMERALD_DOOR_GREY_BLUE):
                    if (Playfield.bLightOn) {
                        uTextureIndex = TEX_DOOR_GREY_BLUE;     // Geheimtür
                    } else {
                        uTextureIndex = TEX_DOOR_GREY;     // Geheimtür
                    }
                    break;
                case (EMERALD_DOOR_GREY_YELLOW):
                    if (Playfield.bLightOn) {
                        uTextureIndex = TEX_DOOR_GREY_YELLOW;     // Geheimtür
                    } else {
                        uTextureIndex = TEX_DOOR_GREY;     // Geheimtür
                    }
                    break;
                case (EMERALD_DOOR_GREY_WHITE):
                    if (Playfield.bLightOn) {
                        uTextureIndex = TEX_DOOR_GREY_WHITE;     // Geheimtür
                    } else {
                        uTextureIndex = TEX_DOOR_GREY;     // Geheimtür
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
                         (uSelfStatus == EMERALD_ANIM_SHIELD_COIN_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_HAMMER_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_DYNAMITE_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_MESSAGE_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_SAPPHIRE_SHRINK) ||
                         (uSelfStatus == EMERALD_ANIM_GRASS_SHRINK) ||
                         ((uSelfStatus == EMERALD_ANIM_EARTH_INVISIBLE_SHRINK) && (Playfield.bLightOn)) ||
                         (uSelfStatus == EMERALD_ANIM_EARTH_SHRINK) ) {
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
                                uTextureIndex = TEX_MAN_PUSH_RIGHT_1 + nAnimationCount / 2;     // Man links schiebend
                            } else {
                                uTextureIndex = TEX_MAN_PUSH_RIGHT_4 + 5 - nAnimationCount / 2;
                            }
                        } else {
                            if (uSelfStatus == EMERALD_ANIM_MAN_BLOCKED_LEFT) {
                                nXoffs = 0;
                            }
                            uTextureIndex = TEX_MAN_LEFT_1 + nAnimationCount % 8;     // Man links
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        nXoffs = nAnimationCount * 2 - FONT_W;      // Man steht bereits auf neuer Position, daher - FONT_W
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT) {
                            if (nAnimationCount <= 9) {
                                uTextureIndex = TEX_MAN_PUSH_RIGHT_1 + nAnimationCount / 2;     // Man rechts schiebend
                            } else {
                                uTextureIndex = TEX_MAN_PUSH_RIGHT_4 + 5 - nAnimationCount / 2;
                            }
                        } else {
                            if (uSelfStatus == EMERALD_ANIM_MAN_BLOCKED_RIGHT) {
                                nXoffs = 0;
                            }
                            uTextureIndex = TEX_MAN_LEFT_1 + nAnimationCount % 8; // Man rechts
                            Flip = SDL_FLIP_HORIZONTAL;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_UP) {
                        uTextureIndex = TEX_MAN_UP_1 + nAnimationCount % 8;     // Man hoch
                        if (uSelfStatus == EMERALD_ANIM_MAN_BLOCKED_UP) {
                            nYoffs = 0;
                        } else {
                            nYoffs = -nAnimationCount * 2 + FONT_H;        // Man steht bereits auf neuer Position, daher + FONT_H
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        uTextureIndex = TEX_MAN_DOWN_1 + nAnimationCount % 8;     // Man runter

                        if (uSelfStatus == EMERALD_ANIM_MAN_BLOCKED_DOWN) {
                            nYoffs = 0;
                        } else {
                          nYoffs = nAnimationCount * 2 - FONT_H;         // Man steht bereits auf neuer Position, daher - FONT_H
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_LEFT_DOUBLESPEED) {
                        uTextureIndex = TEX_MAN_LEFT_1 + nAnimationCount % 8;     // Man links
                        nXoffs = -nAnimationCount * 4 + FONT_W * 2;    // Man steht bereits auf neuer Position, daher + FONT_W * 2 (DoubleSpeed)
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT_DOUBLESPEED) {
                        uTextureIndex = TEX_MAN_LEFT_1 + nAnimationCount % 8;     // Man rechts
                        nXoffs = nAnimationCount * 4 - FONT_W * 2;     // Man steht bereits auf neuer Position, daher - FONT_W * 2 (DoubleSpeed)
                        Flip = SDL_FLIP_HORIZONTAL;
                    } else if (uAnimStatus == EMERALD_ANIM_UP_DOUBLESPEED) {
                        uTextureIndex = TEX_MAN_UP_1 + nAnimationCount % 8;     // Man hoch
                        nYoffs = -nAnimationCount * 4 + FONT_H * 2;    // Man steht bereits auf neuer Position, daher + FONT_H * 2 (DoubleSpeed)
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN_DOUBLESPEED) {
                        uTextureIndex = TEX_MAN_DOWN_1 + nAnimationCount % 8;     // Man runter
                        nYoffs = nAnimationCount * 4 - FONT_H * 2;    // Man steht bereits auf neuer Position, daher - FONT_H * 2 (DoubleSpeed)
                    } else {
                        if (uSelfStatus == EMERALD_ANIM_MAN_LEFT_ARM) {
                            uTextureIndex = TEX_MAN_ARM_LEFT;                 // Man stehend, linker Arm
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_RIGHT_ARM) {
                            uTextureIndex = TEX_MAN_ARM_RIGHT;                // Man stehend, rechter Arm
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_UP_ARM) {
                            uTextureIndex = TEX_MAN_ARM_UP;                   // Man stehend, Arm hoch
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_DOWN_ARM) {
                            uTextureIndex = TEX_MAN_ARM_DOWN;                 // Man stehend, Arm runter
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT) {
                            if (nAnimationCount <= 9) {
                                uTextureIndex = TEX_MAN_PUSH_RIGHT_1 + nAnimationCount / 2;     // Man rechts schiebend
                            } else {
                                uTextureIndex = TEX_MAN_PUSH_RIGHT_4 + 5 - nAnimationCount / 2;
                            }
                        } else if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_LEFT) {
                            Flip = SDL_FLIP_HORIZONTAL;
                            if (nAnimationCount <= 9) {
                                uTextureIndex = TEX_MAN_PUSH_RIGHT_1 + nAnimationCount / 2;     // Man rechts schiebend
                            } else {
                                uTextureIndex = TEX_MAN_PUSH_RIGHT_4 + 5 - nAnimationCount / 2;
                            }
                        } else {
                            // Steht Man noch auf selbst gezündeten Dynamit?
                            if ((Playfield.uManXpos + Playfield.uManYpos * Playfield.uLevel_X_Dimension) == Playfield.uDynamitePos) {
                                bExtendedElement = true;
                                uTextureIndex_0 = TEX_MAN;      // Man stehend, unter Dynamit
                                switch (Playfield.uDynamiteStatusAnim) {
                                    case (EMERALD_ANIM_DYNAMITE_ON_P1):
                                        uTextureIndex = TEX_DYNAMITE_ON_1;
                                        break;
                                    case (EMERALD_ANIM_DYNAMITE_ON_P2):
                                        uTextureIndex = TEX_DYNAMITE_ON_2;
                                        break;
                                    case (EMERALD_ANIM_DYNAMITE_ON_P3):
                                        uTextureIndex = TEX_DYNAMITE_ON_3;
                                        break;
                                    case (EMERALD_ANIM_DYNAMITE_ON_P4):
                                        uTextureIndex = TEX_DYNAMITE_ON_4;
                                        break;
                                    default:
                                        uTextureIndex = TEX_DYNAMITE_ON_1;
                                        break;
                                }
                            } else {
                                uTextureIndex = TEX_MAN;                            // Man stehend
                            }
                        }
                    }
                    break;
                case (EMERALD_BOMB):
                    uTextureIndex = TEX_BOMB_1;     // Bomb, liegend
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
                        uTextureIndex = TEX_BOMB_1 + nAnimationCount % 8;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_LEFT2) {
                            nXoffs = -nAnimationCount * 2 + FONT_W;
                        } else {
                            nXoffs = -nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        uTextureIndex = TEX_BOMB_1 + nAnimationCount % 8;
                        if (uSelfStatus == EMERALD_ANIM_MAN_PUSH_RIGHT2) {
                            nXoffs = nAnimationCount * 2 - FONT_W;
                        } else {
                            nXoffs = nAnimationCount * 2;
                        }
                    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                        nYoffs = nAnimationCount * 2;
                        uTextureIndex = TEX_BOMB_1 + nAnimationCount % 8;
                    }
                    break;
                case (EMERALD_MEGABOMB):
                    uTextureIndex = TEX_BOMB_MEGA_1;     // Mega-Bomb, liegend
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
                        uTextureIndex = TEX_BOMB_MEGA_1 + ((Playfield.uFrameCounter & 0xFFFFFFFC) >> 2) % 5;
                        nYoffs = nAnimationCount * 2;
                    }
                    break;
                case (EMERALD_PERL):
                    uTextureIndex = TEX_PERL_1;     // Perle, liegend
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
                        uTextureIndex = TEX_PERL_1 + nAnimationCount % 8;
                        nXoffs = -nAnimationCount * 2;
                    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        uTextureIndex = TEX_PERL_1 + nAnimationCount % 8;
                        nXoffs = nAnimationCount * 2;
                    } else if ((uAnimStatus == EMERALD_ANIM_DOWN) || (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL)) {
                        nYoffs = nAnimationCount * 2;
                        uTextureIndex = TEX_PERL_1 + nAnimationCount % 8;
                    } else if (uSelfStatus == EMERALD_ANIM_PERL_BREAK) {
                        if (nAnimationCount <= 2) {   // 0, 1, 2
                            uTextureIndex = TEX_PERL_BROKEN_1;
                        } else if (nAnimationCount <= 5) { // 3, 4, 5
                            uTextureIndex = TEX_PERL_BROKEN_2;
                        } else if (nAnimationCount <= 8) { // 6, 7, 8
                            uTextureIndex = TEX_PERL_BROKEN_3;
                        } else {
                            uTextureIndex = TEX_SPACE;  // Space
                        }
                    }
                    break;
                case (EMERALD_STONE):
                    uTextureIndex = TEX_STONE;     // Stein, liegend
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
                            uTextureIndex_0 = TEX_SAPPHIRE_1; // Saphir, liegend
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
                    uTextureIndex = TEX_NUT;     // Nut, liegend
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
                        if (nAnimationCount < 12) {
                            uTextureIndex = TEX_NUT_CRACK_1 + nAnimationCount / 4;
                        } else {
                            uTextureIndex = TEX_EMERALD_1;
                        }
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
                        uTextureIndex = TEX_RUBY_1;     // Rubin, liegend
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
                            uTextureIndex = TEX_RUBY_1 + nAnimationCount / 2;     // Rubin, liegend
                            nXoffs = -nAnimationCount * 2;
                            fAngleOffs = -nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            uTextureIndex = TEX_RUBY_1 + nAnimationCount / 2;     // Rubin, liegend
                            nXoffs = nAnimationCount * 2;
                            fAngleOffs = nAnimationCount * 22.5;
                        } else if ((uAnimStatus == EMERALD_ANIM_DOWN) || (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL)) {
                            nYoffs = nAnimationCount * 2;
                            uTextureIndex = TEX_RUBY_1 + nAnimationCount / 2;     // Rubin, liegend
                        }
                    } else {
                        bExtendedElement = true;
                        fAngle_0 = 0;
                        uTextureIndex_0 = TEX_RUBY_1;     // Rubin, liegend
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
                            uTextureIndex_0 = TEX_RUBY_1 + nAnimationCount / 2;     // Rubin, liegend
                            nXoffs_0 = -nAnimationCount * 2;
                            fAngleOffs_0 = -nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            uTextureIndex_0 = TEX_RUBY_1 + nAnimationCount / 2;     // Rubin, liegend
                            nXoffs_0 = nAnimationCount * 2;
                            fAngleOffs_0 = nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs_0 = nAnimationCount * 2;
                            uTextureIndex_0 = TEX_RUBY_1 + nAnimationCount / 2;     // Rubin, liegend
                        }
                        // Blitz-Animation über Rubin
                        nXoffs = 0;
                        nYoffs = 0;
                        if (nAnimationCount <= 7) {
                            uTextureIndex = TEX_FLASH_GEM_1 + nAnimationCount / 2;
                        } else {
                            uTextureIndex = TEX_FLASH_GEM_4 - (nAnimationCount - 8) / 2;
                        }
                    }
                    break;
                case (EMERALD_CRYSTAL):
                    // Soll Kristall blitzen ?
                    uTextureIndex = TEX_CRYSTAL;       // Kristall, liegend
                    uTextureIndex_0 = TEX_CRYSTAL;     // Kristall, liegend
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
                            uTextureIndex = TEX_FLASH_GEM_1 + nAnimationCount / 2;
                        } else {
                            uTextureIndex = TEX_FLASH_GEM_4 - (nAnimationCount - 8) / 2;
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
                        uTextureIndex = TEX_EMERALD_1;     // Emerald, liegend
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
                            uTextureIndex = TEX_EMERALD_1 + nAnimationCount / 2;     // Emerald, liegend
                            nXoffs = -nAnimationCount * 2;
                            fAngleOffs = -nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            uTextureIndex = TEX_EMERALD_1 + nAnimationCount / 2;     // Emerald, liegend
                            nXoffs = nAnimationCount * 2;
                            fAngleOffs = nAnimationCount * 22.5;
                        } else if ((uAnimStatus == EMERALD_ANIM_DOWN) || (uSelfStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL)) {
                            nYoffs = nAnimationCount * 2;
                            uTextureIndex = TEX_EMERALD_1 + nAnimationCount / 2;     // Emerald, liegend
                        }
                    } else {
                        bExtendedElement = true;
                        fAngle_0 = 0;
                        uTextureIndex_0 = TEX_EMERALD_1;     // Emerald, liegend
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
                            uTextureIndex_0 = TEX_EMERALD_1 + nAnimationCount / 2;     // Emerald, liegend
                            nXoffs_0 = -nAnimationCount * 2;
                            fAngleOffs_0 = -nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                            uTextureIndex_0 = TEX_EMERALD_1 + nAnimationCount / 2;     // Emerald, liegend
                            nXoffs_0 = nAnimationCount * 2;
                            fAngleOffs_0 = nAnimationCount * 22.5;
                        } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
                            nYoffs_0 = nAnimationCount * 2;
                            uTextureIndex_0 = TEX_EMERALD_1 + nAnimationCount / 2;     // Emerald, liegend
                        }
                        // Blitz-Animation über Emerald
                        nXoffs = 0;
                        nYoffs = 0;
                        if (nAnimationCount <= 7) {
                            uTextureIndex = TEX_FLASH_GEM_1 + nAnimationCount / 2;
                        } else {
                            uTextureIndex = TEX_FLASH_GEM_4 - (nAnimationCount - 8) / 2;
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
                        uTextureIndex = TEX_SAPPHIRE_1;
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
                            uTextureIndex = TEX_SAPPHIRE_1 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
                        }
                    } else {
                        bExtendedElement = true;
                        fAngle_0 = 0;
                        uTextureIndex_0 = TEX_SAPPHIRE_1;     // Saphir, liegend
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
                            uTextureIndex_0 = TEX_SAPPHIRE_1 + ((Playfield.uFrameCounter & 0xFFFFFFFE) >> 1) % 9; // Saphir fallend
                        }
                        // Blitz-Animation über Saphir
                        nXoffs = 0;
                        nYoffs = 0;
                        if (nAnimationCount <= 7) {
                            uTextureIndex = TEX_FLASH_GEM_1 + nAnimationCount / 2;
                        } else {
                            uTextureIndex = TEX_FLASH_GEM_4 - (nAnimationCount - 8) / 2;
                        }
                    }
                    break;
                case (EMERALD_TIME_COIN):
                    uTextureIndex = TEX_COIN_TIME_1 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 6; // Zeitmünze, drehend
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
                case (EMERALD_SHIELD_COIN):
                    uTextureIndex = TEX_COIN_SHIELD_1 + ((Playfield.uFrameCounter & 0xFFFFFFF8) >> 3) % 6; // Schildmünze, drehend
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
                    } else if (uSelfStatus == EMERALD_ANIM_SHIELD_COIN_SHRINK) { // Man mit Richtung und Fire
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 10;
                    }
                    break;
                case (EMERALD_HAMMER):
                    uTextureIndex = TEX_HAMMER;     // Hammer
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
                    uTextureIndex = TEX_DYNAMITE_OFF;                                // Dynamit
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
                        uTextureIndex = TEX_ALIEN_STAND_1 + nAnimationCount / 2;     // Alien stehend
                        nXoffs = 15 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_BORN2) {
                        uTextureIndex = TEX_ALIEN_STAND_1 + nAnimationCount / 2;     // Alien stehend
                        nXoffs = 7 - nAnimationCount / 2;
                        nYoffs = nXoffs;
                        fScaleW = 0.5 + nAnimationCount * 0.031;
                        fScaleH = fScaleW;
                    } else if (uAnimStatus == EMERALD_ANIM_STAND) {
                        uTextureIndex = TEX_ALIEN_STAND_1 + nAnimationCount / 2;     // Alien stehend
                    } else {
                        if ((nAnimationCount >= 4) && (nAnimationCount <= 11)) {
                            uTextureIndex = TEX_ALIEN_MOVE_1;                        // Alien geht 1
                        } else {
                            uTextureIndex = TEX_ALIEN_MOVE_2;                        // Alien geht 2, Flügel voll ausgebreitet
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
                    uTextureIndex = TEX_WHEEL;     // Alien-Rad
                    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_WHEEL_RUN) {
                        fAngleOffs = nAnimationCount * 11;
                    } else {
                        fAngleOffs = 0;
                    }
                    break;
                case (EMERALD_MINE_EARTH):
                    // Hinweis: Die äußeren Klammern sind wichtig !
                    uTextureIndex = TEX_MINE_EARTH_LEFT_RIGHT_TOP_BOTTOM + (((Playfield.pStatusAnimation[I] & 0xFF00) - EMERALD_ANIM_EARTH_0) >> 8);    // Erd-Mine mit richtigem Rand aussuchen
                    break;
                case (EMERALD_EARTH):
                    if (uSelfStatus == EMERALD_ANIM_EARTH_SHRINK) { // Man mit Richtung und Fire
                        uTextureIndex = TEX_EARTH_LEFT_RIGHT_TOP_BOTTOM; // kompl. freie Erde
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 22.5;
                    } else {
                        // Hinweis: Die äußeren Klammern sind wichtig !
                        uTextureIndex = TEX_EARTH_LEFT_RIGHT_TOP_BOTTOM + (((Playfield.pStatusAnimation[I] & 0xFF00) - EMERALD_ANIM_EARTH_0) >> 8);    // Erde mit richtigem Rand aussuchen
                    }
                    break;
                case (EMERALD_EARTH_INVISIBLE):
                    if (Playfield.bLightOn) {
                        if (uSelfStatus == EMERALD_ANIM_EARTH_INVISIBLE_SHRINK) { // Man mit Richtung und Fire
                            uTextureIndex = TEX_EARTH_INVISIBLE_LEFT_RIGHT_TOP_BOTTOM; // kompl. freie Erde
                            nXoffs = nAnimationCount;
                            nYoffs = nAnimationCount;
                            fScaleW = 1 - nAnimationCount * 0.06;
                            fScaleH = fScaleW;
                            fAngleOffs = nAnimationCount * 22.5;
                        } else {
                            // Hinweis: Die äußeren Klammern sind wichtig !
                            uTextureIndex = TEX_EARTH_INVISIBLE_LEFT_RIGHT_TOP_BOTTOM + (((Playfield.pStatusAnimation[I] & 0xFF00) - EMERALD_ANIM_EARTH_0) >> 8);    // Erde mit richtigem Rand aussuchen
                        }
                    } else {
                        uTextureIndex = TEX_SPACE;
                    }
                    break;
                case (EMERALD_GRASS):
                    if (uSelfStatus == EMERALD_ANIM_GRASS_SHRINK) { // Man mit Richtung und Fire
                        uTextureIndex = TEX_GRASS_LEFT_RIGHT_TOP_BOTTOM; // kompl. freies Gras
                        nXoffs = nAnimationCount;
                        nYoffs = nAnimationCount;
                        fScaleW = 1 - nAnimationCount * 0.06;
                        fScaleH = fScaleW;
                        fAngleOffs = nAnimationCount * 22.5;
                    } else {
                        // Hinweis: Die äußeren Klammern sind wichtig !
                        uTextureIndex = TEX_GRASS_LEFT_RIGHT_TOP_BOTTOM + (((Playfield.pStatusAnimation[I] & 0xFF00) - EMERALD_ANIM_EARTH_0) >> 8);    // Gras mit richtigem Rand aussuchen
                    }
                    break;
                case (EMERALD_GRASS_COMES):
                    uTextureIndex = TEX_GRASS_LEFT_RIGHT_TOP_BOTTOM; // kompl. freies Gras
                    nXoffs = (FONT_W / 2) - nAnimationCount;
                    nYoffs = (FONT_H / 2) - nAnimationCount;
                    fScaleW = nAnimationCount * 0.06;
                    fScaleH = fScaleW;
                    break;
                case (EMERALD_BEETLE_UP):   // Käfer hoch
                    uTextureIndex = TEX_BEETLE_LEFT_1 + nAnimationCount % 8;     // Käfer links
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
                    uTextureIndex = TEX_BEETLE_LEFT_1 + nAnimationCount % 8;     // Käfer links
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
                    uTextureIndex = TEX_BEETLE_LEFT_1 + nAnimationCount % 8;     // Käfer links
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
                    uTextureIndex = TEX_BEETLE_LEFT_1 + nAnimationCount % 8;     // Käfer links
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
                     uTextureIndex = TEX_MOLE_01 + Playfield.uFrameCounter % 11;     // Mole links
                    if (uAnimStatus == EMERALD_ANIM_UP) {
                        nYoffs = - nAnimationCount * 2;
                        // Molen-Erde
                        bExtendedElement = true;
                        uTextureIndex_0 = TEX_EARTH;
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
                     uTextureIndex = TEX_MOLE_01 + Playfield.uFrameCounter % 11;     // Mole links
                    if (uAnimStatus == EMERALD_ANIM_RIGHT) {
                        nXoffs = nAnimationCount * 2;
                        // Molen-Erde
                        bExtendedElement = true;
                        uTextureIndex_0 = TEX_EARTH;
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
                    uTextureIndex = TEX_MOLE_01 + Playfield.uFrameCounter % 11;     // Mole links
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
                        // Molen-Erde
                        bExtendedElement = true;
                        uTextureIndex_0 = TEX_EARTH;
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
                     uTextureIndex = TEX_MOLE_01 + Playfield.uFrameCounter % 11;     // Mole links
                    if (uAnimStatus == EMERALD_ANIM_LEFT) {
                        nXoffs = -nAnimationCount * 2;
                        // Molen-Erde
                        bExtendedElement = true;
                        uTextureIndex_0 = TEX_EARTH;
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
                        uTextureIndex = TEX_MINE_LEFT_OFF;    // Mine links
                    } else {
                        uTextureIndex = TEX_MINE_LEFT_ON;     // Mine links an
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
                        uTextureIndex = TEX_MINE_LEFT_OFF;     // Mine links
                    } else {
                        uTextureIndex = TEX_MINE_LEFT_ON;     // Mine links an
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
                        uTextureIndex = TEX_MINE_LEFT_OFF;     // Mine links
                    } else {
                        uTextureIndex = TEX_MINE_LEFT_ON;     // Mine links an
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
                        uTextureIndex = TEX_MINE_LEFT_OFF;     // Mine links
                    } else {
                        uTextureIndex = TEX_MINE_LEFT_ON;     // Mine links an
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
                case (EMERALD_SPACE):
                case (EMERALD_DOOR_EMERALD):
                case (EMERALD_DOOR_MULTICOLOR):
                case (EMERALD_DOOR_ONLY_UP_STEEL):
                case (EMERALD_DOOR_ONLY_DOWN_STEEL):
                case (EMERALD_DOOR_ONLY_LEFT_STEEL):
                case (EMERALD_DOOR_ONLY_RIGHT_STEEL):
                case (EMERALD_DOOR_ONLY_UP_WALL):
                case (EMERALD_DOOR_ONLY_DOWN_WALL):
                case (EMERALD_DOOR_ONLY_LEFT_WALL):
                case (EMERALD_DOOR_ONLY_RIGHT_WALL):
                case (EMERALD_DOOR_WHITE):
                case (EMERALD_DOOR_WHITE_WOOD):
                case (EMERALD_DOOR_RED_WOOD):     // rote Holztür
                case (EMERALD_DOOR_YELLOW_WOOD):  // gelbe Holztür
                case (EMERALD_DOOR_BLUE_WOOD):    // blaue Holztür
                case (EMERALD_DOOR_GREEN_WOOD):   // grüne Holztür
                case (EMERALD_DOOR_RED):
                case (EMERALD_DOOR_YELLOW):
                case (EMERALD_DOOR_BLUE):
                case (EMERALD_DOOR_GREEN):
                case (EMERALD_DOOR_END_NOT_READY_STEEL):
                case (EMERALD_ACIDPOOL_TOP_LEFT):
                case (EMERALD_ACIDPOOL_TOP_RIGHT):
                case (EMERALD_ACIDPOOL_BOTTOM_LEFT):
                case (EMERALD_ACIDPOOL_BOTTOM_MID):
                case (EMERALD_ACIDPOOL_BOTTOM_RIGHT):
                case (EMERALD_LIGHT_SWITCH):
                case (EMERALD_MAGIC_WALL_SWITCH):
                case (EMERALD_BEAM_RED_VERTICAL):
                case (EMERALD_BEAM_RED_HORIZONTAL):
                case (EMERALD_BEAM_GREEN_VERTICAL):
                case (EMERALD_BEAM_GREEN_HORIZONTAL):
                case (EMERALD_BEAM_BLUE_VERTICAL):
                case (EMERALD_BEAM_BLUE_HORIZONTAL):
                case (EMERALD_BEAM_YELLOW_VERTICAL):
                case (EMERALD_BEAM_YELLOW_HORIZONTAL):
                case (EMERALD_LEVELEDITOR_MESSAGE_1_4):
                case (EMERALD_LEVELEDITOR_MESSAGE_2_4):
                case (EMERALD_LEVELEDITOR_MESSAGE_3_4):
                case (EMERALD_LEVELEDITOR_MESSAGE_4_4):
                case (EMERALD_WALL_CORNERED):
                case (EMERALD_WALL_ROUND):
                case (EMERALD_WALL_ROUND_PIKE):
                case (EMERALD_WALL_WITH_TIME_COIN):
                case (EMERALD_WALL_WITH_SHIELD_COIN):
                case (EMERALD_WALL_WITH_EMERALD):
                case (EMERALD_WALL_WITH_RUBY):
                case (EMERALD_WALL_WITH_SAPPHIRE):
                case (EMERALD_WALL_WITH_PERL):
                case (EMERALD_WALL_WITH_CRYSTAL):
                case (EMERALD_WALL_WITH_KEY_RED):
                case (EMERALD_WALL_WITH_KEY_YELLOW):
                case (EMERALD_WALL_WITH_KEY_GREEN):
                case (EMERALD_WALL_WITH_KEY_BLUE):
                case (EMERALD_WALL_WITH_KEY_WHITE):
                case (EMERALD_WALL_WITH_KEY_GENERAL):
                case (EMERALD_WALL_WITH_BOMB):
                case (EMERALD_WALL_WITH_MEGABOMB):
                case (EMERALD_WALL_WITH_STONE):
                case (EMERALD_WALL_WITH_NUT):
                case (EMERALD_WALL_WITH_WHEEL):
                case (EMERALD_WALL_WITH_DYNAMITE):
                case (EMERALD_WALL_WITH_ENDDOOR):
                case (EMERALD_WALL_WITH_ENDDOOR_READY):
                case (EMERALD_WALL_WITH_MINE_UP):
                case (EMERALD_WALL_WITH_BEETLE_UP):
                case (EMERALD_WALL_WITH_YAM):
                case (EMERALD_WALL_WITH_SLIME):
                case (EMERALD_WALL_WITH_ALIEN):
                case (EMERALD_WALL_WITH_MOLE_UP):
                case (EMERALD_WALL_WITH_GREEN_CHEESE):
                case (EMERALD_WALL_WITH_YELLOW_CHEESE):
                case (EMERALD_STEEL):
                case (EMERALD_STEEL_ROUND):
                case (EMERALD_STEEL_ROUND_PIKE):
                case (EMERALD_STEEL_MODERN_LEFT_END):
                case (EMERALD_STEEL_MODERN_LEFT_RIGHT):
                case (EMERALD_STEEL_MODERN_RIGHT_END):
                case (EMERALD_STEEL_MODERN_UP_END):
                case (EMERALD_STEEL_MODERN_UP_DOWN):
                case (EMERALD_STEEL_MODERN_DOWN_END):
                case (EMERALD_STEEL_MODERN_MIDDLE):
                case (EMERALD_STEEL_STRIPE_LEFT_TOP):
                case (EMERALD_STEEL_STRIPE_TOP):
                case (EMERALD_STEEL_STRIPE_RIGHT_TOP):
                case (EMERALD_STEEL_STRIPE_LEFT):
                case (EMERALD_STEEL_STRIPE_RIGHT):
                case (EMERALD_STEEL_STRIPE_LEFT_BOTTOM):
                case (EMERALD_STEEL_STRIPE_BOTTOM):
                case (EMERALD_STEEL_STRIPE_RIGHT_BOTTOM):
                case (EMERALD_STEEL_HEART):
                case (EMERALD_STEEL_PLAYERHEAD):
                case (EMERALD_STEEL_PLAYERHEAD_2):
                case (EMERALD_STEEL_NO_ENTRY):
                case (EMERALD_STEEL_GIVE_WAY):
                case (EMERALD_STEEL_YING):
                case (EMERALD_STEEL_WHEELCHAIR):
                case (EMERALD_STEEL_ARROW_DOWN):
                case (EMERALD_STEEL_ARROW_UP):
                case (EMERALD_STEEL_ARROW_LEFT):
                case (EMERALD_STEEL_ARROW_RIGHT):
                case (EMERALD_STEEL_TRASHCAN):
                case (EMERALD_STEEL_JOYSTICK):
                case (EMERALD_STEEL_EDIT_LEVEL):
                case (EMERALD_STEEL_MOVE_LEVEL):
                case (EMERALD_STEEL_ADD_LEVELGROUP):
                case (EMERALD_STEEL_COPY_LEVEL):
                case (EMERALD_STEEL_CLIPBOARD_LEVEL):
                case (EMERALD_STEEL_DC3_IMPORT):
                case (EMERALD_STEEL_RENAME_LEVELGROUP):
                case (EMERALD_STEEL_PASSWORD):
                case (EMERALD_STEEL_FORBIDDEN):
                case (EMERALD_STEEL_EXIT):
                case (EMERALD_STEEL_RADIOACTIVE):
                case (EMERALD_STEEL_EXPLOSION):
                case (EMERALD_STEEL_ACID):
                case (EMERALD_STEEL_NOT_ROUND):
                case (EMERALD_WALL_NOT_ROUND):
                case (EMERALD_STEEL_PARKING):
                case (EMERALD_STEEL_STOP):
                case (EMERALD_STEEL_DEADEND):
                case (EMERALD_STEEL_BIOHAZARD):
                case (EMERALD_STEEL_WARNING):
                case (EMERALD_FONT_BLUE_EXCLAMATION):
                case (EMERALD_FONT_BLUE_ARROW_RIGHT):
                case (EMERALD_FONT_BLUE_ARROW_UP):
                case (EMERALD_FONT_BLUE_ARROW_DOWN):
                case (EMERALD_FONT_BLUE_APOSTROPHE):
                case (EMERALD_FONT_BLUE_BRACE_OPEN):
                case (EMERALD_FONT_BLUE_BRACE_CLOSE):
                case (EMERALD_FONT_BLUE_COPYRIGHT):
                case (EMERALD_FONT_BLUE_PLUS):
                case (EMERALD_FONT_BLUE_COMMA):
                case (EMERALD_FONT_BLUE_MINUS):
                case (EMERALD_FONT_BLUE_DOT):
                case (EMERALD_FONT_BLUE_SLASH):
                case (EMERALD_FONT_BLUE_DOUBLE_QUOTE):
                case (EMERALD_FONT_BLUE_SEMICOLON):
                case (EMERALD_FONT_BLUE_0):
                case (EMERALD_FONT_BLUE_1):
                case (EMERALD_FONT_BLUE_2):
                case (EMERALD_FONT_BLUE_3):
                case (EMERALD_FONT_BLUE_4):
                case (EMERALD_FONT_BLUE_5):
                case (EMERALD_FONT_BLUE_6):
                case (EMERALD_FONT_BLUE_7):
                case (EMERALD_FONT_BLUE_8):
                case (EMERALD_FONT_BLUE_9):
                case (EMERALD_FONT_BLUE_DOUBLE_DOT):
                case (EMERALD_FONT_BLUE_PLATE):
                case (EMERALD_FONT_BLUE_ARROW_LEFT):
                case (EMERALD_FONT_BLUE_QUESTION_MARK):
                case (EMERALD_FONT_BLUE_A):
                case (EMERALD_FONT_BLUE_B):
                case (EMERALD_FONT_BLUE_C):
                case (EMERALD_FONT_BLUE_D):
                case (EMERALD_FONT_BLUE_E):
                case (EMERALD_FONT_BLUE_F):
                case (EMERALD_FONT_BLUE_G):
                case (EMERALD_FONT_BLUE_H):
                case (EMERALD_FONT_BLUE_I):
                case (EMERALD_FONT_BLUE_J):
                case (EMERALD_FONT_BLUE_K):
                case (EMERALD_FONT_BLUE_L):
                case (EMERALD_FONT_BLUE_M):
                case (EMERALD_FONT_BLUE_N):
                case (EMERALD_FONT_BLUE_O):
                case (EMERALD_FONT_BLUE_P):
                case (EMERALD_FONT_BLUE_Q):
                case (EMERALD_FONT_BLUE_R):
                case (EMERALD_FONT_BLUE_S):
                case (EMERALD_FONT_BLUE_T):
                case (EMERALD_FONT_BLUE_U):
                case (EMERALD_FONT_BLUE_V):
                case (EMERALD_FONT_BLUE_W):
                case (EMERALD_FONT_BLUE_X):
                case (EMERALD_FONT_BLUE_Y):
                case (EMERALD_FONT_BLUE_Z):
                case (EMERALD_FONT_BLUE_AE):
                case (EMERALD_FONT_BLUE_OE):
                case (EMERALD_FONT_BLUE_UE):
                case (EMERALD_FONT_BLUE_STEEL_EXCLAMATION):
                case (EMERALD_FONT_BLUE_STEEL_ARROW_RIGHT):
                case (EMERALD_FONT_BLUE_STEEL_ARROW_UP):
                case (EMERALD_FONT_BLUE_STEEL_ARROW_DOWN):
                case (EMERALD_FONT_BLUE_STEEL_APOSTROPHE):
                case (EMERALD_FONT_BLUE_STEEL_BRACE_OPEN):
                case (EMERALD_FONT_BLUE_STEEL_BRACE_CLOSE):
                case (EMERALD_FONT_BLUE_STEEL_COPYRIGHT):
                case (EMERALD_FONT_BLUE_STEEL_PLUS):
                case (EMERALD_FONT_BLUE_STEEL_COMMA):
                case (EMERALD_FONT_BLUE_STEEL_MINUS):
                case (EMERALD_FONT_BLUE_STEEL_DOT):
                case (EMERALD_FONT_BLUE_STEEL_SLASH):
                case (EMERALD_FONT_BLUE_STEEL_DOUBLE_QUOTE):
                case (EMERALD_FONT_BLUE_STEEL_SEMICOLON):
                case (EMERALD_FONT_BLUE_STEEL_0):
                case (EMERALD_FONT_BLUE_STEEL_1):
                case (EMERALD_FONT_BLUE_STEEL_2):
                case (EMERALD_FONT_BLUE_STEEL_3):
                case (EMERALD_FONT_BLUE_STEEL_4):
                case (EMERALD_FONT_BLUE_STEEL_5):
                case (EMERALD_FONT_BLUE_STEEL_6):
                case (EMERALD_FONT_BLUE_STEEL_7):
                case (EMERALD_FONT_BLUE_STEEL_8):
                case (EMERALD_FONT_BLUE_STEEL_9):
                case (EMERALD_FONT_BLUE_STEEL_DOUBLE_DOT):
                case (EMERALD_FONT_BLUE_STEEL_PLATE):
                case (EMERALD_FONT_BLUE_STEEL_ARROW_LEFT):
                case (EMERALD_FONT_BLUE_STEEL_QUESTION_MARK):
                case (EMERALD_FONT_BLUE_STEEL_A):
                case (EMERALD_FONT_BLUE_STEEL_B):
                case (EMERALD_FONT_BLUE_STEEL_C):
                case (EMERALD_FONT_BLUE_STEEL_D):
                case (EMERALD_FONT_BLUE_STEEL_E):
                case (EMERALD_FONT_BLUE_STEEL_F):
                case (EMERALD_FONT_BLUE_STEEL_G):
                case (EMERALD_FONT_BLUE_STEEL_H):
                case (EMERALD_FONT_BLUE_STEEL_I):
                case (EMERALD_FONT_BLUE_STEEL_J):
                case (EMERALD_FONT_BLUE_STEEL_K):
                case (EMERALD_FONT_BLUE_STEEL_L):
                case (EMERALD_FONT_BLUE_STEEL_M):
                case (EMERALD_FONT_BLUE_STEEL_N):
                case (EMERALD_FONT_BLUE_STEEL_O):
                case (EMERALD_FONT_BLUE_STEEL_P):
                case (EMERALD_FONT_BLUE_STEEL_Q):
                case (EMERALD_FONT_BLUE_STEEL_R):
                case (EMERALD_FONT_BLUE_STEEL_S):
                case (EMERALD_FONT_BLUE_STEEL_T):
                case (EMERALD_FONT_BLUE_STEEL_U):
                case (EMERALD_FONT_BLUE_STEEL_V):
                case (EMERALD_FONT_BLUE_STEEL_W):
                case (EMERALD_FONT_BLUE_STEEL_X):
                case (EMERALD_FONT_BLUE_STEEL_Y):
                case (EMERALD_FONT_BLUE_STEEL_Z):
                case (EMERALD_FONT_BLUE_STEEL_AE):
                case (EMERALD_FONT_BLUE_STEEL_OE):
                case (EMERALD_FONT_BLUE_STEEL_UE):
                case (EMERALD_FONT_GREEN_EXCLAMATION):
                case (EMERALD_FONT_GREEN_ARROW_RIGHT):
                case (EMERALD_FONT_GREEN_ARROW_UP):
                case (EMERALD_FONT_GREEN_ARROW_DOWN):
                case (EMERALD_FONT_GREEN_APOSTROPHE):
                case (EMERALD_FONT_GREEN_BRACE_OPEN):
                case (EMERALD_FONT_GREEN_BRACE_CLOSE):
                case (EMERALD_FONT_GREEN_COPYRIGHT):
                case (EMERALD_FONT_GREEN_PLUS):
                case (EMERALD_FONT_GREEN_COMMA):
                case (EMERALD_FONT_GREEN_MINUS):
                case (EMERALD_FONT_GREEN_DOT):
                case (EMERALD_FONT_GREEN_SLASH):
                case (EMERALD_FONT_GREEN_DOUBLE_QUOTE):
                case (EMERALD_FONT_GREEN_SEMICOLON):
                case (EMERALD_FONT_GREEN_0):
                case (EMERALD_FONT_GREEN_1):
                case (EMERALD_FONT_GREEN_2):
                case (EMERALD_FONT_GREEN_3):
                case (EMERALD_FONT_GREEN_4):
                case (EMERALD_FONT_GREEN_5):
                case (EMERALD_FONT_GREEN_6):
                case (EMERALD_FONT_GREEN_7):
                case (EMERALD_FONT_GREEN_8):
                case (EMERALD_FONT_GREEN_9):
                case (EMERALD_FONT_GREEN_DOUBLE_DOT):
                case (EMERALD_FONT_GREEN_PLATE):
                case (EMERALD_FONT_GREEN_ARROW_LEFT):
                case (EMERALD_FONT_GREEN_QUESTION_MARK):
                case (EMERALD_FONT_GREEN_A):
                case (EMERALD_FONT_GREEN_B):
                case (EMERALD_FONT_GREEN_C):
                case (EMERALD_FONT_GREEN_D):
                case (EMERALD_FONT_GREEN_E):
                case (EMERALD_FONT_GREEN_F):
                case (EMERALD_FONT_GREEN_G):
                case (EMERALD_FONT_GREEN_H):
                case (EMERALD_FONT_GREEN_I):
                case (EMERALD_FONT_GREEN_J):
                case (EMERALD_FONT_GREEN_K):
                case (EMERALD_FONT_GREEN_L):
                case (EMERALD_FONT_GREEN_M):
                case (EMERALD_FONT_GREEN_N):
                case (EMERALD_FONT_GREEN_O):
                case (EMERALD_FONT_GREEN_P):
                case (EMERALD_FONT_GREEN_Q):
                case (EMERALD_FONT_GREEN_R):
                case (EMERALD_FONT_GREEN_S):
                case (EMERALD_FONT_GREEN_T):
                case (EMERALD_FONT_GREEN_U):
                case (EMERALD_FONT_GREEN_V):
                case (EMERALD_FONT_GREEN_W):
                case (EMERALD_FONT_GREEN_X):
                case (EMERALD_FONT_GREEN_Y):
                case (EMERALD_FONT_GREEN_Z):
                case (EMERALD_FONT_GREEN_AE):
                case (EMERALD_FONT_GREEN_OE):
                case (EMERALD_FONT_GREEN_UE):
                case (EMERALD_FONT_GREEN_STEEL_EXCLAMATION):
                case (EMERALD_FONT_GREEN_STEEL_ARROW_RIGHT):
                case (EMERALD_FONT_GREEN_STEEL_ARROW_UP):
                case (EMERALD_FONT_GREEN_STEEL_ARROW_DOWN):
                case (EMERALD_FONT_GREEN_STEEL_APOSTROPHE):
                case (EMERALD_FONT_GREEN_STEEL_BRACE_OPEN):
                case (EMERALD_FONT_GREEN_STEEL_BRACE_CLOSE):
                case (EMERALD_FONT_GREEN_STEEL_COPYRIGHT):
                case (EMERALD_FONT_GREEN_STEEL_PLUS):
                case (EMERALD_FONT_GREEN_STEEL_COMMA):
                case (EMERALD_FONT_GREEN_STEEL_MINUS):
                case (EMERALD_FONT_GREEN_STEEL_DOT):
                case (EMERALD_FONT_GREEN_STEEL_SLASH):
                case (EMERALD_FONT_GREEN_STEEL_DOUBLE_QUOTE):
                case (EMERALD_FONT_GREEN_STEEL_SEMICOLON):
                case (EMERALD_FONT_GREEN_STEEL_0):
                case (EMERALD_FONT_GREEN_STEEL_1):
                case (EMERALD_FONT_GREEN_STEEL_2):
                case (EMERALD_FONT_GREEN_STEEL_3):
                case (EMERALD_FONT_GREEN_STEEL_4):
                case (EMERALD_FONT_GREEN_STEEL_5):
                case (EMERALD_FONT_GREEN_STEEL_6):
                case (EMERALD_FONT_GREEN_STEEL_7):
                case (EMERALD_FONT_GREEN_STEEL_8):
                case (EMERALD_FONT_GREEN_STEEL_9):
                case (EMERALD_FONT_GREEN_STEEL_DOUBLE_DOT):
                case (EMERALD_FONT_GREEN_STEEL_PLATE):
                case (EMERALD_FONT_GREEN_STEEL_ARROW_LEFT):
                case (EMERALD_FONT_GREEN_STEEL_QUESTION_MARK):
                case (EMERALD_FONT_GREEN_STEEL_A):
                case (EMERALD_FONT_GREEN_STEEL_B):
                case (EMERALD_FONT_GREEN_STEEL_C):
                case (EMERALD_FONT_GREEN_STEEL_D):
                case (EMERALD_FONT_GREEN_STEEL_E):
                case (EMERALD_FONT_GREEN_STEEL_F):
                case (EMERALD_FONT_GREEN_STEEL_G):
                case (EMERALD_FONT_GREEN_STEEL_H):
                case (EMERALD_FONT_GREEN_STEEL_I):
                case (EMERALD_FONT_GREEN_STEEL_J):
                case (EMERALD_FONT_GREEN_STEEL_K):
                case (EMERALD_FONT_GREEN_STEEL_L):
                case (EMERALD_FONT_GREEN_STEEL_M):
                case (EMERALD_FONT_GREEN_STEEL_N):
                case (EMERALD_FONT_GREEN_STEEL_O):
                case (EMERALD_FONT_GREEN_STEEL_P):
                case (EMERALD_FONT_GREEN_STEEL_Q):
                case (EMERALD_FONT_GREEN_STEEL_R):
                case (EMERALD_FONT_GREEN_STEEL_S):
                case (EMERALD_FONT_GREEN_STEEL_T):
                case (EMERALD_FONT_GREEN_STEEL_U):
                case (EMERALD_FONT_GREEN_STEEL_V):
                case (EMERALD_FONT_GREEN_STEEL_W):
                case (EMERALD_FONT_GREEN_STEEL_X):
                case (EMERALD_FONT_GREEN_STEEL_Y):
                case (EMERALD_FONT_GREEN_STEEL_Z):
                case (EMERALD_FONT_GREEN_STEEL_AE):
                case (EMERALD_FONT_GREEN_STEEL_OE):
                case (EMERALD_FONT_GREEN_STEEL_UE):
                    uTextureIndex = GetTextureIndexByElement(uLevelElement,0,NULL);
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
                            uTextureIndex = TEX_SPACE;      // Space
                            fAngle = 0;
                        }
                    } else {
                        uTextureIndex = TEX_SPACE;      // Space
                        fAngle = 0;
                    }
                    break;
                default:
                    fAngle = 0;
                    uTextureIndex = TEX_SPACE;      // Space
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
            uTextureIndex = TEX_SPACE;
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
