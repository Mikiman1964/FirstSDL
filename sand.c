#include "EmeraldMine.h"
#include "sand.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlSand
------------------------------------------------------------------------------
Beschreibung: Überprüft die freien Ränder eines Sand-Stückes und gibt diesem
              Element den richtigen Zustand.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSand(uint32_t I) {
    uint16_t uLeftRightUpDownSand = 0;       // Bit 3 = links hat Sand, Bit 2 = rechts hat Sand, Bit 1 = oben hat Sand, Bit 0 = unten hat Sand

    if ((Playfield.pStatusAnimation[I] & 0xFF000000) != EMERALD_ANIM_SAND_SHRINK) {     // Sand entfernen bzw. wegschnippen hat Vorrang
        if ((Playfield.pLevel[I - 1] == EMERALD_SAND)) uLeftRightUpDownSand |= 0x08;    // Bit 3 setzen
        if ((Playfield.pLevel[I + 1] == EMERALD_SAND)) uLeftRightUpDownSand |= 0x04;    // Bit 2 setzen
        if ((Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SAND)) uLeftRightUpDownSand |= 0x02;    // Bit 1 setzen
        if ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SAND)) uLeftRightUpDownSand |= 0x01;    // Bit 0 setzen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SAND_0 + (uLeftRightUpDownSand << 8);
    }
}
