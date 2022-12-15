#include "EmeraldMine.h"
#include "magicwall.h"
#include "man.h"
#include "sound.h"
#include "stone.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlStone
------------------------------------------------------------------------------
Beschreibung: Steuert einen Stein.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlStone(uint32_t I) {
    uint8_t uFree;              // Richtung, in die Stone rollen könnte: 0 = kann nicht rollen, 1 = kann links rollen, 2 = kann rechts rollen, 3 = kann links und rechts rollen
    uint8_t uStoneHitElement;   // Element, welches durch Stein getroffen wird
    uint32_t uHitCoordinate;    // Lineare Koordinate des getroffenen Elements

    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        SDL_Log("%s: ack double control",__FUNCTION__);
        return;
    }
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MAN_PUSH_RIGHT) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MAN_PUSH_RIGHT2 | EMERALD_ANIM_RIGHT;
        return;
    } else if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MAN_PUSH_LEFT) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MAN_PUSH_LEFT2 | EMERALD_ANIM_LEFT;
        return;
    } else if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_SINK_IN_MAGIC_WALL) {
        Playfield.pLevel[I] = EMERALD_SPACE;    // Stein ist in Magic Wall eingetaucht und wird zu Space
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        return;
    }
    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MAN_PUSH_RIGHT2) || ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MAN_PUSH_LEFT2) ) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    }
    if (    ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) ||
            ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ) {
        // Stein kann vom Replikator geboren werden, dann hier nichts machen
        return;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist nach unten frei?
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN_SELF | EMERALD_STONE | EMERALD_ANIM_CLEAN_UP;
        // Aktuelles Element auf Animation "unten"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Stein ins Säurebecken?
        SDL_Log("Stone falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pStatusAnimation[I] = EMERALD_STONE;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
     } else {                            // Unten ist nicht frei
        // Stone bleibt zunächst auf Platz liegen
        uHitCoordinate = I + Playfield.uLevel_X_Dimension;
        uStoneHitElement = Playfield.pLevel[uHitCoordinate];
        if ( (Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_DOWN_SELF) {  // Stein noch in Bewegung
            Playfield.pStatusAnimation[I] &= 0x00FFFFFF;
            uHitCoordinate = I + Playfield.uLevel_X_Dimension;
            uStoneHitElement = Playfield.pLevel[uHitCoordinate];
            switch (uStoneHitElement) {
                case (EMERALD_NUT):
                    SDL_Log("Stone hit nut");
                    PreparePlaySound(SOUND_NUT_CRACK,I);
                    Playfield.pStatusAnimation[uHitCoordinate] = EMERALD_ANIM_NUT_CRACK1;
                    Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreNutCracking;
                    break;
                case (EMERALD_SAPPHIRE):
                    SDL_Log("Stone hit sapphire");
                    // ursprüngliche Stone-Position mit Space besetzen
                    Playfield.pLevel[I] = EMERALD_SPACE;
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
                    // Stone auf neue Position setzen
                    Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_STONE;
                    Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL | EMERALD_ANIM_DOWN | EMERALD_ANIM_SAPPHIRE_SQUEAK;
                    PreparePlaySound(SOUND_SQUEAK,I);
                    return;     // Stone wurde bereits vollständig gesteuert, daher hier beenden
                    break;
                case (EMERALD_SWAMP):
                    SDL_Log("Stone hit empty swamp");
                    // Stein in versumpften Stein wandeln
                    Playfield.pLevel[I] = EMERALD_STONE_SINK;
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STONE_SWAMP1;
                    PreparePlaySound(SOUND_STONE_FALL,I);
                    break;
                case (EMERALD_MAGIC_WALL):
                    if (Playfield.bMagicWallRunning) {
                        SDL_Log("Stone hit running magic wall");
                        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SINK_IN_MAGIC_WALL;
                        ElementGoesMagicWall(I,EMERALD_EMERALD);
                    } else if (!Playfield.bMagicWallWasOn) {
                        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SINK_IN_MAGIC_WALL;
                        SDL_Log("Stone start magic wall");
                        Playfield.bMagicWallWasOn = true;
                        Playfield.uTimeMagicWallLeft = Playfield.uTimeMagicWall;
                        Playfield.bMagicWallRunning = true;
                        ElementGoesMagicWall(I,EMERALD_EMERALD);
                    } else {
                        SDL_Log("Stone hit used magic wall");
                        PreparePlaySound(SOUND_STONE_FALL,I);
                    }
                    break;
                default:
                    SDL_Log("Stone hit other element");
                    PreparePlaySound(SOUND_STONE_FALL,I);
                    break;
            }
        } else {
            if (uStoneHitElement == EMERALD_SWAMP) {
                SDL_Log("Stone lies on empty swamp");
                // Stein in versumpften Stein wandeln
                Playfield.pLevel[I] = EMERALD_STONE_SINK;
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_STONE_SWAMP1;
            }
        }
        Playfield.pStatusAnimation[I] |= EMERALD_ANIM_STAND;
        if ((Playfield.uRollUnderground[Playfield.pLevel[I + Playfield.uLevel_X_Dimension]] & EMERALD_CHECKROLL_STONE) != 0) {
            uFree = GetFreeRollDirections(I);
            if (uFree == 1) {   // Stone kann links rollen
                // neuen Platz mit ungültigem Element besetzen
                Playfield.pLevel[I - 1] = EMERALD_INVALID;
                // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                Playfield.pStatusAnimation[I - 1] = EMERALD_STONE | EMERALD_ANIM_CLEAN_RIGHT;
                // Aktuelles Element auf Animation "links"
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;

            } else if (uFree == 2) {    // Stone kann rechts rollen
                // neuen Platz mit ungültigem Element besetzen
                Playfield.pLevel[I + 1] = EMERALD_INVALID;
                // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                Playfield.pStatusAnimation[I + 1] = EMERALD_STONE | EMERALD_ANIM_CLEAN_LEFT;
                // Aktuelles Element auf Animation "rechts"
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
            } else if (uFree == 3) {    // Stpne kann in beide Richtungen rollen
                // Hier entscheiden, ob links oder rechts gerollt wird
                if ((rand() & 0x01) == 0) {   // links
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I - 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pStatusAnimation[I - 1] = EMERALD_STONE | EMERALD_ANIM_CLEAN_RIGHT;
                    // Aktuelles Element auf Animation "links"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
                } else {                    // rechts
                    // neuen Platz mit ungültigem Element besetzen
                    Playfield.pLevel[I + 1] = EMERALD_INVALID;
                    // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
                    Playfield.pStatusAnimation[I + 1] = EMERALD_STONE | EMERALD_ANIM_CLEAN_LEFT;
                    // Aktuelles Element auf Animation "rechts"
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlStoneSink
------------------------------------------------------------------------------
Beschreibung: Steuert einen versinkenden Stein in den Sumpf.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlStoneSink(uint32_t I) {
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_STONE_SWAMP1) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STONE_SWAMP2;  // Von Phase 1 in 2
    } else if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_STONE_SWAMP2) {
        Playfield.pLevel[I] = EMERALD_SPACE;      // Stein ist versackt
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_SWAMP_STONE;      // Sumpf ist nun mit Stein besetzt
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlStoneSag
------------------------------------------------------------------------------
Beschreibung: Steuert einen durchsackenden Stein, der den Sumpf verlässt.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlStoneSag(uint32_t I) {
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        return;
    }
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_STONE_SWAMP1) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STONE_SWAMP2;  // Von Phase 1 in 2
    } else if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_STONE_SWAMP2) {
        Playfield.pLevel[I] = EMERALD_STONE;      // Stein ist durchgesackt
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN_SELF;
        ControlStone(I);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlSwampStone
------------------------------------------------------------------------------
Beschreibung: Steuert Sumpf, der einen Stein enthält.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSwampStone(uint32_t I) {

    if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Kann Stein in Space durchsacken?
        SDL_Log("%s: Stone can leave swamp in Space",__FUNCTION__);
        Playfield.pLevel[I] = EMERALD_SWAMP;    // Sumpf nun leer
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_STONE_SAG;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STONE_SWAMP1 | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SWAMP) {  // Kann Stein in anderen Sumpf durchsacken?
        if (Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] == EMERALD_ANIM_STAND) {
            SDL_Log("%s: Stone can leave swamp into another swamp",__FUNCTION__);
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STONE_SWAMP1;
        } else if (Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] == EMERALD_ANIM_STONE_SWAMP1) {
            SDL_Log("%s: P1 Stone can leave swamp into another swamp",__FUNCTION__);
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STONE_SWAMP2;
        } else if (Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] == EMERALD_ANIM_STONE_SWAMP2) {
            SDL_Log("%s: P2 Stone can leave swamp into another swamp",__FUNCTION__);
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STONE_SWAMP3;
        } else if (Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] == EMERALD_ANIM_STONE_SWAMP3) {
            SDL_Log("%s: P3 Stone can leave swamp into another swamp",__FUNCTION__);
            // Aktuelles Feld von Sumpf+Stone in Sumpf wandeln
            Playfield.pLevel[I] = EMERALD_SWAMP;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            // Unteres Feld von Sumpf in Sumpf+Stone wandeln
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_SWAMP_STONE;
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
        }
    }
}


