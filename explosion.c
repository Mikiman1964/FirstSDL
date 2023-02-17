#include "EmeraldMine.h"
#include "explosion.h"
#include "sound.h"

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
            Playfield.pLevel[I] = Playfield.pStatusAnimation[I] & 0xFFFF;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        } else {
            SDL_Log("%s: Error: try to set bad element (0x%04x) at position %u",__FUNCTION__,uNewElement,I);
        }
    }
}


/*----------------------------------------------------------------------------
Name:           CleanInvalidFieldsForCentralExplosion
------------------------------------------------------------------------------
Beschreibung: Entfernt ggf. invalide Felder innerhalb des Explosionsbereichs. Die
              Routine sucht innerhalb des Bereichs nach beweglichen Elementen und
              deren invaliden Feldern.
Parameter
      Eingang: I, uint32_t, Index im Level (zentraler Punkt)
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CleanInvalidFieldsForCentralExplosion(uint32_t I) {
    uint32_t K;
    uint32_t uCoordinate;

    for (K = 0; K < 8; K++) {
        uCoordinate = I + Playfield.nCentralExplosionCoordinates[K];
        switch (Playfield.pLevel[uCoordinate]) {    // Element
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
            case (EMERALD_BOMB):
            case (EMERALD_CRYSTAL):
            case (EMERALD_EMERALD):
            case (EMERALD_RUBY):
            case (EMERALD_BEETLE_UP):
            case (EMERALD_BEETLE_RIGHT):
            case (EMERALD_BEETLE_DOWN):
            case (EMERALD_BEETLE_LEFT):
            case (EMERALD_NUT):
            case (EMERALD_ALIEN):
            case (EMERALD_YAM):
            case (EMERALD_MEGABOMB):
            case (EMERALD_GREEN_DROP):
                switch (Playfield.pStatusAnimation[uCoordinate] & 0x0000FF00) { // Animationsstatus
                    case (EMERALD_ANIM_UP):
                        if (Playfield.pLevel[uCoordinate - Playfield.uLevel_X_Dimension] == EMERALD_INVALID) {
                            Playfield.pLevel[uCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_SPACE;
                            Playfield.pStatusAnimation[uCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                            Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_STAND;   // Nicht sprengbares aber bewegliches Objekt zum Stillstand bringen
                        } else {
                            SDL_Log("%s: Warning: Element 0x%04x has no invalid field, AnimStatus: up",__FUNCTION__,Playfield.pLevel[uCoordinate]);
                        }
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        if (Playfield.pLevel[uCoordinate + 1] == EMERALD_INVALID) {
                            Playfield.pLevel[uCoordinate + 1] = EMERALD_SPACE;
                            Playfield.pStatusAnimation[uCoordinate + 1] = EMERALD_ANIM_STAND;
                            Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_STAND;   // Nicht sprengbares aber bewegliches Objekt zum Stillstand bringen
                        } else {
                            SDL_Log("%s: Warning: Element 0x%04x has no invalid field, AnimStatus: right",__FUNCTION__,Playfield.pLevel[uCoordinate]);
                        }
                        break;
                    case (EMERALD_ANIM_DOWN):
                        if (Playfield.pLevel[uCoordinate + Playfield.uLevel_X_Dimension] == EMERALD_INVALID) {
                            Playfield.pLevel[uCoordinate + Playfield.uLevel_X_Dimension] = EMERALD_SPACE;
                            Playfield.pStatusAnimation[uCoordinate + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                            Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_STAND;   // Nicht sprengbares aber bewegliches Objekt zum Stillstand bringen
                        } else {
                            SDL_Log("%s: Warning: Element 0x%04x has no invalid field, AnimStatus: down",__FUNCTION__,Playfield.pLevel[uCoordinate]);
                        }
                        break;
                    case (EMERALD_ANIM_LEFT):
                        if (Playfield.pLevel[uCoordinate - 1] == EMERALD_INVALID) {
                            Playfield.pLevel[uCoordinate - 1] = EMERALD_SPACE;
                            Playfield.pStatusAnimation[uCoordinate - 1] = EMERALD_ANIM_STAND;
                            Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_STAND;   // Nicht sprengbares aber bewegliches Objekt zum Stillstand bringen
                        } else {
                            SDL_Log("%s: Warning: Element 0x%04x has no invalid field, AnimStatus: left",__FUNCTION__,Playfield.pLevel[uCoordinate]);
                        }
                        break;
                }
                break;
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
        CleanInvalidFieldsForCentralExplosion(I);
        for (K = 0; K < 8; K++) {
            uCoordinate = I + Playfield.nCentralExplosionCoordinates[K];
            uCheckExplosion = CheckExplosionElement(Playfield.pLevel[uCoordinate]);
            switch (uCheckExplosion & 0xFFFF) {
                case (EMERALD_EXPLOSION_EMPTY):
                    Playfield.pLevel[uCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_SPACE;
                    break;
                case (EMERALD_EXPLOSION_ELEMENT):
                    Playfield.pLevel[uCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                    Playfield.pStatusAnimation[uCoordinate] = uCheckExplosion >> 16;
                    break;
                case (EMERALD_EXPLOSION_NEWCENTRAL):
                    Playfield.pLevel[uCoordinate] = EMERALD_CENTRAL_EXPLOSION;
                    if (uCoordinate > I) {
                        Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    } else {
                        Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_STAND;
                    }
                    break;
                case (EMERALD_EXPLOSION_NEWCENTRAL_BEETLE):
                    Playfield.pLevel[uCoordinate] = EMERALD_CENTRAL_EXPLOSION_BEETLE;
                    if (uCoordinate > I) {
                        Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    } else {
                        Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_STAND;
                    }
                    break;
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
        CleanInvalidFieldsForCentralExplosion(I);
        for (K = 0; K < 8; K++) {
            uCoordinate = I + Playfield.nCentralExplosionCoordinates[K];
            uCheckExplosion = CheckExplosionElement(Playfield.pLevel[uCoordinate]);
            switch (uCheckExplosion & 0xFFFF) {
                case (EMERALD_EXPLOSION_EMPTY):
                    Playfield.pLevel[uCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_EMERALD;
                    break;
                case (EMERALD_EXPLOSION_ELEMENT):   // Käfer sprengt Elemente aus Mauer frei statt Emeralds zu erzeugen
                    Playfield.pLevel[uCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                    Playfield.pStatusAnimation[uCoordinate] = uCheckExplosion >> 16;
                    break;
                case (EMERALD_EXPLOSION_NEWCENTRAL):
                    Playfield.pLevel[uCoordinate] = EMERALD_CENTRAL_EXPLOSION;
                    if (uCoordinate > I) {
                        Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    } else {
                        Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_STAND;
                    }
                    break;
                case (EMERALD_EXPLOSION_NEWCENTRAL_BEETLE):
                    Playfield.pLevel[uCoordinate] = EMERALD_CENTRAL_EXPLOSION_BEETLE;
                    if (uCoordinate > I) {
                        Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    } else {
                        Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_STAND;
                    }
                    break;
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
      Ausgang: -
Rückgabewert:  uint32_t LOW WORD: Explosionstype
                        HIGH WORD: Element das an dieser Szelle neu entsteht

                EMERALD_EXPLOSION_NONE                              bei diesem Element kann keine Explosion stattfinden, z.B. Stahl
                EMERALD_EXPLOSION_EMPTY                             an dieser Stelle wird eine Explosion durchgeführt und das bestehende Element entfernt
                EMERALD_EXPLOSION_ELEMENT                           an dieser Stelle entsteht nach der Explosion ein neues Element
                EMERALD_EXPLOSION_NEWCENTRAL_EXPLOSION              an dieser Stelle entsteht eine neue zentrale 3x3-Explosion
                EMERALD_EXPLOSION_NEWCENTRAL_MEGA_EXPLOSION         an dieser Stelle entsteht eine neue zentrale Mega-Explosion
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t CheckExplosionElement(uint16_t uElement) {
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
        case (EMERALD_WALL_CORNERED):
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
            break;
        case (EMERALD_MAN):
            SDL_Log("kill man");
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
            uExplosion = EMERALD_EXPLOSION_NEWCENTRAL;
            break;
        case (EMERALD_STANDMINE):
            uExplosion = EMERALD_EXPLOSION_NONE;
            break;
        case (EMERALD_BEETLE_UP):
        case (EMERALD_BEETLE_RIGHT):
        case (EMERALD_BEETLE_DOWN):
        case (EMERALD_BEETLE_LEFT):
            uExplosion = EMERALD_EXPLOSION_NEWCENTRAL_BEETLE;
            break;
        //case (EMERALD_MAN):
            //uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_ALIEN << 16);
            //break;
        case (EMERALD_MEGABOMB):
            uExplosion = EMERALD_EXPLOSION_ELEMENT | (EMERALD_ALIEN << 16);
            break;


        default:
            uExplosion = EMERALD_EXPLOSION_NONE;

    }

    return uExplosion;
}
