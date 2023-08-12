#include "config.h"
#include "EmeraldMine.h"
#include "emerald.h"
#include "explosion.h"
#include "gamecontroller.h"
#include "KeyboardMouse.h"
#include "loadlevel.h"
#include "man.h"
#include "mySDL.h"
#include "saphir.h"
#include "sound.h"
#include "stone.h"
#include "teleporter.h"

MANKEY ManKey;
extern PLAYFIELD Playfield;
extern INPUTSTATES InputStates;
extern char ge_szElementNames[][64];
extern SDL_DisplayMode ge_DisplayMode;
extern CONFIG Config;

/*----------------------------------------------------------------------------
Name:           UpdateManKey
------------------------------------------------------------------------------
Beschreibung: Frischt die Tastaturabfrage für den Man auf.
              Ergebnisse werden in der Struktur ManKey.x abgelegt.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: ManKey.x, InputStates.x, PLayfield.x, Config.x
------------------------------------------------------------------------------*/
void UpdateManKey() {
    MAN_DIRECTIONS ManDirections;

    UpdateInputStates();
    if (GetJoystickOrGameControllerStatus(&ManDirections) == 0) {     // Falls Device nicht abgefragt werden kann/soll, dann Keyboard nehmen
        if (ManDirections.bLeft) {
            ManKey.uDirection = MANKEY_LEFT;
        } else if (ManDirections.bRight) {
            ManKey.uDirection = MANKEY_RIGHT;
        } else if (ManDirections.bUp) {
            ManKey.uDirection = MANKEY_UP;
        } else if (ManDirections.bDown) {
            ManKey.uDirection = MANKEY_DOWN;
        } else {
            ManKey.uDirection = MANKEY_NONE;
        }
        ManKey.bFire = ManDirections.bFire;
        if (ManDirections.bExit) {
            ManKey.bExit = true;    // wird in RunGame() bestätigt
        }
        if ((ManDirections.bDynamite) || (InputStates.pKeyboardArray[SDL_SCANCODE_SPACE])) {
            ManKey.uFireCount++;
            //SDL_Log("FireCount: %u",ManKey.uFireCount);
        } else {
            ManKey.uFireCount = 0;
        }
    } else {
        // Keyboard
        if (InputStates.pKeyboardArray[SDL_SCANCODE_LEFT]) {
            ManKey.uDirection = MANKEY_LEFT;
        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_RIGHT]) {
            ManKey.uDirection = MANKEY_RIGHT;
        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_UP]) {
            ManKey.uDirection = MANKEY_UP;
        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_DOWN]) {
            ManKey.uDirection = MANKEY_DOWN;
        } else {
            ManKey.uDirection = MANKEY_NONE;
        }
        ManKey.bFire = (InputStates.pKeyboardArray[SDL_SCANCODE_LCTRL]);
        // Zündung des Dynamits mit Feuertaste oder Space
        if (  (((ManKey.bFire) && (!Config.bStartDynamiteWithSpace)) || ((InputStates.pKeyboardArray [SDL_SCANCODE_SPACE]) && (Config.bStartDynamiteWithSpace)))  && (Playfield.uDynamitePos == 0xFFFFFFFF) && (ManKey.uDirection == MANKEY_NONE)) {
            ManKey.uFireCount++;
            //SDL_Log("FireCount: %u",ManKey.uFireCount);
        } else {
            ManKey.uFireCount = 0;
        }
    }
    if (ManKey.bFire) {
        ManKey.uLastFireFrameCount = Playfield.uFrameCounter;
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
}


/*----------------------------------------------------------------------------
Name:           ControlManDies
------------------------------------------------------------------------------
Beschreibung: Steuert den sterbenden Man.
Parameter
      Eingang: I, uint32_t, Index im Level (lineare Man-Koordinate)
               uDirection, uint32_t, Richtung, in die Man laufen möchte
      Ausgang: -
Rückgabewert:  Richtung, in die Man gelaufen ist als Animation
Seiteneffekte: Playfield.x, Mankey.x
------------------------------------------------------------------------------*/
void ControlManDies(uint32_t I) {
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        SDL_Log("%s: ack double control",__FUNCTION__);
    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_MAN_DIES_P1) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_MAN_DIES_P2;
    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_MAN_DIES_P2) {
        Playfield.pLevel[I] = EMERALD_SPACE;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    }
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
Seiteneffekte: Playfield.x, ManKey.x, Config.x
------------------------------------------------------------------------------*/
uint32_t ControlMan(uint32_t I, uint32_t uDirection) {
    uint32_t uRetDirection;

    uRetDirection = EMERALD_ANIM_STAND;
    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MAN_GOES_ENDDOOR) {
        Playfield.pLevel[I] = EMERALD_SPACE;
    }
    if ((Playfield.bManDead) || (Playfield.bWellDone))  {
        return uRetDirection;
    }
    if (IsDangerousEnemyAround(I)) {
        Playfield.pLevel[I] = EMERALD_CENTRAL_EXPLOSION;
        PreparePlaySound(SOUND_MAN_CRIES,I);
        Playfield.bManDead = true;
        return uRetDirection;
    }

    if ((ManKey.uFireCount > (4 * 16)) && (Playfield.uDynamiteCount > 0)) { // Bei 6 zündet das Dynamit
        Playfield.uDynamiteCount--;
        ManKey.uFireCount = 0;
        Playfield.uDynamitePos = I;
        Playfield.uDynamiteStatusAnim = EMERALD_ANIM_DYNAMITE_START;
        PreparePlaySound(SOUND_DYNAMITE_START,I);
        return uRetDirection;
    }

    if (Playfield.uTimeToPlay > 0) {
        Playfield.bPushStone = !Playfield.bPushStone;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
        switch (uDirection) {
            case (MANKEY_UP):
                if (Playfield.pLevel[I - Playfield.uLevel_X_Dimension] == EMERALD_SPACE) {  // Ist nach oben frei?
                    if (!ManKey.bFire) {
                        if ((Playfield.pStatusAnimation[I - Playfield.uLevel_X_Dimension] & 0xFF000000) == EMERALD_ANIM_BLOCK_MAN) {
                            SDL_Log("%s: Man is blocked!",__FUNCTION__);
                            return uRetDirection;
                        }
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
                        if ((Playfield.pStatusAnimation[I + 1] & 0xFF000000) == EMERALD_ANIM_BLOCK_MAN) {
                            SDL_Log("%s: Man is blocked!",__FUNCTION__);
                            return uRetDirection;
                        }
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
                        if ((Playfield.pStatusAnimation[I + Playfield.uLevel_X_Dimension] & 0xFF000000) == EMERALD_ANIM_BLOCK_MAN) {
                            SDL_Log("%s: Man is blocked!",__FUNCTION__);
                            return uRetDirection;
                        }
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
                        if ((Playfield.pStatusAnimation[I - 1] & 0xFF000000) == EMERALD_ANIM_BLOCK_MAN) {
                            SDL_Log("%s: Man is blocked!",__FUNCTION__);
                            return uRetDirection;
                        }
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
        if (!Playfield.bWellDone) { // Nur Tötung einleiten, wenn Spiel noch nicht gewonnen
            Playfield.bManDead = true;
            PreparePlaySound(SOUND_MAN_CRIES,I);
            Playfield.pLevel[I] = EMERALD_MAN_DIES;
            Playfield.pStatusAnimation[I] = EMERALD_ANIM_AVOID_DOUBLE_CONTROL | EMERALD_ANIM_MAN_DIES_P1;
        }
    }
    if ((Playfield.uDynamitePos != 0xFFFFFFFF) && ((Playfield.uManXpos + Playfield.uManYpos * Playfield.uLevel_X_Dimension) != Playfield.uDynamitePos)) {   // Steht Man nicht mehr selbst gezündeten Dynamit?
        // Da wo der man vorher mit Dynamit stand jetzt ein gezündetes Dynamit setzen
        Playfield.pLevel[Playfield.uDynamitePos] = EMERALD_DYNAMITE_ON;
        Playfield.pStatusAnimation[Playfield.uDynamitePos] = Playfield.uDynamiteStatusAnim;
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
Seiteneffekte: Playfield.x, ge_szElementNames[], ge_DisplayMode.refresh_rate,
               ManKey.x
------------------------------------------------------------------------------*/
uint32_t ManTouchElement(uint32_t uActPos, uint32_t uTouchPos, uint32_t uAnimation) {
    uint32_t uElement;      // Element, dass Man berührt bzw. anläuft
    uint32_t uRetAnimation;
    uint32_t uTouchStatus;
    uint32_t uTeleporterCoordinate;

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
            if (!ManKey.bFire) {
                if (uAnimation == EMERALD_ANIM_RIGHT) {
                    Playfield.pStatusAnimation[uActPos] = EMERALD_ANIM_MAN_PUSH_RIGHT;  // Falls Man gegen einen blockierten Gegenstand schiebt (durchdrehende Beine)
                    if ( (Playfield.pLevel[uTouchPos + 1] == EMERALD_SPACE) && Playfield.bPushStone && (uTouchStatus != EMERALD_ANIM_DOWN_SELF)) {
                        // ursprüngliche Man-Position mit Space besetzen
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
                        // ursprüngliche Man-Position mit Space besetzen
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
            }
            break;
        case (EMERALD_WHEEL):
            ControlWheels(uTouchPos);
            SetManArm(uActPos,uAnimation);
            break;
        case (EMERALD_WHEEL_TIMEDOOR):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                Playfield.uTimeDoorTimeLeft =  Playfield.uTimeDoorTime;
                if (Playfield.uTimeDoorTime > 0) {
                    Playfield.bTimeDoorOpen = true;
                }
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_CONVEYORBELT_SWITCH_RED):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                switch (Playfield.uConveybeltRedState) {
                    case (EMERALD_CONVEYBELT_LEFT):
                    case (EMERALD_CONVEYBELT_RIGHT):
                        Playfield.uConveybeltRedState = EMERALD_CONVEYBELT_OFF;
                        break;
                    case (EMERALD_CONVEYBELT_OFF):
                        if (Playfield.uConveybeltRedDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                            Playfield.uConveybeltRedState = EMERALD_CONVEYBELT_LEFT;
                            Playfield.uConveybeltRedDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                        } else {
                            Playfield.uConveybeltRedState = EMERALD_CONVEYBELT_RIGHT;
                            Playfield.uConveybeltRedDirection = EMERALD_CONVEYBELT_TO_LEFT;
                        }
                        break;
                }
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_CONVEYORBELT_SWITCH_GREEN):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                switch (Playfield.uConveybeltGreenState) {
                    case (EMERALD_CONVEYBELT_LEFT):
                    case (EMERALD_CONVEYBELT_RIGHT):
                        Playfield.uConveybeltGreenState = EMERALD_CONVEYBELT_OFF;
                        break;
                    case (EMERALD_CONVEYBELT_OFF):
                        if (Playfield.uConveybeltGreenDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                            Playfield.uConveybeltGreenState = EMERALD_CONVEYBELT_LEFT;
                            Playfield.uConveybeltGreenDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                        } else {
                            Playfield.uConveybeltGreenState = EMERALD_CONVEYBELT_RIGHT;
                            Playfield.uConveybeltGreenDirection = EMERALD_CONVEYBELT_TO_LEFT;
                        }
                        break;
                }
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_CONVEYORBELT_SWITCH_BLUE):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                switch (Playfield.uConveybeltBlueState) {
                    case (EMERALD_CONVEYBELT_LEFT):
                    case (EMERALD_CONVEYBELT_RIGHT):
                        Playfield.uConveybeltBlueState = EMERALD_CONVEYBELT_OFF;
                        break;
                    case (EMERALD_CONVEYBELT_OFF):
                        if (Playfield.uConveybeltBlueDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                            Playfield.uConveybeltBlueState = EMERALD_CONVEYBELT_LEFT;
                            Playfield.uConveybeltBlueDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                        } else {
                            Playfield.uConveybeltBlueState = EMERALD_CONVEYBELT_RIGHT;
                            Playfield.uConveybeltBlueDirection = EMERALD_CONVEYBELT_TO_LEFT;
                        }
                        break;
                }
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_CONVEYORBELT_SWITCH_YELLOW):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                switch (Playfield.uConveybeltYellowState) {
                    case (EMERALD_CONVEYBELT_LEFT):
                    case (EMERALD_CONVEYBELT_RIGHT):
                        Playfield.uConveybeltYellowState = EMERALD_CONVEYBELT_OFF;
                        break;
                    case (EMERALD_CONVEYBELT_OFF):
                        if (Playfield.uConveybeltYellowDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                            Playfield.uConveybeltYellowState = EMERALD_CONVEYBELT_LEFT;
                            Playfield.uConveybeltYellowDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                        } else {
                            Playfield.uConveybeltYellowState = EMERALD_CONVEYBELT_RIGHT;
                            Playfield.uConveybeltYellowDirection = EMERALD_CONVEYBELT_TO_LEFT;
                        }
                        break;
                }
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_LIGHT_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                Playfield.uTimeLightLeft = Playfield.uTimeLight;
                if (Playfield.uTimeLight > 0) {
                    Playfield.bLightOn = true;
                }
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_UP):
            Playfield.bSwitchRemoteBombUp = true;
            SetManArm(uActPos,uAnimation);
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_DOWN):
            Playfield.bSwitchRemoteBombDown = true;
            SetManArm(uActPos,uAnimation);
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_LEFT):
            Playfield.bSwitchRemoteBombLeft = true;
            SetManArm(uActPos,uAnimation);
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_RIGHT):
            Playfield.bSwitchRemoteBombRight = true;
            SetManArm(uActPos,uAnimation);
            break;
        case (EMERALD_SWITCH_REMOTEBOMB_IGNITION):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                Playfield.bSwitchRemoteBombIgnition = true;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_SWITCH_SWITCHDOOR):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                Playfield.bSwitchDoorImpluse = true;
                Playfield.bSwitchDoorState = !Playfield.bSwitchDoorState;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_MAGIC_WALL_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                if (!Playfield.bMagicWallRunning) {
                    Playfield.bMagicWallWasOn = false;  // Magic Wall zurücksetzen
                }
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_LIGHTBARRIER_RED_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                Playfield.bLightBarrierRedOn = !Playfield.bLightBarrierRedOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_LIGHTBARRIER_YELLOW_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                Playfield.bLightBarrierYellowOn = !Playfield.bLightBarrierYellowOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_LIGHTBARRIER_GREEN_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                Playfield.bLightBarrierGreenOn = !Playfield.bLightBarrierGreenOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_LIGHTBARRIER_BLUE_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                Playfield.bLightBarrierBlueOn = !Playfield.bLightBarrierBlueOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_REPLICATOR_RED_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                Playfield.bReplicatorRedOn = !Playfield.bReplicatorRedOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_REPLICATOR_YELLOW_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                Playfield.bReplicatorYellowOn = !Playfield.bReplicatorYellowOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_REPLICATOR_GREEN_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                Playfield.bReplicatorGreenOn = !Playfield.bReplicatorGreenOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_REPLICATOR_BLUE_SWITCH):
            if (ManKey.uLastSwitchFrameCount != ManKey.uLastDirectionFrameCount) {  // Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird
                Playfield.bReplicatorBlueOn = !Playfield.bReplicatorBlueOn;
                ManKey.uLastSwitchFrameCount = ManKey.uLastDirectionFrameCount;
                SetManArm(uActPos,uAnimation);
                PreparePlaySound(SOUND_SWITCH,uTouchPos);
            }
            break;
        case (EMERALD_DOOR_ONLY_UP_STEEL):
            if ((uAnimation == EMERALD_ANIM_UP) && (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) && (!ManKey.bFire)) {
                uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
            }
            break;
        case (EMERALD_DOOR_ONLY_DOWN_STEEL):
            if ((uAnimation == EMERALD_ANIM_DOWN) && (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) && (!ManKey.bFire)) {
                uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
            }
            break;
        case (EMERALD_DOOR_ONLY_LEFT_STEEL):
            if ((uAnimation == EMERALD_ANIM_LEFT) && (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) && (!ManKey.bFire)) {
                uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
            }
            break;
        case (EMERALD_DOOR_ONLY_RIGHT_STEEL):
            if ((uAnimation == EMERALD_ANIM_RIGHT) && (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) && (!ManKey.bFire)) {
                uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
            }
            break;
        case (EMERALD_DOOR_MULTICOLOR):
            if (((Playfield.bHasRedKey) || (Playfield.bHasYellowKey) || (Playfield.bHasGreenKey) || (Playfield.bHasBlueKey)) && (!ManKey.bFire)) {
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
                    if ((Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) && (!ManKey.bFire)) {
                        uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                        ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        if (Playfield.uEmeraldsToCollect > 0) {
                            Playfield.uEmeraldsToCollect++;
                        }
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if ((Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) && (!ManKey.bFire)) {
                        uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                        ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        if (Playfield.uEmeraldsToCollect > 0) {
                            Playfield.uEmeraldsToCollect++;
                        }
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if ((Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) && (!ManKey.bFire)) {
                        uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                        ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        if (Playfield.uEmeraldsToCollect > 0) {
                            Playfield.uEmeraldsToCollect++;
                        }
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if ((Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) && (!ManKey.bFire)) {
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
            if ((Playfield.bTimeDoorOpen) && (!ManKey.bFire)) {
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
            if (!ManKey.bFire) {
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
        case (EMERALD_DOOR_WHITE):
        case (EMERALD_DOOR_WHITE_WOOD):
        case (EMERALD_DOOR_GREY_WHITE):
            if (((Playfield.bHasGeneralKey) || (Playfield.uWhiteKeyCount > 0)) && (!ManKey.bFire)) {
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
            if ((Playfield.bHasRedKey) && (!ManKey.bFire)) {
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
            if ((Playfield.bHasGreenKey) && (!ManKey.bFire)) {
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
            if ((Playfield.bHasBlueKey) && (!ManKey.bFire)) {
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
            if ((Playfield.bHasYellowKey) && (!ManKey.bFire)) {
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
        case (EMERALD_TELEPORTER_RED):
        case (EMERALD_TELEPORTER_YELLOW):
        case (EMERALD_TELEPORTER_GREEN):
        case (EMERALD_TELEPORTER_BLUE):
            if (!ManKey.bFire) {
                switch (uAnimation) {
                    case (EMERALD_ANIM_UP):
                        uTeleporterCoordinate = GetDestinationTeleporterCoordinate(uActPos - Playfield.uLevel_X_Dimension,uAnimation);
                        if (uTeleporterCoordinate != EMERALD_INVALID_TELEPORTER_COORDINATE) {
                            ManGoTeleporter(uActPos,uTeleporterCoordinate,uAnimation);
                        } else {
                            Playfield.pStatusAnimation[uActPos] = EMERALD_ANIM_UP | EMERALD_ANIM_MAN_BLOCKED_UP;
                        }
                        break;
                    case (EMERALD_ANIM_DOWN):
                        uTeleporterCoordinate = GetDestinationTeleporterCoordinate(uActPos + Playfield.uLevel_X_Dimension,uAnimation);
                        if (uTeleporterCoordinate != EMERALD_INVALID_TELEPORTER_COORDINATE) {
                            ManGoTeleporter(uActPos,uTeleporterCoordinate,uAnimation);
                        } else {
                            Playfield.pStatusAnimation[uActPos] = EMERALD_ANIM_DOWN | EMERALD_ANIM_MAN_BLOCKED_DOWN;
                        }
                        break;
                    case (EMERALD_ANIM_LEFT):
                        uTeleporterCoordinate = GetDestinationTeleporterCoordinate(uActPos - 1,uAnimation);
                        if (uTeleporterCoordinate != EMERALD_INVALID_TELEPORTER_COORDINATE) {
                            ManGoTeleporter(uActPos,uTeleporterCoordinate,uAnimation);
                        } else {
                            Playfield.pStatusAnimation[uActPos] = EMERALD_ANIM_LEFT | EMERALD_ANIM_MAN_BLOCKED_LEFT;
                        }
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        uTeleporterCoordinate = GetDestinationTeleporterCoordinate(uActPos + 1,uAnimation);
                        if (uTeleporterCoordinate != EMERALD_INVALID_TELEPORTER_COORDINATE) {
                            ManGoTeleporter(uActPos,uTeleporterCoordinate,uAnimation);
                        } else {
                            Playfield.pStatusAnimation[uActPos] = EMERALD_ANIM_RIGHT | EMERALD_ANIM_MAN_BLOCKED_RIGHT;
                        }
                        break;
                }
            }
            break;
        case (EMERALD_KEY_GREEN):
            Playfield.bHasGreenKey = true;
            Playfield.uTotalScore = Playfield.uTotalScore + Playfield.uScoreKey;
            ManKey.uFireCount = 0;
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
            ManKey.uFireCount = 0;
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
            ManKey.uFireCount = 0;
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
            ManKey.uFireCount = 0;
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
            ManKey.uFireCount = 0;
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
            ManKey.uFireCount = 0;
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
            ManKey.uFireCount = 0;
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
            ManKey.uFireCount = 0;
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
            ManKey.uFireCount = 0;
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
            Playfield.pStatusAnimation[uTouchPos] = 0x00;       // Entsprechender Sand-Rand-Status muss gelöscht werden
            ManKey.uFireCount = 0;
            PreparePlaySound(SOUND_DIG_SAND,uTouchPos);
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_SAND_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_SAND_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_SAND_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_SAND_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_SAND_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_SAND_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_SAND_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_SAND_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_SAND_INVISIBLE):
            Playfield.pStatusAnimation[uTouchPos] = 0x00;       // Entsprechender Sand-Rand-Status muss gelöscht werden
            ManKey.uFireCount = 0;
            PreparePlaySound(SOUND_DIG_SAND,uTouchPos);
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_SAND_INVISIBLE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_SAND_INVISIBLE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_SAND_INVISIBLE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_SAND_INVISIBLE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_SAND_INVISIBLE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_SAND_INVISIBLE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_SAND_INVISIBLE_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_SAND_INVISIBLE_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
            }
            break;
        case (EMERALD_SANDMINE):
            if ((uAnimation != EMERALD_ANIM_STAND) && (ManKey.bFire)) { // Man "schnippt" Sandmine weg
                ManKey.uFireCount = 0;
                SetManArm(uActPos,uAnimation);
                Playfield.pLevel[uTouchPos] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                Playfield.pStatusAnimation[uTouchPos] = EMERALD_SPACE;
            } else {                                                    // Man läuft auf Sandmine und löst Explosion aus
                ControlCentralExplosion(uTouchPos);
            }
            PreparePlaySound(SOUND_EXPLOSION,uTouchPos);
            break;
        case (EMERALD_GRASS):
        case (EMERALD_GRASS_COMES):
            Playfield.pStatusAnimation[uTouchPos] = 0x00;       // Entsprechender Gras-Rand-Status muss gelöscht werden
            ManKey.uFireCount = 0;
            PreparePlaySound(SOUND_DIG_SAND,uTouchPos);
            switch (uAnimation) {
                case (EMERALD_ANIM_UP):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_GRASS_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoUp(uActPos,EMERALD_ANIM_GRASS_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_DOWN):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_GRASS_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoDown(uActPos,EMERALD_ANIM_GRASS_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_LEFT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_GRASS_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoLeft(uActPos,EMERALD_ANIM_GRASS_SHRINK,EMERALD_STANDARD_SPEED);
                        uRetAnimation = uAnimation;
                    }
                    break;
                case (EMERALD_ANIM_RIGHT):
                    if (ManKey.bFire) {
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ANIM_GRASS_SHRINK;
                        SetManArm(uActPos,uAnimation);
                    } else {
                        ManGoRight(uActPos,EMERALD_ANIM_GRASS_SHRINK,EMERALD_STANDARD_SPEED);
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
            ManKey.uFireCount = 0;
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
            ManKey.uFireCount = 0;
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
            ManKey.uFireCount = 0;
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
            ManKey.uFireCount = 0;
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
            ManKey.uFireCount = 0;
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
            ManKey.uFireCount = 0;
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
        case (EMERALD_WALL_GROW_LEFT):
        case (EMERALD_WALL_GROW_RIGHT):
        case (EMERALD_WALL_GROW_UP):
        case (EMERALD_WALL_GROW_DOWN):
        case (EMERALD_WALL_GROW_LEFT_RIGHT):
        case (EMERALD_WALL_GROW_UP_DOWN):
        case (EMERALD_WALL_GROW_ALL):
        case (EMERALD_WALL_ROUND):
        case (EMERALD_WALL_ROUND_PIKE):
        case (EMERALD_WALL_NOT_ROUND):
        case (EMERALD_MAGIC_WALL):
        case (EMERALD_WALL_INVISIBLE):
        case (EMERALD_WALL_WITH_KEY_RED):
        case (EMERALD_WALL_WITH_KEY_GREEN):
        case (EMERALD_WALL_WITH_KEY_BLUE):
        case (EMERALD_WALL_WITH_KEY_YELLOW):
        case (EMERALD_WALL_WITH_KEY_WHITE):
        case (EMERALD_WALL_WITH_KEY_GENERAL):
        case (EMERALD_WALL_WITH_EMERALD):
        case (EMERALD_WALL_WITH_RUBY):
        case (EMERALD_WALL_WITH_SAPPHIRE):
        case (EMERALD_WALL_WITH_PERL):
        case (EMERALD_WALL_WITH_CRYSTAL):
        case (EMERALD_WALL_WITH_BOMB):
        case (EMERALD_WALL_WITH_MEGABOMB):
        case (EMERALD_WALL_WITH_STONE):
        case (EMERALD_WALL_WITH_NUT):
        case (EMERALD_WALL_WITH_WHEEL):
        case (EMERALD_WALL_WITH_DYNAMITE):
        case (EMERALD_WALL_WITH_ENDDOOR):
        case (EMERALD_WALL_WITH_ENDDOOR_READY):
        case (EMERALD_WALL_WITH_MINE_UP):
        case (EMERALD_WALL_WITH_MOLE_UP):
        case (EMERALD_WALL_WITH_GREEN_CHEESE):
        case (EMERALD_WALL_WITH_BEETLE_UP):
        case (EMERALD_WALL_WITH_YAM):
        case (EMERALD_WALL_WITH_ALIEN):
        case (EMERALD_WALL_WITH_TIME_COIN):
        case (EMERALD_DOOR_ONLY_UP_WALL):
        case (EMERALD_DOOR_ONLY_DOWN_WALL):
        case (EMERALD_DOOR_ONLY_LEFT_WALL):
        case (EMERALD_DOOR_ONLY_RIGHT_WALL):
            if ((uAnimation != EMERALD_ANIM_STAND) && (ManKey.bFire) && (Playfield.uHammerCount > 0)) {
                Playfield.uHammerCount--;
                ManKey.uFireCount = 0;
                SetManArm(uActPos,uAnimation);
                Playfield.pLevel[uTouchPos] = EMERALD_EXPLOSION_TO_ELEMENT_1;
                switch (uElement) {
                    case (EMERALD_WALL_WITH_TIME_COIN):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_TIME_COIN;
                        break;
                    case (EMERALD_WALL_WITH_KEY_RED):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_KEY_RED;
                        break;
                    case (EMERALD_WALL_WITH_KEY_GREEN):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_KEY_GREEN;
                        break;
                    case (EMERALD_WALL_WITH_KEY_BLUE):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_KEY_BLUE;
                        break;
                    case (EMERALD_WALL_WITH_KEY_YELLOW):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_KEY_YELLOW;
                        break;
                    case (EMERALD_WALL_WITH_KEY_WHITE):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_KEY_WHITE;
                        break;
                    case (EMERALD_WALL_WITH_KEY_GENERAL):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_KEY_GENERAL;
                        break;
                    case (EMERALD_WALL_WITH_EMERALD):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_EMERALD;
                        break;
                    case (EMERALD_WALL_WITH_RUBY):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_RUBY;
                        break;
                    case (EMERALD_WALL_WITH_SAPPHIRE):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_SAPPHIRE;
                        break;
                    case (EMERALD_WALL_WITH_PERL):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_PERL;
                        break;
                    case (EMERALD_WALL_WITH_CRYSTAL):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_CRYSTAL;
                        break;
                    case (EMERALD_WALL_WITH_BOMB):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_BOMB;
                        break;
                    case (EMERALD_WALL_WITH_MEGABOMB):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_MEGABOMB;
                        break;
                    case (EMERALD_WALL_WITH_STONE):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_STONE;
                        break;
                    case (EMERALD_WALL_WITH_NUT):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_NUT;
                        break;
                    case (EMERALD_WALL_WITH_WHEEL):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_WHEEL;
                        break;
                    case (EMERALD_WALL_WITH_DYNAMITE):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_DYNAMITE_OFF;
                        break;
                    case (EMERALD_WALL_WITH_ENDDOOR):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_DOOR_END_NOT_READY;
                        break;
                    case (EMERALD_WALL_WITH_ENDDOOR_READY):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_DOOR_END_READY;
                        break;
                    case (EMERALD_WALL_WITH_MINE_UP):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_MINE_UP;
                        break;
                    case (EMERALD_WALL_WITH_MOLE_UP):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_MOLE_UP;
                        break;
                    case (EMERALD_WALL_WITH_GREEN_CHEESE):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_GREEN_DROP;
                        break;
                    case (EMERALD_WALL_WITH_BEETLE_UP):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_BEETLE_UP;
                        break;
                    case (EMERALD_WALL_WITH_YAM):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_YAM;
                        break;
                    case (EMERALD_WALL_WITH_ALIEN):
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_ALIEN;
                        break;
                    default:
                        Playfield.pStatusAnimation[uTouchPos] = EMERALD_SPACE;
                }
                PreparePlaySound(SOUND_EXPLOSION,uTouchPos);
            } else {
                // Einbahnstraßentüren (soft) können durchlaufen und mit Hammer gesprengt werden, daher dieser Extrazweig
                switch (uElement) {
                    case (EMERALD_DOOR_ONLY_UP_WALL):
                        if ((uAnimation == EMERALD_ANIM_UP) && (Playfield.pLevel[uActPos - 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) && (!ManKey.bFire)) {
                            uRetAnimation = EMERALD_ANIM_UP_DOUBLESPEED;
                            ManGoUp(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_DOOR_ONLY_DOWN_WALL):
                        if ((uAnimation == EMERALD_ANIM_DOWN) && (Playfield.pLevel[uActPos + 2 * Playfield.uLevel_X_Dimension] == EMERALD_SPACE) && (!ManKey.bFire)) {
                            uRetAnimation = EMERALD_ANIM_DOWN_DOUBLESPEED;
                            ManGoDown(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_DOOR_ONLY_LEFT_WALL):
                        if ((uAnimation == EMERALD_ANIM_LEFT) && (Playfield.pLevel[uActPos - 2] == EMERALD_SPACE) && (!ManKey.bFire)) {
                            uRetAnimation = EMERALD_ANIM_LEFT_DOUBLESPEED;
                            ManGoLeft(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                    case (EMERALD_DOOR_ONLY_RIGHT_WALL):
                        if ((uAnimation == EMERALD_ANIM_RIGHT) && (Playfield.pLevel[uActPos + 2] == EMERALD_SPACE) && (!ManKey.bFire)) {
                            uRetAnimation = EMERALD_ANIM_RIGHT_DOUBLESPEED;
                            ManGoRight(uActPos,EMERALD_NO_ADDITIONAL_ANIMSTATUS,EMERALD_DOUBLE_SPEED);
                        }
                        break;
                }
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
        case (EMERALD_DOOR_END_READY):
        case (EMERALD_DOOR_END_READY_STEEL):
            if (!ManKey.bFire) {
                switch (uAnimation) {
                    case (EMERALD_ANIM_UP):
                        SDL_Log("Man reach enddoor -> go up");
                        Playfield.pStatusAnimation[uActPos - Playfield.uLevel_X_Dimension] = EMERALD_ANIM_UP | EMERALD_ANIM_MAN_GOES_ENDDOOR | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                        break;
                    case (EMERALD_ANIM_DOWN):
                        SDL_Log("Man reach enddoor -> go down");
                        Playfield.pStatusAnimation[uActPos + Playfield.uLevel_X_Dimension] = EMERALD_ANIM_DOWN | EMERALD_ANIM_MAN_GOES_ENDDOOR | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                        break;
                    case (EMERALD_ANIM_LEFT):
                        SDL_Log("Man reach enddoor -> go left");
                        Playfield.pStatusAnimation[uActPos - 1] = EMERALD_ANIM_LEFT | EMERALD_ANIM_MAN_GOES_ENDDOOR | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                        break;
                    case (EMERALD_ANIM_RIGHT):
                        SDL_Log("Man reach enddoor -> go right");
                        Playfield.pStatusAnimation[uActPos + 1] = EMERALD_ANIM_RIGHT | EMERALD_ANIM_MAN_GOES_ENDDOOR | EMERALD_ANIM_AVOID_DOUBLE_CONTROL;
                        break;
                }
                Playfield.pLevel[uActPos] = EMERALD_SPACE;  // Man in Space wandeln
                PreparePlaySound(SOUND_ENDDOOR,uActPos);
                Playfield.bWellDone = true;
                // Zusätzlicher Score: (Restzeit * Timefaktor) / 10: siehe DC3
                Playfield.uTotalScore = Playfield.uTotalScore + (Playfield.uTimeToPlay * Playfield.uTimeScoreFactor) / (ge_DisplayMode.refresh_rate * 10);
                // Falls Score > 9999, wird das in ShowPanel() korrigiert
            }
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
               uAdditionalAnimStatus, uint32_t, zusätzlicher Animations-Status
               bDoubleSpeed, bool, true = Man läuft doppelte Geschwindigkeit
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ManGoDown(uint32_t I, uint32_t uAdditionalAnimStatus, bool bDoubleSpeed) {
    // ursprüngliche Man-Position mit Space besetzen
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
               uAdditionalAnimStatus, uint32_t, zusätzlicher Animations-Status
               bDoubleSpeed, bool, true = Man läuft doppelte Geschwindigkeit
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ManGoUp(uint32_t I, uint32_t uAdditionalAnimStatus, bool bDoubleSpeed) {
    // ursprüngliche Man-Position mit Space besetzen
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
               uAdditionalAnimStatus, uint32_t, zusätzlicher Animations-Status
               bDoubleSpeed, bool, true = Man läuft doppelte Geschwindigkeit
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ManGoRight(uint32_t I, uint32_t uAdditionalAnimStatus, bool bDoubleSpeed) {
    // ursprüngliche Man-Position mit Space besetzen
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
               uAdditionalAnimStatus, uint32_t, zusätzlicher Animations-Status
               bDoubleSpeed, bool, true = Man läuft doppelte Geschwindigkeit
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ManGoLeft(uint32_t I, uint32_t uAdditionalAnimStatus, bool bDoubleSpeed) {
    // ursprüngliche Man-Position mit Space besetzen
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
    if (Playfield.uTimeWheelRotation > 0) {
        Playfield.bWheelRunning = true;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_WHEEL_RUN;             // Rad animieren
        Playfield.uWheelRunningXpos = I % Playfield.uLevel_X_Dimension;     // X- und
        Playfield.uWheelRunningYpos = I / Playfield.uLevel_X_Dimension;     // Y-Koordinate des (neuen) laufenden Rades eintragen
        Playfield.uTimeWheelRotationLeft = Playfield.uTimeWheelRotation;    // Zeit neu setzen
    }
}


/*----------------------------------------------------------------------------
Name:           CheckRunningWheel
------------------------------------------------------------------------------
Beschreibung: Steuert laufendes Rad.
              Wird aus EmeraldMine.c/RunGame() aufgerufen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CheckRunningWheel(void) {
    uint32_t uWheelposRunning;              // lineare Kooridnate des laufenden Rades

    if (Playfield.bWheelRunning) {          // Läuft bereits ein Rad ?
        if (Playfield.uTimeWheelRotationLeft > 0) {
            Playfield.uTimeWheelRotationLeft--;
        }
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
              betätigt, wird hier die "richtige" Armbewegung gesetzt.
Parameter
      Eingang: I, uint32_t, aktuelle Position des Man
      Ausgang: -
Rückgabewert:  -
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


/*----------------------------------------------------------------------------
Name:           ControlEnddoorReady
------------------------------------------------------------------------------
Beschreibung: Steuert die blinkende Endtür, wenn der Man hineinläuft
Parameter
      Eingang: I, uint32_t, aktuelle Position der Tür.
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlEnddoorReady(uint32_t I) {
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        SDL_Log("%s: ack double control",__FUNCTION__);
        return;
    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_DOOR_READY_SHRINK) {
        Playfield.pLevel[I] = EMERALD_SPACE;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    }

    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MAN_GOES_ENDDOOR) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOOR_READY_SHRINK;
    }
}


/*----------------------------------------------------------------------------
Name:           ControlEnddoorReadySteel
------------------------------------------------------------------------------
Beschreibung: Steuert die blinkende Stahl-Endtür, wenn der Man hineinläuft
Parameter
      Eingang: I, uint32_t, aktuelle Position der Tür.
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ControlEnddoorReadySteel(uint32_t I) {
    // Doppelte Steuerung vermeiden
    if ((Playfield.pStatusAnimation[I] & 0x00FF0000) == EMERALD_ANIM_AVOID_DOUBLE_CONTROL) {
        Playfield.pStatusAnimation[I] = Playfield.pStatusAnimation[I] & 0xFF00FFFF;
        SDL_Log("%s: ack double control",__FUNCTION__);
        return;
    } else if (Playfield.pStatusAnimation[I] == EMERALD_ANIM_DOOR_READY_SHRINK) {
        Playfield.pLevel[I] = EMERALD_STEEL_PLAYERHEAD;
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_STAND;
    }

    if ((Playfield.pStatusAnimation[I] & 0xFF000000) == EMERALD_ANIM_MAN_GOES_ENDDOOR) {
        Playfield.pStatusAnimation[I] = EMERALD_ANIM_DOOR_READY_SHRINK;
    }
}


/*----------------------------------------------------------------------------
Name:           ManGoTeleporter
------------------------------------------------------------------------------
Beschreibung: Man geht durch einen Teleporter
Parameter
      Eingang: uActCoordinate, uint32_t, aktuelle lineare Koordinate des Mans
               uDestTeleporterCoordinate, uint32_t, lineare Koordinate des Ziel-Teleporters
               uAnimation, uint32_t, Richtung des Mans durch den Teleporter
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void ManGoTeleporter(uint32_t uActCoordinate, uint32_t uDestTeleporterCoordinate, uint32_t uAnimation) {
    bool bMoved = false;
    uint32_t uDestManCoordinate;

    switch (uAnimation) {
        case (EMERALD_ANIM_UP):
            // ursprüngliche Man-Position mit Space besetzen
            Playfield.pLevel[uActCoordinate] = EMERALD_SPACE;
            Playfield.pStatusAnimation[uActCoordinate] = EMERALD_ANIM_STAND;
            uDestManCoordinate = uDestTeleporterCoordinate - Playfield.uLevel_X_Dimension;
            bMoved = true;
            break;
        case (EMERALD_ANIM_DOWN):
            // ursprüngliche Man-Position mit Space besetzen
            Playfield.pLevel[uActCoordinate] = EMERALD_SPACE;
            Playfield.pStatusAnimation[uActCoordinate] = EMERALD_ANIM_STAND;
            uDestManCoordinate = uDestTeleporterCoordinate + Playfield.uLevel_X_Dimension;
            bMoved = true;
            break;
        case (EMERALD_ANIM_LEFT):
            // ursprüngliche Man-Position mit Space besetzen
            Playfield.pLevel[uActCoordinate] = EMERALD_SPACE;
            Playfield.pStatusAnimation[uActCoordinate] = EMERALD_ANIM_STAND;
            uDestManCoordinate = uDestTeleporterCoordinate - 1;
            bMoved = true;
            break;
        case (EMERALD_ANIM_RIGHT):
            // ursprüngliche Man-Position mit Space besetzen
            Playfield.pLevel[uActCoordinate] = EMERALD_SPACE;
            Playfield.pStatusAnimation[uActCoordinate] = EMERALD_ANIM_STAND;
            uDestManCoordinate = uDestTeleporterCoordinate + 1;
            bMoved = true;
            break;
    }
    if (bMoved) {
        Playfield.uManXpos = uDestManCoordinate % Playfield.uLevel_X_Dimension;
        Playfield.uManYpos = uDestManCoordinate / Playfield.uLevel_X_Dimension;
        // Man auf neue Position setzen
        Playfield.pLevel[uActCoordinate] = EMERALD_SPACE;
        Playfield.pLevel[uDestManCoordinate] = EMERALD_MAN;
        Playfield.pStatusAnimation[uDestManCoordinate] = uAnimation;
        // Neue Position im Level berechnen
        if (Playfield.uManXpos >= Playfield.uVisibleCenterX) {
            Playfield.nTopLeftXpos = (Playfield.uManXpos - Playfield.uVisibleCenterX) * FONT_W;
        } else {
            Playfield.nTopLeftXpos = 0;
        }
        if (Playfield.uManYpos >= Playfield.uVisibleCenterY) {
            Playfield.nTopLeftYpos = (Playfield.uManYpos - Playfield.uVisibleCenterY) * FONT_H;
        } else {
            Playfield.nTopLeftYpos = 0;
        }
        PreparePlaySound(SOUND_TELEPORTER,uDestManCoordinate);
    }
}
