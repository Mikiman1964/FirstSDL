#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include "mySDL.h"

#define MAX_BUTTONS             100
#define MAX_BUTTONTEXT_LEN      100

typedef struct {
    char *pszLabel;
    char *pszText;
    bool bActive;
    bool bPressed;
    uint32_t uXpos;
    uint32_t uYpos;
} BUTTON;


void InitButtons(void);
void FreeButton(char *pszLabel);
void FreeAllButtons(void);
int GetButtonIndex(char *pszLabel);
int GetFreeButtonIndex(void);
int CreateButton(char *pszLabel, char *pszText,uint32_t uXpos, int32_t uYpos, bool bActive);
int SetButtonActivity(char *pszLabel, bool bActive);
int SetButtonPosition(char *pszLabel, uint32_t uXpos, uint32_t uYpos);
int ShowButtons(SDL_Renderer *pRenderer);
int ShowOtherButtons(SDL_Renderer *pRenderer);
bool IsButtonPressed(char *pszLabel);
#endif // BUTTONS_H_INCLUDED
