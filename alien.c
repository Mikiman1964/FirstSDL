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
    int  nRandom;
    uint32_t uCatchXpos;
    uint32_t uCatchYpos;
    uint32_t uManSelfStatus;

    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) || ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ) {
        // Alien kann vom Replikator geboren werden, dann hier nichts machen
        return;
    }
    if ((Playfield.pStatusAnimation[I] == EMERALD_ANIM_ALIEN_MOVED) || Playfield.bManDead || Playfield.bWellDone) {
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
        nRandom = randn(1,3);       // Ergibt Zufallszahl zwischen 1-3
        if (nRandom == 1) {         // X testen
            if (uCatchXpos > uXpos) {  // Ist Man-X-Koordinate > als eigene X-Koordinate ?
                // Versuche nach rechts zu laufen
                if (Playfield.pLevel[I + 1] == EMERALD_SPACE) {  // Ist nach rechts frei?
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_ALIEN;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT | EMERALD_ANIM_ALIEN_MOVED;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                    PreparePlaySound(SOUND_ALIEN,I);
                } else if (Playfield.pLevel[I + 1] == EMERALD_MAN) {
                    uManSelfStatus = Playfield.pStatusAnimation[I + 1] & 0xFF000000;
                    // Kann (stehender/blockierter) Man erwischt werden?
                    if ((Playfield.pStatusAnimation[I + 1] == EMERALD_ANIM_STAND) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_LEFT) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_RIGHT) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_UP) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_DOWN)) {
                        SDL_Log("Alien kills man -> right");
                        Playfield.pLevel[I] = EMERALD_ALIEN_KILLS_MAN;
                        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MONSTER_KILLS_RIGHT | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                        Playfield.pLevel[I + 1] = EMERALD_MAN_DIES;
                        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_MAN_DIES_P1 | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                        Playfield.bManDead = true;
                        PreparePlaySound(SOUND_MAN_CRIES,I);
                    }
                } else {                        // rechts ist nicht frei
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
                }
            } else if (uCatchXpos < uXpos) {
                // Versuche nach links zu laufen
                if (Playfield.pLevel[I - 1] == EMERALD_SPACE) {  // Ist nach links frei?
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_ALIEN;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT | EMERALD_ANIM_ALIEN_MOVED;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                    PreparePlaySound(SOUND_ALIEN,I);
                } else if (Playfield.pLevel[I - 1] == EMERALD_MAN) {
                    uManSelfStatus = Playfield.pStatusAnimation[I - 1] & 0xFF000000;
                    // Kann (stehender/blockierter) Man erwischt werden?
                    if ((Playfield.pStatusAnimation[I - 1] == EMERALD_ANIM_STAND) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_LEFT) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_RIGHT) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_UP) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_DOWN)) {
                        SDL_Log("Alien kills man - > left");
                        Playfield.pLevel[I] = EMERALD_ALIEN_KILLS_MAN;
                        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MONSTER_KILLS_LEFT;
                        Playfield.pLevel[I - 1] = EMERALD_MAN_DIES;
                        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_MAN_DIES_P1;
                        Playfield.bManDead = true;
                        PreparePlaySound(SOUND_MAN_CRIES,I);
                    }
                } else {                        // links ist nicht frei
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
                }
            } else {                            // Steht bereits auf richtiger Koordinate
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;    // gibt es später nicht mehr
            }
        } else if (nRandom == 2) {  // Y testen
            if (uCatchYpos > uYpos) {  // Ist Man-Y-Koordinate > als eigene Y-Koordinate ?
                // Versuche nach unten zu laufen
                if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach unten frei?
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = EMERALD_ALIEN;
                    Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_UP | EMERALD_ANIM_ALIEN_MOVED;
                    Playfield.pLastStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN;
                    // Aktuelles Element auf Animation "unten"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
                    PreparePlaySound(SOUND_ALIEN,I);
                } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MAN) {
                    uManSelfStatus = Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] & 0xFF000000;
                    // Kann (stehender/blockierter) Man erwischt werden?
                    if ((Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] == EMERALD_ANIM_STAND) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_LEFT) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_RIGHT) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_UP) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_DOWN)) {
                        SDL_Log("Alien kills man - > down");
                        Playfield.pLevel[I] = EMERALD_ALIEN_KILLS_MAN;
                        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MONSTER_KILLS_DOWN | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_MAN_DIES;
                        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_MAN_DIES_P1 | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                        Playfield.bManDead = true;
                        PreparePlaySound(SOUND_MAN_CRIES,I);
                    }
                } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Alien ins Säurebecken?
                        SDL_Log("Alien falls in pool");
                        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
                        Playfield.pInvalidElement[I] = EMERALD_ALIEN;
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
                    Playfield.pInvalidElement[I - Playfield.uLevel_X_Dimension] = EMERALD_ALIEN;
                    Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_DOWN | EMERALD_ANIM_ALIEN_MOVED;
                    // Aktuelles Element auf Animation "oben"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_UP;
                    PreparePlaySound(SOUND_ALIEN,I);
                } else if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_MAN) {
                    uManSelfStatus = Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] & 0xFF000000;
                    // Kann (stehender/blockierter) Man erwischt werden?
                    if ((Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] == EMERALD_ANIM_STAND) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_LEFT) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_RIGHT) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_UP) ||
                        (uManSelfStatus == EMERALD_ANIM_MAN_BLOCKED_DOWN)) {
                        SDL_Log("Alien kills man - > up      man anim: 0x%08X",Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension]);
                        Playfield.pLevel[I] = EMERALD_ALIEN_KILLS_MAN;
                        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MONSTER_KILLS_UP;
                        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_MAN_DIES;
                        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_MAN_DIES_P1;
                        Playfield.bManDead = true;
                        PreparePlaySound(SOUND_MAN_CRIES,I);
                    }
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


/*----------------------------------------------------------------------------
Name:           ControlAlienKillsMan
------------------------------------------------------------------------------
Beschreibung: Steuert die Tötung des Mans durch einen Alien.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlAlienKillsMan(uint32_t I) {
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        SDL_Log("%s: ack double control",__FUNCTION__);
        return;
    }
    // Ab hier: Nach Tötung des Mans den Alien auf ursprüngliche Man-Position setzen
    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_MONSTER_KILLS_UP) {
        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_ALIEN;
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_MONSTER_KILLS_LEFT) {
        Playfield.pLevel[I - 1] = EMERALD_ALIEN;
        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_STAND;
    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_MONSTER_KILLS_RIGHT) {
        Playfield.pLevel[I + 1] = EMERALD_ALIEN;
        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_STAND;
    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_MONSTER_KILLS_DOWN) {
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_ALIEN;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
    }
    Playfield.pLevel[I] = EMERALD_SPACE;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
}
