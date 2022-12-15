#include "EmeraldMine.h"
#include "explosion.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlStone
------------------------------------------------------------------------------
Beschreibung: Steuert eine Explosion.
Parameter
      Eingang: I, uint32_t, Index im Level, zentraler Punkt der Explosion
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlExplosion(uint32_t I) {
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        SDL_Log("%s: ack double control",__FUNCTION__);
        return;
    }
    if (Playfield.pLevel[I] == EMERALD_EXPLOSION_PHASE_1) {
        Playfield.pLevel[I] = EMERALD_EXPLOSION_PHASE_2;
    } else if (Playfield.pLevel[I] == EMERALD_EXPLOSION_PHASE_2) {
        Playfield.pLevel[I] = EMERALD_SPACE;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    }
}
