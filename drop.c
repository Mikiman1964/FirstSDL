#include "drop.h"
#include "EmeraldMine.h"
#include "explosion.h"
#include "mystd.h"
#include "sound.h"

extern PLAYFIELD Playfield;

/*----------------------------------------------------------------------------
Name:           ControlDrop
------------------------------------------------------------------------------
Beschreibung: Steuert einen Tropfen. Dieser fällt mit halber Geschwindigkeit.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlDrop(uint32_t I) {
    uint32_t uSelfStatus;
    uint16_t uDropElement;  // kann grüner oder gelber Tropfen sein

    uDropElement = Playfield.pLevel[I];
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        return;
    }
    uSelfStatus = Playfield.pStatusAnimation[I] & 0xFF000000;
    if ( (uSelfStatus == EMERALD_ANIM_BORN1) || (uSelfStatus == EMERALD_ANIM_BORN2) ) {
        // Tropfen kann vom Replikator geboren werden, dann hier nichts machen
        return;
    } else if (uSelfStatus == EMERALD_ANIM_DROP_1) {
        // Tropfen wurde bereits behandelt
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DROP_2;
        //Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_UP | uDropElement;
    } else if (IS_SPACE(I + Playfield.uLevel_X_Dimension)) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DROP_1;
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Zunächst invalides Element ignorieren, da Tropfen in 2 Phasen nach unten fällt.
        Playfield.pInvalidElement[I + Playfield.uLevel_X_Dimension] = uDropElement;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_CLEAN_NOTHING;
    } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_ACIDPOOL) {   // Fällt Tropfen ins Säurebecken?
        /*
        if (uDropElement == EMERALD_GREEN_DROP) {
            SDL_Log("Green drop falls in pool");
        } else {
            SDL_Log("Yellow drop falls in pool");
        }
        */
        Playfield.pLevel[I] = EMERALD_ACIDPOOL_DESTROY;
        Playfield.pInvalidElement[I] = uDropElement;
        PreparePlaySound(SOUND_POOL_BLUB,I);
        return;
    } else {
        // Unter Tropfen ist nicht frei
        if ((Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MAN) && (!Playfield.bManProtected) && (Playfield.uShieldCoinTimeLeft == 0)) {
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_MAN_DIES;
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL | EMERALD_ANIM_MAN_DIES_P1;
            PreparePlaySound(SOUND_MAN_CRIES,I);
            Playfield.bManDead = true;
            if (uDropElement == EMERALD_GREEN_DROP) {
                // SDL_Log("Green drop kills man");
                Playfield.pLevel[I] = EMERALD_GREEN_CHEESE;     // Tropfen, der Man getroffen hat, verwandelt sich in Käse
            } else {
                // SDL_Log("Yellow drop kills man");
                Playfield.pLevel[I] = EMERALD_YELLOW_CHEESE;     // Tropfen, der Man getroffen hat, verwandelt sich in Käse
            }
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            PreparePlaySound(SOUND_CHEESE,I);
        } else if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MINE_CONTACT) {
            /*
            if (uDropElement == EMERALD_GREEN_DROP) {
                SDL_Log("Green drop hit contact mine");
            } else {
                SDL_Log("Yellow drop hit contact mine");
            }
            */
            ControlCentralExplosion(I + Playfield.uLevel_X_Dimension,EMERALD_SPACE);
            PreparePlaySound(SOUND_EXPLOSION,I);
        } else {
            if (uDropElement == EMERALD_GREEN_DROP) {
                Playfield.pLevel[I] = EMERALD_GREEN_CHEESE;
            } else {
                Playfield.pLevel[I] = EMERALD_YELLOW_CHEESE;
            }
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            PreparePlaySound(SOUND_CHEESE,I);
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlCheese
------------------------------------------------------------------------------
Beschreibung: Steuert den Käse. Die Ausbreitungsgeschwindigkeit ist
              im Leveleditor einstellbar.
Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlCheese(uint32_t I) {
    int nRandom;
    int nLoops;
    int nDirectionRandom;
    uint16_t uCheeseElement;            // kann grüner oder gelber Käse sein
    uint16_t uCheeseComesElement;       // Element, wenn grüner oder gelber Käse entsteht
    uint32_t uCheeseSpreadSpeed;        // grüner und gelber Käse können unterschiedliche Ausbreitungsgeschwindigkeiten haben

    uCheeseElement = Playfield.pLevel[I];
    if (uCheeseElement == EMERALD_GREEN_CHEESE) {
        uCheeseSpreadSpeed = Playfield.uGreenCheeseSpreadSpeed;
        uCheeseComesElement = EMERALD_GREEN_DROP_COMES;
    } else {
        uCheeseSpreadSpeed = Playfield.uYellowCheeseSpreadSpeed;
        uCheeseComesElement = EMERALD_YELLOW_DROP_COMES;
    }
    nRandom = randn(1,9990);       // Ergibt Zufallszahl zwischen 1-9990
    if (uCheeseSpreadSpeed >= nRandom) {
        if (uCheeseSpreadSpeed > 9000) {
            nLoops = 3;
        } else if (uCheeseSpreadSpeed > 8000) {
            nLoops = 2;
        } else {
            nLoops = 1;
        }
        do {
            nDirectionRandom = randn(1,4);       // Ergibt Zufallszahl zwischen 1-4  (1 = links, 2 = oben, 3 = rechts, 4 = unten
            switch (nDirectionRandom) {
                case (1):           // links prüfen
                    if (IsFieldPosForCheeseFree(I - 1)) {
                        Playfield.pLevel[I - 1] = uCheeseComesElement;
                        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_STAND;
                    }
                    break;
                case (2):           // oben prüfen
                    if (IsFieldPosForCheeseFree(I - Playfield.uLevel_X_Dimension)) {
                        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = uCheeseComesElement;
                        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                    }
                    break;
                case (3):           // rechts prüfen
                    if (IsFieldPosForCheeseFree(I + 1)) {
                        Playfield.pLevel[I + 1] = uCheeseComesElement;
                        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_STAND | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    }
                    break;
                case (4):           // unten prüfen
                    if (IsFieldPosForCheeseFree(I + Playfield.uLevel_X_Dimension)) {
                        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = uCheeseComesElement;
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
Beschreibung: Ein enstehender Tropfen (EMERALD_GREEN_DROP_COMES oder EMERALD_YELLOW_DROP_COMES)
              wird gesteuert und somit zum Tropfen.
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
    if (Playfield.pLevel[I] == EMERALD_GREEN_DROP_COMES) {
        Playfield.pLevel[I] = EMERALD_GREEN_DROP;           // Enstehenden Tropfen in Tropfen wandeln.
    } else {
        Playfield.pLevel[I] = EMERALD_YELLOW_DROP;          // Enstehenden Tropfen in Tropfen wandeln.
    }
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    ControlDrop(I);
}


/*----------------------------------------------------------------------------
Name:           IsCheeseAround
------------------------------------------------------------------------------
Beschreibung: Prüft, ob sich Käse um die angegebene Position I befindet.
Parameter
      Eingang: I, uint32_t, Index im Level, Position, die geprüft werden soll
      Ausgang: -
Rückgabewert:  bool, true = Käse hat Kontakt mit Position I
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
bool IsCheeseAround(uint32_t I) {
    return  ((Playfield.pLevel[I - 1] == EMERALD_GREEN_CHEESE) || (Playfield.pLevel[I - 1] == EMERALD_YELLOW_CHEESE) ||   // links
            (Playfield.pLevel[I + 1] == EMERALD_GREEN_CHEESE) || (Playfield.pLevel[I + 1] == EMERALD_YELLOW_CHEESE) ||   // rechts
            (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_GREEN_CHEESE) || (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_YELLOW_CHEESE) || // oben
            (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_GREEN_CHEESE) || (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_YELLOW_CHEESE)); // unten
}


/*----------------------------------------------------------------------------
Name:           IsFieldPosForCheeseFree
------------------------------------------------------------------------------
Beschreibung: Prüft, ob die angegebene Position durch Käse eingenommen werden kann.
Parameter
      Eingang: I, uint32_t, Index im Level, Position, die geprüft werden soll
      Ausgang: -
Rückgabewert:  bool, true = Position kann durch Käse eingenommen werden, sonst nicht
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
bool IsFieldPosForCheeseFree(uint32_t uPosition) {

 return ( (IS_SPACE(uPosition)) ||
          (Playfield.pLevel[uPosition] == EMERALD_EARTH) ||
          (Playfield.pLevel[uPosition] == EMERALD_EARTH_INVISIBLE) ||
          (Playfield.pLevel[uPosition] == EMERALD_QUICKSAND) ||
          (Playfield.pLevel[uPosition] == EMERALD_QUICKSAND_SLOW) );
}
