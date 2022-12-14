#include "EmeraldMine.h"
#include "mole.h"
#include "mystd.h"
#include "sound.h"

extern PLAYFIELD Playfield;

// Hinweis: Bei allen Funktion gilt: bLeftFree, bRightFree aus Sicht der Mole.


/*----------------------------------------------------------------------------
Name:           ControlMoleUp
------------------------------------------------------------------------------
Beschreibung: Steuert einen nach oben laufenden Maulwurf im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
R?ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMoleUp(uint32_t I) {
    uint8_t uRandom;
    bool bLeftFree;
    bool bRightFree;

    if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach oben frei?
        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_MOLE_UP;
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_UP;
        Playfield.pLevel[I] = EMERALD_SAND_MOLE;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    } else if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_GREEN_CHEESE) {  // Gibt es oben gr?nen K?se ?
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MOLE_STAND;   // Mole bleibt oben und l?uft auf der Stelle
        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_GREEN_CHEESE_GOES;
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
        PreparePlaySound(SOUND_CHEESE,I);
    } else {                        // Oben ist nicht frei
        // Mole bleibt zun?chst auf "oben" muss sich aber bei Blockade entscheiden nach rechts oder links zu gehen
        bLeftFree = (Playfield.pLevel[I - 1] == EMERALD_SPACE);
        bRightFree = (Playfield.pLevel[I + 1] == EMERALD_SPACE);
        if (bLeftFree && bRightFree) {
            // Ist links und rechts frei ?
            uRandom = randf(1,255);         // Ergibt Zufallszahl zwischen 1-254
            if (uRandom & 0x01) {           // Mole entscheidet sich bei ungeraden Zufallszahl nach rechts
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_RIGHT;
            } else {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_LEFT;
            }
        } else if (bLeftFree) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_LEFT;
        } else if (bRightFree) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_RIGHT;
        } else {
            // Hier ist weder rechts noch links frei
            // War Mole bereits beim letztem Mal blockiert?
            if  ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_RIGHT | EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT;
            } else if  ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_LEFT | EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT;
            } else {
                uRandom = randf(1,255);         // Ergibt Zufallszahl zwischen 1-254
                if (uRandom & 0x01) {           // Mole entscheidet sich bei ungeraden Zufallszahl rechts zu drehen
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_RIGHT | EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT;
                } else {
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_LEFT | EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT;
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlMoleRight
------------------------------------------------------------------------------
Beschreibung: Steuert einen nach rechts laufenden Maulwurf im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
R?ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMoleRight(uint32_t I) {
    uint8_t uRandom;
    bool bLeftFree;
    bool bRightFree;

    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        return;
    }
    if (Playfield.pLevel[I + 1] == EMERALD_SPACE) {  // Ist nach rechts frei?
        Playfield.pLevel[I + 1] = EMERALD_MOLE_RIGHT;
        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
        Playfield.pLevel[I] = EMERALD_SAND_MOLE;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    } else if (Playfield.pLevel[I + 1] == EMERALD_GREEN_CHEESE) {  // Gibt es rechts gr?nen K?se ?
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MOLE_STAND;   // Mole bleibt rechts und l?uft auf der Stelle
        Playfield.pLevel[I + 1] = EMERALD_GREEN_CHEESE_GOES;
        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_STAND;
        PreparePlaySound(SOUND_CHEESE,I);
    } else {                        // Rechts ist nicht frei
        // Mole bleibt zun?chst auf "rechts" muss sich aber bei Blockade entscheiden nach links oder rechts zu gehen
        bLeftFree = (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE);
        bRightFree = (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE);
        if (bLeftFree && bRightFree) {
            // Ist oben und unten frei ?
            uRandom = randf(1,255);         // Ergibt Zufallszahl zwischen 1-254
            if (uRandom & 0x01) {           // Mole entscheidet sich bei ungeraden Zufallszahl nach rechts
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_DOWN;
            } else {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_UP;
            }
        } else if (bLeftFree) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_UP;
        } else if (bRightFree) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_DOWN;
        } else {
            // Hier ist weder oben noch unten frei
            // War Mole bereits beim letztem Mal blockiert?
            if  ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_DOWN | EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT;
            } else if  ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_UP | EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT;
            } else {
                uRandom = randf(1,255);         // Ergibt Zufallszahl zwischen 1-254
                if (uRandom & 0x01) {           // Mole entscheidet sich bei ungeraden Zufallszahl rechts zu drehen
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_DOWN | EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT;
                } else {
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_UP | EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT;
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlMoleDown
------------------------------------------------------------------------------
Beschreibung: Steuert einen nach unten laufenden Maulwurf im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
R?ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMoleDown(uint32_t I) {
    uint8_t uRandom;
    bool bLeftFree;
    bool bRightFree;

    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) || ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ) {
        // MoleDown kann vom Replikator geboren werden, dann hier nichts machen
        return;
    }
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        return;
    }
    if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach unten frei?
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_MOLE_DOWN;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
        Playfield.pLevel[I] = EMERALD_SAND_MOLE;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_GREEN_CHEESE) {  // Gibt es unten gr?nen K?se ?
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MOLE_STAND;   // Mole bleibt unten und l?uft auf der Stelle
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_GREEN_CHEESE_GOES;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
        PreparePlaySound(SOUND_CHEESE,I);
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // F?llt Mole ins S?urebecken?
        SDL_Log("Mole falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = EMERALD_MOLE_DOWN;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else {                        // Unten ist nicht frei
        // Mole bleibt zun?chst auf "unten" muss sich aber bei Blockade entscheiden nach rechts oder links zu gehen
        bRightFree = (Playfield.pLevel[I - 1] == EMERALD_SPACE);
        bLeftFree = (Playfield.pLevel[I + 1] == EMERALD_SPACE);
        if (bLeftFree && bRightFree) {
            // Ist links und rechts frei ?
            uRandom = randf(1,255);         // Ergibt Zufallszahl zwischen 1-254
            if (uRandom & 0x01) {           // Mole entscheidet sich bei ungeraden Zufallszahl nach rechts
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_RIGHT;
            } else {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_LEFT;
            }
        } else if (bLeftFree) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_RIGHT;
        } else if (bRightFree) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_LEFT;
        } else {
            // Hier ist weder rechts noch links frei
            // War Mole bereits beim letztem Mal blockiert?
            if  ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_LEFT | EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT;
            } else if  ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_RIGHT | EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT;
            } else {
                uRandom = randf(1,255);         // Ergibt Zufallszahl zwischen 1-254
                if (uRandom & 0x01) {           // Mole entscheidet sich bei ungeraden Zufallszahl rechts zu drehen
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_RIGHT | EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT;
                } else {
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_LEFT | EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT;
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlMoleLeft
------------------------------------------------------------------------------
Beschreibung: Steuert einen nach links laufenden Maulwurf im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
R?ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMoleLeft(uint32_t I) {
    uint8_t uRandom;
    bool bLeftFree;
    bool bRightFree;

    if (Playfield.pLevel[I - 1] == EMERALD_SPACE) {  // Ist nach links frei?
        Playfield.pLevel[I - 1] = EMERALD_MOLE_LEFT;
        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_LEFT;
        Playfield.pLevel[I] = EMERALD_SAND_MOLE;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    } else if (Playfield.pLevel[I - 1] == EMERALD_GREEN_CHEESE) {  // Gibt es links gr?nen K?se ?
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MOLE_STAND;   // Mole bleibt links und l?uft auf der Stelle
        Playfield.pLevel[I - 1] = EMERALD_GREEN_CHEESE_GOES;
        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_STAND;
        PreparePlaySound(SOUND_CHEESE,I);
    } else {                        // Links ist nicht frei
        // Mole bleibt zun?chst auf "links" muss sich aber bei Blockade entscheiden nach unten oder oben zu gehen
        bRightFree = (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE);
        bLeftFree = (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE);
        if (bRightFree && bLeftFree) {
            // Ist oben und unten frei ?
            uRandom = randf(1,255);         // Ergibt Zufallszahl zwischen 1-254
            if (uRandom & 0x01) {           // Mole entscheidet sich bei ungeraden Zufallszahl nach unten
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_DOWN;
            } else {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_UP;
            }
        } else if (bRightFree) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_UP;
        } else if (bLeftFree) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_DOWN;
        } else {
            // Hier ist weder oben noch unten frei
            // War Mole bereits beim letztem Mal blockiert?
            if  ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_UP | EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT;
            } else if  ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_DOWN | EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT;
            } else {
                uRandom = randf(1,255);         // Ergibt Zufallszahl zwischen 1-254
                if (uRandom & 0x01) {           // Mole entscheidet sich bei ungeraden Zufallszahl rechts zu drehen
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_UP | EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT;
                } else {
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_DOWN | EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT;
                }
            }
        }
    }
}
