#ifndef EMERALDMINEMAINMENUE_H_INCLUDED
#define EMERALDMINEMAINMENUE_H_INCLUDED

#include <SDL2/SDL.h>
#include "loadlevel.h"
#include "mySDL.h"

#define EMERALD_STEEL_ARROW_DOWN_PRESSED    1
#define EMERALD_STEEL_ARROW_UP_PRESSED      2

#define MAX_NAMES_IN_LIST                   4
#define MAX_LEVELGROUPS_IN_LIST             4
#define MAX_LEVELTITLES_IN_LIST             31


typedef struct {
    uint32_t uXdim;
    uint32_t uYdim;
    uint32_t uXoffs;
    uint32_t uYoffs;
    uint16_t uMenuScreen[(DEFAULT_WINDOW_W / FONT_W) * (DEFAULT_WINDOW_H / FONT_H)];
    uint8_t  uLevelgroupList[MAX_LEVELGROUPS_IN_LIST];  // Es sind maximal 4 Elemente zu sehen
    uint8_t  uNamesList[MAX_NAMES_IN_LIST];             // Es sind maximal 4 Elemente zu sehen
    uint16_t uLevelTitleList[MAX_LEVELTITLES_IN_LIST];  // Es sind maximal 31 Elemente zu sehen
    uint8_t  uFlashBrightness[20];
    char     szTempName[EMERALD_PLAYERNAME_LEN + 1];
    uint32_t uCursorPos;
    int      nState;
    uint32_t uMaxFlashIndex;
    uint32_t uFlashIndex;
} MAINMENU;

void InitMainMenu(void);
int GetLevelButton(void);
int GetPlayerListButton(void);
int GetLevelgroupListButton(void);
int EmeraldMineMainMenu(SDL_Renderer *pRenderer);
int ShowAuthorAndLevelname(SDL_Renderer *pRenderer, uint32_t uLevel);
int DimmMainMenu(SDL_Renderer *pRenderer, bool bDimmUp);
int ShowHighScores(SDL_Renderer *pRenderer, uint32_t uLevel, int nNewHighScoreIndex);
int RenderMenuElements(SDL_Renderer *pRenderer);
int MenuSelectLevelgroup(SDL_Renderer *pRenderer);
int MenuSelectName(SDL_Renderer *pRenderer);
void ScrollLevelGroups(int nButton);
void ScrollPlayernames(int nButton);
int SetMenuText(uint16_t *pMenuScreen, char *pszText, int nXpos, int nYpos,uint32_t uFont);
void SetMenuBorderAndClear(void);
void SetStaticMenuElements(void);
int SetDynamicMenuElements(void);
uint32_t GetNamesBeamPosition(void);
uint32_t GetLevelgroupBeamPosition(void);
void ActivateInputPlayernameMode(void);
void InitLists(void);
int EvaluateGame(int *pnNewHighscoreIndex, uint32_t *puLevelPlayed);
uint16_t GetFontElementByChar(char sign,uint32_t uFont);
uint32_t GetRainbowColors(uint16_t uWert);

#endif // EMERALDMINEMAINMENUE_H_INCLUDED
