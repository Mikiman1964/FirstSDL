#ifndef BUTTONS_CHECKBOXES_H_INCLUDED
#define BUTTONS_CHECKBOXES_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include "mySDL.h"

#define MAX_BUTTONS             100
#define MAX_BUTTONTEXT_LEN      100

#define MAX_CHECKBOXES          100
#define MAX_CHECKBOXTEXT_LEN    100

#define CHK_USE                 true
#define CHK_UNUSE               false

typedef struct {
    bool        bActive;
    bool        bPressed;
    bool        bWaitRelease;
    char        *pszLabel;
    char        *pszText;
    uint32_t    uXpos;
    uint32_t    uYpos;
} BUTTON;


typedef struct {
    int32_t     nRegisterIndex;
    bool        bUse;
    bool        bActive;
    bool        bPressed;
    bool        bChanged;
    bool        bRadio;
    char        szText[MAX_CHECKBOXTEXT_LEN + 1];
    int32_t     nXpos;
    int32_t     nYpos;
} CHECKBOX;


void InitCheckboxes(void);
int32_t RegisterCheckbox(CHECKBOX *pCheckbox, bool bActive, char *pszText, int32_t nXpos, int32_t nYpos, bool bRadio, bool bUse);
int32_t DeRegisterCheckbox(CHECKBOX *pCheckbox);
int32_t ShowCheckboxes(SDL_Renderer *pRenderer, int32_t nDimm, bool bAbsolute);

void InitButtons(void);
void FreeButton(char *pszLabel);
void FreeAllButtons(void);
int32_t GetButtonIndex(char *pszLabel);
int32_t GetFreeButtonIndex(void);
int32_t CreateButton(char *pszLabel, char *pszText,uint32_t uXpos, int32_t uYpos, bool bActive, bool bWaitRelease);
int32_t SetButtonActivity(char *pszLabel, bool bActive);
int32_t SetButtonPosition(char *pszLabel, uint32_t uXpos, uint32_t uYpos);
int32_t ShowButtons(SDL_Renderer *pRenderer, bool bAbsolute);
int32_t ShowOtherButtons(SDL_Renderer *pRenderer, bool bAbsolute);
bool IsButtonPressed(char *pszLabel);
#endif // BUTTONS_CHECKBOXES_H_INCLUDED
