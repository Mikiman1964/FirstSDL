#include "EmeraldMine.h"
#include "explosion.h"
#include "greendrop.h"
#include "mystd.h"
#include "sound.h"

extern PLAYFIELD Playfield;


/*----------------------------------------------------------------------------
Name:           ControlGreenDrop
------------------------------------------------------------------------------
Beschreibung: Steuert eine grünen Tropfen. Dieser fällt mit halber Geschwindigkeit
              nach unten.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlGreenDrop(uint32_t I) {
    uint32_t uSelfStatus;

    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        return;
    }
    uSelfStatus = Playfield.pStatusAnimation[I] & 0xFF000000;
    if ( (uSelfStatus == EMERALD_ANIM_BORN1) || (uSelfStatus == EMERALD_ANIM_BORN2) ) {
        // Grüner Tropfen kann vom Replikator geboren werden, dann hier nichts machen
        return;
    } else if (uSelfStatus == EMERALD_ANIM_GREEN_DROP_1) {
        // Tropfen wurde bereits behandelt
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_GREEN_DROP_2;
        //Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_UP | EMERALD_GREEN_DROP;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_GREEN_DROP_1;
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Zunächst invalides Element ignorieren, da Tropfen in 2 Phasen nach unten fällt.

        Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = EMERALD_GREEN_DROP;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_NOTHING;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Tropfen ins Säurebecken?
        SDL_Log("Green drop falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = EMERALD_GREEN_DROP;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else {
        // Unter Tropfen ist nicht frei
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MAN) {
            SDL_Log("Green drop kills man");
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_MAN_DIES;
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL | EMERALD_ANIM_MAN_DIES_P1;
            PreparePlaySound(SOUND_MAN_CRIES,I);
            Playfield.bManDead = true;
            Playfield.pLevel[I] = EMERALD_GREEN_CHEESE;     // Tropfen, der Man getroffen hat, verwandelt sich in Käse
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            PreparePlaySound(SOUND_CHEESE,I);
        } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_STANDMINE) {
            SDL_Log("Green drop hit stand mine");
            ControlCentralExplosion(I + Playfield.uLevel_X_Dimension);
            PreparePlaySound(SOUND_EXPLOSION,I);
        } else {
            Playfield.pLevel[I] = EMERALD_GREEN_CHEESE;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            PreparePlaySound(SOUND_CHEESE,I);
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlGreenCheese
------------------------------------------------------------------------------
Beschreibung: Steuert den grünen Käse. Die Ausbreitungsgeschwindigkeit ist
              im Leveleditor einstellbar.
              nach unten.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlGreenCheese(uint32_t I) {
    int nRandom;
    int nLoops;
    int nDirectionRandom;

    nRandom = randn(1,9990);       // Ergibt Zufallszahl zwischen 1-9990
    if (Playfield.uCheeseSpreadSpeed >= nRandom) {
        if (Playfield.uCheeseSpreadSpeed > 9000) {
            nLoops = 3;
        } else if (Playfield.uCheeseSpreadSpeed > 8000) {
            nLoops = 2;
        } else {
            nLoops = 1;
        }
        do {
            nDirectionRandom = randn(1,4);       // Ergibt Zufallszahl zwischen 1-4  (1 = links, 2 = oben, 3 = rechts, 4 = unten
            switch (nDirectionRandom) {
                case (1):           // links prüfen
                    if ( (Playfield.pLevel[I - 1] == EMERALD_SPACE) || (Playfield.pLevel[I - 1] == EMERALD_SAND) || (Playfield.pLevel[I - 1] == EMERALD_SAND_INVISIBLE) || (Playfield.pLevel[I - 1] == EMERALD_SWAMP) ) {
                        Playfield.pLevel[I - 1] = EMERALD_GREEN_DROP_COMES;
                        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_STAND;
                    }
                    break;
                case (2):           // oben prüfen
                    if ( (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) || (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SAND) || (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SAND_INVISIBLE) || (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SWAMP) ) {
                        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_GREEN_DROP_COMES;
                        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                    }
                    break;
                case (3):           // rechts prüfen
                    if ( (Playfield.pLevel[I + 1] == EMERALD_SPACE) || (Playfield.pLevel[I + 1] == EMERALD_SAND) || (Playfield.pLevel[I + 1] == EMERALD_SAND_INVISIBLE) || (Playfield.pLevel[I + 1] == EMERALD_SWAMP) ) {
                        Playfield.pLevel[I + 1] = EMERALD_GREEN_DROP_COMES;
                        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_STAND | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    }
                    break;
                case (4):           // unten prüfen
                    if ( (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SAND) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SAND_INVISIBLE) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SWAMP)) {
                        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_GREEN_DROP_COMES;
                        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    }
                    break;
                default:            // nichts machen
                    break;
            }
            nLoops--;
        } while (nLoops > 0);
    }
}


/*----------------------------------------------------------------------------
Name:           ControlSpreadCheese
------------------------------------------------------------------------------
Beschreibung: Ein enstehender Tropfen wird gesteuert und somit zum Tropfen.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSpreadCheese(uint32_t I) {
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        return;
    }
   Playfield.pLevel[I] = EMERALD_GREEN_DROP;           // Enstehenden Tropfen in Tropfen wandeln.
   Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
   ControlGreenDrop(I);
}


/*----------------------------------------------------------------------------
Name:           IsGreenCheeseAround
------------------------------------------------------------------------------
Beschreibung: Prüft, ob sich grüner Käse um die angegebene Position I befindet.
Parameter
      Eingang: I, uint32_t, Index im Level, Position, die geprüft werden soll
      Ausgang: -
Rückgabewert:  bool, true = Käse hat Kontakt mit Position I
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
bool IsGreenCheeseAround(uint32_t I) {
    return  ((Playfield.pLevel[I - 1] == EMERALD_GREEN_CHEESE) ||    // links
            (Playfield.pLevel[I + 1] == EMERALD_GREEN_CHEESE) ||    // rechts
            (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_GREEN_CHEESE) || // oben
            (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_GREEN_CHEESE)); // unten
}
