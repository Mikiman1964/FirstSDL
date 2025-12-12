#include "checkelement.h"
#include "EmeraldMine.h"

extern PLAYFIELD Playfield;


/*----------------------------------------------------------------------------
Name:           IsElementAlive
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein angegebenes Element "lebt".

Parameter
      Eingang: uElement, uint16_t, Element, das geprüft werden soll
      Ausgang: -
Rückgabewert:  bool, true = Element lebt, sonst nicht
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
bool IsElementAlive(uint16_t uElement) {
    bool bAlive;

    switch (uElement) {
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
        case (EMERALD_GREEN_CHEESE):
        case (EMERALD_YELLOW_DROP):
        case (EMERALD_YELLOW_CHEESE):
        case (EMERALD_SLIME):
        case (EMERALD_GRASS):
        case (EMERALD_STEEL_GROW_LEFT):
        case (EMERALD_STEEL_GROW_RIGHT):
        case (EMERALD_STEEL_GROW_UP):
        case (EMERALD_STEEL_GROW_DOWN):
        case (EMERALD_STEEL_GROW_LEFT_RIGHT):
        case (EMERALD_STEEL_GROW_UP_DOWN):
        case (EMERALD_STEEL_GROW_ALL):
        case (EMERALD_WALL_GROW_LEFT):
        case (EMERALD_WALL_GROW_RIGHT):
        case (EMERALD_WALL_GROW_UP):
        case (EMERALD_WALL_GROW_DOWN):
        case (EMERALD_WALL_GROW_LEFT_RIGHT):
        case (EMERALD_WALL_GROW_UP_DOWN):
        case (EMERALD_WALL_GROW_ALL):
            bAlive = true;
            break;
        default:
            bAlive = false;
            break;
    }
    return bAlive;
}


/*----------------------------------------------------------------------------
Name:           IsSteel
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Element Stahleigenschaften hat.
Parameter
      Eingang: uElement, uint16_t, Element, das geprüft wird
      Ausgang: -
Rückgabewert:  bool, true = Element gehört zum Stahl.
Seiteneffekte: -
------------------------------------------------------------------------------*/
bool IsSteel(uint16_t uElement) {
    switch (uElement) {
        case (EMERALD_STEEL):
        case (EMERALD_STEEL_ROUND_PIKE):
        case (EMERALD_STEEL_ROUND):
        case (EMERALD_STEEL_WARNING):
        case (EMERALD_STEEL_BIOHAZARD):
        case (EMERALD_STEEL_DEADEND):
        case (EMERALD_STEEL_STOP):
        case (EMERALD_STEEL_PARKING):
        case (EMERALD_STEEL_FORBIDDEN):
        case (EMERALD_STEEL_EXIT):
        case (EMERALD_STEEL_RADIOACTIVE):
        case (EMERALD_STEEL_EXPLOSION):
        case (EMERALD_STEEL_ACID):
        case (EMERALD_STEEL_NOT_ROUND):
        case (EMERALD_STEEL_STRIPE_LEFT_TOP):
        case (EMERALD_STEEL_STRIPE_TOP):
        case (EMERALD_STEEL_STRIPE_RIGHT_TOP):
        case (EMERALD_STEEL_STRIPE_LEFT):
        case (EMERALD_STEEL_STRIPE_RIGHT):
        case (EMERALD_STEEL_STRIPE_LEFT_BOTTOM):
        case (EMERALD_STEEL_STRIPE_BOTTOM):
        case (EMERALD_STEEL_STRIPE_RIGHT_BOTTOM):
        case (EMERALD_STEEL_STRIPE_CORNER_LEFT_TOP):
        case (EMERALD_STEEL_STRIPE_CORNER_RIGHT_TOP):
        case (EMERALD_STEEL_STRIPE_CORNER_LEFT_BOTTOM):
        case (EMERALD_STEEL_STRIPE_CORNER_RIGHT_BOTTOM):
        case (EMERALD_STEEL_HEART):
        case (EMERALD_STEEL_PLAYERHEAD):
        case (EMERALD_STEEL_PLAYERHEAD_2):
        case (EMERALD_STEEL_NO_ENTRY):
        case (EMERALD_STEEL_GIVE_WAY):
        case (EMERALD_STEEL_YING):
        case (EMERALD_STEEL_WHEELCHAIR):
        case (EMERALD_STEEL_ARROW_DOWN):
        case (EMERALD_STEEL_ARROW_UP):
        case (EMERALD_STEEL_ARROW_LEFT):
        case (EMERALD_STEEL_ARROW_RIGHT):
        case (EMERALD_STEEL_INVISIBLE):
        case (EMERALD_DOOR_END_NOT_READY_STEEL):
        case (EMERALD_DOOR_END_READY_STEEL):
        case (EMERALD_STEEL_TRASHCAN):
        case (EMERALD_STEEL_JOYSTICK):
        case (EMERALD_STEEL_EDIT_LEVEL):
        case (EMERALD_STEEL_MOVE_LEVEL):
        case (EMERALD_STEEL_COPY_LEVEL):
        case (EMERALD_STEEL_CLIPBOARD_LEVEL):
        case (EMERALD_STEEL_DC3_IMPORT):
        case (EMERALD_STEEL_ADD_LEVELGROUP):
        case (EMERALD_REPLICATOR_RED_TOP_LEFT):
        case (EMERALD_REPLICATOR_RED_TOP_MID):
        case (EMERALD_REPLICATOR_RED_TOP_RIGHT):
        case (EMERALD_REPLICATOR_RED_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_RED_BOTTOM_RIGHT):
        case (EMERALD_REPLICATOR_RED_SWITCH):
        case (EMERALD_REPLICATOR_YELLOW_TOP_LEFT):
        case (EMERALD_REPLICATOR_YELLOW_TOP_MID):
        case (EMERALD_REPLICATOR_YELLOW_TOP_RIGHT):
        case (EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT):
        case (EMERALD_REPLICATOR_YELLOW_SWITCH):
        case (EMERALD_MAGIC_WALL_SWITCH):
        case (EMERALD_MAGIC_WALL_STEEL):
        case (EMERALD_LIGHT_SWITCH):
        case (EMERALD_REPLICATOR_GREEN_TOP_LEFT):
        case (EMERALD_REPLICATOR_GREEN_TOP_MID):
        case (EMERALD_REPLICATOR_GREEN_TOP_RIGHT):
        case (EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT):
        case (EMERALD_REPLICATOR_GREEN_SWITCH):
        case (EMERALD_REPLICATOR_BLUE_TOP_LEFT):
        case (EMERALD_REPLICATOR_BLUE_TOP_MID):
        case (EMERALD_REPLICATOR_BLUE_TOP_RIGHT):
        case (EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT):
        case (EMERALD_REPLICATOR_BLUE_SWITCH):
        case (EMERALD_LIGHTBARRIER_RED_UP):
        case (EMERALD_LIGHTBARRIER_RED_DOWN):
        case (EMERALD_LIGHTBARRIER_RED_LEFT):
        case (EMERALD_LIGHTBARRIER_RED_RIGHT):
        case (EMERALD_LIGHTBARRIER_GREEN_UP):
        case (EMERALD_LIGHTBARRIER_GREEN_DOWN):
        case (EMERALD_LIGHTBARRIER_GREEN_LEFT):
        case (EMERALD_LIGHTBARRIER_GREEN_RIGHT):
        case (EMERALD_LIGHTBARRIER_BLUE_UP):
        case (EMERALD_LIGHTBARRIER_BLUE_DOWN):
        case (EMERALD_LIGHTBARRIER_BLUE_LEFT):
        case (EMERALD_LIGHTBARRIER_BLUE_RIGHT):
        case (EMERALD_LIGHTBARRIER_YELLOW_UP):
        case (EMERALD_LIGHTBARRIER_YELLOW_DOWN):
        case (EMERALD_LIGHTBARRIER_YELLOW_LEFT):
        case (EMERALD_LIGHTBARRIER_YELLOW_RIGHT):
        case (EMERALD_LIGHTBARRIER_RED_SWITCH):
        case (EMERALD_LIGHTBARRIER_GREEN_SWITCH):
        case (EMERALD_LIGHTBARRIER_BLUE_SWITCH):
        case (EMERALD_LIGHTBARRIER_YELLOW_SWITCH):
        case (EMERALD_ACIDPOOL_TOP_LEFT):
        case (EMERALD_ACIDPOOL_TOP_RIGHT):
        case (EMERALD_ACIDPOOL_BOTTOM_LEFT):
        case (EMERALD_ACIDPOOL_BOTTOM_MID):
        case (EMERALD_ACIDPOOL_BOTTOM_RIGHT):
        case (EMERALD_WHEEL_TIMEDOOR):
        case (EMERALD_SWITCH_SWITCHDOOR):
        case (EMERALD_CONVEYORBELT_SWITCH_RED):
        case (EMERALD_CONVEYORBELT_SWITCH_GREEN):
        case (EMERALD_CONVEYORBELT_SWITCH_BLUE):
        case (EMERALD_CONVEYORBELT_SWITCH_YELLOW):
        case (EMERALD_SWITCH_REMOTEBOMB_UP):
        case (EMERALD_SWITCH_REMOTEBOMB_DOWN):
        case (EMERALD_SWITCH_REMOTEBOMB_LEFT):
        case (EMERALD_SWITCH_REMOTEBOMB_RIGHT):
        case (EMERALD_SWITCH_REMOTEBOMB_IGNITION):
        case (EMERALD_STEEL_MODERN_LEFT_END):
        case (EMERALD_STEEL_MODERN_LEFT_RIGHT):
        case (EMERALD_STEEL_MODERN_RIGHT_END):
        case (EMERALD_STEEL_MODERN_UP_END):
        case (EMERALD_STEEL_MODERN_UP_DOWN):
        case (EMERALD_STEEL_MODERN_DOWN_END):
        case (EMERALD_STEEL_MODERN_MIDDLE):
        case (EMERALD_FONT_BLUE_STEEL_DOUBLE_QUOTE):
        case (EMERALD_FONT_GREEN_STEEL_DOUBLE_QUOTE):
        case (EMERALD_FONT_BLUE_STEEL_SEMICOLON):
        case (EMERALD_FONT_GREEN_STEEL_SEMICOLON):
        case (EMERALD_STEEL_GROW_LEFT):
        case (EMERALD_STEEL_GROW_RIGHT):
        case (EMERALD_STEEL_GROW_UP):
        case (EMERALD_STEEL_GROW_DOWN):
        case (EMERALD_STEEL_GROW_LEFT_RIGHT):
        case (EMERALD_STEEL_GROW_UP_DOWN):
        case (EMERALD_STEEL_GROW_ALL):
            return true;
        default:
            if (((uElement >= EMERALD_FONT_GREEN_STEEL_EXCLAMATION) && (uElement <= EMERALD_FONT_GREEN_STEEL_UE)) ||
                ((uElement >= EMERALD_FONT_BLUE_STEEL_EXCLAMATION) && (uElement <= EMERALD_FONT_BLUE_STEEL_UE))) {
                return true;
            } else {
                return false;
            }
    }
}


/*----------------------------------------------------------------------------
Name:           IsPipeElement
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Element eine Röhre ist.
Parameter
      Eingang: uElement, uint16_t, Element, das geprüft werden soll.
      Ausgang: -
Rückgabewert:  bool, true = Element ist eine Röhre
Seiteneffekte: -
------------------------------------------------------------------------------*/
bool IsPipeElement(uint16_t uElement) {
    bool bIsPipe;

    switch (uElement) {
        case (EMERALD_PIPE_UP_DOWN):
        case (EMERALD_PIPE_LEFT_RIGHT):
        case (EMERALD_PIPE_LEFT_UP):
        case (EMERALD_PIPE_LEFT_DOWN):
        case (EMERALD_PIPE_RIGHT_UP):
        case (EMERALD_PIPE_RIGHT_DOWN):
        case (EMERALD_PIPE_LEFT_UP_DOWN):
        case (EMERALD_PIPE_RIGHT_UP_DOWN):
        case (EMERALD_PIPE_LEFT_RIGHT_UP):
        case (EMERALD_PIPE_LEFT_RIGHT_DOWN):
        case (EMERALD_PIPE_LEFT_RIGHT_UP_DOWN):
            bIsPipe = true;
            break;
        default:
            bIsPipe = false;
            break;
    }
    return bIsPipe;
}


/*----------------------------------------------------------------------------
Name:           IsAcidPoolOrReplicatorElement
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Element zu einem Säurebad oder Replikator gehört.
Parameter
      Eingang: uElement, uint16_t, Element, das geprüft werden soll.
      Ausgang: -
Rückgabewert:  bool, true = Element gehört zu Säurebad oder Replikator
Seiteneffekte: -
------------------------------------------------------------------------------*/
bool IsAcidPoolOrReplicatorElement(uint16_t uElement) {
    return IsReplicatorElement(uElement,NULL) || IsAcidPoolElement(uElement);
}


/*----------------------------------------------------------------------------
Name:           IsConveyorbeltElement
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Element zu einem Laufband gehört.
Parameter
      Eingang: uElement, uint16_t, Element, das geprüft werden soll.
      Ausgang: puColor, uint8_t *, Zeiger auf Farbe, darf NULL sein
            COLOR_UNKNOWN
            COLOR_RED
            COLOR_YELLOW
            COLOR_GREEN
            COLOR_BLUE
Rückgabewert:  bool, true = Element gehört zu Laufband
Seiteneffekte: -
------------------------------------------------------------------------------*/
bool IsConveyorbeltElement(uint16_t uElement, uint8_t *puColor) {
    bool bIsConveyorbeltElement;
    uint8_t uColor;

    switch (uElement) {
        case (EMERALD_CONVEYORBELT_RED):
            bIsConveyorbeltElement = true;
            uColor = COLOR_RED;
            break;
        case (EMERALD_CONVEYORBELT_YELLOW):
            bIsConveyorbeltElement = true;
            uColor = COLOR_YELLOW;
            break;
        case (EMERALD_CONVEYORBELT_GREEN):
            bIsConveyorbeltElement = true;
            uColor = COLOR_GREEN;
            break;
        case (EMERALD_CONVEYORBELT_BLUE):
            bIsConveyorbeltElement = true;
            uColor = COLOR_BLUE;
            break;
        default:
            bIsConveyorbeltElement = false;
            uColor = COLOR_UNKNOWN;
            break;
    }
    if (puColor != NULL) {
        *puColor = uColor;
    }
    return bIsConveyorbeltElement;
}


/*----------------------------------------------------------------------------
Name:           IsReplicatorElement
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Element zu einem Replikator gehört.
Parameter
      Eingang: uElement, uint16_t, Element, das geprüft werden soll.
      Ausgang: puColor, uint8_t *, Zeiger auf Farbe, darf NULL sein
            COLOR_UNKNOWN
            COLOR_RED
            COLOR_YELLOW
            COLOR_GREEN
            COLOR_BLUE
Rückgabewert:  bool, true = Element gehört zu Replikator
Seiteneffekte: -
------------------------------------------------------------------------------*/
bool IsReplicatorElement(uint16_t uElement, uint8_t *puColor) {
    bool bIsReplicatorElement;
    uint8_t uColor;

    switch (uElement) {
        case (EMERALD_REPLICATOR_RED_TOP_LEFT):
        case (EMERALD_REPLICATOR_RED_TOP_MID):
        case (EMERALD_REPLICATOR_RED_TOP_RIGHT):
        case (EMERALD_REPLICATOR_RED_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_RED_BOTTOM_RIGHT):
            bIsReplicatorElement = true;
            uColor = COLOR_RED;
            break;
        case (EMERALD_REPLICATOR_YELLOW_TOP_LEFT):
        case (EMERALD_REPLICATOR_YELLOW_TOP_MID):
        case (EMERALD_REPLICATOR_YELLOW_TOP_RIGHT):
        case (EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT):
            bIsReplicatorElement = true;
            uColor = COLOR_YELLOW;
            break;
        case (EMERALD_REPLICATOR_GREEN_TOP_LEFT):
        case (EMERALD_REPLICATOR_GREEN_TOP_MID):
        case (EMERALD_REPLICATOR_GREEN_TOP_RIGHT):
        case (EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT):
            bIsReplicatorElement = true;
            uColor = COLOR_GREEN;
            break;
        case (EMERALD_REPLICATOR_BLUE_TOP_LEFT):
        case (EMERALD_REPLICATOR_BLUE_TOP_MID):
        case (EMERALD_REPLICATOR_BLUE_TOP_RIGHT):
        case (EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT):
            bIsReplicatorElement = true;
            uColor = COLOR_BLUE;
            break;
        default:
            bIsReplicatorElement = false;
            uColor = COLOR_UNKNOWN;
            break;
    }
    if (puColor != NULL) {
        *puColor = uColor;
    }
    return bIsReplicatorElement;
}


/*----------------------------------------------------------------------------
Name:           GetReplicatorPosition
------------------------------------------------------------------------------
Beschreibung: Ermittelt, an welche Stelle ein Replikator-Element gehört
Parameter
      Eingang: uElement, uint16_t, Element, das geprüft werden soll.
      Ausgang: -
Rückgabewert:  uint32_t, möglich sind folgende Werte:
                        REPLICATOR_UNKNOWN
                        REPLICATOR_TOP_LEFT
                        REPLICATOR_TOP_MID
                        REPLICATOR_TOP_RIGHT
                        REPLICATOR_BOTTOM_LEFT
                        REPLICATOR_BOTTOM_RIGHT
Seiteneffekte: -
------------------------------------------------------------------------------*/
uint32_t GetReplicatorPosition(uint16_t uElement) {
    uint32_t uReplicatorPosition;

    switch (uElement) {
        case (EMERALD_REPLICATOR_RED_TOP_LEFT):
        case (EMERALD_REPLICATOR_YELLOW_TOP_LEFT):
        case (EMERALD_REPLICATOR_GREEN_TOP_LEFT):
        case (EMERALD_REPLICATOR_BLUE_TOP_LEFT):
            uReplicatorPosition = REPLICATOR_TOP_LEFT;
            break;
        case (EMERALD_REPLICATOR_RED_TOP_MID):
        case (EMERALD_REPLICATOR_YELLOW_TOP_MID):
        case (EMERALD_REPLICATOR_GREEN_TOP_MID):
        case (EMERALD_REPLICATOR_BLUE_TOP_MID):
            uReplicatorPosition = REPLICATOR_TOP_MID;
            break;
        case (EMERALD_REPLICATOR_RED_TOP_RIGHT):
        case (EMERALD_REPLICATOR_YELLOW_TOP_RIGHT):
        case (EMERALD_REPLICATOR_GREEN_TOP_RIGHT):
        case (EMERALD_REPLICATOR_BLUE_TOP_RIGHT):
            uReplicatorPosition = REPLICATOR_TOP_RIGHT;
            break;
        case (EMERALD_REPLICATOR_RED_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT):
            uReplicatorPosition = REPLICATOR_BOTTOM_LEFT;
            break;
        case (EMERALD_REPLICATOR_RED_BOTTOM_RIGHT):
        case (EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT):
        case (EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT):
        case (EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT):
            uReplicatorPosition = REPLICATOR_BOTTOM_RIGHT;
            break;
        default:
            uReplicatorPosition = REPLICATOR_UNKNOWN;
            break;
    }
    return uReplicatorPosition;
}


/*----------------------------------------------------------------------------
Name:           IsReplicatorElementRed
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Element zu einem roten Replikator gehört.
Parameter
      Eingang: uElement, uint16_t, Element, das geprüft werden soll.
      Ausgang: -
Rückgabewert:  bool, true = Element gehört zu roten Replikator
Seiteneffekte: -
------------------------------------------------------------------------------*/
bool IsReplicatorElementRed(uint16_t uElement) {
    bool bIsReplikatorElementRed;

    switch (uElement) {
        case (EMERALD_REPLICATOR_RED_TOP_LEFT):
        case (EMERALD_REPLICATOR_RED_TOP_MID):
        case (EMERALD_REPLICATOR_RED_TOP_RIGHT):
        case (EMERALD_REPLICATOR_RED_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_RED_BOTTOM_RIGHT):
            bIsReplikatorElementRed = true;
            break;
        default:
            bIsReplikatorElementRed = false;
            break;
    }
    return bIsReplikatorElementRed;
}


/*----------------------------------------------------------------------------
Name:           IsReplicatorElementYellow
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Element zu einem gelben Replikator gehört.
Parameter
      Eingang: uElement, uint16_t, Element, das geprüft werden soll.
      Ausgang: -
Rückgabewert:  bool, true = Element gehört zu gelben Replikator
Seiteneffekte: -
------------------------------------------------------------------------------*/
bool IsReplicatorElementYellow(uint16_t uElement) {
    bool bIsReplikatorElementYellow;

    switch (uElement) {
        case (EMERALD_REPLICATOR_YELLOW_TOP_LEFT):
        case (EMERALD_REPLICATOR_YELLOW_TOP_MID):
        case (EMERALD_REPLICATOR_YELLOW_TOP_RIGHT):
        case (EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT):
            bIsReplikatorElementYellow = true;
            break;
        default:
            bIsReplikatorElementYellow = false;
            break;
    }
    return bIsReplikatorElementYellow;
}


/*----------------------------------------------------------------------------
Name:           IsReplicatorElementGreen
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Element zu einem grünen Replikator gehört.
Parameter
      Eingang: uElement, uint16_t, Element, das geprüft werden soll.
      Ausgang: -
Rückgabewert:  bool, true = Element gehört zu grünen Replikator
Seiteneffekte: -
------------------------------------------------------------------------------*/
bool IsReplicatorElementGreen(uint16_t uElement) {
    bool bIsReplikatorElementGreen;

    switch (uElement) {
        case (EMERALD_REPLICATOR_GREEN_TOP_LEFT):
        case (EMERALD_REPLICATOR_GREEN_TOP_MID):
        case (EMERALD_REPLICATOR_GREEN_TOP_RIGHT):
        case (EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT):
            bIsReplikatorElementGreen = true;
            break;
        default:
            bIsReplikatorElementGreen = false;
            break;
    }
    return bIsReplikatorElementGreen;
}


/*----------------------------------------------------------------------------
Name:           IsReplicatorElementBlue
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Element zu einem blauen Replikator gehört.
Parameter
      Eingang: uElement, uint16_t, Element, das geprüft werden soll.
      Ausgang: -
Rückgabewert:  bool, true = Element gehört zu blauen Replikator
Seiteneffekte: -
------------------------------------------------------------------------------*/
bool IsReplicatorElementBlue(uint16_t uElement) {
    bool bIsReplikatorElementBlue;

    switch (uElement) {
        case (EMERALD_REPLICATOR_BLUE_TOP_LEFT):
        case (EMERALD_REPLICATOR_BLUE_TOP_MID):
        case (EMERALD_REPLICATOR_BLUE_TOP_RIGHT):
        case (EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT):
        case (EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT):
            bIsReplikatorElementBlue = true;
            break;
        default:
            bIsReplikatorElementBlue = false;
            break;
    }
    return bIsReplikatorElementBlue;
}


/*----------------------------------------------------------------------------
Name:           IsAcidPoolElement
------------------------------------------------------------------------------
Beschreibung: Prüft, ob ein Element zu einem Säurebad gehört.
Parameter
      Eingang: uElement, uint16_t, Element, das geprüft werden soll.
      Ausgang: -
Rückgabewert:  bool, true = Element gehört zu Säurebad
Seiteneffekte: -
------------------------------------------------------------------------------*/
bool IsAcidPoolElement(uint16_t uElement) {
    bool bIsAcidPoolElement;

    switch (uElement) {
        case (EMERALD_ACIDPOOL_TOP_LEFT):
        case (EMERALD_ACIDPOOL_TOP_MID):
        case (EMERALD_ACIDPOOL_TOP_RIGHT):
        case (EMERALD_ACIDPOOL_BOTTOM_LEFT):
        case (EMERALD_ACIDPOOL_BOTTOM_MID):
        case (EMERALD_ACIDPOOL_BOTTOM_RIGHT):
            bIsAcidPoolElement = true;
            break;
        default:
            bIsAcidPoolElement = false;
            break;
    }
    return bIsAcidPoolElement;
}


/*----------------------------------------------------------------------------
Name:           IsContactMineExplode
------------------------------------------------------------------------------
Beschreibung: Prüft für eine Kontakt-Mine, ob sich "Monster" in der Nähe befinden.
Parameter
      Eingang: uElement, uint16_t, Element
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
bool IsContactMineExplode(uint16_t uElement) {
    return ( (uElement == EMERALD_ALIEN) || (uElement == EMERALD_YAM) ||
             (uElement == EMERALD_MINE_UP) || (uElement == EMERALD_MINE_RIGHT) || (uElement == EMERALD_MINE_DOWN) || (uElement == EMERALD_MINE_LEFT) ||
             (uElement == EMERALD_MOLE_UP) || (uElement == EMERALD_MOLE_RIGHT) || (uElement == EMERALD_MOLE_DOWN) || (uElement == EMERALD_MOLE_LEFT) ||
             (uElement == EMERALD_BEETLE_UP) || (uElement == EMERALD_BEETLE_RIGHT) || (uElement == EMERALD_BEETLE_DOWN) || (uElement == EMERALD_BEETLE_LEFT) ||
             (uElement == EMERALD_GREEN_CHEESE) ||
             (uElement == EMERALD_SLIME)
             );
}
