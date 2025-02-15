#ifndef EMERALDMINEMAINMENUE_H_INCLUDED
#define EMERALDMINEMAINMENUE_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "levelconverter.h"
#include "loadlevel.h"
#include "mySDL.h"

#define EMERALD_STEEL_ARROW_DOWN_PRESSED    1
#define EMERALD_STEEL_ARROW_UP_PRESSED      2
#define EMERALD_STEEL_SETTINGS_PRESSED      3
#define EMERALD_STEEL_EXIT_PRESSED          4

#define MAX_NAMES_IN_LIST                   4
#define MIN_LEVELGROUPS_IN_LIST             4
// Farbige Rahmen für Gamecontroller und Joystick im Settingsmenü
#define COLOR_REC_1_W                       224
#define COLOR_REC_1_H                       128
#define COLOR_REC_2_W                       480
#define COLOR_REC_2_H                       160

#define MAX_STAR_POINTS                     10

#define WINDOW_CENTER_DEFAULT               true
#define WINDOW_FULL_SIZE                    false


typedef struct {
    uint32_t uXdim;
    uint32_t uYdim;
    bool     bCenterDefault;
    uint16_t *pMenuScreen;
    uint32_t uMaxLevelgroupsInList;                     // Maximale Anzahl der zu sehenden Levelgruppen in der Liste, >= 4
    uint8_t  *pLevelgroupList;                          // Anzahl der Levelelemente ist abhängig von Y-Auflösung, minimal sind  4 Elemente zu sehen
    uint8_t  uNamesList[MAX_NAMES_IN_LIST];             // Es sind maximal 4 Elemente zu sehen
    uint32_t uMaxLevelTitlesInList;
    uint16_t *puLevelTitleList;
    uint16_t *puLevelTitleListCopy;
    uint16_t uImportFileListDc3[EMERALD_MAX_MAXIMPORTFILES_IN_LIST];
    uint8_t  uFlashBrightness[20];
    char     szTempName[EMERALD_PLAYERNAME_LEN + 1];
    uint32_t uCursorPos;
    int      nState;            // 0 = Hauptmenü, 1 = Namenseingabe
    uint32_t uMaxFlashIndex;
    uint32_t uFlashIndex;
    float    fSettingsbuttonAngle;
} MAINMENU;

void PreInitMainMenu(void);
int InitMainMenu(bool bCenterDefault);
int GetMainMenuButton(void);
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
int SetMenuText(uint16_t *pMenuScreen, char *pszText, int nXpos, int nYpos, uint32_t uFont);
void SetMenuBorderAndClear(void);
void SetStaticMenuElements(void);
int SetDynamicMenuElements(void);
uint32_t GetNamesBeamPosition(void);
uint32_t GetLevelgroupBeamPosition(void);
void ActivateInputPlayernameMode(void);
void InitLists(void);
int EvaluateGame(int *pnNewHighscoreIndex, uint32_t *puLevelPlayed);
uint16_t GetFontElementByChar(char sign, uint32_t uFont);
uint32_t GetRainbowColors(uint16_t uWert);
int RenderSettingsbutton(SDL_Renderer *pRenderer);
int SettingsMenu(SDL_Renderer *pRenderer);
void RotateColors(RGBCOLOR *pColors, int nCount, int nCycles);
void CleanUpMemoryAndSDL(SDL_Renderer *pRenderer);
int ShowControllersAndJoysticks(SDL_Renderer *pRenderer);
int ShowRec(SDL_Renderer *pRenderer, RGBCOLOR *pColors, int nXpos, int nYpos, int nWidth, int nHeight, int nDimm);
void PrepareSettingsMenu(void);
int GetSettingsMenuButton(void);
int GetSettingsMenuKeyboardButton(void);
int GetSettingsMenuAxisButton(void);
#endif // EMERALDMINEMAINMENUE_H_INCLUDED
