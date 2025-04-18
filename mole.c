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
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMoleUp(uint32_t I) {
    int nRandom;
    bool bLeftFree;
    bool bRightFree;

    if (IS_SPACE(I - Playfield.uLevel_X_Dimension)) {  // Ist nach oben frei?
        SetElementToNextPosition(I,EMERALD_ANIM_UP,EMERALD_ANIM_CLEAN_DOWN,EMERALD_MOLE_UP);
    } else if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_GREEN_CHEESE) {  // Gibt es oben grünen Käse ?
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MOLE_STAND;   // Mole bleibt oben und läuft auf der Stelle
        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_GREEN_CHEESE_GOES;
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
        PreparePlaySound(SOUND_CHEESE,I);
    } else if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_YELLOW_CHEESE) {  // Gibt es oben gelben Käse ?
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MOLE_STAND;   // Mole bleibt oben und läuft auf der Stelle
        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_YELLOW_CHEESE_GOES;
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
        PreparePlaySound(SOUND_CHEESE,I);
    } else {                        // Oben ist nicht frei
        // Mole bleibt zunächst auf "oben" muss sich aber bei Blockade entscheiden nach rechts oder links zu gehen
        bLeftFree = (IS_SPACE(I - 1));
        bRightFree = (IS_SPACE(I + 1));
        if (bLeftFree && bRightFree) {
            // Ist links und rechts frei ?
            nRandom = randn(0,1);         // Ergibt Zufallszahl 0 oder 1
            if (nRandom == 1) {           // Mole entscheidet sich bei 1 nach rechts
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
                nRandom = randn(0,1);         // Ergibt Zufallszahl 0 oder 1
                if (nRandom == 1) {           // Mole entscheidet sich bei 1 nach rechts
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
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMoleRight(uint32_t I) {
    int nRandom;
    bool bLeftFree;
    bool bRightFree;

    if (IS_SPACE(I + 1)) {  // Ist nach rechts frei?
        SetElementToNextPosition(I,EMERALD_ANIM_RIGHT,EMERALD_ANIM_CLEAN_LEFT,EMERALD_MOLE_RIGHT);
    } else if (Playfield.pLevel[I + 1] == EMERALD_GREEN_CHEESE) {  // Gibt es rechts grünen Käse ?
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MOLE_STAND;   // Mole bleibt rechts und läuft auf der Stelle
        Playfield.pLevel[I + 1] = EMERALD_GREEN_CHEESE_GOES;
        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_STAND;
        PreparePlaySound(SOUND_CHEESE,I);
    } else if (Playfield.pLevel[I + 1] == EMERALD_YELLOW_CHEESE) {  // Gibt es rechts gelben Käse ?
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MOLE_STAND;   // Mole bleibt rechts und läuft auf der Stelle
        Playfield.pLevel[I + 1] = EMERALD_YELLOW_CHEESE_GOES;
        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_STAND;
        PreparePlaySound(SOUND_CHEESE,I);
    } else {                        // Rechts ist nicht frei
        // Mole bleibt zunächst auf "rechts" muss sich aber bei Blockade entscheiden nach links oder rechts zu gehen
        bLeftFree = (IS_SPACE(I - Playfield.uLevel_X_Dimension));
        bRightFree = (IS_SPACE(I + Playfield.uLevel_X_Dimension));
        if (bLeftFree && bRightFree) {
            // Ist oben und unten frei ?
            nRandom = randn(0,1);         // Ergibt Zufallszahl 0 oder 1
            if (nRandom == 1) {           // Mole entscheidet sich bei 1 nach rechts
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
                nRandom = randn(0,1);         // Ergibt Zufallszahl 0 oder 1
                if (nRandom == 1) {           // Mole entscheidet sich bei 1 nach rechts
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
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMoleDown(uint32_t I) {
    int nRandom;
    bool bLeftFree;
    bool bRightFree;

    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) || ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ) {
        // MoleDown kann vom Replikator geboren werden, dann hier nichts machen
        return;
    }
    if (IS_SPACE(I + Playfield.uLevel_X_Dimension)) {  // Ist nach unten frei?
        SetElementToNextPosition(I,EMERALD_ANIM_DOWN,EMERALD_ANIM_CLEAN_UP,EMERALD_MOLE_DOWN);
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_GREEN_CHEESE) {  // Gibt es unten grünen Käse ?
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MOLE_STAND;   // Mole bleibt unten und läuft auf der Stelle
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_GREEN_CHEESE_GOES;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
        PreparePlaySound(SOUND_CHEESE,I);
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_YELLOW_CHEESE) {  // Gibt es unten gelben Käse ?
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MOLE_STAND;   // Mole bleibt unten und läuft auf der Stelle
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_YELLOW_CHEESE_GOES;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
        PreparePlaySound(SOUND_CHEESE,I);
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Mole ins Säurebecken?
        // SDL_Log("Mole falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = EMERALD_MOLE_DOWN;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else {                        // Unten ist nicht frei
        // Mole bleibt zunächst auf "unten" muss sich aber bei Blockade entscheiden nach rechts oder links zu gehen
        bRightFree = (IS_SPACE(I - 1));
        bLeftFree = (IS_SPACE(I + 1));
        if (bLeftFree && bRightFree) {
            // Ist links und rechts frei ?
            nRandom = randn(0,1);         // Ergibt Zufallszahl 0 oder 1
            if (nRandom == 1) {           // Mole entscheidet sich bei 1 nach rechts
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
                nRandom = randn(0,1);         // Ergibt Zufallszahl 0 oder 1
                if (nRandom == 1) {           // Mole entscheidet sich bei 1 nach rechts
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
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlMoleLeft(uint32_t I) {
    int nRandom;
    bool bLeftFree;
    bool bRightFree;

    if (IS_SPACE(I - 1)) {  // Ist nach links frei?
        SetElementToNextPosition(I,EMERALD_ANIM_LEFT,EMERALD_ANIM_CLEAN_RIGHT,EMERALD_MOLE_LEFT);
    } else if (Playfield.pLevel[I - 1] == EMERALD_GREEN_CHEESE) {  // Gibt es links grünen Käse ?
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MOLE_STAND;   // Mole bleibt links und läuft auf der Stelle
        Playfield.pLevel[I - 1] = EMERALD_GREEN_CHEESE_GOES;
        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_STAND;
        PreparePlaySound(SOUND_CHEESE,I);
    } else if (Playfield.pLevel[I - 1] == EMERALD_YELLOW_CHEESE) {  // Gibt es links gelben Käse ?
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MOLE_STAND;   // Mole bleibt links und läuft auf der Stelle
        Playfield.pLevel[I - 1] = EMERALD_YELLOW_CHEESE_GOES;
        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_STAND;
        PreparePlaySound(SOUND_CHEESE,I);
    } else {                        // Links ist nicht frei
        // Mole bleibt zunächst auf "links" muss sich aber bei Blockade entscheiden nach unten oder oben zu gehen
        bRightFree = (IS_SPACE(I - Playfield.uLevel_X_Dimension));
        bLeftFree = (IS_SPACE(I + Playfield.uLevel_X_Dimension));
        if (bRightFree && bLeftFree) {
            // Ist oben und unten frei ?
            nRandom = randn(0,1);         // Ergibt Zufallszahl 0 oder 1
            if (nRandom == 1) {           // Mole entscheidet sich bei 1 nach rechts
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
                nRandom = randn(0,1);         // Ergibt Zufallszahl 0 oder 1
                if (nRandom == 1) {           // Mole entscheidet sich bei 1 nach rechts
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_UP | EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT;
                } else {
                    Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_DOWN | EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT;
                }
            }
        }
    }
}
