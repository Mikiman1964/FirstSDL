#include "EmeraldMine.h"
#include "earth.h"
#include "mystd.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlEarth
------------------------------------------------------------------------------
Beschreibung: Überprüft die freien Ränder eines Erd-Stückes und gibt diesem
              Element den richtigen Zustand.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlEarth(uint32_t I) {
    uint16_t uLeftRightUpDownEarth = 0;       // Bit 3 = links hat Erde, Bit 2 = rechts hat Erde, Bit 1 = oben hat Erde, Bit 0 = unten hat Erde

    if ((Playfield.pStatusAnimation[I] & 0xFF000000) != EMERALD_ANIM_EARTH_SHRINK) {     // Erde entfernen bzw. wegschnippen hat Vorrang
        if ((Playfield.pLevel[I - 1] == EMERALD_EARTH) || (Playfield.pLevel[I - 1] == EMERALD_MINE_EARTH)) uLeftRightUpDownEarth |= 0x08;    // Bit 3 setzen
        if ((Playfield.pLevel[I + 1] == EMERALD_EARTH) || (Playfield.pLevel[I + 1] == EMERALD_MINE_EARTH)) uLeftRightUpDownEarth |= 0x04;    // Bit 2 setzen
        if ((Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_EARTH) || (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_MINE_EARTH)) uLeftRightUpDownEarth |= 0x02;    // Bit 1 setzen
        if ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_EARTH) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MINE_EARTH)) uLeftRightUpDownEarth |= 0x01;    // Bit 0 setzen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_EARTH_0 + (uLeftRightUpDownEarth << 8);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlEarthInvisible
------------------------------------------------------------------------------
Beschreibung: Überprüft die freien Ränder eines unsichtbaren Erd-Stückes und gibt diesem
              Element den richtigen Zustand.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlEarthInvisible(uint32_t I) {
    uint16_t uLeftRightUpDownEarth = 0;       // Bit 3 = links hat Erde, Bit 2 = rechts hat Erde, Bit 1 = oben hat Erde, Bit 0 = unten hat Erde

    if ((Playfield.pStatusAnimation[I] & 0xFF000000) != EMERALD_ANIM_EARTH_INVISIBLE_SHRINK) {     // Erde entfernen bzw. wegschnippen hat Vorrang
        if ((Playfield.pLevel[I - 1] == EMERALD_EARTH_INVISIBLE) || (Playfield.pLevel[I - 1] == EMERALD_MINE_EARTH)) uLeftRightUpDownEarth |= 0x08;    // Bit 3 setzen
        if ((Playfield.pLevel[I + 1] == EMERALD_EARTH_INVISIBLE) || (Playfield.pLevel[I + 1] == EMERALD_MINE_EARTH)) uLeftRightUpDownEarth |= 0x04;    // Bit 2 setzen
        if ((Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_EARTH_INVISIBLE) || (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_MINE_EARTH)) uLeftRightUpDownEarth |= 0x02;    // Bit 1 setzen
        if ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_EARTH_INVISIBLE) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MINE_EARTH)) uLeftRightUpDownEarth |= 0x01;    // Bit 0 setzen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_EARTH_0 + (uLeftRightUpDownEarth << 8);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlEarthMine
------------------------------------------------------------------------------
Beschreibung: Überprüft die freien Ränder eines Erd-Minen-Stückes und gibt diesem
              Element den richtigen Zustand.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlEarthMine(uint32_t I) {
    uint16_t uLeftRightUpDownEarthMine = 0;       // Bit 3 = links hat Erdmine, Bit 2 = rechts hat Erdmine, Bit 1 = oben hat Erdmine, Bit 0 = unten hat Erdmine

    if ((Playfield.pLevel[I - 1] == EMERALD_EARTH) || (Playfield.pLevel[I - 1] == EMERALD_MINE_EARTH)) uLeftRightUpDownEarthMine |= 0x08;    // Bit 3 setzen
    if ((Playfield.pLevel[I + 1] == EMERALD_EARTH) || (Playfield.pLevel[I + 1] == EMERALD_MINE_EARTH)) uLeftRightUpDownEarthMine |= 0x04;    // Bit 2 setzen
    if ((Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_EARTH) || (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_MINE_EARTH)) uLeftRightUpDownEarthMine |= 0x02;    // Bit 1 setzen
    if ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_EARTH) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MINE_EARTH)) uLeftRightUpDownEarthMine |= 0x01;    // Bit 0 setzen
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_EARTH_0 + (uLeftRightUpDownEarthMine << 8);
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

    if ((Playfield.pStatusAnimation[I] & 0xFF000000) != EMERALD_ANIM_GRASS_SHRINK) {      // Gras entfernen bzw. wegschnippen hat Vorrang
        if (Playfield.pLevel[I - 1] == EMERALD_GRASS) uLeftRightUpDownGrass |= 0x08;    // Bit 3 setzen
        if (Playfield.pLevel[I + 1] == EMERALD_GRASS) uLeftRightUpDownGrass |= 0x04;    // Bit 2 setzen
        if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_GRASS) uLeftRightUpDownGrass |= 0x02;    // Bit 1 setzen
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_GRASS) uLeftRightUpDownGrass |= 0x01;    // Bit 0 setzen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_EARTH_0 + (uLeftRightUpDownGrass << 8);
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
                    if (IS_SPACE(I - 1)) {
                        Playfield.pLevel[I - 1] = EMERALD_GRASS_COMES;
                        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_STAND;
                    }
                    break;
                case (2):           // oben prüfen
                    if (IS_SPACE(I - Playfield.uLevel_X_Dimension)) {
                        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_GRASS_COMES;
                        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                    }
                    break;
                case (3):           // rechts prüfen
                    if (IS_SPACE(I + 1)) {
                        Playfield.pLevel[I + 1] = EMERALD_GRASS_COMES;
                        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_STAND | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    }
                    break;
                case (4):           // unten prüfen
                    if (IS_SPACE(I + Playfield.uLevel_X_Dimension)) {
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
        if (Playfield.pLevel[I - 1] == EMERALD_GRASS) uLeftRightUpDownGrass |= 0x08;    // Bit 3 setzen
        if (Playfield.pLevel[I + 1] == EMERALD_GRASS) uLeftRightUpDownGrass |= 0x04;    // Bit 2 setzen
        if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_GRASS) uLeftRightUpDownGrass |= 0x02;    // Bit 1 setzen
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_GRASS) uLeftRightUpDownGrass |= 0x01;    // Bit 0 setzen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_EARTH_0 + (uLeftRightUpDownGrass << 8);
    }
}
