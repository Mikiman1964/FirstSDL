#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include "mySDL.h"

#define MAX_BUTTONS             100
#define MAX_BUTTONTEXT_LEN      100

#define MAX_CHECKBOXES          100
#define MAX_CHECKBOXTEXT_LEN    100

typedef struct {
    bool      bActive;
    bool      bPressed;
    bool      bWaitRelease;
    char      *pszLabel;
    char      *pszText;
    uint32_t  uXpos;
    uint32_t  uYpos;
} BUTTON;


typedef struct {
    int     nRegisterIndex;
    bool    bActive;
    bool    bPressed;
    bool    bChanged;
    bool    bRadio;
    char    szText[MAX_CHECKBOXTEXT_LEN + 1];
    int     nXpos;
    int     nYpos;
} CHECKBOX;


void InitCheckboxes(void);
int RegisterCheckbox(CHECKBOX *pCheckbox, bool bActive, char *pszText, int nXpos, int nYpos, bool bRadio);
int DeRegisterCheckbox(CHECKBOX *pCheckbox);
int ShowCheckboxes(SDL_Renderer *pRenderer, int nDimm);

void InitButtons(void);
void FreeButton(char *pszLabel);
void FreeAllButtons(void);
int GetButtonIndex(char *pszLabel);
int GetFreeButtonIndex(void);
int CreateButton(char *pszLabel, char *pszText,uint32_t uXpos, int32_t uYpos, bool bActive, bool bWaitRelease);
int SetButtonActivity(char *pszLabel, bool bActive);
int SetButtonPosition(char *pszLabel, uint32_t uXpos, uint32_t uYpos);
int ShowButtons(SDL_Renderer *pRenderer);
int ShowOtherButtons(SDL_Renderer *pRenderer);
bool IsButtonPressed(char *pszLabel);
#endif // BUTTONS_H_INCLUDED