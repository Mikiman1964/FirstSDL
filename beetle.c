#include "EmeraldMine.h"
#include "beetle.h"
#include "drop.h"
#include "explosion.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlBeetleUp
------------------------------------------------------------------------------
Beschreibung: Steuert einen nach oben laufenden Käfer im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlBeetleUp(uint32_t I) {
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BEETLE_WILL_EXPLODE) {
        ControlCentralBeetleExplosion(I);
        PreparePlaySound(SOUND_EXPLOSION,I);
        return; // Für den Käfer ist das Spiel hier zu Ende
    }
    // Hat Käfer Kontakt zu grünem/gelbem Käse ?
    if (IsCheeseAround(I)) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_BEETLE_WILL_EXPLODE | EMERALD_ANIM_SPIN_UP_TO_LEFT;
        return; // Für den Käfer ist das Spiel nächste Runde zu Ende
    }
    // Hatte Käfer vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE) {
        Playfield.pStatusAnimation[I] = 0;
        if (IS_SPACE(I - Playfield.uLevel_X_Dimension)) {  // Ist nach oben frei?
            // neuen Platz mit ungültigem Element besetzen
            SetElementToNextPosition(I,EMERALD_ANIM_UP,EMERALD_ANIM_CLEAN_DOWN,EMERALD_BEETLE_UP);
        } else {                        // Oben ist nicht frei
            // Käfer bleibt zunächst auf "oben" muss sich aber bei Blockade nach links drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_LEFT;
        }
    } else if (!IS_SPACE(I + 1)) { // // Hat Käfer rechts Führung?
        if (IS_SPACE(I - Playfield.uLevel_X_Dimension)) {  // Ist nach oben frei?
            SetElementToNextPosition(I,EMERALD_ANIM_UP,EMERALD_ANIM_CLEAN_DOWN,EMERALD_BEETLE_UP);
        } else {                        // Oben ist nicht frei
            // Käfer bleibt zunächst auf "oben" muss sich aber bei Blockade nach links drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_LEFT;
        }
    } else {                      // Rechte Wand verloren
        // Käfer bleibt zunächst auf "oben" muss sich aber bei Wand-Verlust nach rechts drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_RIGHT | EMERALD_ANIM_LOST_GUIDE;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlBeetleRight
------------------------------------------------------------------------------
Beschreibung: Steuert einen nach rechts laufenden Käfer im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlBeetleRight(uint32_t I) {
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BEETLE_WILL_EXPLODE) {
        ControlCentralBeetleExplosion(I);
        PreparePlaySound(SOUND_EXPLOSION,I);
        return; // Für den Käfer ist das Spiel hier zu Ende
    }
    // Hat Käfer Kontakt zu grünem/gelbem Käse ?
    if (IsCheeseAround(I)) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_BEETLE_WILL_EXPLODE | EMERALD_ANIM_SPIN_RIGHT_TO_UP;
        return; // Für den Käfer ist das Spiel nächste Runde zu Ende
    }
    // Hatte Käfer vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE) {
        Playfield.pStatusAnimation[I] = 0;
        if (IS_SPACE(I + 1)) {    // Ist rechts frei?
            SetElementToNextPosition(I,EMERALD_ANIM_RIGHT,EMERALD_ANIM_CLEAN_LEFT,EMERALD_BEETLE_RIGHT);
        } else {                           // Rechts ist nicht frei
            // Käfer bleibt zunächst auf "rechts" muss sich aber bei Blockade nach oben drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_UP;
        }
    }
    else if ( (!IS_SPACE(I + Playfield.uLevel_X_Dimension)) &&   // Hat Käfer rechts Führung? AcidPool ist wie Space.
              (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] != EMERALD_ACIDPOOL) ) {
        if (IS_SPACE(I + 1)) {    // Ist nach rechts frei?
            SetElementToNextPosition(I,EMERALD_ANIM_RIGHT,EMERALD_ANIM_CLEAN_LEFT,EMERALD_BEETLE_RIGHT);
        } else {                            // Rechts ist nicht frei
            // Käfer bleibt zunächst auf "rechts" muss sich aber bei Blockade nach oben drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_UP;
        }
    } else {                         // Rechte Wand verloren
        // Käfer bleibt zunächst auf "rechts" muss sich aber bei Wand-Verlust nach unten drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_DOWN | EMERALD_ANIM_LOST_GUIDE;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlBeetleDown
------------------------------------------------------------------------------
Beschreibung: Steuert einen nach unten laufenden Käfer im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlBeetleDown(uint32_t I) {
    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) || ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ) {
        // BeetleDown kann vom Replikator geboren werden, dann hier nichts machen
        return;
    }
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BEETLE_WILL_EXPLODE) {
        ControlCentralBeetleExplosion(I);
        PreparePlaySound(SOUND_EXPLOSION,I);
        return; // Für den Käfer ist das Spiel hier zu Ende
    }
    // Hat Käfer Kontakt zu grünem/gelbem Käse ?
    if (IsCheeseAround(I)) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_BEETLE_WILL_EXPLODE | EMERALD_ANIM_SPIN_DOWN_TO_RIGHT;
        return; // Für den Käfer ist das Spiel nächste Runde zu Ende
    }
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE) {
        // Hatte Käfer vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
        Playfield.pStatusAnimation[I] = 0;
        if (IS_SPACE(I + Playfield.uLevel_X_Dimension)) {    // Ist nach unten frei?
            SetElementToNextPosition(I,EMERALD_ANIM_DOWN,EMERALD_ANIM_CLEAN_UP,EMERALD_BEETLE_DOWN);
        } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Käfer ins Säurebecken?
            // SDL_Log("Beetle falls in pool");
            Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
            Playfield.pInvalidElement[I] = EMERALD_BEETLE_DOWN;
            PreparePlaySound(SOUND_POOL_BLUB,I);
            return;
        } else {                           // Unten ist nicht frei
            // Käfer bleibt zunächst auf "unten" muss sich aber bei Blockade nach rechts drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_RIGHT;
        }
    } else if (!IS_SPACE(I - 1)) {   // Rechts von Käfer irgendwas?
        if (IS_SPACE(I + Playfield.uLevel_X_Dimension)) {    // Ist nach unten frei?
            SetElementToNextPosition(I,EMERALD_ANIM_DOWN,EMERALD_ANIM_CLEAN_UP,EMERALD_BEETLE_DOWN);
        } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Käfer ins Säurebecken?
            // SDL_Log("Beetle falls in pool");
            Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
            Playfield.pInvalidElement[I] = EMERALD_BEETLE_DOWN;
            PreparePlaySound(SOUND_POOL_BLUB,I);
            return;
        } else {                            // Unten ist nicht frei
            // Käfer bleibt zunächst auf "unten" muss sich aber bei Blockade nach rechts drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_RIGHT;
        }
    } else {                         // Rechte Wand verloren
        // Käfer bleibt zunächst auf "unten" muss sich aber bei Wand-Verlust nach links drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_LEFT | EMERALD_ANIM_LOST_GUIDE;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlBeetleLeft
------------------------------------------------------------------------------
Beschreibung: Steuert einen nach links laufenden Käfer im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlBeetleLeft(uint32_t I) {
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BEETLE_WILL_EXPLODE) {
        ControlCentralBeetleExplosion(I);
        PreparePlaySound(SOUND_EXPLOSION,I);
        return; // Für den Käfer ist das Spiel hier zu Ende
    }
    // Hat Käfer Kontakt zu grünem/gelbem Käse ?
    if (IsCheeseAround(I)) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_BEETLE_WILL_EXPLODE | EMERALD_ANIM_SPIN_LEFT_TO_DOWN;
        return; // Für den Käfer ist das Spiel nächste Runde zu Ende
    }
    // Hatte Käfer vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE) {
        Playfield.pStatusAnimation[I] = 0;
        if (IS_SPACE(I - 1)) {    // Ist nach links frei?
            SetElementToNextPosition(I,EMERALD_ANIM_LEFT,EMERALD_ANIM_CLEAN_RIGHT,EMERALD_BEETLE_LEFT);
        } else {                           // Links ist nicht frei
            // Käfer bleibt zunächst auf "links" muss sich aber bei Blockade nach unten drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_DOWN;
        }
    } else if (!IS_SPACE(I - Playfield.uLevel_X_Dimension)) {   // Rechts von Käfer irgendwas?{
        if (IS_SPACE(I - 1)) {    // Ist nach links frei?
            SetElementToNextPosition(I,EMERALD_ANIM_LEFT,EMERALD_ANIM_CLEAN_RIGHT,EMERALD_BEETLE_LEFT);
        } else {                           // Links ist nicht frei
            // Käfer bleibt zunächst auf "links" muss sich aber bei Blockade nach unten drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_DOWN;
        }
    } else {                        // Rechte Wand verloren
        // Käfer bleibt zunächst auf "links" muss sich aber bei Wand-Verlust nach oben drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_UP | EMERALD_ANIM_LOST_GUIDE;
    }
}
