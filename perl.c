#include "EmeraldMine.h"
#include "explosion.h"
#include "magicwall.h"
#include "man.h"
#include "perl.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlPerl
------------------------------------------------------------------------------
Beschreibung: Steuert eine Perle.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlPerl(uint32_t I) {
    uint8_t uFree;  // Richtung, in die Perle rollen könnte: 0 = kann nicht rollen, 1 = kann links rollen, 2 = kann rechts rollen, 3 = kann links und rechts rollen
    uint16_t uHitElement;       // Element, welches getroffen wird
    uint32_t uHitCoordinate;    // Lineare Koordinate des getroffenen Elements

    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        SDL_Log("%s: ack double control",__FUNCTION__);
        return;
    }
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_SINK_IN_MAGIC_WALL) {
        Playfield.pLevel[I] = EMERALD_SPACE;    // Perle ist in Magic Wall eingetaucht und wird zu Space
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        return;
    }
    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) ||
         ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ||
         ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_PERL_SHRINK) ) {
        // Perle kann vom Replikator geboren werden, dann hier nichts machen
        // Perle kann durch Man "geshrinkt" werden, dann hier auch nichts machen
        return;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist nach unten frei?
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = EMERALD_PERL;
        Playfield.pLastStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN_SELF | EMERALD_ANIM_CLEAN_UP;
        // Aktuelles Element auf Animation "unten"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Perle ins Säurebecken?
        SDL_Log("Perl falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = EMERALD_PERL;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else {                            // Unten ist nicht frei
        // Perle bleibt zunächst auf Platz liegen
        uHitCoordinate = I + Playfield.uLevel_X_Dimension;
        uHitElement = Playfield.pLevel[uHitCoordinate];
        if ( (Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_DOWN_SELF) {
            Playfield.pStatusAnimation[I] &= 0x00FFFFFF;
            if ((uHitElement == EMERALD_MAGIC_WALL) || (uHitElement == EMERALD_MAGIC_WALL_STEEL)) { // Perle trifft auf Magic wall
                if (Playfield.bMagicWallRunning) {
                    SDL_Log("Perl hit running magic wall");
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SINK_IN_MAGIC_WALL;
                    ElementGoesMagicWall(I,EMERALD_BOMB);
                    PreparePlaySound(SOUND_SQUEAK,I);
                } else if ((!Playfield.bMagicWallWasOn) && (Playfield.uTimeMagicWall > 0)) {
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SINK_IN_MAGIC_WALL;
                    SDL_Log("Perl start magic wall");
                    Playfield.bMagicWallWasOn = true;
                    Playfield.uTimeMagicWallLeft = Playfield.uTimeMagicWall;
                    Playfield.bMagicWallRunning = true;
                    ElementGoesMagicWall(I,EMERALD_BOMB);
                    PreparePlaySound(SOUND_SQUEAK,I);
                } else {
                    SDL_Log("Perl hit used magic wall");
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_PERL_BREAK;
                    PreparePlaySound(SOUND_SQUEAK,I);
                }
            } else {
                if (uHitElement == EMERALD_MAN) {
                    SDL_Log("Perl kills man");
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_PERL_BREAK;
                    Playfield.pLevel[uHitCoordinate] = EMERALD_MAN_DIES;
                    Playfield.pStatusAnimation[uHitCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL | EMERALD_ANIM_MAN_DIES_P1;
                    PreparePlaySound(SOUND_MAN_CRIES,I);
                    Playfield.bManDead = true;
                } else if (uHitElement == EMERALD_STANDMINE) {
                    SDL_Log("Perl hit stand mine");
                    ControlCentralExplosion(uHitCoordinate);
                    PreparePlaySound(SOUND_EXPLOSION,I);
                } else {
                    SDL_Log("Perl hit other element");
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_PERL_BREAK;
                    PreparePlaySound(SOUND_SQUEAK,I);
                }
                return;
            }
        }
        if ((Playfield.uRollUnderground[uHitElement] & EMERALD_CHECKROLL_PERL) != 0) {
            uFree = GetFreeRollDirections(I);
            if (uFree == 1) {   // Perle kann links rollen
                // neuen Platz mit ungültigem Element besetzen
                Playfield.pLevel[I - 1] = EMERALD_INVALID;
                // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                Playfield.pInvalidElement[I - 1] = EMERALD_PERL;
                Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                // Aktuelles Element auf Animation "links"
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
            } else if (uFree == 2) {    // Perle kann rechts rollen
                // neuen Platz mit ungültigem Element besetzen
                Playfield.pLevel[I + 1] = EMERALD_INVALID;
                // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                Playfield.pInvalidElement[I + 1] = EMERALD_PERL;
                Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                // Aktuelles Element auf Animation "rechts"
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
            } else if (uFree == 3) {    // Perle kann in beide Richtungen rollen
                // Hier entscheiden, ob links oder rechts gerollt wird
                if ((rand() & 0x01) == 0) {   // links
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_PERL;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else {                    // rechts
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_PERL;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            }
        } else {    // Ab hier prüfen, ob Perle durch Laufband bewegt werden kann
            if (uHitElement == EMERALD_CONVEYORBELT_RED) {
                if  ((Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_LEFT) && (Playfield.pLevel[I - 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_PERL;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_RIGHT) && (Playfield.pLevel[I + 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_PERL;
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
                    Playfield.pInvalidElement[I - 1] = EMERALD_PERL;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_RIGHT) && (Playfield.pLevel[I + 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_PERL;
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
                    Playfield.pInvalidElement[I - 1] = EMERALD_PERL;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_RIGHT) && (Playfield.pLevel[I + 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_PERL;
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
                    Playfield.pInvalidElement[I - 1] = EMERALD_PERL;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_RIGHT) && (Playfield.pLevel[I + 1] == EMERALD_SPACE)) {
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_PERL;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            } else {
                // SDL_Log("Perl sleeps, Hitelement = %x",uHitElement);
            }
        }
    }
}
