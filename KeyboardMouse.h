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
    int32_t nMouseWheelY;           // Y-Mausrad, < 0 = Rad scroll down, > 0 = Rad scroll up
    int32_t nMouseXpos_Absolute;    // Absolute Maus X-Position
    int32_t nMouseYpos_Absolute;    // Absolute Maus Y-Position
    int32_t nMouseXpos_Relative;    // Relative Maus X-Position
    int32_t nMouseYpos_Relative;    // Relative Maus Y-Position
    bool bQuit;                     // Fensterkreuz gedrückt
    bool bFkey[12 + 1];             // F1 ... F12
    uint32_t uLastFkeyTime[12 + 1]; // "Tasten-Entprellung"
    bool bMouseInWindow;
} INPUTSTATES;


void ClearFkeys(void);
void GetFkeys(void);
int32_t InitInputStates(void);
void UpdateInputStates(void);
uint32_t GetKey(void);
uint32_t FilterBigFontKey(uint32_t uKey);
void WaitNoSpecialKey(SDL_Scancode ScanCode);
void WaitNoKey(void);

int32_t GetScancodeIndex(void);
int32_t GetSdlScanCodeNameIndex(uint32_t uSDL_ScanCode);
#endif // KEYBOARDMOUSE_H_INCLUDED
