#ifndef EMERALDMINEMAINMENUE_H_INCLUDED
#define EMERALDMINEMAINMENUE_H_INCLUDED

#include <SDL2/SDL.h>
#include "loadlevel.h"


typedef struct {
    uint32_t uXdim;
    uint32_t uYdim;
    uint32_t uXoffs;
    uint32_t uYoffs;
    uint16_t uMenuScreen[(DEFAULT_WINDOW_W / FONT_W) * (DEFAULT_WINDOW_H / FONT_H)];
    uint8_t  uLevelgroupList[4]; // Es sind maximal 4 Elemente zu sehen
    uint8_t  uNamesList[4];      // Es sind maximal 4 Elemente zu sehen
    uint8_t  uFlashBrightness[20];
    char     szTempName[EMERALD_PLAYERNAME_LEN + 1];
    uint32_t uCursorPos;
    int      nState;
    uint32_t uMaxFlashIndex;
    uint32_t uFlashIndex;
} MAINMENU;

void InitMainMenu(void);
int GetLevelButton(void);
int  EmeraldMineMainMenu(SDL_Renderer *pRenderer);
int ShowAuthorAndLevelname(SDL_Renderer *pRenderer, uint32_t uLevel);
int RenderMenuElements(SDL_Renderer *pRenderer);
int MenuSelectLevelgroup(SDL_Renderer *pRenderer);
int MenuSelectName(SDL_Renderer *pRenderer);
int DrawBeam(SDL_Renderer *pRenderer,uint32_t uXpos, uint32_t uYpos, uint32_t uWidth, uint32_t uHeight, uint8_t uRed, uint32_t uGreen, uint32_t uBlue, uint8_t uTransp);
void ScrollLevelGroups(void);
int SetMenuText(uint16_t *pMenuScreen, char *pszText, int nXpos, int nYpos,uint32_t uFont);
void SetMenuBorderAndClear(void);
void SetStaticMenuElements(void);
int SetDynamicMenuElements(void);
uint32_t GetNamesBeamPosition(void);
uint32_t GetLevelgroupBeamPosition(void);
void ActivateInputPlayernameMode(void);
void InitLists(void);
int EvaluateGame(void);
uint16_t GetFontElementByChar(char sign,uint32_t uFont);

#endif // EMERALDMINEMAINMENUE_H_INCLUDED
