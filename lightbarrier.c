#include "EmeraldMine.h"
#include "lightbarrier.h"


extern PLAYFIELD Playfield;

uint8_t ge_uBeamColors[] = {
//                            R    G    B
                            0xFF,0x00,0x00,         // rot
                            0x00,0xEF,0x00,         // grün
                            0x0A,0x5A,0xFF,         // blau
                            0xFF,0xFF,0x00          // gelb
                            };


/*----------------------------------------------------------------------------
Name:           CleanLightBarriers
------------------------------------------------------------------------------
Beschreibung: Wandelt alle Lichtstrahlen in Space um;
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CleanLightBarriers(void) {
    uint32_t I;

    for (I = 0; I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension; I++) {
        if ((Playfield.pLevel[I] >= EMERALD_BEAM_RED_VERTICAL) && (Playfield.pLevel[I] <= EMERALD_BEAM_CROSS)) {
            Playfield.pLevel[I] = EMERALD_SPACE;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlLightBarriers
------------------------------------------------------------------------------
Beschreibung: Steuert die Lichtschranken.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlLightBarriers(void) {
    uint32_t I;

    CleanLightBarriers();
    for (I = 0; I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension; I++) {
        switch(Playfield.pLevel[I]) {
            case (EMERALD_LIGHTBARRIER_RED_UP):
                if (Playfield.bLightBarrierRedOn) {
                    ControlBarrierUp(I,EMERALD_BARRIER_COLOR_RED);
                }
                break;
            case (EMERALD_LIGHTBARRIER_RED_DOWN):
                if (Playfield.bLightBarrierRedOn) {
                    ControlBarrierDown(I,EMERALD_BARRIER_COLOR_RED);
                }
                break;
            case (EMERALD_LIGHTBARRIER_RED_LEFT):
                if (Playfield.bLightBarrierRedOn) {
                    ControlBarrierLeft(I,EMERALD_BARRIER_COLOR_RED);
                }
                break;
            case (EMERALD_LIGHTBARRIER_RED_RIGHT):
                if (Playfield.bLightBarrierRedOn) {
                    ControlBarrierRight(I,EMERALD_BARRIER_COLOR_RED);
                }
                break;
            case (EMERALD_LIGHTBARRIER_GREEN_UP):
                if (Playfield.bLightBarrierGreenOn) {
                    ControlBarrierUp(I,EMERALD_BARRIER_COLOR_GREEN);
                }
                break;
            case (EMERALD_LIGHTBARRIER_GREEN_DOWN):
                if (Playfield.bLightBarrierGreenOn) {
                    ControlBarrierDown(I,EMERALD_BARRIER_COLOR_GREEN);
                }
                break;
            case (EMERALD_LIGHTBARRIER_GREEN_LEFT):
                if (Playfield.bLightBarrierGreenOn) {
                    ControlBarrierLeft(I,EMERALD_BARRIER_COLOR_GREEN);
                }
                break;
            case (EMERALD_LIGHTBARRIER_GREEN_RIGHT):
                if (Playfield.bLightBarrierGreenOn) {
                    ControlBarrierRight(I,EMERALD_BARRIER_COLOR_GREEN);
                }
                break;
            case (EMERALD_LIGHTBARRIER_BLUE_UP):
                if (Playfield.bLightBarrierBlueOn) {
                    ControlBarrierUp(I,EMERALD_BARRIER_COLOR_BLUE);
                }
                break;
            case (EMERALD_LIGHTBARRIER_BLUE_DOWN):
                if (Playfield.bLightBarrierBlueOn) {
                    ControlBarrierDown(I,EMERALD_BARRIER_COLOR_BLUE);
                }
                break;
            case (EMERALD_LIGHTBARRIER_BLUE_LEFT):
                if (Playfield.bLightBarrierBlueOn) {
                    ControlBarrierLeft(I,EMERALD_BARRIER_COLOR_BLUE);
                }
                break;
            case (EMERALD_LIGHTBARRIER_BLUE_RIGHT):
                if (Playfield.bLightBarrierBlueOn) {
                    ControlBarrierRight(I,EMERALD_BARRIER_COLOR_BLUE);
                }
                break;
            case (EMERALD_LIGHTBARRIER_YELLOW_UP):
                if (Playfield.bLightBarrierYellowOn) {
                    ControlBarrierUp(I,EMERALD_BARRIER_COLOR_YELLOW);
                }
                break;
            case (EMERALD_LIGHTBARRIER_YELLOW_DOWN):
                if (Playfield.bLightBarrierYellowOn) {
                    ControlBarrierDown(I,EMERALD_BARRIER_COLOR_YELLOW);
                }
                break;
            case (EMERALD_LIGHTBARRIER_YELLOW_LEFT):
                if (Playfield.bLightBarrierYellowOn) {
                    ControlBarrierLeft(I,EMERALD_BARRIER_COLOR_YELLOW);
                }
                break;
            case (EMERALD_LIGHTBARRIER_YELLOW_RIGHT):
                if (Playfield.bLightBarrierYellowOn) {
                    ControlBarrierRight(I,EMERALD_BARRIER_COLOR_YELLOW);
                }
                break;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlBarrierUp
------------------------------------------------------------------------------
Beschreibung: Steuert eine Lichtschranke, die nach oben strahlt.
Parameter
      Eingang: I, uint32_t, Index im Level
               uColor, uint32_t, Farbe der Lichtschranke, siehe lightbarrier.h
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlBarrierUp(uint32_t I, uint32_t uColor) {
    uint16_t uElement;
    bool bGoOn;

    uElement = EMERALD_SPACE;
    switch (uColor) {
        case (EMERALD_BARRIER_COLOR_RED):
            uElement = EMERALD_BEAM_RED_VERTICAL;
            break;
        case (EMERALD_BARRIER_COLOR_GREEN):
            uElement = EMERALD_BEAM_GREEN_VERTICAL;
            break;
        case (EMERALD_BARRIER_COLOR_BLUE):
            uElement = EMERALD_BEAM_BLUE_VERTICAL;
            break;
        case (EMERALD_BARRIER_COLOR_YELLOW):
            uElement = EMERALD_BEAM_YELLOW_VERTICAL;
            break;
    }
    // Falls Lichtschranke bereits von einer nach unten strahlenden Lichschranke beschienen wird, dann kann diese Lichtschranke nicht mehr strahlen.
    if ( (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] != EMERALD_BEAM_RED_VERTICAL) && (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] != EMERALD_BEAM_YELLOW_VERTICAL) &&
         (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] != EMERALD_BEAM_GREEN_VERTICAL) && (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] != EMERALD_BEAM_BLUE_VERTICAL) ) {
        do {
            I = I - Playfield.uLevel_X_Dimension;
            bGoOn = (HandleBeam(I,uElement,uColor) != 0);
        } while (bGoOn);
    }
 }


/*----------------------------------------------------------------------------
Name:           ControlBarrierDown
------------------------------------------------------------------------------
Beschreibung: Steuert eine Lichtschranke, die nach unten strahlt.
Parameter
      Eingang: I, uint32_t, Index im Level
               uColor, uint32_t, Farbe der Lichtschranke, siehe lightbarrier.h
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlBarrierDown(uint32_t I, uint32_t uColor) {
    uint16_t uElement;
    bool bGoOn;

    uElement = EMERALD_SPACE;
    switch (uColor) {
        case (EMERALD_BARRIER_COLOR_RED):
            uElement = EMERALD_BEAM_RED_VERTICAL;
            break;
        case (EMERALD_BARRIER_COLOR_GREEN):
            uElement = EMERALD_BEAM_GREEN_VERTICAL;
            break;
        case (EMERALD_BARRIER_COLOR_BLUE):
            uElement = EMERALD_BEAM_BLUE_VERTICAL;
            break;
        case (EMERALD_BARRIER_COLOR_YELLOW):
            uElement = EMERALD_BEAM_YELLOW_VERTICAL;
            break;
    }
    do {
        I = I + Playfield.uLevel_X_Dimension;
        bGoOn = (HandleBeam(I,uElement,uColor) != 0);
    } while (bGoOn);
}


/*----------------------------------------------------------------------------
Name:           ControlBarrierLeft
------------------------------------------------------------------------------
Beschreibung: Steuert eine Lichtschranke, die nach links strahlt.
Parameter
      Eingang: I, uint32_t, Index im Level
               uColor, uint32_t, Farbe der Lichtschranke, siehe lightbarrier.h
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlBarrierLeft(uint32_t I, uint32_t uColor) {
    uint16_t uElement;
    bool bGoOn;

    uElement = EMERALD_SPACE;
    switch (uColor) {
        case (EMERALD_BARRIER_COLOR_RED):
            uElement = EMERALD_BEAM_RED_HORIZONTAL;
            break;
        case (EMERALD_BARRIER_COLOR_GREEN):
            uElement = EMERALD_BEAM_GREEN_HORIZONTAL;
            break;
        case (EMERALD_BARRIER_COLOR_BLUE):
            uElement = EMERALD_BEAM_BLUE_HORIZONTAL;
            break;
        case (EMERALD_BARRIER_COLOR_YELLOW):
            uElement = EMERALD_BEAM_YELLOW_HORIZONTAL;
            break;
    }
    // Falls Lichtschranke bereits von einer nach rechts strahlenden Lichschranke beschienen wird, dann kann diese Lichtschranke nicht mehr strahlen.
    if ( (Playfield.pLevel[I - 1] != EMERALD_BEAM_RED_HORIZONTAL) && (Playfield.pLevel[I - 1] != EMERALD_BEAM_YELLOW_HORIZONTAL) &&
         (Playfield.pLevel[I - 1] != EMERALD_BEAM_GREEN_HORIZONTAL) && (Playfield.pLevel[I - 1] != EMERALD_BEAM_BLUE_HORIZONTAL) ) {
        do {
            I--;
            bGoOn = (HandleBeam(I,uElement,uColor) != 0);
        } while (bGoOn);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlBarrierRight
------------------------------------------------------------------------------
Beschreibung: Steuert eine Lichtschranke, die nach rechts strahlt.
Parameter
      Eingang: I, uint32_t, Index im Level
               uColor, uint32_t, Farbe der Lichtschranke, siehe lightbarrier.h
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlBarrierRight(uint32_t I, uint32_t uColor) {
    uint16_t uElement;
    bool bGoOn;

    uElement = EMERALD_SPACE;
    switch (uColor) {
        case (EMERALD_BARRIER_COLOR_RED):
            uElement = EMERALD_BEAM_RED_HORIZONTAL;
            break;
        case (EMERALD_BARRIER_COLOR_GREEN):
            uElement = EMERALD_BEAM_GREEN_HORIZONTAL;
            break;
        case (EMERALD_BARRIER_COLOR_BLUE):
            uElement = EMERALD_BEAM_BLUE_HORIZONTAL;
            break;
        case (EMERALD_BARRIER_COLOR_YELLOW):
            uElement = EMERALD_BEAM_YELLOW_HORIZONTAL;
            break;
    }
    do {
        I++;
        bGoOn = (HandleBeam(I,uElement,uColor) != 0);
    } while (bGoOn);
}


/*----------------------------------------------------------------------------
Name:           HandleBeam
------------------------------------------------------------------------------
Beschreibung: Prüft, ob an der Position I der eigene Lichtstrahl fortgesetzt werden kann.
              Fall ja, wird die Position mit uOwnElement besetzt und die Funktion gibt > 0 zurück.
              Ist an dieser Stelle bereits ein Lichtstrahl wird dort ein Lichtkreuz gesetzt und
              ebenfalls > 0 zurückgegeben.
Parameter
      Eingang: I, uint32_t, lineare Position, die im Level geprüft/gesetzt werden soll
               uOwnElement, uint16_t, Element, das fortgesetzt werden soll.
               uColor, uint32_t, Farbe des Lichtstrahls, der fortgesetzt werden soll (siehe lightbarrier.h)
      Ausgang: -
Rückgabewert:  uint32_t, >0 = Strahl konnte weitergeführt werden, 0 = Strahl konnte nicht weitergeführt werden.
Seiteneffekte: -
------------------------------------------------------------------------------*/
uint32_t HandleBeam(uint32_t I, uint16_t uOwnElement,uint32_t uOwnColor) {
    uint16_t uLevelElement;
    uint32_t uRet;

    // Hinweis: Beim Kreuz-Element ist StatusAnimation wie folgt aufgebaut:
    // High-Nibble (Bit 7 - 4) = Farbe vertikal
    // Low-Nibble (Bit 3 - 0) = Farbe horizontal
    uLevelElement = Playfield.pLevel[I];
    switch (uLevelElement) {
        case (EMERALD_SPACE):
            Playfield.pLevel[I] = uOwnElement;
            uRet = 1;
            break;
        case (EMERALD_BEAM_RED_VERTICAL):
            Playfield.pLevel[I] = EMERALD_BEAM_CROSS;
            Playfield.pStatusAnimation[I] = (EMERALD_BARRIER_COLOR_RED << 4) | uOwnColor;
            uRet = 1;
            break;
        case (EMERALD_BEAM_RED_HORIZONTAL):
            Playfield.pLevel[I] = EMERALD_BEAM_CROSS;
            Playfield.pStatusAnimation[I] = (uOwnColor << 4) | EMERALD_BARRIER_COLOR_RED;
            uRet = 1;
            break;
        case (EMERALD_BEAM_GREEN_VERTICAL):
            Playfield.pLevel[I] = EMERALD_BEAM_CROSS;
            Playfield.pStatusAnimation[I] = (EMERALD_BARRIER_COLOR_GREEN << 4) | uOwnColor;
            uRet = 1;
            break;
        case (EMERALD_BEAM_GREEN_HORIZONTAL):
            Playfield.pLevel[I] = EMERALD_BEAM_CROSS;
            Playfield.pStatusAnimation[I] = (uOwnColor << 4) | EMERALD_BARRIER_COLOR_GREEN;
            uRet = 1;
            break;
        case (EMERALD_BEAM_BLUE_VERTICAL):
            Playfield.pLevel[I] = EMERALD_BEAM_CROSS;
            Playfield.pStatusAnimation[I] = (EMERALD_BARRIER_COLOR_BLUE << 4) | uOwnColor;
            uRet = 1;
            break;
        case (EMERALD_BEAM_BLUE_HORIZONTAL):
            Playfield.pLevel[I] = EMERALD_BEAM_CROSS;
            Playfield.pStatusAnimation[I] = (uOwnColor << 4) | EMERALD_BARRIER_COLOR_BLUE;
            uRet = 1;
            break;
        case (EMERALD_BEAM_YELLOW_VERTICAL):
            Playfield.pLevel[I] = EMERALD_BEAM_CROSS;
            Playfield.pStatusAnimation[I] = (EMERALD_BARRIER_COLOR_YELLOW << 4) | uOwnColor;
            uRet = 1;
            break;
        case (EMERALD_BEAM_YELLOW_HORIZONTAL):
            Playfield.pLevel[I] = EMERALD_BEAM_CROSS;
            Playfield.pStatusAnimation[I] = (uOwnColor << 4) | EMERALD_BARRIER_COLOR_YELLOW;
            uRet = 1;
            break;
        default:
            uRet = 0;
            break;
    }
    return uRet;
}
