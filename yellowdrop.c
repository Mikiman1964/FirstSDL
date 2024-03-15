#include "EmeraldMine.h"
#include "explosion.h"
#include "yellowdrop.h"
#include "mystd.h"
#include "sound.h"

extern PLAYFIELD Playfield;


/*----------------------------------------------------------------------------
Name:           ControlYellowDrop
------------------------------------------------------------------------------
Beschreibung: Steuert einen gelben Tropfen. Dieser fällt mit halber Geschwindigkeit.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlYellowDrop(uint32_t I) {
    uint32_t uSelfStatus;

    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        return;
    }
    uSelfStatus = Playfield.pStatusAnimation[I] & 0xFF000000;
    if ( (uSelfStatus == EMERALD_ANIM_BORN1) || (uSelfStatus == EMERALD_ANIM_BORN2) ) {
        // Gelber Tropfen kann vom Replikator geboren werden, dann hier nichts machen
        return;
    } else if (uSelfStatus == EMERALD_ANIM_YELLOW_DROP_1) {
        // Tropfen wurde bereits behandelt
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_YELLOW_DROP_2;
        //Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_UP | EMERALD_YELLOW_DROP;
    } else if (IS_SPACE(I + Playfield.uLevel_X_Dimension)) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_YELLOW_DROP_1;
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Zunächst invalides Element ignorieren, da Tropfen in 2 Phasen nach unten fällt.

        Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = EMERALD_YELLOW_DROP;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_NOTHING;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Tropfen ins Säurebecken?
        SDL_Log("Yellow drop falls in pool");
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = EMERALD_YELLOW_DROP;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else {
        // Unter Tropfen ist nicht frei
        if ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MAN) && (!Playfield.bManProtected) && (Playfield.uShieldCoinTimeLeft == 0)) {
            SDL_Log("Yellow drop kills man");
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_MAN_DIES;
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL | EMERALD_ANIM_MAN_DIES_P1;
            PreparePlaySound(SOUND_MAN_CRIES,I);
            Playfield.bManDead = true;
            Playfield.pLevel[I] = EMERALD_YELLOW_CHEESE;     // Tropfen, der Man getroffen hat, verwandelt sich in Käse
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            PreparePlaySound(SOUND_CHEESE,I);
        } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_STANDMINE) {
            SDL_Log("Yellow drop hit stand mine");
            ControlCentralExplosion(I + Playfield.uLevel_X_Dimension);
            PreparePlaySound(SOUND_EXPLOSION,I);
        } else {
            Playfield.pLevel[I] = EMERALD_YELLOW_CHEESE;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            PreparePlaySound(SOUND_CHEESE,I);
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlYellowCheese
------------------------------------------------------------------------------
Beschreibung: Steuert den gelben Käse. Die Ausbreitungsgeschwindigkeit ist
              im Leveleditor einstellbar.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlYellowCheese(uint32_t I) {
    int nRandom;
    int nLoops;
    int nDirectionRandom;

    nRandom = randn(1,9990);       // Ergibt Zufallszahl zwischen 1-9990
    if (Playfield.uYellowCheeseSpreadSpeed >= nRandom) {
        if (Playfield.uYellowCheeseSpreadSpeed > 9000) {
            nLoops = 3;
        } else if (Playfield.uYellowCheeseSpreadSpeed > 8000) {
            nLoops = 2;
        } else {
            nLoops = 1;
        }
        do {
            nDirectionRandom = randn(1,4);       // Ergibt Zufallszahl zwischen 1-4  (1 = links, 2 = oben, 3 = rechts, 4 = unten
            switch (nDirectionRandom) {
                case (1):           // links prüfen
                    if ( (IS_SPACE(I - 1)) || (Playfield.pLevel[I - 1] == EMERALD_SAND) || (Playfield.pLevel[I - 1] == EMERALD_SAND_INVISIBLE) || (Playfield.pLevel[I - 1] == EMERALD_SWAMP) ) {
                        Playfield.pLevel[I - 1] = EMERALD_YELLOW_DROP_COMES;
                        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_STAND;
                    }
                    break;
                case (2):           // oben prüfen
                    if ( (IS_SPACE(I - Playfield.uLevel_X_Dimension)) || (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SAND) || (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SAND_INVISIBLE) || (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SWAMP) ) {
                        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_YELLOW_DROP_COMES;
                        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                    }
                    break;
                case (3):           // rechts prüfen
                    if ( (IS_SPACE(I + 1)) || (Playfield.pLevel[I + 1] == EMERALD_SAND) || (Playfield.pLevel[I + 1] == EMERALD_SAND_INVISIBLE) || (Playfield.pLevel[I + 1] == EMERALD_SWAMP) ) {
                        Playfield.pLevel[I + 1] = EMERALD_YELLOW_DROP_COMES;
                        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_STAND | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    }
                    break;
                case (4):           // unten prüfen
                    if ( (IS_SPACE(I + Playfield.uLevel_X_Dimension)) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SAND) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SAND_INVISIBLE) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SWAMP)) {
                        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_YELLOW_DROP_COMES;
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
Name:           ControlSpreadYellowCheese
------------------------------------------------------------------------------
Beschreibung: Ein enstehender Tropfen wird gesteuert und somit zum Tropfen.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlSpreadYellowCheese(uint32_t I) {
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        return;
    }
   Playfield.pLevel[I] = EMERALD_YELLOW_DROP;           // Enstehenden Tropfen in Tropfen wandeln.
   Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
   ControlYellowDrop(I);
}


/*----------------------------------------------------------------------------
Name:           IsYellowCheeseAround
------------------------------------------------------------------------------
Beschreibung: Prüft, ob sich gelber Käse um die angegebene Position I befindet.
Parameter
      Eingang: I, uint32_t, Index im Level, Position, die geprüft werden soll
      Ausgang: -
Rückgabewert:  bool, true = Käse hat Kontakt mit Position I
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
bool IsYellowCheeseAround(uint32_t I) {
    return  ((Playfield.pLevel[I - 1] == EMERALD_YELLOW_CHEESE) ||    // links
            (Playfield.pLevel[I + 1] == EMERALD_YELLOW_CHEESE) ||    // rechts
            (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_YELLOW_CHEESE) || // oben
            (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_YELLOW_CHEESE)); // unten
}
