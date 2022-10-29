#include "EmeraldMine.h"
#include "man.h"
#include "KeyboardMouse.h"


MANKEY ManKey;
extern PLAYFIELD Playfield;
extern INPUTSTATES InputStates;

/*----------------------------------------------------------------------------
Name:           UpdateManKey
------------------------------------------------------------------------------
Beschreibung: Frischt dir Tastaturabfrage für den Man auf. Sollte erst nach UpdateInputStates()
              aufgerufen werden. Ergebnisse werden in der Struktur ManKey.x abgelegt.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: ManKey.x, InputStates.x, PLayfield.x
------------------------------------------------------------------------------*/
void UpdateManKey() {
    UpdateInputStates();
    if (InputStates.pKeyboardArray[SDL_SCANCODE_UP]) {
        ManKey.uDirection = MANKEY_UP;
    } else if (InputStates.pKeyboardArray[SDL_SCANCODE_DOWN]) {
        ManKey.uDirection = MANKEY_DOWN;
    } else if (InputStates.pKeyboardArray[SDL_SCANCODE_LEFT]) {
        ManKey.uDirection = MANKEY_LEFT;
    } else if (InputStates.pKeyboardArray[SDL_SCANCODE_RIGHT]) {
        ManKey.uDirection = MANKEY_RIGHT;
    } else {
        ManKey.uDirection = MANKEY_NONE;
    }
    if (ManKey.uLastDirection != ManKey.uDirection) {
        if (ManKey.uDirection != MANKEY_NONE) {
            ManKey.uLastDirectionFrameCount = Playfield.uFrameCounter;
            ManKey.uLastActiveDirection = ManKey.uDirection;
        } else {
            ManKey.uLastNoDirectionFrameCount = Playfield.uFrameCounter;
        }
    }
    ManKey.uLastDirection = ManKey.uDirection;
    ManKey.bFire = (InputStates.pKeyboardArray[SDL_SCANCODE_LCTRL]);
}


/*----------------------------------------------------------------------------
Name:           ControlMan
------------------------------------------------------------------------------
Beschreibung: Steuert den Man.
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Man-Koordinate)
               uDirection, uint32_t, Richtung, in die Man laufen möchte
      Ausgang: -
Rückgabewert:  Richtung, in die Man gelaufen ist als Animation
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t ControlMan(uint32_t I, uint32_t uDirection) {
    uint32_t uRetDirection;

    uRetDirection = EMERALD_ANIM_STAND;

    switch (uDirection) {
        case (MANKEY_UP):
            if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach oben frei?
                if (!ManKey.bFire) {
                    ManGoUp(I,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                    uRetDirection = EMERALD_ANIM_UP;
                }
            } else {
                uRetDirection =  ManTouchElement(I,I - Playfield.uLevel_X_Dimension,EMERALD_ANIM_UP);
            }
            break;
        case (MANKEY_RIGHT):
            if (Playfield.pLevel[I + 1] == EMERALD_SPACE) {  // Ist nach rechts frei?
                if (!ManKey.bFire) {
                    ManGoRight(I,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                    uRetDirection = EMERALD_ANIM_RIGHT;
                }
            } else {
                uRetDirection =  ManTouchElement(I,I + 1,EMERALD_ANIM_RIGHT);
            }
            break;
        case (MANKEY_DOWN):
            if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach unten frei?
                if (!ManKey.bFire) {
                    ManGoDown(I,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                    uRetDirection = EMERALD_ANIM_DOWN;
                }
            } else {
                uRetDirection =  ManTouchElement(I,I + Playfield.uLevel_X_Dimension,EMERALD_ANIM_DOWN);
            }
            break;
        case (MANKEY_LEFT):
            if (Playfield.pLevel[I - 1] == EMERALD_SPACE) {  // Ist nach links frei?
                if (!ManKey.bFire) {
                    ManGoLeft(I,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                    uRetDirection = EMERALD_ANIM_LEFT;
                }
            } else {
                uRetDirection =  ManTouchElement(I,I - 1,EMERALD_ANIM_LEFT);
            }
            break;
    }
    return uRetDirection;
}


/*----------------------------------------------------------------------------
Name:           ManTouchElement
------------------------------------------------------------------------------
Beschreibung: Man berührt bzw. will auf ein Element zulaufen.
Parameter
      Eingang: uActPos, uint32_t, aktuelle Position des Man
               uTouchPos, uint32_t, Position die berührt bzw. angelaufen wird
               uAnimation, uint32_t, gewünschte Animation des Man
      Ausgang: -
Rückgabewert:  uint32_t, tatsächlich ausgeführte Animation des Man
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t ManTouchElement(uint32_t uActPos, uint32_t uTouchPos, uint32_t uAnimation) {
    uint32_t uElement;      // Element, dass Man berührt bzw. anläuft
    uint32_t uRetAnimation;

    uRetAnimation = EMERALD_ANIM_STAND;
    uElement = Playfield.pLevel[uTouchPos];
    switch (uElement) {
        case (EMERALD_WHEEL):
            ControlWheels(uTouchPos);
            break;
        case (EMERALD_REPLICATOR_RED_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                Playfield.bReplicatorRedOn = !Playfield.bReplicatorRedOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
            }
            break;
        case (EMERALD_DOOR_RED):
            if (Playfield.bHasRedKey) {
                switch (uAnimation) {
                    case (EMERALD_ANIM_UP):
                        if (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                            ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                    case (EMERALD_ANIM_DOWN):
                        if (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                            ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                    case (EMERALD_ANIM_LEFT):
                        if (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                            ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        if (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                            ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                }
            }
            break;
        case (EMERALD_DOOR_GREEN):
            if (Playfield.bHasGreenKey) {
                switch (uAnimation) {
                    case (EMERALD_ANIM_UP):
                        if (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                            ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                    case (EMERALD_ANIM_DOWN):
                        if (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                            ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                    case (EMERALD_ANIM_LEFT):
                        if (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                            ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        if (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                            ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                }
            }
            break;
        case (EMERALD_DOOR_BLUE):
            if (Playfield.bHasBlueKey) {
                switch (uAnimation) {
                    case (EMERALD_ANIM_UP):
                        if (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                            ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                    case (EMERALD_ANIM_DOWN):
                        if (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                            ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                    case (EMERALD_ANIM_LEFT):
                        if (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                            ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        if (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                            ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                }
            }
            break;
        case (EMERALD_DOOR_YELLOW):
            if (Playfield.bHasYellowKey) {
                switch (uAnimation) {
                    case (EMERALD_ANIM_UP):
                        if (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                            ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                    case (EMERALD_ANIM_DOWN):
                        if (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                            ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                    case (EMERALD_ANIM_LEFT):
                        if (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                            ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        if (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                            ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_DOUBLESPEED);
                        }
                        break;
                }
            }
            break;
        case (EMERALD_KEY_GREEN):
            Playfield.bHasGreenKey = true;
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreKey;
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_GREEN_SHRINK;
                    } else{
                        ManGoUp(uActPos,EMERALD_ANIM_KEY_GREEN_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_GREEN_SHRINK;
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_KEY_GREEN_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_GREEN_SHRINK;
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_KEY_GREEN_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_GREEN_SHRINK;
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_KEY_GREEN_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_KEY_BLUE):
            Playfield.bHasBlueKey = true;
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreKey;
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_BLUE_SHRINK;
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_KEY_BLUE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_BLUE_SHRINK;
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_KEY_BLUE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_BLUE_SHRINK;
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_KEY_BLUE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_BLUE_SHRINK;
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_KEY_BLUE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_KEY_YELLOW):
            Playfield.bHasYellowKey = true;
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreKey;
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_YELLOW_SHRINK;
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_KEY_YELLOW_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_YELLOW_SHRINK;
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_KEY_YELLOW_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_YELLOW_SHRINK;
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_KEY_YELLOW_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_YELLOW_SHRINK;
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_KEY_YELLOW_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_KEY_RED):
            Playfield.bHasRedKey = true;
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreKey;
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_RED_SHRINK;
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_KEY_RED_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_RED_SHRINK;
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_KEY_RED_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_RED_SHRINK;
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_KEY_RED_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_RED_SHRINK;
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_KEY_RED_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_SAND):
            Playfield.pStatusAnimation[uTouchPos] = 0x00;       // Entsprechender Sand-Rand-Status muss gelöscht werden
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_SAND_SHRINK;
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_SAND_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_SAND_SHRINK;

                        SDL_Log("fire&down  act:%u   touch:%u   touch element: x%02x",uActPos,uTouchPos,Playfield.pLevel[uTouchPos]);

                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_SAND_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_SAND_SHRINK;
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_SAND_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_SAND_SHRINK;
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_SAND_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        default:
            SDL_Log("%s:  unhandled element %u",__FUNCTION__,uElement);
            break;
    }
    return uRetAnimation;
}


/*----------------------------------------------------------------------------
Name:           ManGoUp
------------------------------------------------------------------------------
Beschreibung: Man geht nach oben.
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Man-Koordinate)
               uAdditionalAnimStatus, uint32_t, zusätzlicher Animations-Status
               bDoubleSpeed, bool, true = Man läuft doppelte Geschwindigkeit
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ManGoUp(uint32_t I, uint32_t uAdditionalAnimStatus, bool bDoubleSpeed) {
    if (bDoubleSpeed) {
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I - 2 * Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pStatusAnimation[I - 2 * Playfield.uLevel_X_Dimension] |= uAdditionalAnimStatus | EMERALD_MAN | EMERALD_ANIM_CLEAN_DOWN_DOUBLESPEED;
        // Aktuelles Element auf Animation "oben"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_UP_DOUBLESPEED;
    } else {
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] |= uAdditionalAnimStatus | EMERALD_MAN | EMERALD_ANIM_CLEAN_DOWN;
        // Aktuelles Element auf Animation "oben"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_UP;
    }
}


/*----------------------------------------------------------------------------
Name:           ManGoLeft
------------------------------------------------------------------------------
Beschreibung: Man geht nach links.
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Man-Koordinate)
               uAdditionalAnimStatus, uint32_t, zusätzlicher Animations-Status
               bDoubleSpeed, bool, true = Man läuft doppelte Geschwindigkeit
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ManGoLeft(uint32_t I, uint32_t uAdditionalAnimStatus, bool bDoubleSpeed) {
    if (bDoubleSpeed) {
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I - 2] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pStatusAnimation[I - 2] |= uAdditionalAnimStatus | EMERALD_MAN | EMERALD_ANIM_CLEAN_RIGHT_DOUBLESPEED;
        // Aktuelles Element auf Animation "links"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT_DOUBLESPEED;
    } else {
        // neuen Platz mit ungültigem Element besetzen
        Playfield.pLevel[I - 1] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pStatusAnimation[I - 1] |= uAdditionalAnimStatus | EMERALD_MAN | EMERALD_ANIM_CLEAN_RIGHT;
        // Aktuelles Element auf Animation "links"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_LEFT;
    }
}


/*----------------------------------------------------------------------------
Name:           ManGoDown
------------------------------------------------------------------------------
Beschreibung: Man geht nach unten.
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Man-Koordinate)
               uAdditionalAnimStatus, uint32_t, zusätzlicher Animations-Status
               bDoubleSpeed, bool, true = Man läuft doppelte Geschwindigkeit
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ManGoDown(uint32_t I, uint32_t uAdditionalAnimStatus, bool bDoubleSpeed) {
    // neuen Platz mit ungültigem Element besetzen
    if (bDoubleSpeed) {
        Playfield.pLevel[I + 2 * Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pStatusAnimation[I + 2 * Playfield.uLevel_X_Dimension] |= uAdditionalAnimStatus | EMERALD_MAN | EMERALD_ANIM_CLEAN_UP_DOUBLESPEED;
        // Aktuelles Element auf Animation "unten"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN_DOUBLESPEED;
    } else {
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] |= uAdditionalAnimStatus | EMERALD_MAN | EMERALD_ANIM_CLEAN_UP;
        // Aktuelles Element auf Animation "unten"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOWN;
    }
}


/*----------------------------------------------------------------------------
Name:           ManGoRight
------------------------------------------------------------------------------
Beschreibung: Man geht nach rechts.
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Man-Koordinate)
               uAdditionalAnimStatus, uint32_t, zusätzlicher Animations-Status
               bDoubleSpeed, bool, true = Man läuft doppelte Geschwindigkeit
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ManGoRight(uint32_t I, uint32_t uAdditionalAnimStatus, bool bDoubleSpeed) {
    // neuen Platz mit ungültigem Element besetzen
    if (bDoubleSpeed) {
        Playfield.pLevel[I + 2] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pStatusAnimation[I + 2] |= uAdditionalAnimStatus | EMERALD_MAN | EMERALD_ANIM_CLEAN_LEFT_DOUBLESPEED;
        // Aktuelles Element auf Animation "rechts"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT_DOUBLESPEED;
    } else {
        Playfield.pLevel[I + 1] = EMERALD_INVALID;
        // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
        Playfield.pStatusAnimation[I + 1] |= uAdditionalAnimStatus | EMERALD_MAN | EMERALD_ANIM_CLEAN_LEFT;
        // Aktuelles Element auf Animation "rechts"
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_RIGHT;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlWheels
------------------------------------------------------------------------------
Beschreibung: Steuert die Räder.
Parameter
      Eingang: I, uint32_t, Rad, das vom Man gedrückt wurde
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlWheels(uint32_t I) {
    uint32_t uWheelposRunning;              // lineare Kooridnate des laufenden Rades

    if (Playfield.bWheelRunning) {          // Läuft bereits ein Rad ?
        uWheelposRunning = Playfield.uWheelRunningYpos * Playfield.uLevel_X_Dimension + Playfield.uWheelRunningXpos;
        Playfield.pStatusAnimation[uWheelposRunning] = EMERALD_ANIM_STAND;    // Voriges Rad stoppen
    }
    Playfield.bWheelRunning = true;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_WHEEL_RUN;             // Rad animieren
    Playfield.uWheelRunningXpos = I % Playfield.uLevel_X_Dimension;     // X- und
    Playfield.uWheelRunningYpos = I / Playfield.uLevel_X_Dimension;     // Y-Koordinate des (neuen) laufenden rades eintragen
    Playfield.uTimeWheelRotationLeft = Playfield.uTimeWheelRotation;    // Zeit neu setzen
}


/*----------------------------------------------------------------------------
Name:           CheckRunningWheel
------------------------------------------------------------------------------
Beschreibung: Steuert laufendes Rad.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CheckRunningWheel(void) {
    uint32_t uWheelposRunning;              // lineare Kooridnate des laufenden Rades

    if (Playfield.bWheelRunning) {          // Läuft bereits ein Rad ?
        Playfield.uTimeWheelRotationLeft--;
        if (Playfield.uTimeWheelRotationLeft == 0) {
            // laufendes Rad anhalten, da Zeit abgelaufen ist
            Playfield.bWheelRunning = false;
            uWheelposRunning = Playfield.uWheelRunningYpos * Playfield.uLevel_X_Dimension + Playfield.uWheelRunningXpos;
            Playfield.pStatusAnimation[uWheelposRunning] = EMERALD_ANIM_STAND;    // Voriges Rad stoppen
            Playfield.uWheelRunningXpos = 0;
            Playfield.uWheelRunningYpos = 0;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           ControlRedReplicator
------------------------------------------------------------------------------
Beschreibung: Steuert den roten Repliktor
Parameter
      Eingang: I, uint32_t, Position der oberen Mittelteils des Replikators
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlRedReplikator(uint32_t I) {

    if (Playfield.bReplicatorRedOn) {          // Laufen die roten Replikatoren?
        if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {   // Ist der Replikator frei?
            Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_MINE_DOWN;
            Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_BORN1;
        }
    }
}
