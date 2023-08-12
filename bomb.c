#include "EmeraldMine.h"
#include "bomb.h"
#include "explosion.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlBomb
------------------------------------------------------------------------------
Beschreibung: Steuert eine Bombe.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlBomb(uint32_t I) {
    uint8_t uFree;  // Richtung, in die Bombe rollen könnte: 0 = kann nicht rollen, 1 = kann links rollen, 2 = kann rechts rollen, 3 = kann links und rechts rollen
    uint16_t uHitElement;       // Element, welches getroffen wird
    uint32_t uHitCoordinate;    // Lineare Koordinate des getroffenen Elements

    uHitCoordinate = I + Playfield.uLevel_X_Dimension;
    uHitElement = Playfield.pLevel[uHitCoordinate];
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MAN_PUSH_RIGHT) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MAN_PUSH_RIGHT2 | EMERALD_ANIM_RIGHT;
        return;
    } else if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MAN_PUSH_LEFT) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MAN_PUSH_LEFT2 | EMERALD_ANIM_LEFT;
        return;
    }

    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MAN_PUSH_RIGHT2) || ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MAN_PUSH_LEFT2) ) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    }

    if (    ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) ||
            ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2)  ) {
        // Bombe kann vom Replikator geboren werden, dann hier nichts machen
        return;
    } else if (uHitElement == EMERALD_SPACE) {   // Ist nach unten frei?
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[uHitCoordinate] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pInvalidElement[uHitCoordinate] = EMERALD_BOMB;
        Playfield.pLastStatusAnimation[uHitCoordinate] = EMERALD_ANIM_DOWN;
        Playfield.pStatusAnimation[uHitCoordinate] = EMERALD_ANIM_DOWN_SELF | EMERALD_ANIM_CLEAN_UP;
        // Aktuelles Element auf Animation "unten"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
    } else if (Playfield.pLevel[uHitCoordinate] == EMERALD_ACIDPOOL) {   // Fällt Bombe ins Säurebecken?
        SDL_Log("Bomb falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = EMERALD_BOMB;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else {                            // Unten ist nicht frei
        // Bombe bleibt zunächst auf Platz liegen
        if ( (Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_DOWN_SELF) {
            // Bombe fällt auf Etwas
            ControlCentralExplosion(I);
            PreparePlaySound(SOUND_EXPLOSION,I);
        }
        if ((Playfield.uRollUnderground[uHitElement] & EMERALD_CHECKROLL_BOMB) != 0) {
            uFree = GetFreeRollDirections(I);
            if (uFree == 1) {   // Bombe kann links rollen
                // neuen Platz mit ungültigem Element besetzen
                Playfield.pLevel[I - 1] = EMERALD_INVALID;
                // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                Playfield.pInvalidElement[I - 1] = EMERALD_BOMB;
                Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                // Aktuelles Element auf Animation "links"
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;

            } else if (uFree == 2) {    // Bombe kann rechts rollen
                // neuen Platz mit ungültigem Element besetzen
                Playfield.pLevel[I + 1] = EMERALD_INVALID;
                // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                Playfield.pInvalidElement[I + 1] = EMERALD_BOMB;
                Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                // Aktuelles Element auf Animation "rechts"
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
            } else if (uFree == 3) {    // Bombe kann in beide Richtungen rollen
                // Hier entscheiden, ob links oder rechts gerollt wird
                if ((rand() & 0x01) == 0) {   // links
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_BOMB;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else {                    // rechts
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_BOMB;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            }
        } else {    // Ab hier prüfen, ob Bombe durch Laufband bewegt werden kann
            if (uHitElement == EMERALD_CONVEYORBELT_RED) {
                if  ((Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_LEFT) && (Playfield.pLevel[I - 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_BOMB;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_RIGHT) && (Playfield.pLevel[I + 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_BOMB;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            } else if (uHitElement == EMERALD_CONVEYORBELT_GREEN) {
                if  ((Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_LEFT) && (Playfield.pLevel[I - 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_BOMB;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_RIGHT) && (Playfield.pLevel[I + 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_BOMB;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            } else if (uHitElement == EMERALD_CONVEYORBELT_BLUE) {
                if  ((Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_LEFT) && (Playfield.pLevel[I - 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_BOMB;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_RIGHT) && (Playfield.pLevel[I + 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_BOMB;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            } else if (uHitElement == EMERALD_CONVEYORBELT_YELLOW) {
                if  ((Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_LEFT) && (Playfield.pLevel[I - 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_BOMB;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_RIGHT) && (Playfield.pLevel[I + 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_BOMB;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            } else {
                // SDL_Log("Bomb sleeps, Hitelement = %x",uHitElement);
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlRemoteBomb
------------------------------------------------------------------------------
Beschreibung: Steuert eine ferngesteuerte Bombe.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlRemoteBomb(uint32_t I) {
    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) || ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ) {
        // Ferngesteuerte Bombe kann vom Replikator geboren werden, dann hier nichts machen
        return;
    }
    if (Playfield.bSwitchRemoteBombIgnition) {
        Playfield.pLevel[I] = EMERALD_CENTRAL_EXPLOSION;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        ControlCentralExplosion(I);
        PreparePlaySound(SOUND_EXPLOSION,I);
    } else if (Playfield.bSwitchRemoteBombUp) {
        if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach oben frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I - Playfield.uLevel_X_Dimension] = EMERALD_REMOTEBOMB;
            Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_DOWN;
            // Aktuelles Element auf Animation "oben"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_UP;
            Playfield.bRemoteBombMoved = true;
        }
    } else if (Playfield.bSwitchRemoteBombDown) {
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist nach unten frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = EMERALD_REMOTEBOMB;
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_UP;
            Playfield.pLastStatusAnimation[I + Playfield.uLevel_X_Dimension] = Playfield.pStatusAnimation[I];
            // Aktuelles Element auf Animation "unten"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
            Playfield.bRemoteBombMoved = true;
        } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt ferngesteuerte Bombe ins Säurebecken?
            SDL_Log("Remote bomb falls in pool");
            Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
            Playfield.pInvalidElement[I] = EMERALD_REMOTEBOMB;
            PreparePlaySound(SOUND_POOL_BLUB,I + Playfield.uLevel_X_Dimension);
            return;
        }
    } else if (Playfield.bSwitchRemoteBombLeft) {
        if (Playfield.pLevel[I - 1] == EMERALD_SPACE) {    // Ist nach links frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I - 1] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I - 1] = EMERALD_REMOTEBOMB;
            Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
            // Aktuelles Element auf Animation "links"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
            Playfield.bRemoteBombMoved = true;
        }
    } else if (Playfield.bSwitchRemoteBombRight) {
        if (Playfield.pLevel[I + 1] == EMERALD_SPACE) {    // Ist nach rechts frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I + 1] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I + 1] = EMERALD_REMOTEBOMB;
            Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
            Playfield.pLastStatusAnimation[I + 1] = Playfield.pStatusAnimation[I];
            // Aktuelles Element auf Animation "rechts"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
            Playfield.bRemoteBombMoved = true;
        }
    }
}
