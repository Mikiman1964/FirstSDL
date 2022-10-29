#ifndef KEYBOARDMOUSE_H_INCLUDED
#define KEYBOARDMOUSE_H_INCLUDED

typedef struct {
    const Uint8 *pKeyboardArray;
    bool bLeftMouseButton;
    bool bRightMouseButton;
    int nMouseWheelY;               // Y-Mausrad, < 0 = Rad scroll down, > 0 = Rad scroll up
    int nMouseXpos;                 // Maus X-Position
    int nMouseYpos;                 // Maus Y-Position
    bool bQuit;                     // Fensterkreuz gedr�ckt
} INPUTSTATES;


int InitInputStates(void);
int UpdateInputStates(void);

#endif // KEYBOARDMOUSE_H_INCLUDED
