#include "EmeraldMine.h"
#include "emerald.h"
#include "explosion.h"
#include "nut.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlNut
------------------------------------------------------------------------------
Beschreibung: Steuert eine Nuss.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlNut(uint32_t I) {
    uint8_t uFree;  // Richtung, in die Nut rollen könnte: 0 = kann nicht rollen, 1 = kann links rollen, 2 = kann rechts rollen, 3 = kann links und rechts rollen
    uint16_t uHitElement;       // Element, welches getroffen wird
    uint32_t uHitCoordinate;    // Lineare Koordinate des getroffenen Elements

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
        // Nut kann vom Replikator geboren werden, dann hier nichts machen
        return;
    } else if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_NUT_CRACK1) {
        // bei Knack-Phase 1 nur auf Phase 2 umschalten
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_NUT_CRACK2;
    } else if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_NUT_CRACK2) {
        Playfield.pLevel[I] = EMERALD_EMERALD;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        ControlEmerald(I);
        return;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist nach unten frei?
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = EMERALD_NUT;
        Playfield.pLastStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN_SELF | EMERALD_ANIM_CLEAN_UP;
        // Aktuelles Element auf Animation "unten"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Nut ins Säurebecken?
        SDL_Log("Nut falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = EMERALD_NUT;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else {                            // Unten ist nicht frei
        // Nut bleibt zunächst auf Platz liegen
        uHitCoordinate = I + Playfield.uLevel_X_Dimension;
        uHitElement = Playfield.pLevel[uHitCoordinate];
        if ( (Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_DOWN_SELF) {
            Playfield.pStatusAnimation[I] &= 0x00FFFFFF;
            PreparePlaySound(SOUND_NUT_FALL,I);
            if ( (uHitElement == EMERALD_WALL_ROUND_PIKE) || (uHitElement == EMERALD_STEEL_ROUND_PIKE) ) {
                SDL_Log("Nut hits wall with pike");
                PreparePlaySound(SOUND_NUT_CRACK,I);
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_NUT_CRACK2;
                Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreNutCracking;
                return;
            } else if (uHitElement == EMERALD_MAN) {
                SDL_Log("Nut kills man");
                Playfield.pLevel[uHitCoordinate] = EMERALD_MAN_DIES;
                Playfield.pStatusAnimation[uHitCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL | EMERALD_ANIM_MAN_DIES_P1;
                PreparePlaySound(SOUND_MAN_CRIES,I);
                Playfield.bManDead = true;
                return;
            } else if (uHitElement == EMERALD_STANDMINE) {
                SDL_Log("Nut hit stand mine");
                ControlCentralExplosion(uHitCoordinate);
                PreparePlaySound(SOUND_EXPLOSION,I);
                return;
            }
        }
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        if ((Playfield.uRollUnderground[uHitElement] & EMERALD_CHECKROLL_NUT) != 0) {
            uFree = GetFreeRollDirections(I);
            if (uFree == 1) {   // Nut kann links rollen
                // neuen Platz mit ungültigem Element besetzen
                Playfield.pLevel[I - 1] = EMERALD_INVALID;
                // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                Playfield.pInvalidElement[I - 1] = EMERALD_NUT;
                Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                // Aktuelles Element auf Animation "links"
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;

            } else if (uFree == 2) {    // Nut kann rechts rollen
                // neuen Platz mit ungültigem Element besetzen
                Playfield.pLevel[I + 1] = EMERALD_INVALID;
                // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                Playfield.pInvalidElement[I + 1] = EMERALD_NUT;
                Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                // Aktuelles Element auf Animation "rechts"
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
            } else if (uFree == 3) {    // Nut kann in beide Richtungen rollen
                // Hier entscheiden, ob links oder rechts gerollt wird
                if ((rand() & 0x01) == 0) {   // links
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_NUT;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else {                    // rechts
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_NUT;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            }
        } else {    // Ab hier prüfen, ob Nuss durch Laufband bewegt werden kann
            if (uHitElement == EMERALD_CONVEYORBELT_RED) {
                if  ((Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_LEFT) && (Playfield.pLevel[I - 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_NUT;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_RIGHT) && (Playfield.pLevel[I + 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_NUT;
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
                    Playfield.pInvalidElement[I - 1] = EMERALD_NUT;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_RIGHT) && (Playfield.pLevel[I + 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_NUT;
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
                    Playfield.pInvalidElement[I - 1] = EMERALD_NUT;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_RIGHT) && (Playfield.pLevel[I + 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_NUT;
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
                    Playfield.pInvalidElement[I - 1] = EMERALD_NUT;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_RIGHT) && (Playfield.pLevel[I + 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_NUT;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            } else {
                // SDL_Log("Nut sleeps, Hitelement = %x",uHitElement);
            }
        }
    }
}
