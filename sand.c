#include "EmeraldMine.h"
#include "mystd.h"
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
        if ((Playfield.pLevel[I - 1] == EMERALD_SAND) || (Playfield.pLevel[I - 1] == EMERALD_SANDMINE)) uLeftRightUpDownSand |= 0x08;    // Bit 3 setzen
        if ((Playfield.pLevel[I + 1] == EMERALD_SAND) || (Playfield.pLevel[I + 1] == EMERALD_SANDMINE)) uLeftRightUpDownSand |= 0x04;    // Bit 2 setzen
        if ((Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SAND) || (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SANDMINE)) uLeftRightUpDownSand |= 0x02;    // Bit 1 setzen
        if ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SAND) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SANDMINE)) uLeftRightUpDownSand |= 0x01;    // Bit 0 setzen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SAND_0 + (uLeftRightUpDownSand << 8);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlSandInvisible
------------------------------------------------------------------------------
Beschreibung: Überprüft die freien Ränder eines unsichtbaren Sand-Stückes und gibt diesem
              Element den richtigen Zustand.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSandInvisible(uint32_t I) {
    uint16_t uLeftRightUpDownSand = 0;       // Bit 3 = links hat Sand, Bit 2 = rechts hat Sand, Bit 1 = oben hat Sand, Bit 0 = unten hat Sand

    if ((Playfield.pStatusAnimation[I] & 0xFF000000) != EMERALD_ANIM_SAND_INVISIBLE_SHRINK) {     // Sand entfernen bzw. wegschnippen hat Vorrang
        if ((Playfield.pLevel[I - 1] == EMERALD_SAND_INVISIBLE) || (Playfield.pLevel[I - 1] == EMERALD_SANDMINE)) uLeftRightUpDownSand |= 0x08;    // Bit 3 setzen
        if ((Playfield.pLevel[I + 1] == EMERALD_SAND_INVISIBLE) || (Playfield.pLevel[I + 1] == EMERALD_SANDMINE)) uLeftRightUpDownSand |= 0x04;    // Bit 2 setzen
        if ((Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SAND_INVISIBLE) || (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SANDMINE)) uLeftRightUpDownSand |= 0x02;    // Bit 1 setzen
        if ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SAND_INVISIBLE) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SANDMINE)) uLeftRightUpDownSand |= 0x01;    // Bit 0 setzen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SAND_0 + (uLeftRightUpDownSand << 8);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlSandMine
------------------------------------------------------------------------------
Beschreibung: Überprüft die freien Ränder eines Sandminen-Stückes und gibt diesem
              Element den richtigen Zustand.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSandMine(uint32_t I) {
    uint16_t uLeftRightUpDownSandMine = 0;       // Bit 3 = links hat Sandmine, Bit 2 = rechts hat Sandmine, Bit 1 = oben hat Sandmine, Bit 0 = unten hat Sandmine

    if ((Playfield.pLevel[I - 1] == EMERALD_SAND) || (Playfield.pLevel[I - 1] == EMERALD_SANDMINE)) uLeftRightUpDownSandMine |= 0x08;    // Bit 3 setzen
    if ((Playfield.pLevel[I + 1] == EMERALD_SAND) || (Playfield.pLevel[I + 1] == EMERALD_SANDMINE)) uLeftRightUpDownSandMine |= 0x04;    // Bit 2 setzen
    if ((Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SAND) || (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SANDMINE)) uLeftRightUpDownSandMine |= 0x02;    // Bit 1 setzen
    if ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SAND) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SANDMINE)) uLeftRightUpDownSandMine |= 0x01;    // Bit 0 setzen
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SAND_0 + (uLeftRightUpDownSandMine << 8);
}


/*----------------------------------------------------------------------------
Name:           ControlGrass
------------------------------------------------------------------------------
Beschreibung: Überprüft die freien Ränder eines Gras-Stückes und gibt diesem
              Element den richtigen Zustand.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlGrass(uint32_t I) {
    uint16_t uLeftRightUpDownGrass = 0;       // Bit 3 = links hat Gras, Bit 2 = rechts hat Gras, Bit 1 = oben hat Gras, Bit 0 = unten hat Gras
    int nRandom;
    int nLoops;
    int nDirectionRandom;

    if ((Playfield.pStatusAnimation[I] & 0xFF000000) != EMERALD_ANIM_GRASS_SHRINK) {     // Gras entfernen bzw. wegschnippen hat Vorrang
        if ((Playfield.pLevel[I - 1] == EMERALD_GRASS)) uLeftRightUpDownGrass |= 0x08;    // Bit 3 setzen
        if ((Playfield.pLevel[I + 1] == EMERALD_GRASS)) uLeftRightUpDownGrass |= 0x04;    // Bit 2 setzen
        if ((Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_GRASS)) uLeftRightUpDownGrass |= 0x02;    // Bit 1 setzen
        if ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_GRASS)) uLeftRightUpDownGrass |= 0x01;    // Bit 0 setzen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SAND_0 + (uLeftRightUpDownGrass << 8);
    }
    nRandom = randn(1,9990);       // Ergibt Zufallszahl zwischen 1-9990
    if (Playfield.uGrassSpreadSpeed >= nRandom) {
        if (Playfield.uGrassSpreadSpeed > 9000) {
            nLoops = 3;
        } else if (Playfield.uGrassSpreadSpeed > 8000) {
            nLoops = 2;
        } else {
            nLoops = 1;
        }
        do {
            nDirectionRandom = randn(1,4);       // Ergibt Zufallszahl zwischen 1-4  (1 = links, 2 = oben, 3 = rechts, 4 = unten)
            switch (nDirectionRandom) {
                case (1):           // links prüfen
                    if (Playfield.pLevel[I - 1] == EMERALD_SPACE) {
                        Playfield.pLevel[I - 1] = EMERALD_GRASS_COMES;
                        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_STAND;
                    }
                    break;
                case (2):           // oben prüfen
                    if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_GRASS_COMES;
                        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                    }
                    break;
                case (3):           // rechts prüfen
                    if (Playfield.pLevel[I + 1] == EMERALD_SPACE) {
                        Playfield.pLevel[I + 1] = EMERALD_GRASS_COMES;
                        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_STAND | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    }
                    break;
                case (4):           // unten prüfen
                    if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_GRASS_COMES;
                        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    }
                    break;
                default:            // nichts machen
                    break;
            }
            nLoops--;
        } while (nLoops > 0);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlGrassComes
------------------------------------------------------------------------------
Beschreibung: Steuert "wachsendes" Gras und wandelt dieses in "stationäres"
              Gras um.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlGrassComes(uint32_t I) {
    uint16_t uLeftRightUpDownGrass = 0;       // Bit 3 = links hat Gras, Bit 2 = rechts hat Gras, Bit 1 = oben hat Gras, Bit 0 = unten hat Gras

   // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        return;
    } else {
        Playfield.pLevel[I] = EMERALD_GRASS;
        if ((Playfield.pLevel[I - 1] == EMERALD_GRASS)) uLeftRightUpDownGrass |= 0x08;    // Bit 3 setzen
        if ((Playfield.pLevel[I + 1] == EMERALD_GRASS)) uLeftRightUpDownGrass |= 0x04;    // Bit 2 setzen
        if ((Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_GRASS)) uLeftRightUpDownGrass |= 0x02;    // Bit 1 setzen
        if ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_GRASS)) uLeftRightUpDownGrass |= 0x01;    // Bit 0 setzen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SAND_0 + (uLeftRightUpDownGrass << 8);
    }
}
