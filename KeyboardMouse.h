#ifndef KEYBOARDMOUSE_H_INCLUDED
#define KEYBOARDMOUSE_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    uint32_t uLastKey0;             // UTF-8
    uint32_t uLastKey1;             // UTF-8
    const Uint8 *pKeyboardArray;
    bool bLeftMouseButton;
    bool bMidMouseButton;
    bool bRightMouseButton;
    int nMouseWheelY;               // Y-Mausrad, < 0 = Rad scroll down, > 0 = Rad scroll up
    int nMouseXpos;                 // Maus X-Position
    int nMouseYpos;                 // Maus Y-Position
    bool bQuit;                     // Fensterkreuz gedr�ckt
} INPUTSTATES;


int InitInputStates(void);
void UpdateInputStates(void);
uint32_t GetKey(void);
void WaitNoSpecialKey(SDL_Scancode ScanCode);
void WaitNoKey(void);
#endif // KEYBOARDMOUSE_H_INCLUDED
