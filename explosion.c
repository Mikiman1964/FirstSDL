#include "EmeraldMine.h"
#include "alien.h"
#include "bomb.h"
#include "crystal.h"
#include "emerald.h"
#include "explosion.h"
#include "greendrop.h"
#include "megabomb.h"
#include "mine.h"
#include "mole.h"
#include "nut.h"
#include "perl.h"
#include "ruby.h"
#include "saphir.h"
#include "sound.h"
#include "stone.h"
#include "teleporter.h"
#include "yam.h"

extern PLAYFIELD Playfield;

// Alle Explosionen laufen über 2 Phasen.

/*----------------------------------------------------------------------------
Name:           ControlExplosionToElement
------------------------------------------------------------------------------
Beschreibung: Steuert eine Einzel-Explosion, die sich in ein beliebiges Element wandelt.
              Das zu erzeugende Element ist im Array Playfield.pStatusAnimation[] untergebracht.
              Wird in ControlPreElements() aufgerufen.

Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlExplosionToElement(uint32_t I) {
    uint16_t uNewElement;

    if (Playfield.pLevel[I] == EMERALD_EXPLOSION_TO_ELEMENT_1) {
        Playfield.pLevel[I] = EMERALD_EXPLOSION_TO_ELEMENT_2;
    } else if (Playfield.pLevel[I] == EMERALD_EXPLOSION_TO_ELEMENT_2) {
        uNewElement = Playfield.pStatusAnimation[I] & 0xFFFF;
        if ((uNewElement > EMERALD_NONE) && (uNewElement <= EMERALD_MAX_ELEMENT)) {
            Playfield.pLevel[I] = uNewElement;
            if (uNewElement == EMERALD_DYNAMITE_ON) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_DYNAMITE_ON_P1;
            } else if (uNewElement == EMERALD_YAM) {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND | EMERALD_ANIM_YAM_WAS_BLOCKED;  // Wenn neuer Yam entsteht, diesen erstmal blocken
            } else if ((uNewElement == EMERALD_TELEPORTER_RED) || (uNewElement == EMERALD_TELEPORTER_YELLOW) || (uNewElement == EMERALD_TELEPORTER_GREEN) || (uNewElement == EMERALD_TELEPORTER_BLUE)) {
                AddTeleporterCoordinate(uNewElement,I);
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            } else {
                Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
            }
        } else {
            SDL_Log("%s: Error: try to set bad element (0x%04x) at position %u",__FUNCTION__,uNewElement,I);
        }
    }
}


/*----------------------------------------------------------------------------
Name:           CleanInvalidFieldsForCentralExplosion
------------------------------------------------------------------------------
Beschreibung: Entfernt invalide Felder innerhalb des Explosionsbereichs. Die
              Routine sucht innerhalb des Bereichs nach beweglichen Elementen und
              deren invaliden Feldern.
              Objekte die von oben (nach unten) oder links (nacht rechts) in die
              Explosion hineingelaufen sind (also zeitlich vorher gesteuert wurden),
              werden neu (nach)gesteuert.

Parameter
      Eingang: I, int, Index im Level (zentraler Punkt)
               bMega, bool, true = Mega-Explosion, sonst 3x3
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CleanInvalidFieldsForCentralExplosion(int I,bool bMega) {
    uint32_t K, MaxK;
    int nCoordinate;
    int *pCoordArray;
    int nX;     // Lineare nCoordinate in X-
    int nY;     // und Y umgerechnet

    if (bMega) {
        MaxK = 20;
        pCoordArray = Playfield.nCentralMegaExplosionCoordinates;
    } else {
        MaxK = 8;
        pCoordArray = Playfield.nCentralExplosionCoordinates;
    }
    // Pass 1: Entfernen der invaliden Elemente von Objekten, die sich innerhalb der Explosion befinden
    for (K = 0; K < MaxK; K++) {
        nCoordinate = I + pCoordArray[K];
        // Mega-Explosionen könnten theoretisch das Spielfeld überwinden. Daher wird hier mit X/Y-Koordinaten abegrüft, ob innerhalb des Spielfeldes gearbeitet wird
        if (nCoordinate > 0) {
            nX = nCoordinate % Playfield.uLevel_X_Dimension;
            nY = nCoordinate / Playfield.uLevel_X_Dimension;
            if ((nX > 0) && (nX < (Playfield.uLevel_X_Dimension - 1)) && (nY > 0) && (nY < (Playfield.uLevel_Y_Dimension - 1))) {
                switch (Playfield.pLevel[nCoordinate]) {    // Element
                    case (EMERALD_STONE):
                    case (EMERALD_EMERALD):
                    case (EMERALD_RUBY):
                    case (EMERALD_SAPPHIRE):
                    case (EMERALD_PERL):
                    case (EMERALD_CRYSTAL):
                    case (EMERALD_NUT):
                    case (EMERALD_BOMB):
                    case (EMERALD_MEGABOMB):
                    case (EMERALD_MOLE_UP):
                    case (EMERALD_MOLE_RIGHT):
                    case (EMERALD_MOLE_DOWN):
                    case (EMERALD_MOLE_LEFT):
                    case (EMERALD_MINE_UP):
                    case (EMERALD_MINE_RIGHT):
                    case (EMERALD_MINE_DOWN):
                    case (EMERALD_MINE_LEFT):
                    case (EMERALD_BEETLE_UP):
                    case (EMERALD_BEETLE_RIGHT):
                    case (EMERALD_BEETLE_DOWN):
                    case (EMERALD_BEETLE_LEFT):
                    case (EMERALD_ALIEN):
                    case (EMERALD_YAM):
                    case (EMERALD_GREEN_DROP):
                    case (EMERALD_REMOTEBOMB):
                        switch (Playfield.pStatusAnimation[nCoordinate] & 0x0000FF00) { // Animationsstatus
                            case (EMERALD_ANIM_UP):
                                if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == EMERALD_INVALID) {
                                    Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_SPACE;
                                    Playfield.pInvalidElement[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_NONE;
                                    Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;   // Bewegtes Objekt zum Stillstand bringen
                                }
                                break;
                            case (EMERALD_ANIM_RIGHT):
                                if (Playfield.pLevel[nCoordinate + 1] == EMERALD_INVALID) {
                                    Playfield.pLevel[nCoordinate + 1] = EMERALD_SPACE;
                                    Playfield.pInvalidElement[nCoordinate + 1] = EMERALD_NONE;
                                    Playfield.pStatusAnimation[nCoordinate + 1] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;   // Bewegtes Objekt zum Stillstand bringen
                                }
                                break;
                            case (EMERALD_ANIM_DOWN):
                                if (Playfield.pLevel[nCoordinate + Playfield.uLevel_X_Dimension] == EMERALD_INVALID) {
                                    Playfield.pLevel[nCoordinate + Playfield.uLevel_X_Dimension] = EMERALD_SPACE;
                                    Playfield.pInvalidElement[nCoordinate + Playfield.uLevel_X_Dimension] = EMERALD_NONE;
                                    Playfield.pStatusAnimation[nCoordinate + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;   // Bewegtes Objekt zum Stillstand bringen
                                }
                                break;
                            case (EMERALD_ANIM_LEFT):
                                if (Playfield.pLevel[nCoordinate - 1] == EMERALD_INVALID) {
                                    Playfield.pLevel[nCoordinate - 1] = EMERALD_SPACE;
                                    Playfield.pInvalidElement[nCoordinate - 1] = EMERALD_NONE;
                                    Playfield.pStatusAnimation[nCoordinate - 1] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;   // Bewegtes Objekt zum Stillstand bringen
                                }
                                break;
                            default:
                                //
                                if ( (Playfield.pLevel[nCoordinate] == EMERALD_GREEN_DROP) && (Playfield.pLevel[nCoordinate + Playfield.uLevel_X_Dimension] == EMERALD_INVALID) &&
                                     ((Playfield.pStatusAnimation[nCoordinate] ==  EMERALD_ANIM_GREEN_DROP_1) ||(Playfield.pStatusAnimation[nCoordinate] == EMERALD_ANIM_GREEN_DROP_2)) ) {
                                    Playfield.pLevel[nCoordinate + Playfield.uLevel_X_Dimension] = EMERALD_SPACE;
                                    Playfield.pInvalidElement[nCoordinate + Playfield.uLevel_X_Dimension] = EMERALD_NONE;
                                    Playfield.pStatusAnimation[nCoordinate + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;   // Bewegtes Objekt zum Stillstand bringen
                                }
                        }
                        break;
                }
            }
        }
    }
    // Pass 2: Entfernen der invaliden Elemente von Objekten, die sich von oben (nach unten) oder links (nach rechts) in die Explosion bewegen
    // Für diese Objekte dann Re-Kontrolle des Objektes veranlassen.
    for (K = 0; K < MaxK; K++) {
        nCoordinate = I + pCoordArray[K];
        // Mega-Explosionen könnten theoretisch das Spielfeld überwinden. Daher wird hier mit X/Y-Koordinaten abegrüft, ob innerhalb des Spielfeldes gearbeitet wird
        if (nCoordinate > 0) {
            nX = nCoordinate % Playfield.uLevel_X_Dimension;
            nY = nCoordinate / Playfield.uLevel_X_Dimension;
            if ((nX > 0) && (nX < (Playfield.uLevel_X_Dimension - 1)) && (nY > 0) && (nY < (Playfield.uLevel_Y_Dimension - 1))) {
                if (Playfield.pLevel[nCoordinate] == EMERALD_INVALID) {
                    // SDL_Log("%s[EMERALD_INVALID]:  at K = %u",__FUNCTION__,K);
                    // SDL_Log("  element:   0x%X     old anim: 0x%X",Playfield.pInvalidElement[uCoordinate],Playfield.pLastStatusAnimation[uCoordinate]);
                    // Zunächst Re-Kontrolle durchführen. Das invalide Feld wird zunächst belassen, damit das re-kontrollierte Objekt dort ein Hindernis erkennen kann und
                    // sich nicht erneut in die Explosion bewegt.
                    switch (Playfield.pInvalidElement[nCoordinate]) {    // Element
                        case (EMERALD_REMOTEBOMB):
                            // Ferngesteuerte Bombe kann von oben (nach unten) oder links (nach rechts) kommen
                            if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_RIGHT) {
                                if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - 1] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_REMOTEBOMB]: warning: Remote bombe not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_DOWN) {
                                if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_DOWN]: warning: Remote bomb not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else {
                                SDL_Log("%s: Remote bomb with bad anim: 0x%X found",__FUNCTION__,Playfield.pLastStatusAnimation[nCoordinate]);
                            }
                            break;
                        case (EMERALD_STONE):
                            // Stein kann von oben (nach unten) oder links (nach rechts) kommen
                            if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_RIGHT) {
                                if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - 1] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlStone(nCoordinate - 1);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_RIGHT]: warning: Stone not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_DOWN) {
                                if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlStone(nCoordinate - Playfield.uLevel_X_Dimension);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_DOWN]: warning: Stone not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else {
                                SDL_Log("%s: Stone with bad anim: 0x%X found",__FUNCTION__,Playfield.pLastStatusAnimation[nCoordinate]);
                            }
                            break;
                        case (EMERALD_EMERALD):
                            // Emerald kann von oben (nach unten) oder links (nach rechts) kommen
                            if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_RIGHT) {
                                if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - 1] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlEmerald(nCoordinate - 1);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_RIGHT]: warning: Emerald not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_DOWN) {
                                if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlEmerald(nCoordinate - Playfield.uLevel_X_Dimension);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_DOWN]: warning: Emerald not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else {
                                SDL_Log("%s: Emerald with bad anim: 0x%X found",__FUNCTION__,Playfield.pLastStatusAnimation[nCoordinate]);
                            }
                            break;
                        case (EMERALD_RUBY):
                            // Rubin kann von oben (nach unten) oder links (nach rechts) kommen
                            if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_RIGHT) {
                                if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - 1] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlRuby(nCoordinate - 1);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_RIGHT]: warning: Ruby not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_DOWN) {
                                if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlRuby(nCoordinate - Playfield.uLevel_X_Dimension);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_DOWN]: warning: Ruby not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else {
                                SDL_Log("%s: Ruby with bad anim: 0x%X found",__FUNCTION__,Playfield.pLastStatusAnimation[nCoordinate]);
                            }
                            break;
                        case (EMERALD_SAPPHIRE):
                            // Saphhire kann von oben (nach unten) oder links (nach rechts) kommen
                            if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_RIGHT) {
                                if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - 1] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlSaphir(nCoordinate - 1);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_RIGHT]: warning: Saphir not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_DOWN) {
                                if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlSaphir(nCoordinate - Playfield.uLevel_X_Dimension);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_DOWN]: warning: Saphir not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else {
                                SDL_Log("%s: Saphir with bad anim: 0x%X found",__FUNCTION__,Playfield.pLastStatusAnimation[nCoordinate]);
                            }
                            break;
                        case (EMERALD_PERL):
                            // Perle kann von oben (nach unten) oder links (nach rechts) kommen
                            if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_RIGHT) {
                                if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - 1] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlPerl(nCoordinate - 1);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_RIGHT]: warning: Perl not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_DOWN) {
                                if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN_SELF; // Damit Perle zerbricht
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlPerl(nCoordinate - Playfield.uLevel_X_Dimension);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_DOWN]: warning: Perl not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else {
                                SDL_Log("%s: Perl with bad anim: 0x%X found",__FUNCTION__,Playfield.pLastStatusAnimation[nCoordinate]);
                            }
                            break;
                        case (EMERALD_CRYSTAL):
                            // Kristall kann von oben (nach unten) oder links (nach rechts) kommen
                            if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_RIGHT) {
                                if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - 1] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlCrystal(nCoordinate - 1);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_RIGHT]: warning: Crystal not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_DOWN) {
                                if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlCrystal(nCoordinate - Playfield.uLevel_X_Dimension);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_DOWN]: warning: Crystal not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else {
                                SDL_Log("%s: Crystal with bad anim: 0x%X found",__FUNCTION__,Playfield.pLastStatusAnimation[nCoordinate]);
                            }
                            break;
                        case (EMERALD_NUT):
                            // Nuss kann von oben (nach unten) oder links (nach rechts) kommen
                            if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_RIGHT) {
                                if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - 1] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlNut(nCoordinate - 1);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_RIGHT]: warning: Nut not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_DOWN) {
                                if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlNut(nCoordinate - Playfield.uLevel_X_Dimension);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_DOWN]: warning: Nut not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else {
                                SDL_Log("%s: Nut with bad anim: 0x%X found",__FUNCTION__,Playfield.pLastStatusAnimation[nCoordinate]);
                            }
                            break;
                        case (EMERALD_BOMB):
                            // Bombe kann von oben (nach unten) oder links (nach rechts) kommen
                            if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_RIGHT) {
                                if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - 1] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlBomb(nCoordinate - 1);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_RIGHT]: warning: Bomb not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_DOWN) {
                                if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                    Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_CENTRAL_EXPLOSION;
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_DOWN]: warning: Bomb not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else {
                                SDL_Log("%s: Bomb with bad anim: 0x%X found",__FUNCTION__,Playfield.pLastStatusAnimation[nCoordinate]);
                            }
                            break;
                        case (EMERALD_MEGABOMB):
                            // Mega-Bombe kann von oben (nach unten) oder links (nach rechts) kommen
                            if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_RIGHT) {
                                if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - 1] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlMegaBomb(nCoordinate - 1);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_RIGHT]: warning: Mega-Bomb not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_DOWN) {
                                if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                    Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_CENTRAL_EXPLOSION_MEGA;
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_DOWN]: warning: Mega-Bomb not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else {
                                SDL_Log("%s: Mega-Bomb with bad anim: 0x%X found",__FUNCTION__,Playfield.pLastStatusAnimation[nCoordinate]);
                            }
                            break;
                        case (EMERALD_MOLE_RIGHT):
                            if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                Playfield.pStatusAnimation[nCoordinate - 1] = Playfield.pLastStatusAnimation[nCoordinate];
                                Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                ControlMoleRight(nCoordinate - 1);
                                Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                            } else {
                                SDL_Log("%s: warning: MoleRight not found. element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                            }
                            break;
                        case (EMERALD_MOLE_DOWN):
                            if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = Playfield.pLastStatusAnimation[nCoordinate];
                                Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                ControlMineDown(nCoordinate - Playfield.uLevel_X_Dimension);
                                Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                            } else {
                                SDL_Log("%s: warning: MoleDown not found. element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                            }
                            break;
                        case (EMERALD_MINE_RIGHT):
                            if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                Playfield.pStatusAnimation[nCoordinate - 1] = Playfield.pLastStatusAnimation[nCoordinate];
                                Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                ControlMineRight(nCoordinate - 1);
                                Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                            } else {
                                SDL_Log("%s: warning: MineRight not found. element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                            }
                            break;
                        case (EMERALD_MINE_DOWN):
                            if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = Playfield.pLastStatusAnimation[nCoordinate];
                                Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                ControlMineDown(nCoordinate - Playfield.uLevel_X_Dimension);
                                Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                            } else {
                                SDL_Log("%s: warning: MineDown not found. element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                            }
                            break;
                        case (EMERALD_BEETLE_RIGHT):
                            if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                Playfield.pStatusAnimation[nCoordinate - 1] = Playfield.pLastStatusAnimation[nCoordinate];
                                Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                ControlMineRight(nCoordinate - 1);
                                Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                            } else {
                                SDL_Log("%s: warning: BeetleRight not found. element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                            }
                            break;
                        case (EMERALD_BEETLE_DOWN):
                            if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = Playfield.pLastStatusAnimation[nCoordinate];
                                Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                ControlMineDown(nCoordinate - Playfield.uLevel_X_Dimension);
                                Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                            } else {
                                SDL_Log("%s: warning: BeetleDown not found. element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                            }
                            break;
                        case (EMERALD_ALIEN):
                            // Alien kann von oben (nach unten) oder links (nach rechts) kommen
                            if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_RIGHT) {
                                if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - 1] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlAlien(nCoordinate - 1);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_RIGHT]: warning: Alien not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_DOWN) {
                                if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlAlien(nCoordinate - Playfield.uLevel_X_Dimension);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_DOWN]: warning: Alien not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else {
                                SDL_Log("%s: Alien with bad anim: 0x%X found",__FUNCTION__,Playfield.pLastStatusAnimation[nCoordinate]);
                            }
                            break;
                        case (EMERALD_YAM):
                            // Yam kann von oben (nach unten) oder links (nach rechts) kommen
                            if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_RIGHT) {
                                if (Playfield.pLevel[nCoordinate - 1] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - 1] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlYam(nCoordinate - 1);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_RIGHT]: warning: Yam not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else if (Playfield.pLastStatusAnimation[nCoordinate] == EMERALD_ANIM_DOWN) {
                                if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                    Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                    Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                    Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                    ControlYam(nCoordinate - Playfield.uLevel_X_Dimension);
                                    Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                                } else {
                                    SDL_Log("%s[EMERALD_ANIM_DOWN]: warning: Yam not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                                }
                            } else {
                                SDL_Log("%s: Yam with bad anim: 0x%X found",__FUNCTION__,Playfield.pLastStatusAnimation[nCoordinate]);
                            }
                            break;
                        case (EMERALD_GREEN_DROP):
                            // Kann nur von oben kommen
                            if (Playfield.pLevel[nCoordinate - Playfield.uLevel_X_Dimension] == Playfield.pInvalidElement[nCoordinate]) {
                                Playfield.pStatusAnimation[nCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                                Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                                Playfield.pInvalidElement[nCoordinate] = EMERALD_NONE; // invalides Feld löschen
                                ControlGreenDrop(nCoordinate - Playfield.uLevel_X_Dimension);
                                Playfield.pLevel[nCoordinate] = EMERALD_SPACE; // Invalides Feld im Playfield löschen.
                            } else {
                                SDL_Log("%s: warning: green drop not found. instead element: 0x%X",__FUNCTION__,Playfield.pLevel[nCoordinate - 1]);
                            }
                            break;
                        default:
                            SDL_Log("%s: warning, element 0x%X can not controlled",__FUNCTION__,Playfield.pInvalidElement[nCoordinate]);
                            break;
                    }
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlCentralExplosion
------------------------------------------------------------------------------
Beschreibung: Steuert eine zentrale Explosion (3x3).
Parameter
      Eingang: I, uint32_t, Index im Level (zentraler Punkt)
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlCentralExplosion(uint32_t I) {
    uint32_t K;
    uint32_t uCoordinate;
    uint32_t uCheckExplosion;

    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    } else {
        Playfield.pLevel[I] = EMERALD_EXPLOSION_TO_ELEMENT_1;    // Mittelpunkt wird auf jedem Fall zur Explosion
        Playfield.pStatusAnimation[I] = EMERALD_SPACE;
        CleanInvalidFieldsForCentralExplosion(I,NO_MEGA_EXPLOSION);
        for (K = 0; K < 8; K++) {
            uCoordinate = I + Playfield.nCentralExplosionCoordinates[K];
            uCheckExplosion = CheckExplosionElement(Playfield.pLevel[uCoordinate],uCoordinate);
            switch (uCheckExplosion & 0xFFFF) {
                case (EMERALD_EXPLOSION_EMPTY):
                    Playfield.pLevel[uCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_SPACE;
                    break;
                case (EMERALD_EXPLOSION_EMPTY_MAN):
                    Playfield.pLevel[uCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_EMERALD;
                    Playfield.bManDead = true;
                    PreparePlaySound(SOUND_MAN_CRIES,I);
                    break;
                case (EMERALD_EXPLOSION_ELEMENT):
                    Playfield.pLevel[uCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                    Playfield.pStatusAnimation[uCoordinate] = uCheckExplosion >> 16;
                    break;
                case (EMERALD_EXPLOSION_NEWCENTRAL):
                    Playfield.pLevel[uCoordinate] = EMERALD_CENTRAL_EXPLOSION;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    break;
                case (EMERALD_EXPLOSION_NEWCENTRAL_MEGA):
                    Playfield.pLevel[uCoordinate] = EMERALD_CENTRAL_EXPLOSION_MEGA;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    break;
                case (EMERALD_EXPLOSION_NEWCENTRAL_BEETLE):
                    Playfield.pLevel[uCoordinate] = EMERALD_CENTRAL_EXPLOSION_BEETLE;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    break;
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlCentralMegaExplosion
------------------------------------------------------------------------------
Beschreibung: Steuert eine Mega-Explosion (3x3) + (4x3)
Parameter
      Eingang: nI, int, Index im Level (zentraler Punkt)
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlCentralMegaExplosion(int nI) {
    uint32_t K;
    int nCoordinate;
    uint32_t uCheckExplosion;
    int nX;     // Lineare nCoordinate in X-
    int nY;     // und Y umgerechnet
    if (Playfield.pStatusAnimation[nI] == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[nI] = EMERALD_ANIM_STAND;
    } else {
        Playfield.pLevel[nI] = EMERALD_EXPLOSION_TO_ELEMENT_1;    // Mittelpunkt wird auf jedem Fall zur Explosion
        Playfield.pStatusAnimation[nI] = EMERALD_SPACE;
        CleanInvalidFieldsForCentralExplosion(nI,MEGA_EXPLOSION);
        for (K = 0; K < 20; K++) {
            nCoordinate = nI + Playfield.nCentralMegaExplosionCoordinates[K];
            // Mega-Explosionen könnten theoretisch das Spielfeld überwinden. Daher wird hier mit X/Y-Koordinaten abegrüft, ob innerhalb des Spielfeldes gesprengt wird
            if (nCoordinate > 0) {
                nX = nCoordinate % Playfield.uLevel_X_Dimension;
                nY = nCoordinate / Playfield.uLevel_X_Dimension;
                if ((nX > 0) && (nX < (Playfield.uLevel_X_Dimension - 1)) && (nY > 0) && (nY < (Playfield.uLevel_Y_Dimension - 1))) {
                    uCheckExplosion = CheckExplosionElement(Playfield.pLevel[nCoordinate],nCoordinate);
                    switch (uCheckExplosion & 0xFFFF) {
                        case (EMERALD_EXPLOSION_EMPTY):
                            Playfield.pLevel[nCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                            Playfield.pStatusAnimation[nCoordinate] = EMERALD_SPACE;
                            break;
                        case (EMERALD_EXPLOSION_EMPTY_MAN):
                            Playfield.pLevel[nCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                            Playfield.pStatusAnimation[nCoordinate] = EMERALD_EMERALD;
                            Playfield.bManDead = true;
                            PreparePlaySound(SOUND_MAN_CRIES,nI);
                            break;
                        case (EMERALD_EXPLOSION_ELEMENT):
                            Playfield.pLevel[nCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                            Playfield.pStatusAnimation[nCoordinate] = uCheckExplosion >> 16;
                            break;
                        case (EMERALD_EXPLOSION_NEWCENTRAL):
                            Playfield.pLevel[nCoordinate] = EMERALD_CENTRAL_EXPLOSION;
                            if (nCoordinate > nI) {
                                Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                            } else {
                                Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                            }
                            break;
                        case (EMERALD_EXPLOSION_NEWCENTRAL_MEGA):
                            Playfield.pLevel[nCoordinate] = EMERALD_CENTRAL_EXPLOSION_MEGA;
                            if (nCoordinate > nI) {
                                Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                            } else {
                                Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                            }
                            break;
                        case (EMERALD_EXPLOSION_NEWCENTRAL_BEETLE):
                            Playfield.pLevel[nCoordinate] = EMERALD_CENTRAL_EXPLOSION_BEETLE;
                            if (nCoordinate > nI) {
                                Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                            } else {
                                Playfield.pStatusAnimation[nCoordinate] = EMERALD_ANIM_STAND;
                            }
                            break;
                    }
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlCentralBeetleExplosion
------------------------------------------------------------------------------
Beschreibung: Steuert eine zentrale Käfer-Explosion (3x3).
Parameter
      Eingang: I, uint32_t, Index im Level (zentraler Punkt)
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlCentralBeetleExplosion(uint32_t I) {
    uint32_t K;
    uint32_t uCoordinate;
    uint32_t uCheckExplosion;

    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    } else {
        Playfield.pLevel[I] = EMERALD_EXPLOSION_TO_ELEMENT_1;    // Mittelpunkt wird auf jedem Fall zur Explosion
        Playfield.pStatusAnimation[I] = EMERALD_SAPPHIRE;
        CleanInvalidFieldsForCentralExplosion(I,NO_MEGA_EXPLOSION);
        for (K = 0; K < 8; K++) {
            uCoordinate = I + Playfield.nCentralExplosionCoordinates[K];
            uCheckExplosion = CheckExplosionElement(Playfield.pLevel[uCoordinate],uCoordinate);
            switch (uCheckExplosion & 0xFFFF) {
                case (EMERALD_EXPLOSION_EMPTY):
                    Playfield.pLevel[uCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_EMERALD;
                    break;
                case (EMERALD_EXPLOSION_EMPTY_MAN):
                    Playfield.pLevel[uCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_EMERALD;
                    Playfield.bManDead = true;
                    PreparePlaySound(SOUND_MAN_CRIES,I);
                    break;
                case (EMERALD_EXPLOSION_ELEMENT):   // Käfer sprengt Elemente aus Mauer frei statt Emeralds zu erzeugen
                    Playfield.pLevel[uCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                    Playfield.pStatusAnimation[uCoordinate] = uCheckExplosion >> 16;
                    break;
                case (EMERALD_EXPLOSION_NEWCENTRAL):
                    Playfield.pLevel[uCoordinate] = EMERALD_CENTRAL_EXPLOSION;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    break;
                case (EMERALD_EXPLOSION_NEWCENTRAL_MEGA):
                    Playfield.pLevel[uCoordinate] = EMERALD_CENTRAL_EXPLOSION_MEGA;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    break;
                case (EMERALD_EXPLOSION_NEWCENTRAL_BEETLE):
                    Playfield.pLevel[uCoordinate] = EMERALD_CENTRAL_EXPLOSION_BEETLE;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    break;
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlCentralYamExplosion
------------------------------------------------------------------------------
Beschreibung: Steuert eine zentrale Yam-Explosion (3x3).
Parameter
      Eingang: I, uint32_t, Index im Level (zentraler Punkt)
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlCentralYamExplosion(uint32_t I) {
    uint32_t K;
    uint32_t uCoordinate;
    uint32_t uCheckExplosion;
    uint32_t Y;
    uint16_t YamElements[9];

    if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    } else {
        // Kopie der Yam-Elemente machen, da diese für Replikator und Säurebecken ggf. angepasst werden müssen
        memcpy(YamElements,Playfield.YamExplosions[Playfield.uYamExplosion].uElement,9 * sizeof(uint16_t));
        CleanInvalidFieldsForCentralExplosion(I,NO_MEGA_EXPLOSION);
        CheckYamContents(I,YamElements);
        Playfield.pLevel[I] = EMERALD_EXPLOSION_TO_ELEMENT_1;    // Mittelpunkt wird auf jedem Fall zur Explosion
        Playfield.pStatusAnimation[I] = YamElements[4];  // 4 ist das mittlere YAM-Element
        Y = 0;
        for (K = 0; K < 8; K++) {
            uCoordinate = I + Playfield.nCentralExplosionCoordinates[K];
            uCheckExplosion = CheckExplosionElement(Playfield.pLevel[uCoordinate],uCoordinate);
            switch (uCheckExplosion & 0xFFFF) {
                case (EMERALD_EXPLOSION_EMPTY_MAN):
                    Playfield.pLevel[uCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_EMERALD;
                    Playfield.bManDead = true;
                    PreparePlaySound(SOUND_MAN_CRIES,I);
                    break;
                case (EMERALD_EXPLOSION_EMPTY):
                    Playfield.pLevel[uCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                    Playfield.pStatusAnimation[uCoordinate] = YamElements[Y];
                    break;
                case (EMERALD_EXPLOSION_ELEMENT):   // Yam sprengt Elemente aus Mauer frei statt Emeralds zu erzeugen
                    Playfield.pLevel[uCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                    Playfield.pStatusAnimation[uCoordinate] = uCheckExplosion >> 16;
                    break;
                case (EMERALD_EXPLOSION_NEWCENTRAL):
                    Playfield.pLevel[uCoordinate] = EMERALD_CENTRAL_EXPLOSION;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    break;
                case (EMERALD_EXPLOSION_NEWCENTRAL_BEETLE):
                    Playfield.pLevel[uCoordinate] = EMERALD_CENTRAL_EXPLOSION_BEETLE;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    break;
            }
            Y++;
            if (Y == 4) {   // Mittelpunkt überspringen
                Y++;
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           CheckExplosionElement
------------------------------------------------------------------------------
Beschreibung: Prüft, wie sich ein bestehendes Objekt, das sich im Bereich einer Explosion
              befindet, verhalten soll.
Parameter
      Eingang: uElement, uint16_t, Element, das bewertet werden soll.
               uCoordinate, uint32_t, lineare Koordinate des Elements in Playfield.x
      Ausgang: -
Rückgabewert:  uint32_t LOW WORD: Explosionstype
                        HIGH WORD: Element das an dieser Szelle neu entsteht

                EMERALD_EXPLOSION_NONE                    bei diesem Element kann keine Explosion stattfinden, z.B. Stahl
                EMERALD_EXPLOSION_EMPTY                   an dieser Stelle wird eine Explosion durchgeführt und das bestehende Element entfernt
                EMERALD_EXPLOSION_ELEMENT                 an dieser Stelle entsteht nach der Explosion ein neues Element
                EMERALD_EXPLOSION_NEWCENTRAL              an dieser Stelle entsteht eine neue zentrale 3x3-Explosion
                EMERALD_EXPLOSION_NEWCENTRAL_MEGA         an dieser Stelle entsteht eine neue zentrale Mega-Explosion
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t CheckExplosionElement(uint16_t uElement, uint32_t uCoordinate) {
    uint32_t  uExplosion;

    switch (uElement) {
        case (EMERALD_EXPLOSION_TO_ELEMENT_1):  // Nachfolgende Sprengungen
        case (EMERALD_EXPLOSION_TO_ELEMENT_2):  // überschreiben ältere Sprengfelder
        case (EMERALD_NONE):
        case (EMERALD_STONE):
        case (EMERALD_SAPPHIRE):
        case (EMERALD_PERL):
        case (EMERALD_MOLE_UP):
        case (EMERALD_MOLE_RIGHT):
        case (EMERALD_MOLE_DOWN):
        case (EMERALD_MOLE_LEFT):
        case (EMERALD_MINE_UP):
        case (EMERALD_MINE_RIGHT):
        case (EMERALD_MINE_DOWN):
        case (EMERALD_MINE_LEFT):
        case (EMERALD_EMERALD):
        case (EMERALD_RUBY):
        case (EMERALD_TIME_COIN):
        case (EMERALD_NUT):
        case (EMERALD_ALIEN):
        case (EMERALD_YAM):
        case (EMERALD_SAND_MOLE):
        case (EMERALD_GREEN_CHEESE_GOES):
        case (EMERALD_DYNAMITE_OFF):
        case (EMERALD_DYNAMITE_ON):
        case (EMERALD_DOOR_RED_WOOD):
        case (EMERALD_DOOR_YELLOW_WOOD):
        case (EMERALD_DOOR_BLUE_WOOD):
        case (EMERALD_DOOR_GREEN_WOOD):
        case (EMERALD_DOOR_WHITE_WOOD):
        case (EMERALD_MESSAGE_1):
        case (EMERALD_MESSAGE_2):
        case (EMERALD_MESSAGE_3):
        case (EMERALD_MESSAGE_4):
        case (EMERALD_MESSAGE_5):
        case (EMERALD_MESSAGE_6):
        case (EMERALD_MESSAGE_7):
        case (EMERALD_MESSAGE_8):
        case (EMERALD_KEY_RED):
        case (EMERALD_KEY_YELLOW):
        case (EMERALD_KEY_BLUE):
        case (EMERALD_KEY_GREEN):
        case (EMERALD_KEY_WHITE):
        case (EMERALD_KEY_GENERAL):
        case (EMERALD_SPACE):
        case (EMERALD_WALL_ROUND):
        case (EMERALD_SAND):
        case (EMERALD_SAND_INVISIBLE):
        case (EMERALD_SANDMINE):
        case (EMERALD_GRASS):
        case (EMERALD_GRASS_COMES):
        case (EMERALD_WALL_CORNERED):
        case (EMERALD_WALL_GROW_LEFT):
        case (EMERALD_WALL_GROW_RIGHT):
        case (EMERALD_WALL_GROW_UP):
        case (EMERALD_WALL_GROW_DOWN):
        case (EMERALD_WALL_GROW_LEFT_RIGHT):
        case (EMERALD_WALL_GROW_UP_DOWN):
        case (EMERALD_WALL_GROW_ALL):
        case (EMERALD_WALL_GROWING_LEFT):
        case (EMERALD_WALL_GROWING_RIGHT):
        case (EMERALD_WALL_GROWING_UP):
        case (EMERALD_WALL_GROWING_DOWN):
        case (EMERALD_DOOR_ONLY_UP_WALL):
        case (EMERALD_DOOR_ONLY_DOWN_WALL):
        case (EMERALD_DOOR_ONLY_LEFT_WALL):
        case (EMERALD_DOOR_ONLY_RIGHT_WALL):
        case (EMERALD_HAMMER):
        case (EMERALD_MAGIC_WALL):
        case (EMERALD_WHEEL):
        case (EMERALD_SWAMP):
        case (EMERALD_SWAMP_STONE):
        case (EMERALD_STONE_SINK):
        case (EMERALD_STONE_SAG):
        case (EMERALD_DOOR_END_NOT_READY):
        case (EMERALD_GREEN_DROP_COMES):
        case (EMERALD_GREEN_DROP):
        case (EMERALD_GREEN_CHEESE):
        case (EMERALD_WALL_ROUND_PIKE):
        case (EMERALD_DOOR_END_READY):
        case (EMERALD_WALL_NOT_ROUND):
        case (EMERALD_WALL_INVISIBLE):
        case (EMERALD_FONT_EXCLAMATION):
        case (EMERALD_FONT_ARROW_RIGHT):
        case (EMERALD_FONT_ARROW_UP):
        case (EMERALD_FONT_ARROW_DOWN):
        case (EMERALD_FONT_APOSTROPHE):
        case (EMERALD_FONT_BRACE_OPEN):
        case (EMERALD_FONT_BRACE_CLOSE):
        case (EMERALD_FONT_COPYRIGHT):
        case (EMERALD_FONT_PLUS):
        case (EMERALD_FONT_COMMA):
        case (EMERALD_FONT_MINUS):
        case (EMERALD_FONT_POINT):
        case (EMERALD_FONT_SLASH):
        case (EMERALD_FONT_0):
        case (EMERALD_FONT_1):
        case (EMERALD_FONT_2):
        case (EMERALD_FONT_3):
        case (EMERALD_FONT_4):
        case (EMERALD_FONT_5):
        case (EMERALD_FONT_6):
        case (EMERALD_FONT_7):
        case (EMERALD_FONT_8):
        case (EMERALD_FONT_9):
        case (EMERALD_FONT_DOUBLE_POINT):
        case (EMERALD_FONT_PLATE):
        case (EMERALD_FONT_ARROW_LEFT):
        case (EMERALD_FONT_PAFF):
        case (EMERALD_FONT_QUESTION_MARK):
        case (EMERALD_FONT_A):
        case (EMERALD_FONT_B):
        case (EMERALD_FONT_C):
        case (EMERALD_FONT_D):
        case (EMERALD_FONT_E):
        case (EMERALD_FONT_F):
        case (EMERALD_FONT_G):
        case (EMERALD_FONT_H):
        case (EMERALD_FONT_I):
        case (EMERALD_FONT_J):
        case (EMERALD_FONT_K):
        case (EMERALD_FONT_L):
        case (EMERALD_FONT_M):
        case (EMERALD_FONT_N):
        case (EMERALD_FONT_O):
        case (EMERALD_FONT_P):
        case (EMERALD_FONT_Q):
        case (EMERALD_FONT_R):
        case (EMERALD_FONT_S):
        case (EMERALD_FONT_T):
        case (EMERALD_FONT_U):
        case (EMERALD_FONT_V):
        case (EMERALD_FONT_W):
        case (EMERALD_FONT_X):
        case (EMERALD_FONT_Y):
        case (EMERALD_FONT_Z):
        case (EMERALD_FONT_AE):
        case (EMERALD_FONT_OE):
        case (EMERALD_FONT_UE):
        case (EMERALD_FONT_GREEN_EXCLAMATION):
        case (EMERALD_FONT_GREEN_ARROW_RIGHT):
        case (EMERALD_FONT_GREEN_ARROW_UP):
        case (EMERALD_FONT_GREEN_ARROW_DOWN):
        case (EMERALD_FONT_GREEN_APOSTROPHE):
        case (EMERALD_FONT_GREEN_BRACE_OPEN):
        case (EMERALD_FONT_GREEN_BRACE_CLOSE):
        case (EMERALD_FONT_GREEN_COPYRIGHT):
        case (EMERALD_FONT_GREEN_PLUS):
        case (EMERALD_FONT_GREEN_COMMA):
        case (EMERALD_FONT_GREEN_MINUS):
        case (EMERALD_FONT_GREEN_POINT):
        case (EMERALD_FONT_GREEN_SLASH):
        case (EMERALD_FONT_GREEN_0):
        case (EMERALD_FONT_GREEN_1):
        case (EMERALD_FONT_GREEN_2):
        case (EMERALD_FONT_GREEN_3):
        case (EMERALD_FONT_GREEN_4):
        case (EMERALD_FONT_GREEN_5):
        case (EMERALD_FONT_GREEN_6):
        case (EMERALD_FONT_GREEN_7):
        case (EMERALD_FONT_GREEN_8):
        case (EMERALD_FONT_GREEN_9):
        case (EMERALD_FONT_GREEN_DOUBLE_POINT):
        case (EMERALD_FONT_GREEN_PLATE):
        case (EMERALD_FONT_GREEN_ARROW_LEFT):
        case (EMERALD_FONT_GREEN_QUESTION_MARK):
        case (EMERALD_FONT_GREEN_A):
        case (EMERALD_FONT_GREEN_B):
        case (EMERALD_FONT_GREEN_C):
        case (EMERALD_FONT_GREEN_D):
        case (EMERALD_FONT_GREEN_E):
        case (EMERALD_FONT_GREEN_F):
        case (EMERALD_FONT_GREEN_G):
        case (EMERALD_FONT_GREEN_H):
        case (EMERALD_FONT_GREEN_I):
        case (EMERALD_FONT_GREEN_J):
        case (EMERALD_FONT_GREEN_K):
        case (EMERALD_FONT_GREEN_L):
        case (EMERALD_FONT_GREEN_M):
        case (EMERALD_FONT_GREEN_N):
        case (EMERALD_FONT_GREEN_O):
        case (EMERALD_FONT_GREEN_P):
        case (EMERALD_FONT_GREEN_Q):
        case (EMERALD_FONT_GREEN_R):
        case (EMERALD_FONT_GREEN_S):
        case (EMERALD_FONT_GREEN_T):
        case (EMERALD_FONT_GREEN_U):
        case (EMERALD_FONT_GREEN_V):
        case (EMERALD_FONT_GREEN_W):
        case (EMERALD_FONT_GREEN_X):
        case (EMERALD_FONT_GREEN_Y):
        case (EMERALD_FONT_GREEN_Z):
        case (EMERALD_FONT_GREEN_AE):
        case (EMERALD_FONT_GREEN_OE):
        case (EMERALD_FONT_GREEN_UE):
            uExplosion = EMERALD_EXPLOSION_EMPTY;
            if ((uElement == EMERALD_WHEEL) && (Playfield.pStatusAnimation[uCoordinate] == EMERALD_ANIM_WHEEL_RUN)) {
                Playfield.bWheelRunning = false;
                Playfield.uTimeWheelRotationLeft = 0;
                Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_STAND;
                Playfield.uWheelRunningXpos = 0;
                Playfield.uWheelRunningYpos = 0;
            }
            break;
        case (EMERALD_MAN):
            uExplosion = EMERALD_EXPLOSION_EMPTY_MAN;
            break;
        case (EMERALD_WALL_WITH_TIME_COIN):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_TIME_COIN << 16);
            break;
        case (EMERALD_WALL_WITH_EMERALD):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_EMERALD << 16);
            break;
        case (EMERALD_WALL_WITH_RUBY):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_RUBY << 16);
            break;
        case (EMERALD_WALL_WITH_SAPPHIRE):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_SAPPHIRE << 16);
            break;
        case (EMERALD_WALL_WITH_PERL):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_PERL << 16);
            break;
        case (EMERALD_WALL_WITH_CRYSTAL):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_CRYSTAL << 16);
            break;
        case (EMERALD_WALL_WITH_KEY_RED):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_KEY_RED << 16);
            break;
        case (EMERALD_WALL_WITH_KEY_GREEN):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_KEY_GREEN << 16);
            break;
        case (EMERALD_WALL_WITH_KEY_BLUE):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_KEY_BLUE << 16);
            break;
        case (EMERALD_WALL_WITH_KEY_YELLOW):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_KEY_YELLOW << 16);
            break;
        case (EMERALD_WALL_WITH_KEY_WHITE):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_KEY_WHITE << 16);
            break;
        case (EMERALD_WALL_WITH_KEY_GENERAL):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_KEY_GENERAL << 16);
            break;
        case (EMERALD_WALL_WITH_BOMB):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_BOMB << 16);
            break;
        case (EMERALD_WALL_WITH_MEGABOMB):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_MEGABOMB << 16);
            break;
        case (EMERALD_WALL_WITH_STONE):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_STONE << 16);
            break;
        case (EMERALD_WALL_WITH_NUT):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_NUT << 16);
            break;
        case (EMERALD_WALL_WITH_WHEEL):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_WHEEL << 16);
            break;
        case (EMERALD_WALL_WITH_DYNAMITE):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_DYNAMITE_OFF << 16);
            break;
        case (EMERALD_WALL_WITH_ENDDOOR):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_DOOR_END_NOT_READY << 16);
            break;
        case (EMERALD_WALL_WITH_ENDDOOR_READY):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_DOOR_END_READY << 16);
            break;
        case (EMERALD_WALL_WITH_MINE_UP):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_MINE_UP << 16);
            break;
        case (EMERALD_WALL_WITH_MOLE_UP):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_MOLE_UP << 16);
            break;
        case (EMERALD_WALL_WITH_GREEN_CHEESE):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_GREEN_CHEESE << 16);
            break;
        case (EMERALD_WALL_WITH_BEETLE_UP):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_BEETLE_UP << 16);
            break;
        case (EMERALD_WALL_WITH_YAM):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_YAM << 16);
            break;
        case (EMERALD_WALL_WITH_ALIEN):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_ALIEN << 16);
            break;
        case (EMERALD_BOMB):
        case (EMERALD_REMOTEBOMB):
            uExplosion = EMERALD_EXPLOSION_NEWCENTRAL;
            break;
        case (EMERALD_MEGABOMB):
            uExplosion = EMERALD_EXPLOSION_NEWCENTRAL_MEGA;
            break;
        case (EMERALD_STANDMINE):
            uExplosion = EMERALD_EXPLOSION_NEWCENTRAL;
            break;
        case (EMERALD_BEETLE_UP):
        case (EMERALD_BEETLE_RIGHT):
        case (EMERALD_BEETLE_DOWN):
        case (EMERALD_BEETLE_LEFT):
            uExplosion = EMERALD_EXPLOSION_NEWCENTRAL_BEETLE;
            break;
        default:
            uExplosion = EMERALD_EXPLOSION_NONE;
    }
    return uExplosion;
}


/*----------------------------------------------------------------------------
Name:           YamHasReplicatorAcidPool
------------------------------------------------------------------------------
Beschreibung:   Prüft, ob eine YAM-Explosion einen Replikator oder ein Säurebecken enthält.
                Der Leveleditor stellt bereits sicher, dass Replikator und Säurebecken
                korrekt in der Yam-Explosion kodiert sind.
Parameter
      Eingang: YamExplosion, uint16_t *, Zeiger auf 3x3 = 9 Explosions-Elemente eines Yams
      Ausgang: -
Rückgabewert:  0 = kein Replikator oder Säurebecken enthalten
               1 = Replikator in oberer Hälfte (0,1,2,3,5)
               2 = Replikator in unterer Hälfte (3,4,5,6,8)
               3 = Säurebecken in oberer Hälfte (0,1,2,3,4,5)
               4 = Säurebecken in unterer Hälfte (3,4,5,6,7,8)
Seiteneffekte: -
------------------------------------------------------------------------------*/
int YamHasReplicatorAcidPool(uint16_t *YamExplosion) {
    int nRet;

    nRet = 0;
    if  ( (YamExplosion[0] == EMERALD_REPLICATOR_RED_TOP_LEFT) || (YamExplosion[0] == EMERALD_REPLICATOR_GREEN_TOP_LEFT) || (YamExplosion[0] == EMERALD_REPLICATOR_BLUE_TOP_LEFT) || (YamExplosion[0] == EMERALD_REPLICATOR_YELLOW_TOP_LEFT) ) {
        nRet = 1;
    } else  if ( (YamExplosion[3] == EMERALD_REPLICATOR_RED_TOP_LEFT) || (YamExplosion[3] == EMERALD_REPLICATOR_GREEN_TOP_LEFT) || (YamExplosion[3] == EMERALD_REPLICATOR_BLUE_TOP_LEFT) || (YamExplosion[3] == EMERALD_REPLICATOR_YELLOW_TOP_LEFT) ) {
        nRet = 2;
    } else if  (YamExplosion[0] == EMERALD_ACIDPOOL_TOP_LEFT) {
        nRet = 3;
    } else if  (YamExplosion[3] == EMERALD_ACIDPOOL_TOP_LEFT) {
        nRet = 4;
    }
    return nRet;
}


/*----------------------------------------------------------------------------
Name:           CheckYamContents
------------------------------------------------------------------------------
Beschreibung:   Falls eine Yam-Explosion einen Replikator oder ein Säurebecken enthält,
                muss ggf. vorher die Yam-Explosion angepasst werden, wenn der R. oder das S.
                nicht vollständig platziert werden kann.
                Kann die Platzierung nicht vollständig erfolgen, so werden
                die R./S.-Elemente durch EMERALD_STEEL_WARNING ersetzt.
Parameter
      Eingang: I, uint32_t, Zentrum der Yam-Explosion
               YamElements, uint16_t *, Zeiger auf bisherige Yam-Elemente
      Ausgang: YamElements, uint16_t *, Zeiger auf angepasste Yam-Elemente
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CheckYamContents(uint32_t I,uint16_t *YamElements) {
    bool bReplace;
    uint32_t K;
    uint32_t uCoordinate;
    uint32_t uCheckExplosion;
    int nYamContent;
    // YamContent
    // 0 = kein Replikator oder Säurebecken enthalten
    // 1 = Replikator in oberer Hälfte (0,1,2,3,5)
    // 2 = Replikator in unterer Hälfte (3,4,5,6,8)
    // 3 = Säurebecken in oberer Hälfte (0,1,2,3,4,5)
    // 4 = Säurebecken in unterer Hälfte (3,4,5,6,7,8)
    bReplace = false;
    nYamContent = YamHasReplicatorAcidPool(YamElements);
    if (nYamContent == 0) {
        return;                         // Yam-Explosion enthält keine Maschine (Replikator oder Säurebecken)
    } else if (nYamContent == 1) {      // Replikator in oberer Hälfte
        for (K = 0; (K < 5) && (!bReplace); K++) {
            uCoordinate = I + Playfield.nCheckReplicatorForYamExplosionTop[K];
            uCheckExplosion = CheckExplosionElement(Playfield.pLevel[uCoordinate],uCoordinate);
            bReplace = (uCheckExplosion == EMERALD_EXPLOSION_NONE); // true = Replikator-Teil-Element kann nicht erzeugt werden
        }
        if (bReplace) {
            for (K = 0; K <= 5; K++) {
                YamElements[K] = EMERALD_STEEL_WARNING;     // Statt Replikator Stahl-Warnschilder erzeugen
            }
        } else {
            // Replikator kann vollständig erzeugt werden -> Sicherheitshalber im Level an dieser Stelle Spaces erzeugen
            for (K = 0; (K < 5); K++) {
                uCoordinate = I + Playfield.nCheckReplicatorForYamExplosionTop[K];
                Playfield.pLevel[uCoordinate] = EMERALD_SPACE;
            }
        }
    } else if (nYamContent == 2) {      // Replikator in unterer Hälfte
        for (K = 0; (K < 5) && (!bReplace); K++) {
            uCoordinate = I + Playfield.nCheckReplicatorForYamExplosionButtom[K];
            uCheckExplosion = CheckExplosionElement(Playfield.pLevel[uCoordinate],uCoordinate);
            bReplace = (uCheckExplosion == EMERALD_EXPLOSION_NONE); // true = Replikator-Teil-Element kann nicht erzeugt werden
        }
        if (bReplace) {
            for (K = 0; K <= 5; K++) {
                YamElements[3 + K] = EMERALD_STEEL_WARNING;     // Statt Replikator Stahl-Warnschilder erzeugen
            }
        } else {
            // Replikator kann vollständig erzeugt werden -> Sicherheitshalber im Level an dieser Stelle Spaces erzeugen
            for (K = 0; (K < 5); K++) {
                uCoordinate = I + Playfield.nCheckReplicatorForYamExplosionButtom[K];
                Playfield.pLevel[uCoordinate] = EMERALD_SPACE;
            }
        }
    } else if (nYamContent == 3) {       // Säurebecken in oberer Hälfte
        for (K = 0; (K < 6) && (!bReplace); K++) {
            uCoordinate = I + Playfield.nCheckAcidPoolForYamExplosionTop[K];
            uCheckExplosion = CheckExplosionElement(Playfield.pLevel[uCoordinate],uCoordinate);
            bReplace = (uCheckExplosion == EMERALD_EXPLOSION_NONE); // true = Säurebecken-Teil-Element kann nicht erzeugt werden
        }
        if (bReplace) {
            for (K = 0; K <= 5; K++) {
                YamElements[K] = EMERALD_STEEL_WARNING;     // Statt Säurebecken Stahl-Warnschilder erzeugen
            }
        } else {
            // Säurebecken kann vollständig erzeugt werden -> Sicherheitshalber im Level an dieser Stelle Spaces erzeugen
            for (K = 0; (K < 6); K++) {
                uCoordinate = I + Playfield.nCheckAcidPoolForYamExplosionTop[K];
                Playfield.pLevel[uCoordinate] = EMERALD_SPACE;
            }
        }
    } else {                            // Säurebecken in unterer Hälfte
        for (K = 0; (K < 6) && (!bReplace); K++) {
            uCoordinate = I + Playfield.nCheckAcidPoolForYamExplosionButtom[K];
            uCheckExplosion = CheckExplosionElement(Playfield.pLevel[uCoordinate],uCoordinate);
            bReplace = (uCheckExplosion == EMERALD_EXPLOSION_NONE); // true = Säurebecken-Teil-Element kann nicht erzeugt werden
        }
        if (bReplace) {
            for (K = 0; K <= 5; K++) {
                YamElements[3 + K] = EMERALD_STEEL_WARNING;     // Statt Säurebecken Stahl-Warnschilder erzeugen
            }
        } else {
            // Säurebecken kann vollständig erzeugt werden -> Sicherheitshalber im Level an dieser Stelle Spaces erzeugen
            for (K = 0; (K < 6); K++) {
                uCoordinate = I + Playfield.nCheckAcidPoolForYamExplosionButtom[K];
                Playfield.pLevel[uCoordinate] = EMERALD_SPACE;
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           IsDangerousEnemyAround
------------------------------------------------------------------------------
Beschreibung: Prüft, ob sich eine "gefährlicher" Feind (Standmine, Käfer oder Mine)
              um Position I befindet. Die Funktion wird nur in ControlMan() aufgerufen.
Parameter
      Eingang: I, uint32_t, Index im Level, Position, an der geprüft werden soll
      Ausgang: -
Rückgabewert:  bool, true = "gefährlicher" Feind  hat Kontakt mit Position I
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
bool IsDangerousEnemyAround(uint32_t I) {
    return        ( (Playfield.pLevel[I - 1] == EMERALD_STANDMINE) ||    // links
                    (Playfield.pLevel[I + 1] == EMERALD_STANDMINE) ||    // rechts
                    (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_STANDMINE) || // oben
                    (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_STANDMINE) || // unten
                    // Käfer
                    (Playfield.pLevel[I - 1] == EMERALD_BEETLE_DOWN) ||    // rechts
                    (Playfield.pLevel[I + 1] == EMERALD_BEETLE_DOWN) ||    // rechts
                    (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_BEETLE_DOWN) || // oben
                    (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_BEETLE_DOWN) || // unten
                    (Playfield.pLevel[I - 1] == EMERALD_BEETLE_LEFT) ||    // links
                    (Playfield.pLevel[I + 1] == EMERALD_BEETLE_LEFT) ||    // rechts
                    (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_BEETLE_LEFT) || // oben
                    (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_BEETLE_LEFT) || // unten
                    (Playfield.pLevel[I - 1] == EMERALD_BEETLE_RIGHT) ||    // links
                    (Playfield.pLevel[I + 1] == EMERALD_BEETLE_RIGHT) ||    // rechts
                    (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_BEETLE_RIGHT) || // oben
                    (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_BEETLE_RIGHT) || // unten
                    (Playfield.pLevel[I - 1] == EMERALD_BEETLE_UP) ||    // links
                    (Playfield.pLevel[I + 1] == EMERALD_BEETLE_UP) ||    // rechts
                    (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_BEETLE_UP) ||    // oben
                    (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_BEETLE_UP) ||    // unten
                    // Mine
                    (Playfield.pLevel[I - 1] == EMERALD_MINE_DOWN) ||    // rechts
                    (Playfield.pLevel[I + 1] == EMERALD_MINE_DOWN) ||    // rechts
                    (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_MINE_DOWN) || // oben
                    (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MINE_DOWN) || // unten
                    (Playfield.pLevel[I - 1] == EMERALD_MINE_LEFT) ||    // links
                    (Playfield.pLevel[I + 1] == EMERALD_MINE_LEFT) ||    // rechts
                    (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_MINE_LEFT) || // oben
                    (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MINE_LEFT) || // unten
                    (Playfield.pLevel[I - 1] == EMERALD_MINE_RIGHT) ||    // links
                    (Playfield.pLevel[I + 1] == EMERALD_MINE_RIGHT) ||    // rechts
                    (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_MINE_RIGHT) || // oben
                    (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MINE_RIGHT) || // unten
                    (Playfield.pLevel[I - 1] == EMERALD_MINE_UP) ||    // links
                    (Playfield.pLevel[I + 1] == EMERALD_MINE_UP) ||    // rechts
                    (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_MINE_UP) ||    // oben
                    (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_MINE_UP)       // unten
                    );
}
