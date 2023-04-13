#include "EmeraldMine.h"
#include "explosion.h"
#include "sound.h"

extern PLAYFIELD Playfield;

// Alle Explosionen laufen �ber 2 Phasen.

/*----------------------------------------------------------------------------
Name:           ControlExplosionToElement
------------------------------------------------------------------------------
Beschreibung: Steuert eine Einzel-Explosion, die sich in ein beliebiges Element wandelt.
              Das zu erzeugende Element ist im Array Playfield.pStatusAnimation[] untergebracht.
              Wird in ControlPreElements() aufgerufen.

Parameter
      Eingang: I, uint32_t, Index im Level
      Ausgang: -
R�ckgabewert:  -
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
R�ckgabewert:  -
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
                            Playfield.pInvalidElement[uCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_NONE;
                            Playfield.pStatusAnimation[uCoordinate - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                            Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_STAND;   // Nicht sprengbares aber bewegliches Objekt zum Stillstand bringen
                        }
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        if (Playfield.pLevel[uCoordinate + 1] == EMERALD_INVALID) {
                            Playfield.pLevel[uCoordinate + 1] = EMERALD_SPACE;
                            Playfield.pInvalidElement[uCoordinate + 1] = EMERALD_NONE;
                            Playfield.pStatusAnimation[uCoordinate + 1] = EMERALD_ANIM_STAND;
                            Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_STAND;   // Nicht sprengbares aber bewegliches Objekt zum Stillstand bringen
                        }
                        break;
                    case (EMERALD_ANIM_DOWN):
                        if (Playfield.pLevel[uCoordinate + Playfield.uLevel_X_Dimension] == EMERALD_INVALID) {
                            Playfield.pLevel[uCoordinate + Playfield.uLevel_X_Dimension] = EMERALD_SPACE;
                            Playfield.pInvalidElement[uCoordinate + Playfield.uLevel_X_Dimension] = EMERALD_NONE;
                            Playfield.pStatusAnimation[uCoordinate + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_STAND;
                            Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_STAND;   // Nicht sprengbares aber bewegliches Objekt zum Stillstand bringen
                        }
                        break;
                    case (EMERALD_ANIM_LEFT):
                        if (Playfield.pLevel[uCoordinate - 1] == EMERALD_INVALID) {
                            Playfield.pLevel[uCoordinate - 1] = EMERALD_SPACE;
                            Playfield.pInvalidElement[uCoordinate - 1] = EMERALD_NONE;
                            Playfield.pStatusAnimation[uCoordinate - 1] = EMERALD_ANIM_STAND;
                            Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_STAND;   // Nicht sprengbares aber bewegliches Objekt zum Stillstand bringen
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
R�ckgabewert:  -
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
                    if (uCoordinate > I) {
                        Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                    } else {
                        Playfield.pStatusAnimation[uCoordinate] = EMERALD_ANIM_STAND;
                    }
                    break;
                case (EMERALD_EXPLOSION_NEWCENTRAL_MEGA):
                    Playfield.pLevel[uCoordinate] = EMERALD_CENTRAL_EXPLOSION_MEGA;
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
Name:           ControlCentralMegaExplosion
------------------------------------------------------------------------------
Beschreibung: Steuert eine Mega-Explosion (3x3) + (4x3)
Parameter
      Eingang: nI, int, Index im Level (zentraler Punkt)
      Ausgang: -
R�ckgabewert:  -
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
        CleanInvalidFieldsForCentralExplosion(nI);
        for (K = 0; K < 20; K++) {
            nCoordinate = nI + Playfield.nCentralMegaExplosionCoordinates[K];
            // Mega-Explosionen k�nnten theoretisch das Spielfeld �berwinden. Daher wird hier mit X/Y-Koordinaten abegr�ft, ob innerhalb des Spielfeldes gesprengt wird
            if (nCoordinate > 0) {
                nX = nCoordinate % Playfield.uLevel_X_Dimension;
                nY = nCoordinate / Playfield.uLevel_X_Dimension;
                if ((nX > 0) && (nX < (Playfield.uLevel_X_Dimension - 1)) && (nY > 0) && (nY < (Playfield.uLevel_Y_Dimension - 1))) {
                    uCheckExplosion = CheckExplosionElement(Playfield.pLevel[nCoordinate]);
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
Beschreibung: Steuert eine zentrale K�fer-Explosion (3x3).
Parameter
      Eingang: I, uint32_t, Index im Level (zentraler Punkt)
      Ausgang: -
R�ckgabewert:  -
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
                case (EMERALD_EXPLOSION_EMPTY_MAN):
                    Playfield.pLevel[uCoordinate] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                    Playfield.pStatusAnimation[uCoordinate] = EMERALD_EMERALD;
                    Playfield.bManDead = true;
                    PreparePlaySound(SOUND_MAN_CRIES,I);
                    break;
                case (EMERALD_EXPLOSION_ELEMENT):   // K�fer sprengt Elemente aus Mauer frei statt Emeralds zu erzeugen
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
                case (EMERALD_EXPLOSION_NEWCENTRAL_MEGA):
                    Playfield.pLevel[uCoordinate] = EMERALD_CENTRAL_EXPLOSION_MEGA;
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
Name:           ControlCentralYamExplosion
------------------------------------------------------------------------------
Beschreibung: Steuert eine zentrale Yam-Explosion (3x3).
Parameter
      Eingang: I, uint32_t, Index im Level (zentraler Punkt)
      Ausgang: -
R�ckgabewert:  -
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
        // Kopie der Yam-Elemente machen, da diese f�r Replikator und S�urebecken ggf. angepasst werden m�ssen
        memcpy(YamElements,Playfield.YamExplosions[Playfield.uYamExplosion].uElement,9 * sizeof(uint16_t));
        CleanInvalidFieldsForCentralExplosion(I);
        CheckYamContents(I,YamElements);
        Playfield.pLevel[I] = EMERALD_EXPLOSION_TO_ELEMENT_1;    // Mittelpunkt wird auf jedem Fall zur Explosion
        Playfield.pStatusAnimation[I] = YamElements[4];  // 4 ist das mittlere YAM-Element
        Y = 0;
        for (K = 0; K < 8; K++) {
            uCoordinate = I + Playfield.nCentralExplosionCoordinates[K];
            uCheckExplosion = CheckExplosionElement(Playfield.pLevel[uCoordinate]);
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
            Y++;
            if (Y == 4) {   // Mittelpunkt �berspringen
                Y++;
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           CheckExplosionElement
------------------------------------------------------------------------------
Beschreibung: Pr�ft, wie sich ein bestehendes Objekt, das sich im Bereich einer Explosion
              befindet, verhalten soll.
Parameter
      Eingang: uElement, uint16_t, Element, das bewertet werden soll.
      Ausgang: -
R�ckgabewert:  uint32_t LOW WORD: Explosionstype
                        HIGH WORD: Element das an dieser Szelle neu entsteht

                EMERALD_EXPLOSION_NONE                    bei diesem Element kann keine Explosion stattfinden, z.B. Stahl
                EMERALD_EXPLOSION_EMPTY                   an dieser Stelle wird eine Explosion durchgef�hrt und das bestehende Element entfernt
                EMERALD_EXPLOSION_ELEMENT                 an dieser Stelle entsteht nach der Explosion ein neues Element
                EMERALD_EXPLOSION_NEWCENTRAL              an dieser Stelle entsteht eine neue zentrale 3x3-Explosion
                EMERALD_EXPLOSION_NEWCENTRAL_MEGA         an dieser Stelle entsteht eine neue zentrale Mega-Explosion
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t CheckExplosionElement(uint16_t uElement) {
    uint32_t  uExplosion;

    switch (uElement) {
        case (EMERALD_EXPLOSION_TO_ELEMENT_1):  // Nachfolgende Sprengungen
        case (EMERALD_EXPLOSION_TO_ELEMENT_2):  // �berschreiben �ltere Sprengfelder
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
Beschreibung:   Pr�ft, ob eine YAM-Explosion einen Replikator oder ein S�urebecken enth�lt.
                Der Leveleditor stellt bereits sicher, dass Replikator und S�urebecken
                korrekt in der Yam-Explosion kodiert sind.
Parameter
      Eingang: YamExplosion, uint16_t *, Zeiger auf 3x3 = 9 Explosions-Elemente eines Yams
      Ausgang: -
R�ckgabewert:  0 = kein Replikator oder S�urebecken enthalten
               1 = Replikator in oberer H�lfte (0,1,2,3,5)
               2 = Replikator in unterer H�lfte (3,4,5,6,8)
               3 = S�urebecken in oberer H�lfte (0,1,2,3,4,5)
               4 = S�urebecken in unterer H�lfte (3,4,5,6,7,8)
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
Beschreibung:   Falls eine Yam-Explosion einen Replikator oder ein S�urebecken enth�lt,
                muss ggf. vorher die Yam-Explosion angepasst werden, wenn der R. oder das S.
                nicht vollst�ndig platziert werden kann.
                Kann die Platzierung nicht vollst�ndig erfolgen, so werden
                die R./S.-Elemente durch EMERALD_STEEL_WARNING ersetzt.
Parameter
      Eingang: I, uint32_t, Zentrum der Yam-Explosion
               YamElements, uint16_t *, Zeiger auf bisherige Yam-Elemente
      Ausgang: YamElements, uint16_t *, Zeiger auf angepasste Yam-Elemente
R�ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CheckYamContents(uint32_t I,uint16_t *YamElements) {
    bool bReplace;
    uint32_t K;
    uint32_t uCoordinate;
    uint32_t uCheckExplosion;
    int nYamContent;
    // YamContent
    // 0 = kein Replikator oder S�urebecken enthalten
    // 1 = Replikator in oberer H�lfte (0,1,2,3,5)
    // 2 = Replikator in unterer H�lfte (3,4,5,6,8)
    // 3 = S�urebecken in oberer H�lfte (0,1,2,3,4,5)
    // 4 = S�urebecken in unterer H�lfte (3,4,5,6,7,8)
    bReplace = false;
    nYamContent = YamHasReplicatorAcidPool(YamElements);
    if (nYamContent == 0) {
        return;                         // Yam-Explosion enth�lt keine Maschine (Replikator oder S�urebecken)
    } else if (nYamContent == 1) {      // Replikator in oberer H�lfte
        for (K = 0; (K < 5) && (!bReplace); K++) {
            uCoordinate = I + Playfield.nCheckReplicatorForYamExplosionTop[K];
            uCheckExplosion = CheckExplosionElement(Playfield.pLevel[uCoordinate]);
            bReplace = (uCheckExplosion == EMERALD_EXPLOSION_NONE); // true = Replikator-Teil-Element kann nicht erzeugt werden
        }
        if (bReplace) {
            for (K = 0; K <= 5; K++) {
                YamElements[K] = EMERALD_STEEL_WARNING;     // Statt Replikator Stahl-Warnschilder erzeugen
            }
        } else {
            // Replikator kann vollst�ndig erzeugt werden -> Sicherheitshalber im Level an dieser Stelle Spaces erzeugen
            for (K = 0; (K < 5); K++) {
                uCoordinate = I + Playfield.nCheckReplicatorForYamExplosionTop[K];
                Playfield.pLevel[uCoordinate] = EMERALD_SPACE;
            }
        }
    } else if (nYamContent == 2) {      // Replikator in unterer H�lfte
        for (K = 0; (K < 5) && (!bReplace); K++) {
            uCoordinate = I + Playfield.nCheckReplicatorForYamExplosionButtom[K];
            uCheckExplosion = CheckExplosionElement(Playfield.pLevel[uCoordinate]);
            bReplace = (uCheckExplosion == EMERALD_EXPLOSION_NONE); // true = Replikator-Teil-Element kann nicht erzeugt werden
        }
        if (bReplace) {
            for (K = 0; K <= 5; K++) {
                YamElements[3 + K] = EMERALD_STEEL_WARNING;     // Statt Replikator Stahl-Warnschilder erzeugen
            }
        } else {
            // Replikator kann vollst�ndig erzeugt werden -> Sicherheitshalber im Level an dieser Stelle Spaces erzeugen
            for (K = 0; (K < 5); K++) {
                uCoordinate = I + Playfield.nCheckReplicatorForYamExplosionButtom[K];
                Playfield.pLevel[uCoordinate] = EMERALD_SPACE;
            }
        }
    } else if (nYamContent == 3) {       // S�urebecken in oberer H�lfte
        for (K = 0; (K < 6) && (!bReplace); K++) {
            uCoordinate = I + Playfield.nCheckAcidPoolForYamExplosionTop[K];
            uCheckExplosion = CheckExplosionElement(Playfield.pLevel[uCoordinate]);
            bReplace = (uCheckExplosion == EMERALD_EXPLOSION_NONE); // true = S�urebecken-Teil-Element kann nicht erzeugt werden
        }
        if (bReplace) {
            for (K = 0; K <= 5; K++) {
                YamElements[K] = EMERALD_STEEL_WARNING;     // Statt S�urebecken Stahl-Warnschilder erzeugen
            }
        } else {
            // S�urebecken kann vollst�ndig erzeugt werden -> Sicherheitshalber im Level an dieser Stelle Spaces erzeugen
            for (K = 0; (K < 6); K++) {
                uCoordinate = I + Playfield.nCheckAcidPoolForYamExplosionTop[K];
                Playfield.pLevel[uCoordinate] = EMERALD_SPACE;
            }
        }
    } else {                            // S�urebecken in unterer H�lfte
        for (K = 0; (K < 6) && (!bReplace); K++) {
            uCoordinate = I + Playfield.nCheckAcidPoolForYamExplosionButtom[K];
            uCheckExplosion = CheckExplosionElement(Playfield.pLevel[uCoordinate]);
            bReplace = (uCheckExplosion == EMERALD_EXPLOSION_NONE); // true = S�urebecken-Teil-Element kann nicht erzeugt werden
        }
        if (bReplace) {
            for (K = 0; K <= 5; K++) {
                YamElements[3 + K] = EMERALD_STEEL_WARNING;     // Statt S�urebecken Stahl-Warnschilder erzeugen
            }
        } else {
            // S�urebecken kann vollst�ndig erzeugt werden -> Sicherheitshalber im Level an dieser Stelle Spaces erzeugen
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
Beschreibung: Pr�ft, ob sich eine "gef�hrlicher" Feind (Standmine, K�fer oder Mine)
              um Position I befindet. Die Funktion wird nur in ControlMan() aufgerufen.
Parameter
      Eingang: I, uint32_t, Index im Level, Position, an der gepr�ft werden soll
      Ausgang: -
R�ckgabewert:  bool, true = "gef�hrlicher" Feind  hat Kontakt mit Position I
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
bool IsDangerousEnemyAround(uint32_t I) {
    return        ( (Playfield.pLevel[I - 1] == EMERALD_STANDMINE) ||    // links
                    (Playfield.pLevel[I + 1] == EMERALD_STANDMINE) ||    // rechts
                    (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_STANDMINE) || // oben
                    (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_STANDMINE) || // unten
                    // K�fer
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
