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
    int nMouseXpos_Absolute;        // Absolute Maus X-Position
    int nMouseYpos_Absolute;        // Absolute Maus Y-Position
    int nMouseXpos_Relative;        // Relative Maus X-Position
    int nMouseYpos_Relative;        // Relative Maus Y-Position
    bool bQuit;                     // Fensterkreuz gedrückt
} INPUTSTATES;


int InitInputStates(void);
void UpdateInputStates(void);
uint32_t GetKey(void);
uint32_t FilterBigFontKey(uint32_t uKey);
void WaitNoSpecialKey(SDL_Scancode ScanCode);
void WaitNoKey(void);
#endif // KEYBOARDMOUSE_H_INCLUDED
