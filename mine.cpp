#include "EmeraldMine.h"
#include "mine.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlMineUp
------------------------------------------------------------------------------
Beschreibung: Steuert eine nach oben laufende Mine im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMineUp(uint32_t I) {
    // Hatte Mine vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE) {
        Playfield.pStatusAnimation[I] = 0;
        if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach oben frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_MINE_UP | EMERALD_ANIM_CLEAN_DOWN;
            // Aktuelles Element auf Animation "oben"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_UP;
        } else {                        // Oben ist nicht frei
            // Mine bleibt zunächst auf "oben" muss sich aber bei Blockade nach rechts drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_RIGHT;
        }
    }
    else if (Playfield.pLevel[I - 1] != EMERALD_SPACE) { // // Hat Mine links Führung?
        if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach oben frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_MINE_UP | EMERALD_ANIM_CLEAN_DOWN;
            // Aktuelles Element auf Animation "oben"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_UP;
        } else {                        // Oben ist nicht frei
            // Mine bleibt zunächst auf "oben" muss sich aber bei Blockade nach rechts drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_RIGHT;
        }
    } else {                      // Linke Wand verloren
        // Mine bleibt zunächst auf "oben" muss sich aber bei Wand-Verlust nach links drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_LEFT | EMERALD_ANIM_LOST_GUIDE;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlMineRight
------------------------------------------------------------------------------
Beschreibung: Steuert eine nach rechts laufende Mine im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMineRight(uint32_t I) {
    // Hatte Mine vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE) {
        Playfield.pStatusAnimation[I] = 0;
        if (Playfield.pLevel[I + 1] == EMERALD_SPACE) {    // Ist rechts frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I + 1] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I + 1] = EMERALD_MINE_RIGHT | EMERALD_ANIM_CLEAN_LEFT;
            // Aktuelles Element auf Animation "rechts"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
        } else {                            // Rechts ist nicht frei
            // Mine bleibt zunächst auf "rechts" muss sich aber bei Blockade nach unten drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_DOWN;
        }
    } else if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] != EMERALD_SPACE) {  // Hat Mine links Führung?
        if (Playfield.pLevel[I + 1] == EMERALD_SPACE) {    // Ist nach rechts frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I + 1] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I + 1] = EMERALD_MINE_RIGHT | EMERALD_ANIM_CLEAN_LEFT;
            // Aktuelles Element auf Animation "rechts"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
        } else {                            // Rechts ist nicht frei
            // Mine bleibt zunächst auf "rechts" muss sich aber bei Blockade nach unten drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_DOWN;
        }
    } else {                          // Linke Wand verloren
        // Mine bleibt zunächst auf "rechts" muss sich aber bei Wand-Verlust nach oben drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_UP | EMERALD_ANIM_LOST_GUIDE;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlMineDown
------------------------------------------------------------------------------
Beschreibung: Steuert eine nach unten laufende Mine im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMineDown(uint32_t I) {
    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) || ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ) {
        // MineDown kann vom Replikator geboren werden, dann hier nichts machen
        return;
    } else if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE) {
        // Hatte Mine vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
        Playfield.pStatusAnimation[I] = 0;
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist nach unten frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_MINE_DOWN | EMERALD_ANIM_CLEAN_UP;
            // Aktuelles Element auf Animation "unten"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
        } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Mine ins Säurebecken?
            SDL_Log("Mine falls in pool");
            Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
            Playfield.pStatusAnimation[I] = EMERALD_MINE_DOWN;
            PreparePlaySound(SOUND_POOL_BLUB,I);
            return;
        } else {                            // Unten ist nicht frei
            // Mine bleibt zunächst auf "unten" muss sich aber bei Blockade nach links drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_LEFT;
        }
    } else if (Playfield.pLevel[I + 1] != EMERALD_SPACE) {   // Links von Mine irgendwas?
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist nach unten frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_MINE_DOWN | EMERALD_ANIM_CLEAN_UP;
            // Aktuelles Element auf Animation "unten"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
        } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Mine ins Säurebecken?
            SDL_Log("Mine falls in pool");
            Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
            Playfield.pStatusAnimation[I] = EMERALD_MINE_DOWN;
            PreparePlaySound(SOUND_POOL_BLUB,I);
            return;
        } else {                            // Unten ist nicht frei
            // Mine bleibt zunächst auf "unten" muss sich aber bei Blockade nach links drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_LEFT;
        }
    } else {                          // Linke Wand verloren
        // Mine bleibt zunächst auf "unten" muss sich aber bei Wand-Verlust nach rechts drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_RIGHT | EMERALD_ANIM_LOST_GUIDE;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlMineLeft
------------------------------------------------------------------------------
Beschreibung: Steuert eine nach links laufende Mine im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMineLeft(uint32_t I) {
    // Hatte Mine vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE) {
        Playfield.pStatusAnimation[I] = 0;
        if (Playfield.pLevel[I - 1] == EMERALD_SPACE) {    // Ist nach links frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I - 1] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I - 1] = EMERALD_MINE_LEFT | EMERALD_ANIM_CLEAN_RIGHT;
            // Aktuelles Element auf Animation "links"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
        } else {                           // Links ist nicht frei
            // Mine bleibt zunächst auf "links" muss sich aber bei Blockade nach oben drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_UP;
        }
    } else if ( (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] != EMERALD_SPACE) &&
                (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] != EMERALD_ACIDPOOL) ) {   // Links von Mine irgendwas? AcidPool ist wie Space.
        if (Playfield.pLevel[I - 1] == EMERALD_SPACE)    // Ist nach links frei?
        {
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I - 1] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I - 1] = EMERALD_MINE_LEFT | EMERALD_ANIM_CLEAN_RIGHT;
            // Aktuelles Element auf Animation "links"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
        } else {                            // Links ist nicht frei
            // Mine bleibt zunächst auf "links" muss sich aber bei Blockade nach oben drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_UP;
        }
    } else {                          // Linke Wand verloren
        // Mine bleibt zunächst auf "links" muss sich aber bei Wand-Verlust nach unten drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_DOWN | EMERALD_ANIM_LOST_GUIDE;
    }
}
