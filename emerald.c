#include "EmeraldMine.h"
#include "emerald.h"
#include "magicwall.h"
#include "man.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlEmerald
------------------------------------------------------------------------------
Beschreibung: Steuert einen Emerald.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
R�ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlEmerald(uint32_t I) {
    uint8_t uFree;  // Richtung, in die Emerald rollen k�nnte: 0 = kann nicht rollen, 1 = kann links rollen, 2 = kann rechts rollen, 3 = kann links und rechts rollen

    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        SDL_Log("%s: ack double control",__FUNCTION__);
        return;
    }
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_SINK_IN_MAGIC_WALL) {
        Playfield.pLevel[I] = EMERALD_SPACE;    // Emerald ist in Magic Wall eingetaucht und wird zu Space
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        return;
    }
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BLITZ) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    }
    if (    ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) ||
            ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ||
            ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_EMERALD_SHRINK) ) {
        // Emerald kann vom Replikator geboren werden, dann hier nichts machen
        // Emerald kann durch Man "geshrinkt" werden, dann hier auch nichts machen
        return;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist nach unten frei?
        // neuen Platz mit ung�ltigem Element besetzen
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
        Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = EMERALD_EMERALD;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN_SELF | EMERALD_ANIM_CLEAN_UP;
        // Aktuelles Element auf Animation "unten"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // F�llt Emerald ins S�urebecken?
        SDL_Log("Emerald falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = EMERALD_EMERALD;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else {                            // Unten ist nicht frei
        // Emerald bleibt zun�chst auf Platz liegen
        if ( (Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_DOWN_SELF) {
            Playfield.pStatusAnimation[I] &= 0x00FFFFFF;
            if ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MAGIC_WALL) ||
               (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MAGIC_WALL_STEEL)) { // Emerald trifft auf Magic wall
                if (Playfield.bMagicWallRunning) {
                    SDL_Log("Emerald hit running magic wall");
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SINK_IN_MAGIC_WALL;
                    ElementGoesMagicWall(I,EMERALD_SAPPHIRE);
                } else if (!Playfield.bMagicWallWasOn) {
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SINK_IN_MAGIC_WALL;
                    SDL_Log("Emerald start magic wall");
                    Playfield.bMagicWallWasOn = true;
                    Playfield.uTimeMagicWallLeft = Playfield.uTimeMagicWall;
                    Playfield.bMagicWallRunning = true;
                    ElementGoesMagicWall(I,EMERALD_SAPPHIRE);
                } else {
                    SDL_Log("Emerald hit used magic wall");
                    PreparePlaySound(SOUND_PING,I);
                }
            } else {
                PreparePlaySound(SOUND_PING,I);
            }
        }
        if ((Playfield.uRollUnderground[Playfield.pLevel[I + Playfield.uLevel_X_Dimension]] & EMERALD_CHECKROLL_EMERALD) != 0) {
            uFree = GetFreeRollDirections(I);
            if (uFree == 1) {   // Emerald kann links rollen
                // neuen Platz mit ung�ltigem Element besetzen
                Playfield.pLevel[I - 1] = EMERALD_INVALID;
                // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                Playfield.pInvalidElement[I - 1] = EMERALD_EMERALD;
                Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                // Aktuelles Element auf Animation "links"
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;

            } else if (uFree == 2) {    // Emerald kann rechts rollen
                // neuen Platz mit ung�ltigem Element besetzen
                Playfield.pLevel[I + 1] = EMERALD_INVALID;
                // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                Playfield.pInvalidElement[I + 1] = EMERALD_EMERALD;
                Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                // Aktuelles Element auf Animation "rechts"
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
            } else if (uFree == 3) {    // Emerald kann in beide Richtungen rollen
                // Hier entscheiden, ob links oder rechts gerollt wird
                if ((rand() & 0x01) == 0) {   // links
                    // neuen Platz mit ung�ltigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I - 1] = EMERALD_EMERALD;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else {                    // rechts
                    // neuen Platz mit ung�ltigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
                    Playfield.pInvalidElement[I + 1] = EMERALD_EMERALD;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            }
        }
    }
}
