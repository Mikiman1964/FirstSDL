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

    if (  ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) ||
          ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2)  ) {
        // Bombe kann vom Replikator geboren werden, dann hier nichts machen
        return;
    } else if (IS_SPACE(uHitCoordinate)) {   // Ist nach unten frei?
        SetElementToNextPosition(I,EMERALD_ANIM_DOWN,EMERALD_ANIM_DOWN_SELF | EMERALD_ANIM_CLEAN_UP,EMERALD_BOMB);
    } else if (Playfield.pLevel[uHitCoordinate] == EMERALD_ACIDPOOL) {   // Fällt Bombe ins Säurebecken?
        // SDL_Log("Bomb falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = EMERALD_BOMB;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else {                            // Unten ist nicht frei
        // Bombe bleibt zunächst auf Platz liegen
        if ( (Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_DOWN_SELF) {
            // Bombe fällt auf Etwas
            ControlCentralExplosion(I,EMERALD_SPACE);
            PreparePlaySound(SOUND_EXPLOSION,I);
            return;
        }
        if ((Playfield.uRollUnderground[uHitElement] & EMERALD_CHECKROLL_BOMB) != 0) {
            uFree = GetFreeRollDirections(I);
            if (uFree == 1) {   // Bombe kann links rollen
                SetElementToNextPosition(I,EMERALD_ANIM_LEFT,EMERALD_ANIM_CLEAN_RIGHT,EMERALD_BOMB);
            } else if (uFree == 2) {    // Bombe kann rechts rollen
                SetElementToNextPosition(I,EMERALD_ANIM_RIGHT,EMERALD_ANIM_CLEAN_LEFT,EMERALD_BOMB);
            } else if (uFree == 3) {    // Bombe kann in beide Richtungen rollen
                // Hier entscheiden, ob links oder rechts gerollt wird
                if ((rand() & 0x01) == 0) {   // links
                    SetElementToNextPosition(I,EMERALD_ANIM_LEFT,EMERALD_ANIM_CLEAN_RIGHT,EMERALD_BOMB);
                } else {                    // rechts
                    SetElementToNextPosition(I,EMERALD_ANIM_RIGHT,EMERALD_ANIM_CLEAN_LEFT,EMERALD_BOMB);
                }
            }
        // Ab hier prüfen, ob Bombe durch Laufband bewegt werden kann
        } else if ((IS_SPACE(I - 1)) && (((uHitElement == EMERALD_CONVEYORBELT_RED) && (Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_LEFT)) ||
                                         ((uHitElement == EMERALD_CONVEYORBELT_YELLOW) && (Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_LEFT)) ||
                                         ((uHitElement == EMERALD_CONVEYORBELT_GREEN) && (Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_LEFT)) ||
                                         ((uHitElement == EMERALD_CONVEYORBELT_BLUE) && (Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_LEFT)))) {
            SetElementToNextPosition(I,EMERALD_ANIM_LEFT,EMERALD_ANIM_CLEAN_RIGHT,EMERALD_BOMB);
        } else if ((IS_SPACE(I + 1)) && (((uHitElement == EMERALD_CONVEYORBELT_RED) && (Playfield.uConveybeltRedState == EMERALD_CONVEYBELT_RIGHT)) ||
                                         ((uHitElement == EMERALD_CONVEYORBELT_YELLOW) && (Playfield.uConveybeltYellowState == EMERALD_CONVEYBELT_RIGHT)) ||
                                         ((uHitElement == EMERALD_CONVEYORBELT_GREEN) && (Playfield.uConveybeltGreenState == EMERALD_CONVEYBELT_RIGHT)) ||
                                         ((uHitElement == EMERALD_CONVEYORBELT_BLUE) && (Playfield.uConveybeltBlueState == EMERALD_CONVEYBELT_RIGHT)))) {
            SetElementToNextPosition(I,EMERALD_ANIM_RIGHT,EMERALD_ANIM_CLEAN_LEFT,EMERALD_BOMB);
        } else {
          // SDL_Log("Bomb sleeps, Hitelement = %x",uHitElement);
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
        ControlCentralExplosion(I,EMERALD_SPACE);
        PreparePlaySound(SOUND_EXPLOSION,I);
    } else if ((Playfield.bSwitchRemoteBombUp) && (IS_SPACE(I - Playfield.uLevel_X_Dimension))) { // Ist nach oben frei?
        SetElementToNextPosition(I,EMERALD_ANIM_UP,EMERALD_ANIM_CLEAN_DOWN,EMERALD_REMOTEBOMB);
        Playfield.bRemoteBombMoved = true;
    } else if (Playfield.bSwitchRemoteBombDown) {
        if (IS_SPACE(I + Playfield.uLevel_X_Dimension)) { // Ist nach unten frei?
            SetElementToNextPosition(I,EMERALD_ANIM_DOWN,EMERALD_ANIM_CLEAN_UP,EMERALD_REMOTEBOMB);
            Playfield.bRemoteBombMoved = true;
        } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt ferngesteuerte Bombe ins Säurebecken?
            // SDL_Log("Remote bomb falls in pool");
            Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
            Playfield.pInvalidElement[I] = EMERALD_REMOTEBOMB;
            PreparePlaySound(SOUND_POOL_BLUB,I + Playfield.uLevel_X_Dimension);
            return;
        }
    } else if ((Playfield.bSwitchRemoteBombLeft) && (IS_SPACE(I - 1))) {    // Ist nach links frei?
        SetElementToNextPosition(I,EMERALD_ANIM_LEFT,EMERALD_ANIM_CLEAN_RIGHT,EMERALD_REMOTEBOMB);
        Playfield.bRemoteBombMoved = true;
    } else if ((Playfield.bSwitchRemoteBombRight) && (IS_SPACE(I + 1))) {    // Ist nach rechts frei?
        SetElementToNextPosition(I,EMERALD_ANIM_RIGHT,EMERALD_ANIM_CLEAN_LEFT,EMERALD_REMOTEBOMB);
        Playfield.bRemoteBombMoved = true;
    }
}
