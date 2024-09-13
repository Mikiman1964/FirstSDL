#ifndef GAMECONTROLLER_H_INCLUDED
#define GAMECONTROLLER_H_INCLUDED

#include <stdbool.h>
#include "SDL2/SDL_gamecontroller.h"
#include "SDL2/SDL_joystick.h"


#define JOYSTICK_NAME                   "COMPETITION PRO"

#define MAX_JOYSTICKS                   3
#define MAX_GAMECONTROLLERS             3

typedef struct {
    bool bFire;
    bool bDynamite;
    bool bExit;
    bool bLeft;
    bool bRight;
    bool bUp;
    bool bDown;
} MAN_DIRECTIONS;


typedef struct {
    SDL_GameController *pActualGameControllerIdentifier;
    bool bButtonX;
    bool bButtonY;
    bool bButtonA;
    bool bButtonB;
    bool bDigitalPadLeft;
    bool bDigitalPadRight;
    bool bDigitalPadUp;
    bool bDigitalPadDown;
    bool bLeftShoulder;
    bool bRightShoulder;
    bool bBurger;
    bool bGuide;            // beleuchteter Button (X)
    bool bBack;
    bool bLeftStick;
    bool bRightStick;
    int16_t nLeftAxisX;
    int16_t nLeftAxisY;
    int16_t nRightAxisX;
    int16_t nRightAxisY;
    int16_t nTriggerLeft;
    int16_t nTriggerRight;
} GAMECONTROLLER_VALUES;


typedef struct {
    SDL_Joystick *pActualJoystickIdentifier;
    bool bJoystickAsGameController;   // Joystick, der als GameController angesteuert wird (unter Linux)
    bool bLeft;
    bool bRight;
    bool bUp;
    bool bDown;
    bool bButtonX;  // Linker großer runder Button
    bool bButtonY;  // Linker dreieckiger Button
    bool bButtonA;  // Rechter großer runder Button
    bool bButtonB;  // Rechter dreieckiger Button
} JOYSTICK_VALUES;


typedef struct {
    uint32_t uLastDetectTime;
    int nCount;
    int nActiveGameControllerId;
    int ID[MAX_GAMECONTROLLERS];
    GAMECONTROLLER_VALUES Value[MAX_GAMECONTROLLERS];
} GAMECONTROLLER;


typedef struct {
    uint32_t uLastDetectTime;
    int nCount;
    int nActiveJoystickId;
    int ID[MAX_JOYSTICKS];
    JOYSTICK_VALUES Value[MAX_JOYSTICKS];
} JOYSTICK;

void DetectJoystickAndGameController();
int OpenJoystickOrGameController(void);
int GetJoystickOrGameControllerStatus(MAN_DIRECTIONS *pManDirections);
bool IsGameController(int nJoystickIndex, bool *pbJoystickAsGameController);
void CloseJoystickOrGameController(void);
void ShowGameControllerButtons(void);

#endif // GAMECONTROLLER_H_INCLUDED
