#include "EmeraldMine.h"
#include "explosion.h"
#include "mystd.h"
#include "slime.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlSlime
------------------------------------------------------------------------------
Beschreibung: Steuert Schleim.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSlime(uint32_t I) {
    uint32_t uAnimStatus;
    uint32_t uSelfStatus;
    int nRandom;

    uAnimStatus = Playfield.pStatusAnimation[I] & 0x0000FF00;
    uSelfStatus = Playfield.pStatusAnimation[I] & 0xFF000000;

    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        // SDL_Log("%s: ack double control",__FUNCTION__);
        return;
    }

    if ( (uSelfStatus == EMERALD_ANIM_BORN1) || (uSelfStatus == EMERALD_ANIM_BORN2) ) {
        // Schleim kann vom Replikator geboren werden, dann hier nichts machen
        return;
    }
    if (uAnimStatus == EMERALD_ANIM_STAND) {
        // Blockierter Schleim, der zwischen Phase 1 und 4 wechselt
        // Phasen:
        // 1: Schleim blockiert in Phase 1 (horizontal zusammenziehen)
        // 2: Schleim blockiert in Phase 2 (horizontal ausdehnen)
        // 3: Schleim blockiert in Phase 3 (vertikal zusammenziehen)
        // 4: Schleim blockiert in Phase 4 (vertikal ausdehnen)
        if (uSelfStatus == EMERALD_ANIM_SLIME_STAND_PHASE1) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_SLIME_STAND_PHASE2;
        } else if (uSelfStatus == EMERALD_ANIM_SLIME_STAND_PHASE2) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_SLIME_STAND_PHASE3;
        } else if (uSelfStatus == EMERALD_ANIM_SLIME_STAND_PHASE3) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_SLIME_STAND_PHASE4;
            PreparePlaySound(SOUND_SLIME_MOVE,I);
        } else {
            // Falls Schleim vorher etwas gegessen hat, dann versuchen die bisherige Richtung weiterzulaufen
            if (Playfield.pLastYamSlimeDirection[I] == EMERALD_LAST_YAM_DIR_RIGHT) {
                SlimeRight(I);
            } else if (Playfield.pLastYamSlimeDirection[I] == EMERALD_LAST_YAM_DIR_LEFT) {
                SlimeLeft(I);
            } else if (Playfield.pLastYamSlimeDirection[I] == EMERALD_LAST_YAM_DIR_UP) {
                SlimeUp(I);
            } else if (Playfield.pLastYamSlimeDirection[I] == EMERALD_LAST_YAM_DIR_DOWN) {
                SlimeDown(I);
            } else {
                // Ab hier versuchen eine neue Richtung einzuschlagen
                nRandom = randn(1,5);       // Ergibt Zufallszahl zwischen 1-5  (1 = links, 2 = oben, 3 = rechts, 4 = runter, 5 = weiter blockieren)
                switch (nRandom) {
                    case (1):               // links
                        SlimeLeft(I);
                        break;
                    case (2):               // oben
                        SlimeUp(I);
                        break;
                    case (3):               // rechts
                        SlimeRight(I);
                        break;
                    case (4):               // runter
                        SlimeDown(I);
                        break;
                    default:         // weiter blockieren
                        // SDL_Log("%s: default",__FUNCTION__);
                        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_SLIME_STAND_PHASE1;
                        break;
                }
            }
        }
    } else if (uAnimStatus == EMERALD_ANIM_SLIME_GO_LEFT_1) {
        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_LEFT | EMERALD_ANIM_CLEAN_RIGHT;   // Element auf neuem Platz bekommt bisherige Richtung
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
    } else if (uAnimStatus == EMERALD_ANIM_SLIME_GO_UP_1) {
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_UP | EMERALD_ANIM_CLEAN_DOWN;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_UP;
    } else if (uAnimStatus == EMERALD_ANIM_SLIME_GO_RIGHT_1) {
        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT | EMERALD_ANIM_CLEAN_LEFT;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
    } else if (uAnimStatus == EMERALD_ANIM_SLIME_GO_DOWN_1) {
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN | EMERALD_ANIM_CLEAN_UP;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
    } else if (uAnimStatus == EMERALD_ANIM_LEFT) {
        SlimeLeft(I);
    } else if (uAnimStatus == EMERALD_ANIM_UP) {
        SlimeUp(I);
    } else if (uAnimStatus == EMERALD_ANIM_RIGHT) {
        SlimeRight(I);
    } else if (uAnimStatus == EMERALD_ANIM_DOWN) {
        SlimeDown(I);
    } else {
        SDL_Log("%s: unhandled direction",__FUNCTION__);
    }
}


/*----------------------------------------------------------------------------
Name:           SlimeDown
------------------------------------------------------------------------------
Beschreibung: Schleim geht nach unten.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void SlimeDown(uint32_t I) {
    if (IS_SPACE(I + Playfield.uLevel_X_Dimension)) {   // Ist nach unten frei?
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = EMERALD_SLIME;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_NOTHING;
        Playfield.pLastStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN;
        // Aktuelles Element auf Animation "unten", Phase 1
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SLIME_GO_DOWN_1;
        Playfield.pLastYamSlimeDirection[I] = EMERALD_LAST_YAM_DIR_DOWN;
        Playfield.pSlimeElement[I + Playfield.uLevel_X_Dimension] = Playfield.pSlimeElement[I];
        PreparePlaySound(SOUND_SLIME_MOVE,I);
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Schleim ins Säurebecken?
        // SDL_Log("Slime falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = EMERALD_SLIME;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else if (IsSlimeEatElement(Playfield.pLevel[I + Playfield.uLevel_X_Dimension])) {
        HandleSlimeEat(I,EMERALD_ANIM_DOWN);
    } else {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_SLIME_STAND_PHASE1;
        Playfield.pLastYamSlimeDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
    }
}


/*----------------------------------------------------------------------------
Name:           SlimeUp
------------------------------------------------------------------------------
Beschreibung: Schleim geht nach oben.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void SlimeUp(uint32_t I) {
    if (IS_SPACE(I - Playfield.uLevel_X_Dimension)) {   // Ist nach oben frei?
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pInvalidElement[I - Playfield.uLevel_X_Dimension] = EMERALD_SLIME;
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_NOTHING;
        Playfield.pLastStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_UP;
        // Aktuelles Element auf Animation "oben", Phase 1
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SLIME_GO_UP_1;
        Playfield.pLastYamSlimeDirection[I] = EMERALD_LAST_YAM_DIR_UP;
        Playfield.pSlimeElement[I - Playfield.uLevel_X_Dimension] = Playfield.pSlimeElement[I];
        PreparePlaySound(SOUND_SLIME_MOVE,I);
    } else if (IsSlimeEatElement(Playfield.pLevel[I - Playfield.uLevel_X_Dimension])) {
        HandleSlimeEat(I,EMERALD_ANIM_UP);
    } else {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_SLIME_STAND_PHASE1;
        Playfield.pLastYamSlimeDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
    }
}


/*----------------------------------------------------------------------------
Name:           SlimeRight
------------------------------------------------------------------------------
Beschreibung: Schleim geht nach rechts.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void SlimeRight(uint32_t I) {
    if (IS_SPACE(I + 1)) {   // Ist nach rechts frei?
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I + 1] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pInvalidElement[I + 1] = EMERALD_SLIME;
        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_NOTHING;
        Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
        Playfield.pLastYamSlimeDirection[I] = EMERALD_LAST_YAM_DIR_RIGHT;
        // Aktuelles Element auf Animation "rechts", Phase 1
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SLIME_GO_RIGHT_1;
        Playfield.pSlimeElement[I + 1] = Playfield.pSlimeElement[I];
        PreparePlaySound(SOUND_SLIME_MOVE,I);
    } else if (IsSlimeEatElement(Playfield.pLevel[I + 1])) {
        HandleSlimeEat(I,EMERALD_ANIM_RIGHT);
    } else {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_SLIME_STAND_PHASE1;
        Playfield.pLastYamSlimeDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
    }
}


/*----------------------------------------------------------------------------
Name:           SlimeLeft
------------------------------------------------------------------------------
Beschreibung: Schleim geht nach links.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void SlimeLeft(uint32_t I) {
    if (IS_SPACE(I - 1)) {   // Ist nach links frei?
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I - 1] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pInvalidElement[I - 1] = EMERALD_SLIME;
        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_NOTHING;
        Playfield.pLastStatusAnimation[I - 1] = EMERALD_ANIM_LEFT;
        // Aktuelles Element auf Animation "links", Phase 1
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SLIME_GO_LEFT_1;
        Playfield.pLastYamSlimeDirection[I] = EMERALD_LAST_YAM_DIR_LEFT;
        Playfield.pSlimeElement[I - 1] = Playfield.pSlimeElement[I];
        PreparePlaySound(SOUND_SLIME_MOVE,I);
    } else if (IsSlimeEatElement(Playfield.pLevel[I - 1])) {
        HandleSlimeEat(I,EMERALD_ANIM_LEFT);
    } else {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_SLIME_STAND_PHASE1;
        Playfield.pLastYamSlimeDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
    }
}


/*----------------------------------------------------------------------------
Name:           IsSlimeEatElement
------------------------------------------------------------------------------
Beschreibung: Prüft, ob es sich bei dem angegebenen Element, um ein "essbares"
              Element für den Schleim handelt.
Parameter
      Eingang: uElement, uint16_t, Element, dass geprüft werden soll
      Ausgang: -
Rückgabewert:  bool, true = Element ist durch Schleim essbar,
                     false = Element kann nicht durch Schleim gegessen werden
Seiteneffekte: -
------------------------------------------------------------------------------*/
bool IsSlimeEatElement(uint16_t uElement) {
    switch (uElement) {
        case (EMERALD_EMERALD):
        case (EMERALD_RUBY):
        case (EMERALD_SAPPHIRE):
        case (EMERALD_PERL):
        case (EMERALD_CRYSTAL):
        case (EMERALD_SHIELD_COIN):
        case (EMERALD_TIME_COIN):
        case (EMERALD_DYNAMITE_OFF):
        case (EMERALD_HAMMER):
        case (EMERALD_KEY_RED):
        case (EMERALD_KEY_YELLOW):
        case (EMERALD_KEY_BLUE):
        case (EMERALD_KEY_GREEN):
        case (EMERALD_KEY_WHITE):
        case (EMERALD_KEY_GENERAL):
            return true;
        default:
            return false;
    }
}

/*----------------------------------------------------------------------------
Name:           HandleSlimeEat
------------------------------------------------------------------------------
Beschreibung: Falls der Schleim auf etwas "Essbares" läuft, wird in dieser Funktion
              der Fressvorgang durchgeführt und das Element entfernt.

Parameter
      Eingang: I, uint32_t, Index im Level
               uAnimation, uint32_t, in diese Richtung wird gefressen, z.B. EMERALD_ANIM_LEFT für links
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void HandleSlimeEat(uint32_t I, uint32_t uAnimation) {
    uint32_t uDestCoord;    // Koordinate, an der Element gefressen wird
    uint32_t uShrinkAnimation;
    uint8_t  uLastSlimeDirection;
    bool bEat = false;

    switch (uAnimation) {
        case (EMERALD_ANIM_LEFT):
            uDestCoord = I - 1;
            uLastSlimeDirection = EMERALD_LAST_YAM_DIR_LEFT;
            break;
        case (EMERALD_ANIM_RIGHT):
            uDestCoord = I + 1;
            uLastSlimeDirection = EMERALD_LAST_YAM_DIR_RIGHT;
            break;
        case (EMERALD_ANIM_UP):
            uDestCoord = I - Playfield.uLevel_X_Dimension;
            uLastSlimeDirection = EMERALD_LAST_YAM_DIR_UP;
            break;
        case (EMERALD_ANIM_DOWN):
            uDestCoord = I + Playfield.uLevel_X_Dimension;
            uLastSlimeDirection = EMERALD_LAST_YAM_DIR_DOWN;
            break;
        default:
            uDestCoord = 0xFFFFFFFF;
            SDL_Log("%s: invalid animation (0x%x)",__FUNCTION__,uAnimation);
            break;
    }
    // gültige Animation und vorher noch nichts gefressen?
    if ((uDestCoord != 0xFFFFFFFF) && (Playfield.pSlimeElement[I] == EMERALD_NONE)) {
        uShrinkAnimation = GetShrinkByElement(Playfield.pLevel[uDestCoord]);
        bEat = ((uShrinkAnimation != EMERALD_NO_ADDITIONAL_ANIMSTATUS) &&
                ((Playfield.pStatusAnimation[uDestCoord] == EMERALD_ANIM_STAND) || (Playfield.pStatusAnimation[uDestCoord] == EMERALD_ANIM_BLITZ)));
    }
    if (bEat) {
        Playfield.pStatusAnimation[uDestCoord] = uShrinkAnimation;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_SLIME_STAND_PHASE3;   // direkt Phase 3, d.h. 2 Runden weniger warten
        Playfield.pLastYamSlimeDirection[I] = uLastSlimeDirection;
        Playfield.pSlimeElement[I] = Playfield.pLevel[uDestCoord];
        PreparePlaySound(SOUND_SLIME_SLURP,I);
    } else {
        // Aufgrund eines Fehlers oder weil Schleim schon etwas gefressen hat muss er hier blockiert werden
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_SLIME_STAND_PHASE1;
        Playfield.pLastYamSlimeDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
    }
}


/*----------------------------------------------------------------------------
Name:           GetShrinkByElement
------------------------------------------------------------------------------
Beschreibung: Ermittelt anhand eines Elements die entsprechende "Schrumpf"-Animation.

Parameter
      Eingang: uElement, uint16_t, Element, für das die Shrink-Animation ermittelt werden soll.
      Ausgang: -
Rückgabewert:  uint32_t, Shrink-Animation
               EMERALD_NO_ADDITIONAL_ANIMSTATUS = Shrink-Animation konnte nicht ermittelt werden
Seiteneffekte: -
------------------------------------------------------------------------------*/
uint32_t GetShrinkByElement(uint16_t uElement) {
    uint32_t uShrink;

    switch (uElement) {
        case (EMERALD_EMERALD):
            uShrink = EMERALD_ANIM_EMERALD_SHRINK;
            break;
        case (EMERALD_RUBY):
            uShrink = EMERALD_ANIM_RUBY_SHRINK;
            break;
        case (EMERALD_SAPPHIRE):
            uShrink = EMERALD_ANIM_SAPPHIRE_SHRINK;
            break;
        case (EMERALD_PERL):
            uShrink = EMERALD_ANIM_PERL_SHRINK;
            break;
        case (EMERALD_CRYSTAL):
            uShrink = EMERALD_ANIM_CRYSTAL_SHRINK;
            break;
        case (EMERALD_SHIELD_COIN):
            uShrink = EMERALD_ANIM_SHIELD_COIN_SHRINK;
            break;
        case (EMERALD_TIME_COIN):
            uShrink = EMERALD_ANIM_TIME_COIN_SHRINK;
            break;
        case (EMERALD_DYNAMITE_OFF):
            uShrink = EMERALD_ANIM_DYNAMITE_SHRINK;
            break;
        case (EMERALD_HAMMER):
            uShrink = EMERALD_ANIM_HAMMER_SHRINK;
            break;
        case (EMERALD_KEY_RED):
            uShrink = EMERALD_ANIM_KEY_RED_SHRINK;
            break;
        case (EMERALD_KEY_YELLOW):
            uShrink = EMERALD_ANIM_KEY_YELLOW_SHRINK;
            break;
        case (EMERALD_KEY_BLUE):
            uShrink = EMERALD_ANIM_KEY_BLUE_SHRINK;
            break;
        case (EMERALD_KEY_GREEN):
            uShrink = EMERALD_ANIM_KEY_GREEN_SHRINK;
            break;
        case (EMERALD_KEY_WHITE):
            uShrink = EMERALD_ANIM_KEY_WHITE_SHRINK;
            break;
        case (EMERALD_KEY_GENERAL):
            uShrink = EMERALD_ANIM_KEY_GENERAL_SHRINK;
            break;
        default:
            uShrink = EMERALD_NO_ADDITIONAL_ANIMSTATUS;
            SDL_Log("%s: invalid element (0x%x) to shrink",__FUNCTION__,uElement);
            break;
    }
    return uShrink;
}


/*----------------------------------------------------------------------------
Name:           ControlSlimeExplosion
------------------------------------------------------------------------------
Beschreibung: Schleim wird durch einen Stein getroffen. Diese Funktion lässt dann
              eine normale 3x3 Explosion oder eine 3x3 Explosion mit mittigem Objekt
              entstehen, in Abhängigkeit, ob der Schleim etwas gefressen hatte.

Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSlimeExplosion(uint32_t I) {

    // SDL_Log("Slime Explosion element: 0x%04x",Playfield.pSlimeElement[I]);
    Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreStoningSlime;
    if (Playfield.pSlimeElement[I] == EMERALD_NONE) {
        ControlCentralExplosion(I,EMERALD_SPACE);
    } else {
        ControlCentralExplosion(I,Playfield.pSlimeElement[I]);
    }
    PreparePlaySound(SOUND_EXPLOSION,I);
}
