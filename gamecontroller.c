#include <SDL2/SDL.h>
#include "config.h"
#include "gamecontroller.h"
#include "SDL2/SDL_gamecontroller.h"

GAMECONTROLLER GameController;
JOYSTICK Joystick;
extern CONFIG Config;

/*----------------------------------------------------------------------------
Name:           DetectJoystickAndGameController
------------------------------------------------------------------------------
Beschreibung:   Erkennt angeschlossene Joysticks und Gamecontroller.
                Die Funktion füllt die Strukturen Joystick.x und GameController.x.
                Gefundene Joystick- und GameController-IDs werden in Joystick.ID[] und
                GameController.ID[] hinterlegt.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Joystick.x, GameController.x
------------------------------------------------------------------------------*/
void DetectJoystickAndGameController(void) {
    int I;
    int nStickCount;    // Das können Joysticks oder GameController sein

    memset(&GameController,0,sizeof(GameController));
    for (I = 0; I < MAX_GAMECONTROLLERS; I++) {
        GameController.ID[I] = -1;
    }
    GameController.nActiveGameControllerId = -1;
    memset(&Joystick,0,sizeof(Joystick));
    for (I = 0; I < MAX_JOYSTICKS; I++) {
        Joystick.ID[I] = -1;
    }
    nStickCount = SDL_NumJoysticks();
    for (I = 0; I < nStickCount; I++) {
        if (SDL_IsGameController(I)) {      // Stick ist ein Gamecontroller
            if (GameController.nCount < MAX_GAMECONTROLLERS) {
                GameController.ID[GameController.nCount] = I;
                GameController.nCount++;
            }
        } else {                            // Stick ist ein Joystick
            if (Joystick.nCount < MAX_JOYSTICKS) {
                Joystick.ID[Joystick.nCount] = I;
                Joystick.nCount++;
            }
        }
    }
    Joystick.nActiveJoystickId = -1;
    SDL_Log("%s: Input devices:   %d",__FUNCTION__,nStickCount);
    SDL_Log("%s: Joysticks:       %d",__FUNCTION__,Joystick.nCount);
    SDL_Log("%s: GameControllers: %d",__FUNCTION__,GameController.nCount);
    GameController.uLastDetectTime = SDL_GetTicks();
    Joystick.uLastDetectTime = SDL_GetTicks();
}


/*----------------------------------------------------------------------------
Name:           OpenJoystickOrGameController
------------------------------------------------------------------------------
Beschreibung:   Öffnet einen Joystick oder Gamecontroller in Abhängig der gespeicherten
                Einstellungen.
                Die Funktion füllt die Strukturen Joystick.x bzw. GameController.x. mit
                dem Gerätehandle.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = gewünschtes Gerät konnte geöffnet werden, sonst Fehler
Seiteneffekte: Joystick.x, GameController.x, Config.x
------------------------------------------------------------------------------*/
int OpenJoystickOrGameController(void) {
    int nErrorCode = -1;

    if (Config.uInputdevice == 1) {        // Joystick?
        if (Config.uDeviceIndex < MAX_JOYSTICKS) {
            if (Joystick.ID[Config.uDeviceIndex] != -1) {
                Joystick.nActiveJoystickId = Joystick.ID[Config.uDeviceIndex] ;
                Joystick.Value[Config.uDeviceIndex].pActualJoystickIdentifier = SDL_GameControllerOpen(Joystick.nActiveJoystickId);
                if (Joystick.Value[Config.uDeviceIndex].pActualJoystickIdentifier != NULL) {
                    SDL_Log("%s: joystick with device id: %d  opened",__FUNCTION__,Joystick.nActiveJoystickId);
                    nErrorCode = 0;
                } else {
                    SDL_Log("%s: can not open joystick with device id: %d, error: %s ",__FUNCTION__,Joystick.nActiveJoystickId,SDL_GetError());
                }
            } else {
                SDL_Log("%s: joystick with index %d is not available",__FUNCTION__,Config.uDeviceIndex);
            }
        } else {
            SDL_Log("%s: bad joystick device index %d",__FUNCTION__,Config.uDeviceIndex);
        }
    } else if (Config.uInputdevice == 2) { // Gamecontroller?
        if (Config.uDeviceIndex < MAX_GAMECONTROLLERS) {
            if (GameController.ID[Config.uDeviceIndex] != -1) {
                GameController.nActiveGameControllerId = GameController.ID[Config.uDeviceIndex] ;
                GameController.Value[Config.uDeviceIndex].pActualGameControllerIdentifier = SDL_GameControllerOpen(GameController.nActiveGameControllerId);
                if (GameController.Value[Config.uDeviceIndex].pActualGameControllerIdentifier != NULL) {
                    SDL_Log("%s: gamecontroller with device id: %d  opened",__FUNCTION__,GameController.nActiveGameControllerId);
                    nErrorCode = 0;
                } else {
                    SDL_Log("%s: can not open gamecontroller with device id: %d, error: %s ",__FUNCTION__,GameController.nActiveGameControllerId,SDL_GetError());
                }
            } else {
                SDL_Log("%s: gamecontroller with index %d is not available",__FUNCTION__,Config.uDeviceIndex);
            }
        } else {
            SDL_Log("%s: bad gamecontroller device index %d",__FUNCTION__,Config.uDeviceIndex);
        }
    } else {
        SDL_Log("%s: no device opened",__FUNCTION__);
        nErrorCode = 0;                    // Es soll kein Gerät geöffnet werden
    }
    return nErrorCode;
}



/*----------------------------------------------------------------------------
Name:           GetJoystickOrGameControllerStatus
------------------------------------------------------------------------------
Beschreibung:   Fragt einen Joystick oder Gamecontroller ab und legt die
                Informationen in den Strukturen Joystick.x bzw. GameController.x ab.
Parameter
      Eingang: -
      Ausgang: pManDirections, MAN_DIRECTIONS *, Richtungszustände für Man-Bewegung
Rückgabewert:  int, 0 = Statusabfrage OK, sonst Fehler bzw. keine Abfrage durchgeführt
Seiteneffekte: Joystick.x, GameController.x, Config.x
------------------------------------------------------------------------------*/
int GetJoystickOrGameControllerStatus(MAN_DIRECTIONS *pManDirections) {
    int nErrorCode = -1;
    SDL_GameController *pHandle;

    if (Config.uInputdevice == 1) {         // Joystick?
        pHandle = Joystick.Value[Config.uDeviceIndex].pActualJoystickIdentifier;

        // ************** TODO ************** //


        nErrorCode = -1;                    // funktioniert noch nicht, da kein Joystick zu Testzwecken verfügar
    } else if (Config.uInputdevice == 2) { // Gamecontroller?
        pHandle = GameController.Value[Config.uDeviceIndex].pActualGameControllerIdentifier;
        if (pHandle != NULL) {
            if (SDL_IsGameController(GameController.nActiveGameControllerId)) { // Wird false, wenn Controller im Betrieb abgezogen wird
                // Buttons und Digital Pad
                GameController.Value[Config.uDeviceIndex].bButtonA = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_A) == 1);
                GameController.Value[Config.uDeviceIndex].bButtonB = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_B) == 1);
                GameController.Value[Config.uDeviceIndex].bButtonX = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_X) == 1);
                GameController.Value[Config.uDeviceIndex].bButtonY = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_Y) == 1);
                GameController.Value[Config.uDeviceIndex].bDigitalPadLeft = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_DPAD_LEFT) == 1);
                GameController.Value[Config.uDeviceIndex].bDigitalPadRight = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == 1);
                GameController.Value[Config.uDeviceIndex].bDigitalPadUp = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_DPAD_UP) == 1);
                GameController.Value[Config.uDeviceIndex].bDigitalPadDown = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_DPAD_DOWN) == 1);
                GameController.Value[Config.uDeviceIndex].bLeftShoulder = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == 1);
                GameController.Value[Config.uDeviceIndex].bRightShoulder = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == 1);
                GameController.Value[Config.uDeviceIndex].bBurger = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_START) == 1);
                GameController.Value[Config.uDeviceIndex].bGuide = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_GUIDE) == 1);
                GameController.Value[Config.uDeviceIndex].bBack = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_BACK) == 1);
                GameController.Value[Config.uDeviceIndex].bLeftStick = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_LEFTSTICK) == 1);
                GameController.Value[Config.uDeviceIndex].bRightStick = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_RIGHTSTICK) == 1);
                // Achsen
                GameController.Value[Config.uDeviceIndex].nLeftAxisX = SDL_GameControllerGetAxis(pHandle,SDL_CONTROLLER_AXIS_LEFTX);
                GameController.Value[Config.uDeviceIndex].nLeftAxisY = SDL_GameControllerGetAxis(pHandle,SDL_CONTROLLER_AXIS_LEFTY);
                GameController.Value[Config.uDeviceIndex].nRightAxisX = SDL_GameControllerGetAxis(pHandle,SDL_CONTROLLER_AXIS_RIGHTX);
                GameController.Value[Config.uDeviceIndex].nRightAxisY = SDL_GameControllerGetAxis(pHandle,SDL_CONTROLLER_AXIS_RIGHTY);
                GameController.Value[Config.uDeviceIndex].nTriggerLeft = SDL_GameControllerGetAxis(pHandle,SDL_CONTROLLER_AXIS_TRIGGERLEFT);
                GameController.Value[Config.uDeviceIndex].nTriggerRight = SDL_GameControllerGetAxis(pHandle,SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
                /*
                SDL_Log("LAX:%d  LAY:%d  RAX:%d  RAY:%d  TL:%d  TR:%d",
                         GameController.Value[Config.uDeviceIndex].nLeftAxisX,
                         GameController.Value[Config.uDeviceIndex].nLeftAxisY,
                         GameController.Value[Config.uDeviceIndex].nRightAxisX,
                         GameController.Value[Config.uDeviceIndex].nRightAxisY,
                         GameController.Value[Config.uDeviceIndex].nTriggerLeft,
                         GameController.Value[Config.uDeviceIndex].nTriggerRight);
                ShowGameControllerButtons();
                */
                // Firebutton
                if (Config.cControllerFireButton == 'A') {
                    pManDirections->bFire = GameController.Value[Config.uDeviceIndex].bButtonA;
                } else if (Config.cControllerFireButton == 'B') {
                    pManDirections->bFire = GameController.Value[Config.uDeviceIndex].bButtonB;
                } else if (Config.cControllerFireButton == 'X') {
                    pManDirections->bFire = GameController.Value[Config.uDeviceIndex].bButtonX;
                } else if (Config.cControllerFireButton == 'Y') {
                    pManDirections->bFire = GameController.Value[Config.uDeviceIndex].bButtonY;
                } else {
                    pManDirections->bFire = GameController.Value[Config.uDeviceIndex].bButtonB;
                }
                // Dynamit-Button
                if (Config.cControllerStartDynamiteButton == 'A') {
                    pManDirections->bDynamite = GameController.Value[Config.uDeviceIndex].bButtonA;
                } else if (Config.cControllerStartDynamiteButton == 'B') {
                    pManDirections->bDynamite = GameController.Value[Config.uDeviceIndex].bButtonB;
                } else if (Config.cControllerStartDynamiteButton == 'X') {
                    pManDirections->bDynamite = GameController.Value[Config.uDeviceIndex].bButtonX;
                } else if (Config.cControllerStartDynamiteButton == 'Y') {
                    pManDirections->bDynamite = GameController.Value[Config.uDeviceIndex].bButtonY;
                } else {
                    pManDirections->bDynamite = GameController.Value[Config.uDeviceIndex].bButtonB;
                }
                // Exit-Button
                if (Config.cControllerExitButton == 'A') {
                    pManDirections->bExit = GameController.Value[Config.uDeviceIndex].bButtonA;
                } else if (Config.cControllerExitButton == 'B') {
                    pManDirections->bExit = GameController.Value[Config.uDeviceIndex].bButtonB;
                } else if (Config.cControllerExitButton == 'X') {
                    pManDirections->bExit = GameController.Value[Config.uDeviceIndex].bButtonX;
                } else if (Config.cControllerExitButton == 'Y') {
                    pManDirections->bExit = GameController.Value[Config.uDeviceIndex].bButtonY;
                } else {
                    pManDirections->bDynamite = GameController.Value[Config.uDeviceIndex].bButtonB;
                }
                if (Config.uControllerDirections == 0) {            // 0 = Digital Pad, 1 = Left Axis, 2 = Right Axis
                    pManDirections->bLeft = GameController.Value[Config.uDeviceIndex].bDigitalPadLeft;
                    pManDirections->bRight = GameController.Value[Config.uDeviceIndex].bDigitalPadRight;
                    pManDirections->bUp = GameController.Value[Config.uDeviceIndex].bDigitalPadUp;
                    pManDirections->bDown = GameController.Value[Config.uDeviceIndex].bDigitalPadDown;
                } else if (Config.uControllerDirections == 1) {       // 1 = Left Axis
                    pManDirections->bLeft = (GameController.Value[Config.uDeviceIndex].nLeftAxisX <= Config.nAxisLeftThreshold);
                    pManDirections->bRight = (GameController.Value[Config.uDeviceIndex].nLeftAxisX >= Config.nAxisRightThreshold);
                    pManDirections->bUp = (GameController.Value[Config.uDeviceIndex].nLeftAxisY <= Config.nAxisUpThreshold);
                    pManDirections->bDown = (GameController.Value[Config.uDeviceIndex].nLeftAxisY >= Config.nAxisDownThreshold);
                } else if (Config.uControllerDirections == 2) {     // 2 = Right Axis
                    pManDirections->bLeft = (GameController.Value[Config.uDeviceIndex].nRightAxisX <= Config.nAxisLeftThreshold);
                    pManDirections->bRight = (GameController.Value[Config.uDeviceIndex].nRightAxisX >= Config.nAxisRightThreshold);
                    pManDirections->bUp = (GameController.Value[Config.uDeviceIndex].nRightAxisY <= Config.nAxisUpThreshold);
                    pManDirections->bDown = (GameController.Value[Config.uDeviceIndex].nRightAxisY >= Config.nAxisDownThreshold);
                } else {
                    SDL_Log("%s: bad config for Config.uControllerDirections: %u",__FUNCTION__,Config.uControllerDirections);
                }
                nErrorCode = 0;
            } else {
                SDL_Log("%s: lost gamecontroller ... closing device handle",__FUNCTION__);
                CloseJoystickOrGameController();
            }
        } else {
            if (SDL_GetTicks() - GameController.uLastDetectTime > 10000) {
                DetectJoystickAndGameController();
                OpenJoystickOrGameController();
            }
        }
    }
    // else -> kein Gerät -> Rückgabe -1
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CloseJoystickOrGameController
------------------------------------------------------------------------------
Beschreibung:   Schließt das Geräthandle eines Joysticks oder Gamecontrollers.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = gewünschtes Gerät konnte geöffnet werden, sonst Fehler
Seiteneffekte: Joystick.x, GameController.x, Config.x
------------------------------------------------------------------------------*/
void CloseJoystickOrGameController(void) {
    SDL_GameController *pHandle = NULL;

    if (Config.uInputdevice == 1) {         // Joystick?
        pHandle = Joystick.Value[Config.uDeviceIndex].pActualJoystickIdentifier;
    } else if (Config.uInputdevice == 2) { // Gamecontroller?
        pHandle = GameController.Value[Config.uDeviceIndex].pActualGameControllerIdentifier;
    }
    if (pHandle != NULL) {
        SDL_GameControllerClose(pHandle);
    }
    Joystick.uLastDetectTime = SDL_GetTicks();
    GameController.uLastDetectTime = SDL_GetTicks();
    Joystick.Value[Config.uDeviceIndex].pActualJoystickIdentifier = NULL;
    GameController.Value[Config.uDeviceIndex].pActualGameControllerIdentifier = NULL;
}


/*----------------------------------------------------------------------------
Name:           ShowGameControllerButtons
------------------------------------------------------------------------------
Beschreibung:   Zeigt den Zustand der GameController-Buttons.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: GameController.x, Config.x
------------------------------------------------------------------------------*/
void ShowGameControllerButtons(void) {
    SDL_GameController *pHandle;

    if (Config.uInputdevice == 2) { // Gamecontroller?
        pHandle = GameController.Value[Config.uDeviceIndex].pActualGameControllerIdentifier;
        if (pHandle != NULL) {
            // Buttons und Digital Pad
            GameController.Value[Config.uDeviceIndex].bButtonA = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_A) == 1);
            GameController.Value[Config.uDeviceIndex].bButtonB = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_B) == 1);
            GameController.Value[Config.uDeviceIndex].bButtonX = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_X) == 1);
            GameController.Value[Config.uDeviceIndex].bButtonY = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_Y) == 1);
            GameController.Value[Config.uDeviceIndex].bDigitalPadLeft = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_DPAD_LEFT) == 1);
            GameController.Value[Config.uDeviceIndex].bDigitalPadRight = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == 1);
            GameController.Value[Config.uDeviceIndex].bDigitalPadUp = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_DPAD_UP) == 1);
            GameController.Value[Config.uDeviceIndex].bDigitalPadDown = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_DPAD_DOWN) == 1);
            GameController.Value[Config.uDeviceIndex].bLeftShoulder = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == 1);
            GameController.Value[Config.uDeviceIndex].bRightShoulder = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == 1);
            GameController.Value[Config.uDeviceIndex].bBurger = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_START) == 1);
            GameController.Value[Config.uDeviceIndex].bGuide = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_GUIDE) == 1);
            GameController.Value[Config.uDeviceIndex].bBack = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_BACK) == 1);
            GameController.Value[Config.uDeviceIndex].bLeftStick = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_LEFTSTICK) == 1);
            GameController.Value[Config.uDeviceIndex].bRightStick = (SDL_GameControllerGetButton(pHandle,SDL_CONTROLLER_BUTTON_RIGHTSTICK) == 1);
            SDL_Log("X:%d Y:%d A:%d B:%d L:%d R:%d U:%d D:%d LSH:%d RSH:%d BU:%d G:%d B:%d LS:%d RS:%d",
                GameController.Value[Config.uDeviceIndex].bButtonX,
                GameController.Value[Config.uDeviceIndex].bButtonY,
                GameController.Value[Config.uDeviceIndex].bButtonA,
                GameController.Value[Config.uDeviceIndex].bButtonB,
                GameController.Value[Config.uDeviceIndex].bDigitalPadLeft,
                GameController.Value[Config.uDeviceIndex].bDigitalPadRight,
                GameController.Value[Config.uDeviceIndex].bDigitalPadUp,
                GameController.Value[Config.uDeviceIndex].bDigitalPadDown,
                GameController.Value[Config.uDeviceIndex].bLeftShoulder,
                GameController.Value[Config.uDeviceIndex].bRightShoulder,
                GameController.Value[Config.uDeviceIndex].bBurger,
                GameController.Value[Config.uDeviceIndex].bGuide,
                GameController.Value[Config.uDeviceIndex].bBack,
                GameController.Value[Config.uDeviceIndex].bLeftStick,
                GameController.Value[Config.uDeviceIndex].bRightStick
                );
        } else {
            SDL_Log("%s: invalid handle",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: invalid input device: %u",__FUNCTION__,Config.uInputdevice);
    }
}

