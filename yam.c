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
    uint8_t uRandom;

    // Zunächst prüfen, ob es um den YAM einen Saphir gibt, den er fressen kann
    if ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SAPPHIRE) && (Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] == EMERALD_ANIM_STAND)) {
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_SAPPHIRE_SHRINK;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND; // YAM ist beim Fressen blockiert.
        PreparePlaySound(SOUND_YAM,I);
    } else if ((Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SAPPHIRE) && (Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] == EMERALD_ANIM_STAND)) {
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_SAPPHIRE_SHRINK;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND; // YAM ist beim Fressen blockiert.
        PreparePlaySound(SOUND_YAM,I);
    } else if ((Playfield.pLevel[I + 1] == EMERALD_SAPPHIRE) && (Playfield.pStatusAnimation[I + 1] == EMERALD_ANIM_STAND)) {
        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_SAPPHIRE_SHRINK;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND; // YAM ist beim Fressen blockiert.
        PreparePlaySound(SOUND_YAM,I);
    } else if ((Playfield.pLevel[I - 1] == EMERALD_SAPPHIRE) && (Playfield.pStatusAnimation[I - 1] == EMERALD_ANIM_STAND)) {
        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_SAPPHIRE_SHRINK;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND; // YAM ist beim Fressen blockiert.
        PreparePlaySound(SOUND_YAM,I);
    } else {
        // YAM frisst nicht
        switch (Playfield.pStatusAnimation[I]) {
            case (EMERALD_ANIM_STAND):
                // Falls YAM steht und nichts frist, dann neue zufällige Richtung wählen
                uRandom = randf(1,5);       // Ergibt Zufallszahl zwischen 1-4  (1 = links, 2 = oben, 3 = rechts, 4 = runter
                switch (uRandom) {
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
                        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND; // YAM macht nichts.
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
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I - 1] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pInvalidElement[I - 1] = EMERALD_YAM;
        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_CLEAN_RIGHT | EMERALD_ANIM_LEFT;
        // Aktuelles Element auf Animation "links"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
    } else {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
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
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I + 1] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pInvalidElement[I + 1] = EMERALD_YAM;
        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_CLEAN_LEFT | EMERALD_ANIM_RIGHT;
        // Aktuelles Element auf Animation "rechts"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
    } else {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
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
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pInvalidElement[I - Playfield.uLevel_X_Dimension] = EMERALD_YAM;
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_DOWN | EMERALD_ANIM_UP;
        // Aktuelles Element auf Animation "oben"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_UP;
    } else {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
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
    if ( ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN1) || ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_BORN2) ) {
        // YamDown kann vom Replikator geboren werden, dann hier nichts machen
        return;
    }
    if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = EMERALD_YAM;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_UP | EMERALD_ANIM_DOWN;
        // Aktuelles Element auf Animation "stehend"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Yam ins Säurebecken?
        SDL_Log("Yam falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = EMERALD_YAM;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        PreparePlaySound(SOUND_YAM,I);
    }
}
