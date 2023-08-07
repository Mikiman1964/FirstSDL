#include "EmeraldMine.h"
#include "mystd.h"
#include "yam.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlYam
------------------------------------------------------------------------------
Beschreibung: Steuert einen Yam.
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Yam-Koordinate)
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlYam(uint32_t I) {
    int nRandom;
    uint32_t uAnimStatus;

    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) || ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ) {
        // Yam kann vom Replikator geboren werden, dann hier nichts machen
        Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
        return;
    }
    // Zunächst prüfen, ob es um den YAM einen Saphir gibt, den er fressen kann. Es werden keine "blitzenden" Saphire gefressen.
    if ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SAPPHIRE) && (Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] == EMERALD_ANIM_STAND)) {
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_SAPPHIRE_SHRINK;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_YAM_WAS_BLOCKED; // YAM ist beim Fressen blockiert.
        Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
        PreparePlaySound(SOUND_YAM,I);
    } else if ((Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SAPPHIRE) && (Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] == EMERALD_ANIM_STAND)) {
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_SAPPHIRE_SHRINK;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_YAM_WAS_BLOCKED; // YAM ist beim Fressen blockiert.
        Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
        PreparePlaySound(SOUND_YAM,I);
    } else if ((Playfield.pLevel[I + 1] == EMERALD_SAPPHIRE) && (Playfield.pStatusAnimation[I + 1] == EMERALD_ANIM_STAND)) {
        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_SAPPHIRE_SHRINK;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_YAM_WAS_BLOCKED; // YAM ist beim Fressen blockiert.
        Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
        PreparePlaySound(SOUND_YAM,I);
    } else if ((Playfield.pLevel[I - 1] == EMERALD_SAPPHIRE) && (Playfield.pStatusAnimation[I - 1] == EMERALD_ANIM_STAND)) {
        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_SAPPHIRE_SHRINK;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_YAM_WAS_BLOCKED; // YAM ist beim Fressen blockiert.
        Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
        PreparePlaySound(SOUND_YAM,I);
    } else {
        // YAM frisst nicht
        uAnimStatus = Playfield.pLastYamDirection[I] << 8;
        switch (uAnimStatus) {
            case (EMERALD_ANIM_STAND):
                // Falls YAM steht und nichts frist, dann neue zufällige Richtung wählen
                nRandom = randn(1,4);       // Ergibt Zufallszahl zwischen 1-4  (1 = links, 2 = oben, 3 = rechts, 4 = runter)
                switch (nRandom) {
                    case (1):   // Links
                        CheckYamGoLeft(I);
                        break;
                    case (2):   // Oben
                        CheckYamGoUp(I);
                        break;
                    case (3):   // Rechts
                        CheckYamGoRight(I);
                        break;
                    case (4):   // Unten
                        CheckYamGoDown(I);
                        break;
                    default:
                        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_YAM_WAS_BLOCKED; // YAM macht nichts.
                        PreparePlaySound(SOUND_YAM,I);
                        break;
                }
                break;
            case (EMERALD_ANIM_LEFT):
                CheckYamGoLeft(I);
                break;
            case (EMERALD_ANIM_UP):
                CheckYamGoUp(I);
                break;
            case (EMERALD_ANIM_RIGHT):
                CheckYamGoRight(I);
                break;
            case (EMERALD_ANIM_DOWN):
                CheckYamGoDown(I);
                break;
            default:
                // bei EMERALD_ANIM_BORN1 und EMERALD_ANIM_BORN2
                break;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           CheckYamGoLeft
------------------------------------------------------------------------------
Beschreibung: Prüft, ob Yam nach links laufen kann. Falls möglich, wird der Yam
              auf die neue Position gesetzt. Falls nicht, bleibt der Status auf "stand".
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Yam-Koordinate)
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CheckYamGoLeft(uint32_t I) {
    if (Playfield.pLevel[I - 1] == EMERALD_SPACE) {
        // Yam ist jetzt frei: War der Yam letzte Runde blockiert, dann noch mindestens eine Runde warten und Blockadenflag löschen
        if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_YAM_WAS_BLOCKED) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND; // löscht auch Blockadenflag
            Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
            PreparePlaySound(SOUND_YAM,I);
        } else {
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I - 1] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I - 1] = EMERALD_YAM;
            Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT;
            Playfield.pLastYamDirection[I - 1] = EMERALD_LAST_YAM_DIR_LEFT;
            // Aktuelles Element auf Animation "links"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
        }
    } else if (Playfield.pLevel[I - 1] == EMERALD_MAN) {  // Kann Man erwischt werden?
        // Yam ist jetzt frei: War der Yam letzte Runde blockiert, dann noch mindestens eine Runde warten und Blockadenflag löschen
        if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_YAM_WAS_BLOCKED) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND; // löscht auch Blockadenflag
            Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
            PreparePlaySound(SOUND_YAM,I);
        } else {
            SDL_Log("Yam kills man -> left");
            Playfield.pLevel[I] = EMERALD_YAM_KILLS_MAN;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_MONSTER_KILLS_LEFT;
            Playfield.pLevel[I - 1] = EMERALD_MAN_DIES;
            Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_MAN_DIES_P1;
            Playfield.bManDead = true;
            Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_LEFT;
            PreparePlaySound(SOUND_MAN_CRIES,I);
        }
    } else {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
        if (IsYamCompleteBlocked(I)) {
            Playfield.pStatusAnimation[I] |= EMERALD_ANIM_YAM_WAS_BLOCKED;
        }
        PreparePlaySound(SOUND_YAM,I);
    }
}


/*----------------------------------------------------------------------------
Name:           CheckYamGoRight
------------------------------------------------------------------------------
Beschreibung: Prüft, ob Yam nach rechts laufen kann. Falls möglich, wird der Yam
              auf die neue Position gesetzt. Falls nicht, bleibt der Status auf "stand".
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Yam-Koordinate)
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CheckYamGoRight(uint32_t I) {
    if (Playfield.pLevel[I + 1] == EMERALD_SPACE) {
        // Yam ist jetzt frei: War der Yam letzte Runde blockiert, dann noch mindestens eine Runde warten und Blockadenflag löschen
        if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_YAM_WAS_BLOCKED) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND; // löscht auch Blockadenflag
            Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
            PreparePlaySound(SOUND_YAM,I);
        } else {
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I + 1] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I + 1] = EMERALD_YAM;
            Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT | EMERALD_ANIM_RIGHT;
            Playfield.pLastStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT;
            Playfield.pLastYamDirection[I + 1] = EMERALD_LAST_YAM_DIR_RIGHT;
            // Aktuelles Element auf Animation "rechts"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
        }
    } else if (Playfield.pLevel[I + 1] == EMERALD_MAN) {  // Kann Man erwischt werden?
        // Yam ist jetzt frei: War der Yam letzte Runde blockiert, dann noch mindestens eine Runde warten und Blockadenflag löschen
        if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_YAM_WAS_BLOCKED) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND; // löscht auch Blockadenflag
            Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
            PreparePlaySound(SOUND_YAM,I);
        } else {
            SDL_Log("Yam kills man -> right");
            Playfield.pLevel[I] = EMERALD_YAM_KILLS_MAN;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_MONSTER_KILLS_RIGHT | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
            Playfield.pLevel[I + 1] = EMERALD_MAN_DIES;
            Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_MAN_DIES_P1 | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
            Playfield.bManDead = true;
            Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_RIGHT;
            PreparePlaySound(SOUND_MAN_CRIES,I);
        }
    } else {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
        if (IsYamCompleteBlocked(I)) {
            Playfield.pStatusAnimation[I] |= EMERALD_ANIM_YAM_WAS_BLOCKED;
        }
        PreparePlaySound(SOUND_YAM,I);
    }
}


/*----------------------------------------------------------------------------
Name:           CheckYamGoUp
------------------------------------------------------------------------------
Beschreibung: Prüft, ob Yam nach oben laufen kann. Falls möglich, wird der Yam
              auf die neue Position gesetzt. Falls nicht, bleibt der Status auf "stand".
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Yam-Koordinate)
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CheckYamGoUp(uint32_t I) {
    if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
        // Yam ist jetzt frei: War der Yam letzte Runde blockiert, dann noch mindestens eine Runde warten und Blockadenflag löschen
        if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_YAM_WAS_BLOCKED) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND; // löscht auch Blockadenflag
            Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
            PreparePlaySound(SOUND_YAM,I);
        } else {
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I - Playfield.uLevel_X_Dimension] = EMERALD_YAM;
            Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_DOWN;
            Playfield.pLastYamDirection[I - Playfield.uLevel_X_Dimension] = EMERALD_LAST_YAM_DIR_UP;
            // Aktuelles Element auf Animation "oben"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_UP;
        }
    } else if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_MAN) {  // Kann Man erwischt werden?
        // Yam ist jetzt frei: War der Yam letzte Runde blockiert, dann noch mindestens eine Runde warten und Blockadenflag löschen
        if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_YAM_WAS_BLOCKED) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND; // löscht auch Blockadenflag
            Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
            PreparePlaySound(SOUND_YAM,I);
        } else {
            SDL_Log("Yam kills man -> up");
            Playfield.pLevel[I] = EMERALD_YAM_KILLS_MAN;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_MONSTER_KILLS_UP;
            Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_MAN_DIES;
            Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_MAN_DIES_P1;
            Playfield.bManDead = true;
            Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_UP;
            PreparePlaySound(SOUND_MAN_CRIES,I);
        }
    } else {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
        if (IsYamCompleteBlocked(I)) {
            Playfield.pStatusAnimation[I] |= EMERALD_ANIM_YAM_WAS_BLOCKED;
        }
        PreparePlaySound(SOUND_YAM,I);
    }
}


/*----------------------------------------------------------------------------
Name:           CheckYamGoDown
------------------------------------------------------------------------------
Beschreibung: Prüft, ob Yam nach unten laufen kann. Falls möglich, wird der Yam
              auf die neue Position gesetzt. Falls nicht, bleibt der Status auf "stand".
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Yam-Koordinate)
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CheckYamGoDown(uint32_t I) {
    if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
        // Yam ist jetzt frei: War der Yam letzte Runde blockiert, dann noch mindestens eine Runde warten und Blockadenflag löschen
        if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_YAM_WAS_BLOCKED) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND; // löscht auch Blockadenflag
            Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
            PreparePlaySound(SOUND_YAM,I);
        } else {
            // neuen Platz mit ungültigem Element besetzen
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
            // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
            Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = EMERALD_YAM;
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_UP;
            Playfield.pLastStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN;
            Playfield.pLastYamDirection[I + Playfield.uLevel_X_Dimension] = EMERALD_LAST_YAM_DIR_DOWN;
            // Aktuelles Element auf Animation "stehend"
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
        }
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Yam ins Säurebecken?
        SDL_Log("Yam falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = EMERALD_YAM;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MAN) {  // Kann Man erwischt werden?
        // Yam ist jetzt frei: War der Yam letzte Runde blockiert, dann noch mindestens eine Runde warten und Blockadenflag löschen
        if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_YAM_WAS_BLOCKED) {
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND; // löscht auch Blockadenflag
            Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
            PreparePlaySound(SOUND_YAM,I);
        } else {
            SDL_Log("Yam kills man -> down");
            Playfield.pLevel[I] = EMERALD_YAM_KILLS_MAN;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_MONSTER_KILLS_DOWN | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_MAN_DIES;
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_MAN_DIES_P1 | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
            Playfield.bManDead = true;
            Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_DOWN;
            PreparePlaySound(SOUND_MAN_CRIES,I);
        }
    } else {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
        if (IsYamCompleteBlocked(I)) {
            Playfield.pStatusAnimation[I] |= EMERALD_ANIM_YAM_WAS_BLOCKED;
        }
        PreparePlaySound(SOUND_YAM,I);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlYamKillsMan
------------------------------------------------------------------------------
Beschreibung: Steuert die Tötung des Mans durch einen Yam.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlYamKillsMan(uint32_t I) {
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        SDL_Log("%s: ack double control",__FUNCTION__);
        return;
    }
    // Ab hier: Nach Tötung des Mans den Yam auf ursprüngliche Man-Position setzen
    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_MONSTER_KILLS_UP) {
        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_YAM;
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_MONSTER_KILLS_LEFT) {
        Playfield.pLevel[I - 1] = EMERALD_YAM;
        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_STAND;
    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_MONSTER_KILLS_RIGHT) {
        Playfield.pLevel[I + 1] = EMERALD_YAM;
        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_STAND;
    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_MONSTER_KILLS_DOWN) {
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_YAM;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
    }
    Playfield.pLevel[I] = EMERALD_SPACE;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    Playfield.pLastYamDirection[I] = EMERALD_LAST_YAM_DIR_BLOCKED;
}


/*----------------------------------------------------------------------------
Name:           IsYamCompleteBlocked
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Yam komplett (zu allen Seiten) blockiert ist.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  bool, true = Yam komplett blockiert
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
bool IsYamCompleteBlocked(uint32_t I) {
    return ((Playfield.pLevel[I - Playfield.uLevel_X_Dimension] != EMERALD_SPACE) && (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] != EMERALD_MAN) &&
           (Playfield.pLevel[I - 1] != EMERALD_SPACE) && (Playfield.pLevel[I - 1] != EMERALD_MAN) &&
           (Playfield.pLevel[I + 1] != EMERALD_SPACE) && (Playfield.pLevel[I + 1] != EMERALD_MAN) &&
           (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] != EMERALD_SPACE) && (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] != EMERALD_MAN) && (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] != EMERALD_ACIDPOOL_TOP_MID));
}

