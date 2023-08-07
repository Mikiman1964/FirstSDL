#include "EmeraldMine.h"
#include "explosion.h"
#include "greendrop.h"
#include "mine.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlMineUp
------------------------------------------------------------------------------
Beschreibung: Steuert eine nach oben laufende Mine im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMineUp(uint32_t I) {
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MINE_WILL_EXPLODE) {
        ControlCentralExplosion(I);
        PreparePlaySound(SOUND_EXPLOSION,I);
        return; // Für die Mine ist das Spiel hier zu Ende
    }
    // Hat Mine Kontakt zu grünem Käse ?
    if (IsGreenCheeseAround(I)) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MINE_WILL_EXPLODE | EMERALD_ANIM_SPIN_UP_TO_RIGHT;
        return; // Für die Mine ist das Spiel nächste Runde zu Ende
    }
    // Hatte Mine vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE) {
        Playfield.pStatusAnimation[I] = 0;
        if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach oben frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I - Playfield.uLevel_X_Dimension] = EMERALD_MINE_UP;
            Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_DOWN;
            // Aktuelles Element auf Animation "oben"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_UP;
        } else {                        // Oben ist nicht frei
            // Mine bleibt zunächst auf "oben" muss sich aber bei Blockade nach rechts drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_RIGHT;
        }
    }
    else if (Playfield.pLevel[I - 1] != EMERALD_SPACE) { // // Hat Mine links Führung?
        if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach oben frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I - Playfield.uLevel_X_Dimension] = EMERALD_MINE_UP;
            Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_DOWN;
            // Aktuelles Element auf Animation "oben"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_UP;
        } else {                        // Oben ist nicht frei
            // Mine bleibt zunächst auf "oben" muss sich aber bei Blockade nach rechts drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_RIGHT;
        }
    } else {                      // Linke Wand verloren
        // Mine bleibt zunächst auf "oben" muss sich aber bei Wand-Verlust nach links drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_LEFT | EMERALD_ANIM_LOST_GUIDE;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlMineRight
------------------------------------------------------------------------------
Beschreibung: Steuert eine nach rechts laufende Mine im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMineRight(uint32_t I) {
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MINE_WILL_EXPLODE) {
        ControlCentralExplosion(I);
        PreparePlaySound(SOUND_EXPLOSION,I);
        return; // Für die Mine ist das Spiel hier zu Ende
    }
    // Hat Mine Kontakt zu grünem Käse ?
    if (IsGreenCheeseAround(I)) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MINE_WILL_EXPLODE | EMERALD_ANIM_SPIN_RIGHT_TO_DOWN;
        return; // Für die Mine ist das Spiel nächste Runde zu Ende
    }
    // Hatte Mine vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE) {
        Playfield.pStatusAnimation[I] = 0;
        if (Playfield.pLevel[I + 1] == EMERALD_SPACE) {    // Ist rechts frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I + 1] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I + 1] = EMERALD_MINE_RIGHT;
            Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
            Playfield.pLastStatusAnimation[I + 1] = Playfield.pStatusAnimation[I];
            // Aktuelles Element auf Animation "rechts"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
        } else {                            // Rechts ist nicht frei
            // Mine bleibt zunächst auf "rechts" muss sich aber bei Blockade nach unten drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_DOWN;
        }
    } else if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] != EMERALD_SPACE) {  // Hat Mine links Führung?
        if (Playfield.pLevel[I + 1] == EMERALD_SPACE) {    // Ist nach rechts frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I + 1] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I + 1] = EMERALD_MINE_RIGHT;
            Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT;
            Playfield.pLastStatusAnimation[I + 1] = Playfield.pStatusAnimation[I];
            // Aktuelles Element auf Animation "rechts"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
        } else {                            // Rechts ist nicht frei
            // Mine bleibt zunächst auf "rechts" muss sich aber bei Blockade nach unten drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_DOWN;
        }
    } else {                          // Linke Wand verloren
        // Mine bleibt zunächst auf "rechts" muss sich aber bei Wand-Verlust nach oben drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_UP | EMERALD_ANIM_LOST_GUIDE;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlMineDown
------------------------------------------------------------------------------
Beschreibung: Steuert eine nach unten laufende Mine im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMineDown(uint32_t I) {
    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) || ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ) {
        // MineDown kann vom Replikator geboren werden, dann hier nichts machen
        return;
    }
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MINE_WILL_EXPLODE) {
        ControlCentralExplosion(I);
        PreparePlaySound(SOUND_EXPLOSION,I);
        return; // Für die Mine ist das Spiel hier zu Ende
    }
    // Hat Mine Kontakt zu grünem Käse ?
    if (IsGreenCheeseAround(I)) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MINE_WILL_EXPLODE | EMERALD_ANIM_SPIN_DOWN_TO_LEFT;
        return; // Für die Mine ist das Spiel nächste Runde zu Ende
    }
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE) {

        // Hatte Mine vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
        Playfield.pStatusAnimation[I] = 0;
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist nach unten frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = EMERALD_MINE_DOWN;
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_UP;
            Playfield.pLastStatusAnimation[I + Playfield.uLevel_X_Dimension] = Playfield.pStatusAnimation[I];
            // Aktuelles Element auf Animation "unten"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
        } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Mine ins Säurebecken?
            SDL_Log("Mine falls in pool");
            Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
            Playfield.pInvalidElement[I] = EMERALD_MINE_DOWN;
            PreparePlaySound(SOUND_POOL_BLUB,I);
            return;
        } else {                            // Unten ist nicht frei
            // Mine bleibt zunächst auf "unten" muss sich aber bei Blockade nach links drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_LEFT;
        }
    } else if (Playfield.pLevel[I + 1] != EMERALD_SPACE) {   // Links von Mine irgendwas?
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist nach unten frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = EMERALD_MINE_DOWN;
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_UP;
            Playfield.pLastStatusAnimation[I + Playfield.uLevel_X_Dimension] = Playfield.pStatusAnimation[I];
            // Aktuelles Element auf Animation "unten"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
        } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Mine ins Säurebecken?
            SDL_Log("Mine falls in pool");
            Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
            Playfield.pInvalidElement[I] = EMERALD_MINE_DOWN;
            PreparePlaySound(SOUND_POOL_BLUB,I);
            return;
        } else {                            // Unten ist nicht frei
            // Mine bleibt zunächst auf "unten" muss sich aber bei Blockade nach links drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_LEFT;
        }
    } else {                          // Linke Wand verloren
        // Mine bleibt zunächst auf "unten" muss sich aber bei Wand-Verlust nach rechts drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_RIGHT | EMERALD_ANIM_LOST_GUIDE;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlMineLeft
------------------------------------------------------------------------------
Beschreibung: Steuert eine nach links laufende Mine im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMineLeft(uint32_t I) {
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MINE_WILL_EXPLODE) {
        ControlCentralExplosion(I);
        PreparePlaySound(SOUND_EXPLOSION,I);
        return; // Für die Mine ist das Spiel hier zu Ende
    }
    // Hat Mine Kontakt zu grünem Käse ?
    if (IsGreenCheeseAround(I)) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MINE_WILL_EXPLODE | EMERALD_ANIM_SPIN_LEFT_TO_UP;
        return; // Für die Mine ist das Spiel nächste Runde zu Ende
    }
    // Hatte Mine vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE) {
        Playfield.pStatusAnimation[I] = 0;
        if (Playfield.pLevel[I - 1] == EMERALD_SPACE) {    // Ist nach links frei?
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I - 1] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I - 1] = EMERALD_MINE_LEFT;
            Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
            // Aktuelles Element auf Animation "links"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
        } else {                           // Links ist nicht frei
            // Mine bleibt zunächst auf "links" muss sich aber bei Blockade nach oben drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_UP;
        }
    } else if ( (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] != EMERALD_SPACE) &&
                (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] != EMERALD_ACIDPOOL) ) {   // Links von Mine irgendwas? AcidPool ist wie Space.
        if (Playfield.pLevel[I - 1] == EMERALD_SPACE)    // Ist nach links frei?
        {
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I - 1] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I - 1] = EMERALD_MINE_LEFT;
            Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
            // Aktuelles Element auf Animation "links"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
        } else {                            // Links ist nicht frei
            // Mine bleibt zunächst auf "links" muss sich aber bei Blockade nach oben drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_UP;
        }
    } else {                          // Linke Wand verloren
        // Mine bleibt zunächst auf "links" muss sich aber bei Wand-Verlust nach unten drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_DOWN | EMERALD_ANIM_LOST_GUIDE;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlStandMine
------------------------------------------------------------------------------
Beschreibung: Steuert eine Standmine. Sobald sich ein Objekt neben dieser Mine
              befindet, wird eine Explosion ausgelöst.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlStandMine(uint32_t I) {

    if ( IsStandMineExplode(Playfield.pLevel[I - 1]) || IsStandMineExplode(Playfield.pLevel[I - Playfield.uLevel_X_Dimension]) ||
         IsStandMineExplode(Playfield.pLevel[I + 1]) || IsStandMineExplode(Playfield.pLevel[I + Playfield.uLevel_X_Dimension]) ) {
        SDL_Log("%s: Stand mine at position %d exploding",__FUNCTION__,I);
        ControlCentralExplosion(I);
        PreparePlaySound(SOUND_EXPLOSION,I);
    }
}


/*----------------------------------------------------------------------------
Name:           IsStandMineExplode
------------------------------------------------------------------------------
Beschreibung: Prüft für eine Standmine, ob sich "Monster" oder der Man in der Nähe der Mine befinden.
Parameter
      Eingang: uElement, uint16_t, Element
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
bool IsStandMineExplode(uint16_t uElement) {
    return ( (uElement == EMERALD_ALIEN) || (uElement == EMERALD_YAM) ||
             (uElement == EMERALD_MINE_UP) || (uElement == EMERALD_MINE_RIGHT) || (uElement == EMERALD_MINE_DOWN) || (uElement == EMERALD_MINE_LEFT) ||
             (uElement == EMERALD_MOLE_UP) || (uElement == EMERALD_MOLE_RIGHT) || (uElement == EMERALD_MOLE_DOWN) || (uElement == EMERALD_MOLE_LEFT) ||
             (uElement == EMERALD_BEETLE_UP) || (uElement == EMERALD_BEETLE_RIGHT) || (uElement == EMERALD_BEETLE_DOWN) || (uElement == EMERALD_BEETLE_LEFT) ||
             (uElement == EMERALD_GREEN_CHEESE)
             );
}


/*----------------------------------------------------------------------------
Name:           ControlDynamiteOn
------------------------------------------------------------------------------
Beschreibung: Steuert ein angezündetes Dynamit und bringt es zur Explosion.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlDynamiteOn(uint32_t I) {

    switch (Playfield.pStatusAnimation[I] & 0xFF000000) {
        case (EMERALD_ANIM_DYNAMITE_ON_P1):
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_DYNAMITE_ON_P2;    // Von Phase 1 auf 2 schalten
            PreparePlaySound(SOUND_DYNAMITE,I);
            break;
        case (EMERALD_ANIM_DYNAMITE_ON_P2):
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_DYNAMITE_ON_P3;    // Von Phase 2 auf 3 schalten
            PreparePlaySound(SOUND_DYNAMITE,I);
            break;
        case (EMERALD_ANIM_DYNAMITE_ON_P3):
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_DYNAMITE_ON_P4;    // Von Phase 3 auf 4 schalten
            PreparePlaySound(SOUND_DYNAMITE,I);
            break;
        case (EMERALD_ANIM_DYNAMITE_ON_P4):
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            ControlCentralExplosion(I);
            PreparePlaySound(SOUND_EXPLOSION,I);
            break;
        default:
            SDL_Log("%s: Warning, unhandled status: 0x%x",__FUNCTION__,Playfield.pStatusAnimation[I] & 0xFF000000);
            break;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlManWithDynamiteOn
------------------------------------------------------------------------------
Beschreibung: Steuert das Kombi-Element Man/gezündetes Dynamit.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlManWithDynamiteOn(uint32_t I) {

    switch (Playfield.uDynamiteStatusAnim) {
        case (EMERALD_ANIM_DYNAMITE_START):
            Playfield.uDynamiteStatusAnim = EMERALD_ANIM_DYNAMITE_ON_P1;    // Von Start auf Phase 1 schalten
            PreparePlaySound(SOUND_DYNAMITE,I);
            SDL_Log("%s: Start to P1",__FUNCTION__);
            break;
        case (EMERALD_ANIM_DYNAMITE_ON_P1):
            Playfield.uDynamiteStatusAnim = EMERALD_ANIM_DYNAMITE_ON_P2;    // Von Phase 1 auf 2 schalten
            PreparePlaySound(SOUND_DYNAMITE,I);
            SDL_Log("%s: to P2",__FUNCTION__);
            break;
        case (EMERALD_ANIM_DYNAMITE_ON_P2):
            Playfield.uDynamiteStatusAnim = EMERALD_ANIM_DYNAMITE_ON_P3;    // Von Phase 2 auf 3 schalten
            PreparePlaySound(SOUND_DYNAMITE,I);
            SDL_Log("%s: to P3",__FUNCTION__);
            break;
        case (EMERALD_ANIM_DYNAMITE_ON_P3):
            Playfield.uDynamiteStatusAnim = EMERALD_ANIM_DYNAMITE_ON_P4;    // Von Phase 3 auf 4 schalten
            PreparePlaySound(SOUND_DYNAMITE,I);
            SDL_Log("%s: to P4",__FUNCTION__);
            break;
        case (EMERALD_ANIM_DYNAMITE_ON_P4):
            Playfield.uDynamiteStatusAnim = EMERALD_ANIM_STAND;
            Playfield.uDynamitePos = 0xFFFFFFFF;
            if ((Playfield.uManXpos + Playfield.uManYpos * Playfield.uLevel_X_Dimension) == I) {   // Ist Man auf selbst gezündeten Dynamit stehen geblieben?
                Playfield.bManDead = true;
                Playfield.pLevel[I] = EMERALD_CENTRAL_EXPLOSION;
                PreparePlaySound(SOUND_MAN_CRIES,I);
            }
            // Folgender Aufruf hatte zum Ergebnis, dass z.B. eine darüberliegende Standmine in derselben Steuerungsphase mit explodiert.
            // Darunter liegende Standmine explodiert erst in der nächsten Phase. (Richtig)
            // ControlCentralExplosion(I);
            PreparePlaySound(SOUND_EXPLOSION,I);
            break;
        default:
            SDL_Log("%s: Warning, unhandled status: 0x%x",__FUNCTION__,Playfield.pStatusAnimation[I] & 0xFF000000);
            break;
    }
}
