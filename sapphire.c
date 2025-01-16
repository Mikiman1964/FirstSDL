#include "EmeraldMine.h"
#include "explosion.h"
#include "magicwall.h"
#include "man.h"
#include "sapphire.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlSaphir
------------------------------------------------------------------------------
Beschreibung: Steuert einen Saphir.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSaphir(uint32_t I) {
    uint8_t uFree;  // Richtung, in die Saphir rollen könnte: 0 = kann nicht rollen, 1 = kann links rollen, 2 = kann rechts rollen, 3 = kann links und rechts rollen
    uint16_t uHitElement;       // Element, welches getroffen wird
    uint32_t uHitCoordinate;    // Lineare Koordinate des getroffenen Elements

    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        // SDL_Log("%s: ack double control",__FUNCTION__);
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
            ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_SAPPHIRE_SHRINK) ) {
        // Saphir kann vom Replikator geboren werden, dann hier nichts machen
        // Saphir kann durch Man "geshrinkt" werden, dann hier auch nichts machen
        return;
    } else if (IS_SPACE(I + Playfield.uLevel_X_Dimension)) {   // Ist nach unten frei?
        SetElementToNextPosition(I,EMERALD_ANIM_DOWN,EMERALD_ANIM_DOWN_SELF | EMERALD_ANIM_CLEAN_UP,EMERALD_SAPPHIRE);
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Saphir ins Säurebecken?
        // SDL_Log("Saphir falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = EMERALD_SAPPHIRE;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else {                            // Unten ist nicht frei
        // Saphir bleibt zunächst auf Platz liegen
        uHitCoordinate = I + Playfield.uLevel_X_Dimension;
        uHitElement = Playfield.pLevel[uHitCoordinate];
        if ( (Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_DOWN_SELF) {
            Playfield.pStatusAnimation[I] &= 0x00FFFFFF;
            if ((uHitElement == EMERALD_MAGIC_WALL) || (uHitElement == EMERALD_MAGIC_WALL_STEEL)) { // Saphir trifft auf Magic wall
                if (Playfield.bMagicWallRunning) {
                    // SDL_Log("Sapphire hit running magic wall");
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SINK_IN_MAGIC_WALL;
                    ElementGoesMagicWall(I,EMERALD_STONE);
                    PreparePlaySound(SOUND_SQUEAK,I);
                } else if ((!Playfield.bMagicWallWasOn) && (Playfield.uTimeMagicWall > 0)) {
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SINK_IN_MAGIC_WALL;
                    // SDL_Log("Sapphire start magic wall");
                    Playfield.bMagicWallWasOn = true;
                    Playfield.uTimeMagicWallLeft = Playfield.uTimeMagicWall;
                    Playfield.bMagicWallRunning = true;
                    ElementGoesMagicWall(I,EMERALD_STONE);
                    PreparePlaySound(SOUND_SQUEAK,I);
                } else {
                    // SDL_Log("Sapphire hit used magic wall");
                    PreparePlaySound(SOUND_PING,I);
                }
            } else if ((uHitElement == EMERALD_MAN) && (!Playfield.bManProtected) && (Playfield.uShieldCoinTimeLeft == 0)) {
                // SDL_Log("Sapphire kills man");
                Playfield.pLevel[uHitCoordinate] = EMERALD_MAN_DIES;
                Playfield.pStatusAnimation[uHitCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL | EMERALD_ANIM_MAN_DIES_P1;
                PreparePlaySound(SOUND_MAN_CRIES,I);
                Playfield.bManDead = true;
            } else if (uHitElement == EMERALD_MINE_CONTACT) {
                // SDL_Log("Sapphire hit contact mine");
                ControlCentralExplosion(uHitCoordinate,EMERALD_SPACE);
                PreparePlaySound(SOUND_EXPLOSION,I);
            } else {
                PreparePlaySound(SOUND_PING,I);
            }
        }
        if (((Playfield.uRollUnderground[uHitElement] & EMERALD_CHECKROLL_SAPPHIRE) != 0) || (Playfield.pPipeLevel[uHitCoordinate] != EMERALD_SPACE)) {
            uFree = GetFreeRollDirections(I);
            if (uFree == 1) {   // Saphir kann links rollen
                SetElementToNextPosition(I,EMERALD_ANIM_LEFT,EMERALD_ANIM_CLEAN_RIGHT,EMERALD_SAPPHIRE);
            } else if (uFree == 2) {    // Saphir kann rechts rollen
                SetElementToNextPosition(I,EMERALD_ANIM_RIGHT,EMERALD_ANIM_CLEAN_LEFT,EMERALD_SAPPHIRE);
            } else if (uFree == 3) {    // Saphir kann in beide Richtungen rollen
                // Hier entscheiden, ob links oder rechts gerollt wird
                if ((rand() & 0x01) == 0) {   // links
                    SetElementToNextPosition(I,EMERALD_ANIM_LEFT,EMERALD_ANIM_CLEAN_RIGHT,EMERALD_SAPPHIRE);
                } else {                    // rechts
                    SetElementToNextPosition(I,EMERALD_ANIM_RIGHT,EMERALD_ANIM_CLEAN_LEFT,EMERALD_SAPPHIRE);
                }
            }
        // Ab hier prüfen, ob Saphir durch Laufband bewegt werden kann
        } else if ((IS_SPACE(I - 1)) && (((uHitElement == EMERALD_CONVEYORBELT_RED) && (Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_LEFT)) ||
                                         ((uHitElement == EMERALD_CONVEYORBELT_YELLOW) && (Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_LEFT)) ||
                                         ((uHitElement == EMERALD_CONVEYORBELT_GREEN) && (Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_LEFT)) ||
                                         ((uHitElement == EMERALD_CONVEYORBELT_BLUE) && (Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_LEFT)))) {
            SetElementToNextPosition(I,EMERALD_ANIM_LEFT,EMERALD_ANIM_CLEAN_RIGHT,EMERALD_SAPPHIRE);
        } else if ((IS_SPACE(I + 1)) && (((uHitElement == EMERALD_CONVEYORBELT_RED) && (Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_RIGHT)) ||
                                         ((uHitElement == EMERALD_CONVEYORBELT_YELLOW) && (Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_RIGHT)) ||
                                         ((uHitElement == EMERALD_CONVEYORBELT_GREEN) && (Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_RIGHT)) ||
                                         ((uHitElement == EMERALD_CONVEYORBELT_BLUE) && (Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_RIGHT)))) {
            SetElementToNextPosition(I,EMERALD_ANIM_RIGHT,EMERALD_ANIM_CLEAN_LEFT,EMERALD_SAPPHIRE);
        } else {
            // SDL_Log("Sapphire sleeps, Hitelement = %x",uHitElement);
        }
    }
}
