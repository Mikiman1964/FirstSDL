#include <stdio.h>
#include <stdlib.h>
#include "EmeraldMine.h"
#include "alien.h"
#include "mystd.h"
#include "sound.h"

extern PLAYFIELD Playfield;


/*----------------------------------------------------------------------------
Name:           ControlAlien
------------------------------------------------------------------------------
Beschreibung: Steuert einen Alien.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlAlien(uint32_t I) {
    uint32_t uXpos;
    uint32_t uYpos;
    uint8_t uRandom;
    uint32_t uCatchXpos;
    uint32_t uCatchYpos;

    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) || ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ) {
        // Alien kann vom Replikator geboren werden, dann hier nichts machen
        return;
    }
    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_ALIEN_MOVED) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    } else {
        // Aus linearer Koordinate (I) die eigene X- und Y-Koordinate ausrechnen
        uXpos = I % Playfield.uLevel_X_Dimension;
        uYpos = I / Playfield.uLevel_X_Dimension;
        if (Playfield.bWheelRunning) {
            uCatchXpos = Playfield.uWheelRunningXpos;
            uCatchYpos = Playfield.uWheelRunningYpos;
        } else {
            uCatchXpos = Playfield.uManXpos;
            uCatchYpos = Playfield.uManYpos;
        }
        uRandom = randf(1,4);       // Ergibt Zufallszahl zwischen 1-3
        if (uRandom == 1) {         // X testen
            if (uCatchXpos > uXpos) {  // Ist Man-X-Koordinate > als eigene X-Koordinate ?
                // Versuche nach rechts zu laufen
                if (Playfield.pLevel[I + 1] == EMERALD_SPACE) {  // Ist nach rechts frei?
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ALIEN | EMERALD_ANIM_CLEAN_LEFT | EMERALD_ANIM_ALIEN_MOVED;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                    PreparePlaySound(SOUND_ALIEN,I);
                } else {                        // rechts ist nicht frei
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
                }
            } else if (uCatchXpos < uXpos) {
                // Versuche nach links zu laufen
                if (Playfield.pLevel[I - 1] == EMERALD_SPACE) {  // Ist nach links frei?
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ALIEN | EMERALD_ANIM_CLEAN_RIGHT | EMERALD_ANIM_ALIEN_MOVED;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                    PreparePlaySound(SOUND_ALIEN,I);
                } else {                        // links ist nicht frei
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
                }
            } else {                            // Steht bereits auf richtiger Koordinate
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;    // gibt es später nicht mehr
            }
        } else if (uRandom == 2) {  // Y testen
            if (uCatchYpos > uYpos) {  // Ist Man-Y-Koordinate > als eigene Y-Koordinate ?
                // Versuche nach unten zu laufen
                if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach unten frei?
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ALIEN | EMERALD_ANIM_CLEAN_UP | EMERALD_ANIM_ALIEN_MOVED;
                    // Aktuelles Element auf Animation "unten"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
                    PreparePlaySound(SOUND_ALIEN,I);
                } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Alien ins Säurebecken?
                        SDL_Log("Alien falls in pool");
                        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
                        Playfield.pStatusAnimation[I] = EMERALD_ALIEN;
                        PreparePlaySound(SOUND_POOL_BLUB,I);
                        return;
                } else {                        // unten ist nicht frei
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
                }
            } else if (uCatchYpos < uYpos) {
                // Versuche nach oben zu laufen
                if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach oben frei?
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ALIEN | EMERALD_ANIM_CLEAN_DOWN | EMERALD_ANIM_ALIEN_MOVED;
                    // Aktuelles Element auf Animation "oben"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_UP;
                    PreparePlaySound(SOUND_ALIEN,I);
                } else {                        // oben ist nicht frei
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
                }
            } else {                            // Steht bereits auf richtiger Koordinate
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;    // gibt es später nicht mehr
            }
        } else {                    // gar nicht laufen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        }
    }
}
