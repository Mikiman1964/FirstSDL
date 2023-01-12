#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include "mySDL.h"

#define MAX_BUTTONS             10
#define MAX_BUTTONTEXT_LEN      100

// Die Buttons müssen in dieser Reihenfolge mit CreateButton() angelegt werden, ansonsten stimmen die defines hier nicht
#define BUTTON_SAVE_AND_PLAY    0
#define BUTTON_YAMS             1
#define BUTTON_RETURN_TO_LEVEL  2
#define BUTTON_MINUS            3
#define BUTTON_PLUS             4
#define BUTTON_MACHINES         5
#define BUTTON_TIME_AND_SCORE   6

typedef struct {
    bool bActive;
    bool bPressed;
    uint32_t uXpos;
    uint32_t uYpos;
    char *pszText;
} BUTTON;


typedef struct {
    uint32_t uButtons;
    BUTTON Button[MAX_BUTTONS];
} BUTTONS;


void InitButtons(void);
void FreeButtons(void);
int CreateButton(char *pszText,uint32_t uXpos, int32_t uYpos, bool bActive);
int SetButtonActivity(uint32_t uButton, bool bActive);
int SetButtonPosition(uint32_t uButton, uint32_t uXpos, uint32_t uYpos);
int ShowButtons(SDL_Renderer *pRenderer);
int ShowOtherButtons(SDL_Renderer *pRenderer);
#endif // BUTTONS_H_INCLUDED
