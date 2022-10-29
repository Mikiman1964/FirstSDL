#include "EmeraldMine.h"
#include "beetle.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlBeetleUp
------------------------------------------------------------------------------
Beschreibung: Steuert einen nach oben laufenden K�fer im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
R�ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlBeetleUp(uint32_t I) {
    // Hatte K�fer vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE) {
        Playfield.pStatusAnimation[I] = 0;
        if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach oben frei?
            // neuen Platz mit ung�ltigem Element besetzen
            Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_BEETLE_UP | EMERALD_ANIM_CLEAN_DOWN;
            // Aktuelles Element auf Animation "oben"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_UP;
        } else {                        // Oben ist nicht frei
            // K�fer bleibt zun�chst auf "oben" muss sich aber bei Blockade nach links drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_LEFT;
        }
    }
    else if (Playfield.pLevel[I + 1] != EMERALD_SPACE) { // // Hat K�fer rechts F�hrung?
        if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach oben frei?
            // neuen Platz mit ung�ltigem Element besetzen
            Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_BEETLE_UP | EMERALD_ANIM_CLEAN_DOWN;
            // Aktuelles Element auf Animation "oben"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_UP;
        } else {                        // Oben ist nicht frei
            // K�fer bleibt zun�chst auf "oben" muss sich aber bei Blockade nach links drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_LEFT;
        }
    } else {                      // Rechte Wand verloren
        // K�fer bleibt zun�chst auf "oben" muss sich aber bei Wand-Verlust nach rechts drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_UP_TO_RIGHT | EMERALD_ANIM_LOST_GUIDE;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlBeetleRight
------------------------------------------------------------------------------
Beschreibung: Steuert einen nach rechts laufenden K�fer im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
R�ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlBeetleRight(uint32_t I) {
    // Hatte K�fer vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE)
    {
        Playfield.pStatusAnimation[I] = 0;
        if (Playfield.pLevel[I + 1] == EMERALD_SPACE)    // Ist rechts frei?
        {
            // neuen Platz mit ung�ltigem Element besetzen
            Playfield.pLevel[I + 1] = EMERALD_INVALID;
            // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I + 1] = EMERALD_BEETLE_RIGHT | EMERALD_ANIM_CLEAN_LEFT;
            // Aktuelles Element auf Animation "rechts"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
        }
        else                            // Rechts ist nicht frei
        {
            // K�fer bleibt zun�chst auf "rechts" muss sich aber bei Blockade nach oben drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_UP;
        }
    }
    else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] != EMERALD_SPACE)   // Hat K�fer rechts F�hrung?
    {
        if (Playfield.pLevel[I + 1] == EMERALD_SPACE)    // Ist nach rechts frei?
        {
            // neuen Platz mit ung�ltigem Element besetzen
            Playfield.pLevel[I + 1] = EMERALD_INVALID;
            // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I + 1] = EMERALD_BEETLE_RIGHT | EMERALD_ANIM_CLEAN_LEFT;
            // Aktuelles Element auf Animation "rechts"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
        }
        else                            // Rechts ist nicht frei
        {
            // K�fer bleibt zun�chst auf "rechts" muss sich aber bei Blockade nach oben drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_UP;
        }
    }
    else                          // Rechte Wand verloren
    {
        // K�fer bleibt zun�chst auf "rechts" muss sich aber bei Wand-Verlust nach unten drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_RIGHT_TO_DOWN | EMERALD_ANIM_LOST_GUIDE;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlBeetleDown
------------------------------------------------------------------------------
Beschreibung: Steuert einen nach unten laufenden K�fer im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
R�ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlBeetleDown(uint32_t I) {
    // Hatte K�fer vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE)
    {
        Playfield.pStatusAnimation[I] = 0;
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE)    // Ist nach unten frei?
        {
            // neuen Platz mit ung�ltigem Element besetzen
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_BEETLE_DOWN | EMERALD_ANIM_CLEAN_UP;
            // Aktuelles Element auf Animation "unten"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
        }
        else                            // Unten ist nicht frei
        {
            // K�fer bleibt zun�chst auf "unten" muss sich aber bei Blockade nach rechts drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_RIGHT;
        }
    }
    else if (Playfield.pLevel[I - 1] != EMERALD_SPACE)   // Rechts von K�fer irgendwas?
    {
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE)    // Ist nach unten frei?
        {
            // neuen Platz mit ung�ltigem Element besetzen
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_BEETLE_DOWN | EMERALD_ANIM_CLEAN_UP;
            // Aktuelles Element auf Animation "unten"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
        }
        else                            // Unten ist nicht frei
        {
            // K�fer bleibt zun�chst auf "unten" muss sich aber bei Blockade nach rechts drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_RIGHT;
        }
    }
    else                          // Rechte Wand verloren
    {
        // K�fer bleibt zun�chst auf "unten" muss sich aber bei Wand-Verlust nach links drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_DOWN_TO_LEFT | EMERALD_ANIM_LOST_GUIDE;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlBeetleLeft
------------------------------------------------------------------------------
Beschreibung: Steuert einen nach links laufenden K�fer im Level.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
R�ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlBeetleLeft(uint32_t I) {
    // Hatte K�fer vor Drehung Wandverlust -> dann versuchen neue Richtung zu gehen
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_LOST_GUIDE)
    {
        Playfield.pStatusAnimation[I] = 0;
        if (Playfield.pLevel[I - 1] == EMERALD_SPACE)    // Ist nach links frei?
        {
            // neuen Platz mit ung�ltigem Element besetzen
            Playfield.pLevel[I - 1] = EMERALD_INVALID;
            // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I - 1] = EMERALD_BEETLE_LEFT | EMERALD_ANIM_CLEAN_RIGHT;
            // Aktuelles Element auf Animation "links"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
        }
        else                            // Links ist nicht frei
        {
            // K�fer bleibt zun�chst auf "links" muss sich aber bei Blockade nach unten drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_DOWN;
        }
    }
    else if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] != EMERALD_SPACE)   // Rechts von K�fer irgendwas?
    {
        if (Playfield.pLevel[I - 1] == EMERALD_SPACE)    // Ist nach links frei?
        {
            // neuen Platz mit ung�ltigem Element besetzen
            Playfield.pLevel[I - 1] = EMERALD_INVALID;
            // Damit ung�ltiges Feld sp�ter auf richtiges Element gesetzt werden kann
            Playfield.pStatusAnimation[I - 1] = EMERALD_BEETLE_LEFT | EMERALD_ANIM_CLEAN_RIGHT;
            // Aktuelles Element auf Animation "links"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
        }
        else                            // Links ist nicht frei
        {
            // K�fer bleibt zun�chst auf "links" muss sich aber bei Blockade nach unten drehen
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_DOWN;
        }
    }
    else                          // Rechte Wand verloren
    {
        // K�fer bleibt zun�chst auf "links" muss sich aber bei Wand-Verlust nach oben drehen
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_SPIN_LEFT_TO_UP | EMERALD_ANIM_LOST_GUIDE;
    }
}
