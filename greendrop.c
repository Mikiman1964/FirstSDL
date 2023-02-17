#include "EmeraldMine.h"
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
        Playfield.pLevel[I] = EMERALD_GREEN_CHEESE;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        PreparePlaySound(SOUND_CHEESE,I);
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
    int nDirectionRandom;

    nRandom = randn(1,1000);       // Ergibt Zufallszahl zwischen 1-1000
    if (Playfield.uCheeseSpreadSpeed >= nRandom) {
        nDirectionRandom = randn(1,4);       // Ergibt Zufallszahl zwischen 1-4  (1 = links, 2 = oben, 3 = rechts, 4 = unten
        switch (nDirectionRandom) {
            case (1):           // links prüfen
                if ( (Playfield.pLevel[I - 1] == EMERALD_SPACE) || (Playfield.pLevel[I - 1] == EMERALD_SAND) ) {
                    Playfield.pLevel[I - 1] = EMERALD_GREEN_DROP_COMES;
                    Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_STAND;
                }
                break;
            case (2):           // oben prüfen
                if ( (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) || (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SAND) ) {
                    Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_GREEN_DROP_COMES;
                    Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                }
                break;
            case (3):           // rechts prüfen
                if ( (Playfield.pLevel[I + 1] == EMERALD_SPACE) || (Playfield.pLevel[I + 1] == EMERALD_SAND) ) {
                    Playfield.pLevel[I + 1] = EMERALD_GREEN_DROP_COMES;
                    Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_STAND | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                }
                break;
            case (4):           // unten prüfen
                if ( (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SAND) ) {
                    Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_GREEN_DROP_COMES;
                    Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                }
                break;
            default:            // nichts machen
                break;

        }
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
