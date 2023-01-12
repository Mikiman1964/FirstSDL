#ifndef KEYBOARDMOUSE_H_INCLUDED
#define KEYBOARDMOUSE_H_INCLUDED

#include <stdbool.h>

typedef struct {
    const Uint8 *pKeyboardArray;
    bool bLeftMouseButton;
    bool bMidMouseButton;
    bool bRightMouseButton;
    int nMouseWheelY;               // Y-Mausrad, < 0 = Rad scroll down, > 0 = Rad scroll up
    int nMouseXpos;                 // Maus X-Position
    int nMouseYpos;                 // Maus Y-Position
    bool bQuit;                     // Fensterkreuz gedrückt
} INPUTSTATES;


int InitInputStates(void);
int UpdateInputStates(void);
void WaitForKey(SDL_Scancode ScanCode);
void WaitNoKey(void);
#endif // KEYBOARDMOUSE_H_INCLUDED
