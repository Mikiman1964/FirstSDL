#ifndef EMERALDMINEMAINMENUE_H_INCLUDED
#define EMERALDMINEMAINMENUE_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "levelconverter.h"
#include "loadlevel.h"
#include "mySDL.h"

// Rückgabewerte für GetMainMenuButton()
#define GFX_BUTTON_NONE                     0
#define GFX_BUTTON_LEVEL_DOWN               1
#define GFX_BUTTON_LEVEL_UP                 2
#define GFX_BUTTON_SETTINGS                 3
#define GFX_BUTTON_EXIT                     4
#define GFX_BUTTON_PLAYER_DOWN              5
#define GFX_BUTTON_PLAYER_UP                6
#define GFX_BUTTON_LEVELGROUP_DOWN          7
#define GFX_BUTTON_LEVELGROUP_UP            8

#define GFX_BUTTON_ARROW_NONE               0
#define GFX_BUTTON_ARROW_DOWN               1
#define GFX_BUTTON_ARROW_UP                 2

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
    uint16_t uImportFileList[IMPORT_MAX_FILES_IN_LIST];
    uint8_t  uFlashBrightness[20];
    char     szTempName[EMERALD_PLAYERNAME_LEN + 1];
    uint32_t uCursorPos;
    int32_t  nState;            // 0 = Hauptmenü, 1 = Namenseingabe
    uint32_t uMaxFlashIndex;
    uint32_t uFlashIndex;
    float    fSettingsbuttonAngle;
} MAINMENU;

void PreInitMainMenu(void);
int32_t InitMainMenu(bool bCenterDefault);
int32_t GetMainMenuButton(void);
int32_t EmeraldMineMainMenu(void);
int32_t ShowAuthorAndLevelname(uint32_t uLevel);
int32_t DimmMainMenu(bool bDimmUp);
int32_t ShowHighScores(uint32_t uLevel, int32_t nNewHighScoreIndex);
int32_t RenderMenuElements(void);
int32_t MenuSelectLevelgroup(void);
int32_t MenuSelectName(void);
void ScrollLevelGroups(int32_t nButton);
void ScrollPlayernames(int32_t nButton);
int32_t SetMenuText(uint16_t *pMenuScreen, char *pszText, int32_t nXpos, int32_t nYpos, uint32_t uFont);
void SetMenuBorderAndClear(void);
void SetStaticMenuElements(void);
int32_t SetDynamicMenuElements(void);
uint32_t GetNamesBeamPosition(void);
uint32_t GetLevelgroupBeamPosition(void);
void ActivateInputPlayernameMode(void);
void InitLists(void);
int32_t EvaluateGame(int32_t *pnNewHighscoreIndex, uint32_t *puLevelPlayed);
uint16_t GetFontElementByChar(char sign, uint32_t uFont);
int32_t RenderSettingsbutton(void);
int32_t SettingsMenu(void);
void RotateColors(RGBCOLOR *pColors, int32_t nCount, int32_t nCycles);
void CleanUpMemoryAndSDL(void);
int32_t ShowControllersAndJoysticks(void);
int32_t ShowRec(RGBCOLOR *pColors, int32_t nXpos, int32_t nYpos, int32_t nWidth, int32_t nHeight, int32_t nDimm);
void PrepareSettingsMenu(void);
int32_t GetSettingsMenuButton(void);
int32_t GetSettingsMenuKeyboardButton(void);
int32_t GetSettingsMenuAxisButton(void);
#endif // EMERALDMINEMAINMENUE_H_INCLUDED
