#include "EmeraldMine.h"
#include "explosion.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlExplosion
------------------------------------------------------------------------------
Beschreibung: Steuert eine Explosion.
Parameter
      Eingang: I, uint32_t, Index im Level, zentraler Punkt der Explosion
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlExplosion(uint32_t I) {
    if (Playfield.pLevel[I] == EMERALD_EXPLOSION_PHASE_1) {
        Playfield.pLevel[I] = EMERALD_EXPLOSION_PHASE_2;
    } else if (Playfield.pLevel[I] == EMERALD_EXPLOSION_PHASE_2) {
        Playfield.pLevel[I] = EMERALD_SPACE;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    }
}
