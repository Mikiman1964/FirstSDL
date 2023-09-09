#include "EmeraldMine.h"
#include "explosion.h"
#include "crystal.h"
#include "magicwall.h"
#include "man.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlCrystal
------------------------------------------------------------------------------
Beschreibung: Steuert einen Kristall.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
R�ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlCrystal(uint32_t I) {
    uint8_t uFree;  // Richtung, in die Kristall rollen k�nnte: 0 = kann nicht rollen, 1 = kann links rollen, 2 = kann rechts rollen, 3 = kann links und rechts rollen
    uint16_t uHitElement;       // Element, welches getroffen wird
    uint32_t uHitCoordinate;    // Lineare Koordinate des getroffenen Elements

    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        SDL_Log("%s: ack double control",__FUNCTION__);
        return;
    }
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BLITZ) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    }
    if (    ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) ||
            ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ||
            ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_CRYSTAL_SHRINK) ) {
        // Kristall kann vom Replikator geboren werden, dann hier nichts machen
        // Kristall kann durch Man "geshrinkt" werden, dann hier auch nichts machen
        return;
    } else if (IS_SPACE(I + Playfield.uLevel_X_Dimension)) {   // Ist nach unten frei?
        // neuen Platz mit ung�ltigem Element besetzen
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
        Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = EMERALD_CRYSTAL;
        Playfield.pLastStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN_SELF | EMERALD_ANIM_CLEAN_UP;
        // Aktuelles Element auf Animation "unten"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // F�llt Kristall ins S�urebecken?
        SDL_Log("Crystal falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = EMERALD_CRYSTAL;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else {                            // Unten ist nicht frei
        // Kristall bleibt zun�chst auf Platz liegen
        uHitCoordinate = I + Playfield.uLevel_X_Dimension;
        uHitElement = Playfield.pLevel[uHitCoordinate];
        if ( (Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_DOWN_SELF) {
            Playfield.pStatusAnimation[I] &= 0x00FFFFFF;
            if ((uHitElement == EMERALD_MAGIC_WALL) || (uHitElement == EMERALD_MAGIC_WALL_STEEL)) { // Kristall trifft auf Magic wall
                if (Playfield.bMagicWallRunning) {
                    SDL_Log("Crystal hit running magic wall");
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SINK_IN_MAGIC_WALL;
                    ElementGoesMagicWall(I,EMERALD_CRYSTAL);    // Kristall bleibt Kristall
                    PreparePlaySound(SOUND_SQUEAK,I);
                } else if ((!Playfield.bMagicWallWasOn) && (Playfield.uTimeMagicWall > 0)) {
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SINK_IN_MAGIC_WALL;
                    SDL_Log("Crystal start magic wall");
                    Playfield.bMagicWallWasOn = true;
                    Playfield.uTimeMagicWallLeft = Playfield.uTimeMagicWall;
                    Playfield.bMagicWallRunning = true;
                    ElementGoesMagicWall(I,EMERALD_CRYSTAL);    // Kristall bleibt Kristall
                    PreparePlaySound(SOUND_SQUEAK,I);
                } else {
                    SDL_Log("Crystal hit used magic wall");
                    PreparePlaySound(SOUND_PING,I);
                }
            } else if ((uHitElement == EMERALD_MAN) && (!Playfield.bManProtected)) {
                SDL_Log("Crystal kills man");
                Playfield.pLevel[uHitCoordinate] = EMERALD_MAN_DIES;
                Playfield.pStatusAnimation[uHitCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL | EMERALD_ANIM_MAN_DIES_P1;
                PreparePlaySound(SOUND_MAN_CRIES,I);
                Playfield.bManDead = true;
            } else if (uHitElement == EMERALD_STANDMINE) {
                SDL_Log("Crystal hit stand mine");
                ControlCentralExplosion(uHitCoordinate);
                PreparePlaySound(SOUND_EXPLOSION,I);
            } else {
                PreparePlaySound(SOUND_PING,I);
            }
        }
        if (((Playfield.uRollUnderground[uHitElement] & EMERALD_CHECKROLL_CRYSTAL) != 0) || (Playfield.pPipeLevel[uHitCoordinate] != EMERALD_SPACE)) {
            uFree = GetFreeRollDirections(I);
            if (uFree == 1) {   // Kristall kann links rollen
                // neuen Platz mit ung�ltigem Element besetzen
                Playfield.pLevel[I - 1] = EMERALD_INVALID;
                // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                Playfield.pInvalidElement[I - 1] = EMERALD_CRYSTAL;
                Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                // Aktuelles Element auf Animation "links"
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
            } else if (uFree == 2) {    // Kristall kann rechts rollen
                // neuen Platz mit ung�ltigem Element besetzen
                Playfield.pLevel[I + 1] = EMERALD_INVALID;
                // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                Playfield.pInvalidElement[I + 1] = EMERALD_CRYSTAL;
                Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                // Aktuelles Element auf Animation "rechts"
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
            } else if (uFree == 3) {    // Kristall kann in beide Richtungen rollen
                // Hier entscheiden, ob links oder rechts gerollt wird
                if ((rand() & 0x01) == 0) {   // links
                    // neuen Platz mit ung�ltigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_CRYSTAL;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else {                    // rechts
                    // neuen Platz mit ung�ltigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_CRYSTAL;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            }
        } else {    // Ab hier pr�fen, ob Kristall durch Laufband bewegt werden kann
            if (uHitElement == EMERALD_CONVEYORBELT_RED) {
                if ((Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_LEFT) && (IS_SPACE(I - 1))) {
                    // neuen Platz mit ung�ltigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_CRYSTAL;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_RIGHT) && (IS_SPACE(I + 1))) {
                    // neuen Platz mit ung�ltigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_CRYSTAL;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            } else if (uHitElement == EMERALD_CONVEYORBELT_GREEN) {
                if ((Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_LEFT) && (IS_SPACE(I - 1))) {
                    // neuen Platz mit ung�ltigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_CRYSTAL;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_RIGHT) && (IS_SPACE(I + 1))) {
                    // neuen Platz mit ung�ltigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_CRYSTAL;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            } else if (uHitElement == EMERALD_CONVEYORBELT_BLUE) {
                if ((Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_LEFT) && (IS_SPACE(I - 1))) {
                    // neuen Platz mit ung�ltigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_CRYSTAL;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_RIGHT) && (IS_SPACE(I + 1))) {
                    // neuen Platz mit ung�ltigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_CRYSTAL;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            } else if (uHitElement == EMERALD_CONVEYORBELT_YELLOW) {
                if ((Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_LEFT) && (IS_SPACE(I - 1))) {
                    // neuen Platz mit ung�ltigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_CRYSTAL;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else if ((Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_RIGHT) && (IS_SPACE(I + 1))) {
                    // neuen Platz mit ung�ltigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_CRYSTAL;
                    Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            } else {
                // SDL_Log("Crystal sleeps, Hitelement = %x",uHitElement);
            }
        }
    }
}
