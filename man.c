#include "EmeraldMine.h"
#include "emerald.h"
#include "KeyboardMouse.h"
#include "man.h"
#include "saphir.h"
#include "sound.h"
#include "stone.h"

MANKEY ManKey;
extern PLAYFIELD Playfield;
extern INPUTSTATES InputStates;
extern char ge_szElementNames[][64];

/*----------------------------------------------------------------------------
Name:           UpdateManKey
------------------------------------------------------------------------------
Beschreibung: Frischt dir Tastaturabfrage f?r den Man auf.
              Ergebnisse werden in der Struktur ManKey.x abgelegt.
Parameter
      Eingang: -
      Ausgang: -
R?ckgabewert:  -
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
               uDirection, uint32_t, Richtung, in die Man laufen m?chte
      Ausgang: -
R?ckgabewert:  Richtung, in die Man gelaufen ist als Animation
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t ControlMan(uint32_t I, uint32_t uDirection) {
    uint32_t uRetDirection;

    uRetDirection = EMERALD_ANIM_STAND;
    if (Playfield.bManDead) {
        return uRetDirection;
    }
    if (Playfield.uTimeToPlay > 0) {
        Playfield.bPushStone = !Playfield.bPushStone;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        switch (uDirection) {
            case (MANKEY_UP):
                if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach oben frei?
                    if (!ManKey.bFire) {
                        PreparePlaySound(SOUND_MAN,I);
                        ManGoUp(I,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                        uRetDirection = EMERALD_ANIM_UP;
                    }
                } else {
                    uRetDirection = ManTouchElement(I,I - Playfield.uLevel_X_Dimension,EMERALD_ANIM_UP);
                }
                break;
            case (MANKEY_RIGHT):
                if (Playfield.pLevel[I + 1] == EMERALD_SPACE) {  // Ist nach rechts frei?
                    if (!ManKey.bFire) {
                        PreparePlaySound(SOUND_MAN,I);
                        ManGoRight(I,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                        uRetDirection = EMERALD_ANIM_RIGHT;
                    }
                } else {
                    uRetDirection = ManTouchElement(I,I + 1,EMERALD_ANIM_RIGHT);
                }
                break;
            case (MANKEY_DOWN):
                if (Playfield.pLevel[I + Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach unten frei?

                    if (!ManKey.bFire) {
                        PreparePlaySound(SOUND_MAN,I);
                        ManGoDown(I,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                        uRetDirection = EMERALD_ANIM_DOWN;
                    }
                } else {
                    uRetDirection = ManTouchElement(I,I + Playfield.uLevel_X_Dimension,EMERALD_ANIM_DOWN);
                }
                break;
            case (MANKEY_LEFT):
                if (Playfield.pLevel[I - 1] == EMERALD_SPACE) {  // Ist nach links frei?
                    if (!ManKey.bFire) {
                        PreparePlaySound(SOUND_MAN,I);
                        ManGoLeft(I,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                        uRetDirection = EMERALD_ANIM_LEFT;
                    }
                } else {
                    uRetDirection = ManTouchElement(I,I - 1,EMERALD_ANIM_LEFT);
                }
                break;
        }
    } else {
        Playfield.bManDead = true;
        PreparePlaySound(SOUND_MAN_CRIES,I);
        Playfield.pLevel[I] = EMERALD_SPACE;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    }
    return uRetDirection;
}


/*----------------------------------------------------------------------------
Name:           ManTouchElement
------------------------------------------------------------------------------
Beschreibung: Man ber?hrt bzw. will auf ein Element zulaufen.
Parameter
      Eingang: uActPos, uint32_t, aktuelle Position des Man
               uTouchPos, uint32_t, Position die ber?hrt bzw. angelaufen wird
               uAnimation, uint32_t, gew?nschte Animation des Man
      Ausgang: -
R?ckgabewert:  uint32_t, tats?chlich ausgef?hrte Animation des Man
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
uint32_t ManTouchElement(uint32_t uActPos, uint32_t uTouchPos, uint32_t uAnimation) {
    uint32_t uElement;      // Element, dass Man ber?hrt bzw. anl?uft
    uint32_t uRetAnimation;
    uint32_t uTouchStatus;

    uRetAnimation = EMERALD_ANIM_STAND;
    uElement = Playfield.pLevel[uTouchPos];
    uTouchStatus = Playfield.pStatusAnimation[uTouchPos] & 0xFF000000;
    if ((uTouchStatus == EMERALD_ANIM_BORN1) || (uTouchStatus == EMERALD_ANIM_BORN2)) {
        SDL_Log("%s: element '%s' is born!",__FUNCTION__,ge_szElementNames[uElement]);
        return uRetAnimation;
    }
    switch (uElement) {
        case (EMERALD_STONE):
        case (EMERALD_NUT):
        case (EMERALD_BOMB):
        case (EMERALD_MEGABOMB):
            if (uAnimation == EMERALD_ANIM_RIGHT) {
                Playfield.pStatusAnimation[uActPos] = EMERALD_ANIM_MAN_PUSH_RIGHT;  // Falls Man gegen einen blockierten Gegenstand schiebt (durchdrehende Beine)
                if ( (Playfield.pLevel[uTouchPos + 1] == EMERALD_SPACE) && Playfield.bPushStone && (uTouchStatus != EMERALD_ANIM_DOWN_SELF)) {
                    // urspr?ngliche Man-Position mit Space besetzen
                    Playfield.pLevel[uActPos] = EMERALD_SPACE;
                    Playfield.pStatusAnimation[uActPos] = EMERALD_ANIM_STAND;
                    // Man auf neue Position setzen
                    Playfield.pLevel[uActPos + 1] = EMERALD_MAN;
                    Playfield.pStatusAnimation[uActPos + 1]  = EMERALD_ANIM_RIGHT | EMERALD_ANIM_MAN_PUSH_RIGHT;
                    // Stein auf neue Position setzen
                    Playfield.pLevel[uActPos + 2] = uElement;
                    Playfield.pStatusAnimation[uActPos + 2]  = EMERALD_ANIM_RIGHT | EMERALD_ANIM_MAN_PUSH_RIGHT;
                    // Neue Man-Kooridiante setzen
                    Playfield.uManXpos++;
                    PreparePlaySound(SOUND_MAN_PUSH,uActPos + 1);
                    uRetAnimation = EMERALD_ANIM_RIGHT;
                }
            } else if (uAnimation == EMERALD_ANIM_LEFT) {
                Playfield.pStatusAnimation[uActPos] = EMERALD_ANIM_MAN_PUSH_LEFT; // Falls Man gegen einen blockierten Gegenstand schiebt (durchdrehende Beine)
                if ((Playfield.pLevel[uTouchPos - 1] == EMERALD_SPACE) && Playfield.bPushStone) {
                    // urspr?ngliche Man-Position mit Space besetzen
                    Playfield.pLevel[uActPos] = EMERALD_SPACE;
                    Playfield.pStatusAnimation[uActPos] = EMERALD_ANIM_STAND;
                    // Man auf neue Position setzen
                    Playfield.pLevel[uActPos - 1] = EMERALD_MAN;
                    Playfield.pStatusAnimation[uActPos - 1]  = EMERALD_ANIM_LEFT | EMERALD_ANIM_MAN_PUSH_LEFT;
                    // Stein auf neue Position setzen
                    Playfield.pLevel[uActPos - 2] = uElement;
                    Playfield.pStatusAnimation[uActPos - 2]  = EMERALD_ANIM_LEFT | EMERALD_ANIM_MAN_PUSH_LEFT;
                    // Neue Man-Kooridiante setzen
                    Playfield.uManXpos--;
                    PreparePlaySound(SOUND_MAN_PUSH,uActPos - 1);
                    uRetAnimation = EMERALD_ANIM_LEFT;
                }
            }
            break;
        case (EMERALD_WHEEL):
            ControlWheels(uTouchPos);
            SetManArm(uActPos,uAnimation);
            break;
        case (EMERALD_WHEEL_TIMEDOOR):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter l?nger durchgehend gedr?ckt wird
                Playfield.uTimeDoorTimeLeft =  Playfield.uTimeDoorTime;
                Playfield.bTimeDoorOpen = true;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_LIGHT_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter l?nger durchgehend gedr?ckt wird
                Playfield.uTimeLightLeft = Playfield.uTimeLight;
                Playfield.bLightOn = true;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_SWITCH_SWITCHDOOR):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter l?nger durchgehend gedr?ckt wird
                Playfield.bSwitchDoorImpluse = true;
                Playfield.bSwitchDoorState = !Playfield.bSwitchDoorState;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_MAGIC_WALL_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter l?nger durchgehend gedr?ckt wird
                if (!Playfield.bMagicWallRunning) {
                    Playfield.bMagicWallWasOn = false;  // Magic Wall zur?cksetzen
                }
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_LIGHTBARRIER_RED_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter l?nger durchgehend gedr?ckt wird
                Playfield.bLightBarrierRedOn = !Playfield.bLightBarrierRedOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter l?nger durchgehend gedr?ckt wird
                Playfield.bLightBarrierYellowOn = !Playfield.bLightBarrierYellowOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter l?nger durchgehend gedr?ckt wird
                Playfield.bLightBarrierGreenOn = !Playfield.bLightBarrierGreenOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter l?nger durchgehend gedr?ckt wird
                Playfield.bLightBarrierBlueOn = !Playfield.bLightBarrierBlueOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_REPLICATOR_RED_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter l?nger durchgehend gedr?ckt wird
                Playfield.bReplicatorRedOn = !Playfield.bReplicatorRedOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_REPLICATOR_YELLOW_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter l?nger durchgehend gedr?ckt wird
                Playfield.bReplicatorYellowOn = !Playfield.bReplicatorYellowOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_REPLICATOR_GREEN_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter l?nger durchgehend gedr?ckt wird
                Playfield.bReplicatorGreenOn = !Playfield.bReplicatorGreenOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_REPLICATOR_BLUE_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter l?nger durchgehend gedr?ckt wird
                Playfield.bReplicatorBlueOn = !Playfield.bReplicatorBlueOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;


        case (EMERALD_DOOR_ONLY_UP):
            if ((uAnimation == EMERALD_ANIM_UP) && (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE)) {
                uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
            }
            break;
        case (EMERALD_DOOR_ONLY_DOWN):
            if ((uAnimation == EMERALD_ANIM_DOWN) && (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE)) {
                uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
            }
            break;
        case (EMERALD_DOOR_ONLY_LEFT):
            if ((uAnimation == EMERALD_ANIM_LEFT) && (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE)) {
                uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
            }
            break;
        case (EMERALD_DOOR_ONLY_RIGHT):
            if ((uAnimation == EMERALD_ANIM_RIGHT) && (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE)) {
                uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
            }
            break;
        case (EMERALD_DOOR_MULTICOLOR):
            if ((Playfield.bHasRedKey) || (Playfield.bHasYellowKey) || (Playfield.bHasGreenKey) || (Playfield.bHasBlueKey)) {
                switch (uAnimation) {
                    case (EMERALD_ANIM_UP):
                        if (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                            ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_DOWN):
                        if (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                            ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_LEFT):
                        if (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                            ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        if (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                            ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                }
            }
            break;
        case (EMERALD_DOOR_EMERALD):
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                        uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                        ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        if (Playfield.uEmeraldsToCollect > 0) {
                            Playfield.uEmeraldsToCollect++;
                        }
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                        uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                        ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        if (Playfield.uEmeraldsToCollect > 0) {
                            Playfield.uEmeraldsToCollect++;
                        }
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) {
                        uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                        ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        if (Playfield.uEmeraldsToCollect > 0) {
                            Playfield.uEmeraldsToCollect++;
                        }
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) {
                        uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                        ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        if (Playfield.uEmeraldsToCollect > 0) {
                            Playfield.uEmeraldsToCollect++;
                        }
                    }
                    break;
            }
            break;
        case (EMERALD_DOOR_TIME):
            if (Playfield.bTimeDoorOpen) {
                switch (uAnimation) {
                    case (EMERALD_ANIM_UP):
                        if (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                            ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_DOWN):
                        if (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                            ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_LEFT):
                        if (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                            ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        if (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                            ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                }
            }
            break;
        case (EMERALD_SWITCHDOOR_OPEN):
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                        uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                        ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                        uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                        ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) {
                        uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                        ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) {
                        uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                        ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                    }
                    break;
            }
            break;
        case (EMERALD_DOOR_WHITE):
        case (EMERALD_DOOR_WHITE_WOOD):
        case (EMERALD_DOOR_GREY_WHITE):
            if ((Playfield.bHasGeneralKey) || (Playfield.uWhiteKeyCount > 0)) {
                if (!Playfield.bHasGeneralKey) {
                    Playfield.uWhiteKeyCount--;
                }
                switch (uAnimation) {
                    case (EMERALD_ANIM_UP):
                        if (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                            ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_DOWN):
                        if (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                            ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_LEFT):
                        if (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                            ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        if (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                            ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                }
            }
            break;
        case (EMERALD_DOOR_RED):
        case (EMERALD_DOOR_RED_WOOD):
        case (EMERALD_DOOR_GREY_RED):
            if (Playfield.bHasRedKey) {
                switch (uAnimation) {
                    case (EMERALD_ANIM_UP):
                        if (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                            ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_DOWN):
                        if (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                            ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_LEFT):
                        if (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                            ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        if (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                            ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                }
            }
            break;
        case (EMERALD_DOOR_GREEN):
        case (EMERALD_DOOR_GREEN_WOOD):
        case (EMERALD_DOOR_GREY_GREEN):
            if (Playfield.bHasGreenKey) {
                switch (uAnimation) {
                    case (EMERALD_ANIM_UP):
                        if (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                            ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_DOWN):
                        if (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                            ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_LEFT):
                        if (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                            ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        if (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                            ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                }
            }
            break;
        case (EMERALD_DOOR_BLUE):
        case (EMERALD_DOOR_BLUE_WOOD):
        case (EMERALD_DOOR_GREY_BLUE):
            if (Playfield.bHasBlueKey) {
                switch (uAnimation) {
                    case (EMERALD_ANIM_UP):
                        if (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                            ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_DOWN):
                        if (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                            ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_LEFT):
                        if (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                            ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        if (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                            ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                }
            }
            break;
        case (EMERALD_DOOR_YELLOW):
        case (EMERALD_DOOR_YELLOW_WOOD):
        case (EMERALD_DOOR_GREY_YELLOW):
            if (Playfield.bHasYellowKey) {
                switch (uAnimation) {
                    case (EMERALD_ANIM_UP):
                        if (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                            ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_DOWN):
                        if (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                            ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_LEFT):
                        if (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                            ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        if (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) {
                            uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                            ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                }
            }
            break;
        case (EMERALD_KEY_GREEN):
            Playfield.bHasGreenKey = true;
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreKey;
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_GREEN_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else{
                        ManGoUp(uActPos,EMERALD_ANIM_KEY_GREEN_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_GREEN_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_KEY_GREEN_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_GREEN_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_KEY_GREEN_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_GREEN_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_KEY_GREEN_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_TIME_COIN):
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreTimeCoin;
            Playfield.uTimeToPlay = Playfield.uTimeToPlay + Playfield.uAdditonalTimeCoinTime;
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_TIME_COIN_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_TIME_COIN_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_TIME_COIN_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_TIME_COIN_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_TIME_COIN_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_TIME_COIN_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_TIME_COIN_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_TIME_COIN_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_HAMMER):
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreHammer;
            Playfield.uHammerCount++;
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_HAMMER_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_HAMMER_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_HAMMER_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_HAMMER_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_HAMMER_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_HAMMER_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_HAMMER_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_HAMMER_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_DYNAMITE_OFF):
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreDynamite;
            Playfield.uDynamiteCount++;
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_DYNAMITE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_DYNAMITE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_DYNAMITE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_DYNAMITE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_DYNAMITE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_DYNAMITE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_DYNAMITE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_DYNAMITE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_KEY_BLUE):
            Playfield.bHasBlueKey = true;
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreKey;
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_BLUE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_KEY_BLUE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_BLUE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_KEY_BLUE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_BLUE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_KEY_BLUE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_BLUE_SHRINK;
                        SetManArm(uActPos,uAnimation);
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
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_YELLOW_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_KEY_YELLOW_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_YELLOW_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_KEY_YELLOW_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_YELLOW_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_KEY_YELLOW_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_KEY_YELLOW_SHRINK;
                        SetManArm(uActPos,uAnimation);
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
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_KEY_RED_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_KEY_RED_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_KEY_RED_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_KEY_RED_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_KEY_RED_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_KEY_RED_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_KEY_RED_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_KEY_RED_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_KEY_WHITE):
            Playfield.uWhiteKeyCount++;
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreKey;
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_KEY_WHITE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_KEY_WHITE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_KEY_WHITE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_KEY_WHITE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_KEY_WHITE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_KEY_WHITE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_KEY_WHITE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_KEY_WHITE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_KEY_GENERAL):
            Playfield.bHasGeneralKey = true;
            Playfield.bHasRedKey = true;
            Playfield.bHasGreenKey = true;
            Playfield.bHasBlueKey = true;
            Playfield.bHasYellowKey = true;
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreKey;
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_KEY_GENERAL_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_KEY_GENERAL_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_KEY_GENERAL_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_KEY_GENERAL_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_KEY_GENERAL_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_KEY_GENERAL_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_KEY_GENERAL_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_KEY_GENERAL_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_SAND):
            Playfield.pStatusAnimation[uTouchPos] = 0x00;       // Entsprechender Sand-Rand-Status muss gel?scht werden
            PreparePlaySound(SOUND_DIG_SAND,uTouchPos);
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_SAND_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_SAND_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_SAND_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_SAND_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_SAND_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_SAND_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_SAND_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_SAND_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_EMERALD):
            if (uTouchStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL) {
                SDL_Log("%s: emerald sinks already !",__FUNCTION__);
                if (!ManKey.bFire) {
                    if (uAnimation == EMERALD_ANIM_RIGHT) {
                        PreparePlaySound(SOUND_MAN,uActPos);
                        ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    } else if (uAnimation == EMERALD_ANIM_LEFT) {
                        PreparePlaySound(SOUND_MAN,uActPos);
                        ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                }
                return uRetAnimation;
            }
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreEmerald;
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            if (Playfield.uEmeraldsToCollect > 0) {
                Playfield.uEmeraldsToCollect--;
            }
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_EMERALD_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_EMERALD_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_EMERALD_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_EMERALD_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_EMERALD_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_EMERALD_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_EMERALD_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_EMERALD_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_PERL):
            if ( (uTouchStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL) || (uTouchStatus == EMERALD_ANIM_PERL_BREAK) ) {
                SDL_Log("%s: perl sinks or breaks already !",__FUNCTION__);
                if (!ManKey.bFire) {
                    if (uAnimation == EMERALD_ANIM_RIGHT) {
                        PreparePlaySound(SOUND_MAN,uActPos);
                        ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    } else if (uAnimation == EMERALD_ANIM_LEFT) {
                        PreparePlaySound(SOUND_MAN,uActPos);
                        ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                }
                return uRetAnimation;
            }
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScorePerl;
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            if (Playfield.uEmeraldsToCollect < 5) {
                Playfield.uEmeraldsToCollect = 0;
            } else {
                Playfield.uEmeraldsToCollect = Playfield.uEmeraldsToCollect - 5;
            }
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_PERL_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_PERL_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_PERL_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_PERL_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_PERL_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_PERL_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_PERL_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_PERL_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_RUBY):
            if (uTouchStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL) {
                SDL_Log("%s: ruby sinks already !",__FUNCTION__);
                if (!ManKey.bFire) {
                    if (uAnimation == EMERALD_ANIM_RIGHT) {
                        PreparePlaySound(SOUND_MAN,uActPos);
                        ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    } else if (uAnimation == EMERALD_ANIM_LEFT) {
                        PreparePlaySound(SOUND_MAN,uActPos);
                        ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                }
                return uRetAnimation;
            }
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreRuby;
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            if (Playfield.uEmeraldsToCollect < 2) {
                Playfield.uEmeraldsToCollect = 0;
            } else {
                Playfield.uEmeraldsToCollect = Playfield.uEmeraldsToCollect - 2;
            }
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_RUBY_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_RUBY_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_RUBY_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_RUBY_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_RUBY_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_RUBY_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_RUBY_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_RUBY_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_CRYSTAL):
            if (uTouchStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL) {
                SDL_Log("%s: crystal sinks already !",__FUNCTION__);
                if (!ManKey.bFire) {
                    if (uAnimation == EMERALD_ANIM_RIGHT) {
                        PreparePlaySound(SOUND_MAN,uActPos);
                        ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    } else if (uAnimation == EMERALD_ANIM_LEFT) {
                        PreparePlaySound(SOUND_MAN,uActPos);
                        ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                }
                return uRetAnimation;
            }
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreCrystal;
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            if (Playfield.uEmeraldsToCollect < 8) {
                Playfield.uEmeraldsToCollect = 0;
            } else {
                Playfield.uEmeraldsToCollect = Playfield.uEmeraldsToCollect - 8;
            }
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_CRYSTAL_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_CRYSTAL_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_CRYSTAL_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_CRYSTAL_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_CRYSTAL_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_CRYSTAL_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_CRYSTAL_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_CRYSTAL_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_SAPPHIRE):
            if (uTouchStatus == EMERALD_ANIM_SINK_IN_MAGIC_WALL) {
                SDL_Log("%s: sapphire sinks already !",__FUNCTION__);
                if (!ManKey.bFire) {
                    if (uAnimation == EMERALD_ANIM_RIGHT) {
                        PreparePlaySound(SOUND_MAN,uActPos);
                        ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    } else if (uAnimation == EMERALD_ANIM_LEFT) {
                        PreparePlaySound(SOUND_MAN,uActPos);
                        ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                }
                return uRetAnimation;
            }
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreSaphir;
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            if (Playfield.uEmeraldsToCollect < 3) {
                Playfield.uEmeraldsToCollect = 0;
            } else {
                Playfield.uEmeraldsToCollect = Playfield.uEmeraldsToCollect - 3;
            }
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_SAPPHIRE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_SAPPHIRE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_SAPPHIRE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_SAPPHIRE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_SAPPHIRE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_SAPPHIRE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_SAPPHIRE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_SAPPHIRE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_MESSAGE_1): // 0x41
        case (EMERALD_MESSAGE_2):
        case (EMERALD_MESSAGE_3):
        case (EMERALD_MESSAGE_4):
        case (EMERALD_MESSAGE_5):
        case (EMERALD_MESSAGE_6):
        case (EMERALD_MESSAGE_7):
        case (EMERALD_MESSAGE_8): // 0x48
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreMessage;
            Playfield.uShowMessageNo = uElement - EMERALD_MESSAGE_1 + 1;    // Ergibt 1-8
            PreparePlaySound(SOUND_MAN_TAKE,uTouchPos);
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_MESSAGE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_MESSAGE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_MESSAGE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_MESSAGE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_MESSAGE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_MESSAGE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] |= EMERALD_ANIM_MESSAGE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_MESSAGE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_WALL_CORNERED):
        case (EMERALD_WALL_ROUND):
        case (EMERALD_WALL_ROUND_PIKE):
        case (EMERALD_WALL_NOT_ROUND):
        case (EMERALD_MAGIC_WALL):
        case (EMERALD_WALL_INVISIBLE):
            if ((uAnimation != EMERALD_ANIM_STAND) && (ManKey.bFire) && (Playfield.uHammerCount > 0)) {
                Playfield.uHammerCount--;
                SetManArm(uActPos,uAnimation);
                Playfield.pLevel[uTouchPos] = EMERALD_EXPLOSION_PHASE_1;
                Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_STAND;
                PreparePlaySound(SOUND_EXPLOSION,uTouchPos);
            }
            break;
        case (EMERALD_ACIDPOOL):
            SDL_Log("Man falls in pool, I:%d    E:%d",uActPos,Playfield.pLevel[uActPos]);
            Playfield.pLevel[uActPos] = EMERALD_ACIDPOOL_DESTROY;
            Playfield.pStatusAnimation[uActPos] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
            Playfield.pInvalidElement[uActPos] = EMERALD_MAN;
            PreparePlaySound(SOUND_POOL_BLUB,uActPos);
            PreparePlaySound(SOUND_MAN_CRIES,uActPos);
            Playfield.bManDead = true;
            break;
        default:
            SDL_Log("%s:  unhandled element %u",__FUNCTION__,uElement);
            break;
    }
    return uRetAnimation;
}


/*----------------------------------------------------------------------------
Name:           ManGoDown
------------------------------------------------------------------------------
Beschreibung: Man geht nach unten.
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Man-Koordinate)
               uAdditionalAnimStatus, uint32_t, zus?tzlicher Animations-Status
               bDoubleSpeed, bool, true = Man l?uft doppelte Geschwindigkeit
      Ausgang: -
R?ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ManGoDown(uint32_t I, uint32_t uAdditionalAnimStatus, bool bDoubleSpeed) {
    // urspr?ngliche Man-Position mit Space besetzen
    Playfield.pLevel[I] = EMERALD_SPACE;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    if (bDoubleSpeed) {
        // Man auf neue Position setzen
        Playfield.pLevel[I + 2 * Playfield.uLevel_X_Dimension] = EMERALD_MAN;
        Playfield.pStatusAnimation[I + 2 * Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN_DOUBLESPEED;
        Playfield.uManYpos = Playfield.uManYpos + 2;
        PreparePlaySound(SOUND_DOOR,I);
    } else {
        // Man auf neue Position setzen
        Playfield.pLevel[I + Playfield.uLevel_X_Dimension] = EMERALD_MAN;
        Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN | uAdditionalAnimStatus;
        // Man-Koordinate neu setzen
        Playfield.uManYpos++;
    }
}


/*----------------------------------------------------------------------------
Name:           ManGoUp
------------------------------------------------------------------------------
Beschreibung: Man geht nach oben.
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Man-Koordinate)
               uAdditionalAnimStatus, uint32_t, zus?tzlicher Animations-Status
               bDoubleSpeed, bool, true = Man l?uft doppelte Geschwindigkeit
      Ausgang: -
R?ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ManGoUp(uint32_t I, uint32_t uAdditionalAnimStatus, bool bDoubleSpeed) {
    // urspr?ngliche Man-Position mit Space besetzen
    Playfield.pLevel[I] = EMERALD_SPACE;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    if (bDoubleSpeed) {
        // Man auf neue Position setzen
        Playfield.pLevel[I - 2 * Playfield.uLevel_X_Dimension] = EMERALD_MAN;
        Playfield.pStatusAnimation[I - 2 * Playfield.uLevel_X_Dimension] = EMERALD_ANIM_UP_DOUBLESPEED;
        Playfield.uManYpos = Playfield.uManYpos - 2;
        PreparePlaySound(SOUND_DOOR,I);
    } else {
        // Man auf neue Position setzen
        Playfield.pLevel[I - Playfield.uLevel_X_Dimension] = EMERALD_MAN;
        Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_UP | uAdditionalAnimStatus;
        // Man-Koordinate neu setzen
        Playfield.uManYpos--;
    }
}


/*----------------------------------------------------------------------------
Name:           ManGoRight
------------------------------------------------------------------------------
Beschreibung: Man geht nach rechts.
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Man-Koordinate)
               uAdditionalAnimStatus, uint32_t, zus?tzlicher Animations-Status
               bDoubleSpeed, bool, true = Man l?uft doppelte Geschwindigkeit
      Ausgang: -
R?ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ManGoRight(uint32_t I, uint32_t uAdditionalAnimStatus, bool bDoubleSpeed) {
    // urspr?ngliche Man-Position mit Space besetzen
    Playfield.pLevel[I] = EMERALD_SPACE;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    if (bDoubleSpeed) {
        // Man auf neue Position setzen
        Playfield.pLevel[I + 2] = EMERALD_MAN;
        Playfield.pStatusAnimation[I + 2] = EMERALD_ANIM_RIGHT_DOUBLESPEED;
        Playfield.uManXpos = Playfield.uManXpos + 2;
        PreparePlaySound(SOUND_DOOR,I);
    } else {
        // Man auf neue Position setzen
        Playfield.pLevel[I + 1] = EMERALD_MAN;
        Playfield.pStatusAnimation[I + 1] = EMERALD_ANIM_RIGHT | uAdditionalAnimStatus;
        // Man-Koordinate neu setzen
        Playfield.uManXpos++;
    }
}


/*----------------------------------------------------------------------------
Name:           ManGoLeft
------------------------------------------------------------------------------
Beschreibung: Man geht nach links.
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Man-Koordinate)
               uAdditionalAnimStatus, uint32_t, zus?tzlicher Animations-Status
               bDoubleSpeed, bool, true = Man l?uft doppelte Geschwindigkeit
      Ausgang: -
R?ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ManGoLeft(uint32_t I, uint32_t uAdditionalAnimStatus, bool bDoubleSpeed) {
    // urspr?ngliche Man-Position mit Space besetzen
    Playfield.pLevel[I] = EMERALD_SPACE;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    if (bDoubleSpeed) {
        // Man auf neue Position setzen
        Playfield.pLevel[I - 2] = EMERALD_MAN;
        Playfield.pStatusAnimation[I - 2] = EMERALD_ANIM_LEFT_DOUBLESPEED;
        Playfield.uManXpos = Playfield.uManXpos - 2;
        PreparePlaySound(SOUND_DOOR,I);
    } else {
        // Man auf neue Position setzen
        Playfield.pLevel[I - 1] = EMERALD_MAN;
        Playfield.pStatusAnimation[I - 1] = EMERALD_ANIM_LEFT | uAdditionalAnimStatus;
        // Man-Koordinate neu setzen
        Playfield.uManXpos--;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlWheels
------------------------------------------------------------------------------
Beschreibung: Steuert die R?der.
Parameter
      Eingang: I, uint32_t, Rad, das vom Man gedr?ckt wurde
      Ausgang: -
R?ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlWheels(uint32_t I) {
    uint32_t uWheelposRunning;              // lineare Kooridnate des laufenden Rades

    if (Playfield.bWheelRunning) {          // L?uft bereits ein Rad ?
        uWheelposRunning = Playfield.uWheelRunningYpos * Playfield.uLevel_X_Dimension + Playfield.uWheelRunningXpos;
        Playfield.pStatusAnimation[uWheelposRunning] = EMERALD_ANIM_STAND;    // Voriges Rad stoppen
    }
    Playfield.bWheelRunning = true;
    Playfield.pStatusAnimation[I] = EMERALD_ANIM_WHEEL_RUN;             // Rad animieren
    Playfield.uWheelRunningXpos = I % Playfield.uLevel_X_Dimension;     // X- und
    Playfield.uWheelRunningYpos = I / Playfield.uLevel_X_Dimension;     // Y-Koordinate des (neuen) laufenden Rades eintragen
    Playfield.uTimeWheelRotationLeft = Playfield.uTimeWheelRotation;    // Zeit neu setzen
}


/*----------------------------------------------------------------------------
Name:           CheckRunningWheel
------------------------------------------------------------------------------
Beschreibung: Steuert laufendes Rad.
Parameter
      Eingang: -
      Ausgang: -
R?ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CheckRunningWheel(void) {
    uint32_t uWheelposRunning;              // lineare Kooridnate des laufenden Rades

    if (Playfield.bWheelRunning) {          // L?uft bereits ein Rad ?
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
Name:           SetManArm
------------------------------------------------------------------------------
Beschreibung: Falls der Man etwas mit der Firetaste aufnimmt oder einen Schalter/ein Rad
              bet?tigt, wird hier die "richtige" Armbewegung gesetzt.
Parameter
      Eingang: I, uint32_t, aktuelle Position des Man
      Ausgang: -
R?ckgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void SetManArm(uint32_t I,uint32_t uAnimation) {
    switch (uAnimation) {
        case (EMERALD_ANIM_UP):
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_MAN_UP_ARM;
            break;
        case (EMERALD_ANIM_RIGHT):
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_MAN_RIGHT_ARM;
            break;
        case (EMERALD_ANIM_DOWN):
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_MAN_DOWN_ARM;
            break;
        case (EMERALD_ANIM_LEFT):
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_MAN_LEFT_ARM;
            break;
    }
}
