#ifndef EDITOR_H_INCLUDED
#define EDITOR_H_INCLUDED

#include "mySDL.h"

#define EDITORPANEL_X               WINDOW_W - EDITORPANEL_W
#define EDITORPANEL_Y               0

#define EDITORPANEL_W               192
#define EDITORPANEL_H               WINDOW_H

#define MAX_PANEL_ELEMENTS          224 // (8 * 32)

#define MENUSTATE_LEVEL             0
#define MENUSTATE_YAMS              1
#define MENUSTATE_MACHINES          2
#define MENUSTATE_TIME_AND_SCORES   3


#define MENUSTATE_MAX               (MENUSTATE_TIME_AND_SCORES + 1)

typedef struct {
    uint32_t uXstart;
    uint32_t uXend;
    uint32_t uYstart;
    uint32_t uYend;
} YAM_COORDS;


typedef struct {
    // Editor
    bool            bEditorRun;
    bool            bFoundError;
    bool            bHalfSize;
    uint32_t        uVisibleY;
    uint32_t        uVisibleX;
    uint32_t        uVisibleCenterY;
    uint32_t        uVisibleCenterX;
    uint32_t        uShiftLevelXpix;                            // Anzahl Pixel, die das Level nach rechts geshiftet wird, da es kleiner bzw. gleich der Anzeigebreite ist
    uint32_t        uShiftLevelYpix;                            // Anzahl Pixel, die das Level nach unten geshiftet wird, da es kleiner bzw. gleich der Anzeigehöhe ist
    uint32_t        uUpperLeftLevelIndex;                       // oberstes linkes Element, welches entweder komplett oder dessen untere rechte Ecke noch gerade sichtbar ist
    uint16_t        uSelectedElementLeft[MENUSTATE_MAX];        // Ausgewähltes Element, linke Maustaste
    uint16_t        uSelectedElementMiddle[MENUSTATE_MAX];      // Ausgewähltes Element, mittlere Maustaste
    uint16_t        uSelectedElementRight[MENUSTATE_MAX];       // Ausgewähltes Element, rechte Maustaste
    uint32_t        uScrollPixelX;                              // Scrollt bei Tastendruck x Pixel horizontal
    uint32_t        uScrollPixelFastX;                          // Scrollt bei Tastendruck x Pixel horizontal in hoher Geschwindigkeit
    uint32_t        uScrollPixelY;                              // Scrollt bei Tastendruck x Pixel vertikal
    uint32_t        uScrollPixelFastY;                          // Scrollt bei Tastendruck x Pixel vertikal in hoher Geschwindigkeit
    uint32_t        uFont_W;
    uint32_t        uFont_H;
    int             nMaxXpos;                                   // Für maximale X-Positionierung
    int             nMaxYpos;                                   // Für maximale Y-Positionierung
    int             nXpos;                                      // Aktuelle X-Positionierung
    int             nYpos;                                      // Aktuelle Y-Positionierung
    int             nMinXLevel;                                 // Minimale X-Position innerhalb des Levela
    int             nMaxXLevel;                                 // Maximale X-Position innerhalb des Levela
    int             nMinYLevel;                                 // Minimale Y-Position innerhalb des Levela
    int             nMaxYLevel;                                 // Maximale Y-Position innerhalb des Levela
    uint32_t        uFrameCounter;                              // Bildzähler
    uint32_t        uMenuState;                                 // Menü-Status, siehe oben
    // Leveldaten
    uint16_t        *pLevel;
    uint32_t        uLevel_X_Dimension;
    uint32_t        uLevel_Y_Dimension;
    bool            bLightBarrierRedOn;
    bool            bLightBarrierGreenOn;
    bool            bLightBarrierBlueOn;
    bool            bLightBarrierYellowOn;
    bool            bReplicatorRedOn;
    uint8_t         uReplicatorRedObject;
    bool            bReplicatorGreenOn;
    uint8_t         uReplicatorGreenObject;
    bool            bReplicatorBlueOn;
    uint8_t         uReplicatorBlueObject;
    bool            bReplicatorYellowOn;
    uint8_t         uReplicatorYellowObject;
    char            szLevelTitle[EMERALD_TITLE_LEN + 1];        // z.B. "Der Bunker"
    char            szLevelAuthor[EMERALD_AUTHOR_LEN + 1];      // z.B. "Mikiman"
    char            szVersion[EMERALD_VERSION_LEN + 1];         // z.B. "01.00"
    char            *pMessage[EMERALD_MAX_MESSAGES];            // 8 Nachrichtentexte
    uint32_t        uScoreEmerald;
    uint32_t        uScoreRuby;
    uint32_t        uScoreSaphir;
    uint32_t        uScorePerl;
    uint32_t        uScoreCrystal;
    uint32_t        uScoreKey;
    uint32_t        uScoreDynamite;
    uint32_t        uScoreHammer;
    uint32_t        uScoreNutCracking;
    uint32_t        uScoreStoningBeetle;
    uint32_t        uScoreStoningMine;
    uint32_t        uScoreStoningAlien;
    uint32_t        uScoreStoningYam;
    uint32_t        uScoreTimeCoin;
    uint32_t        uScoreMessage;
    uint32_t        uEmeraldsToCollect;
    uint32_t        uTimeScoreFactor;
    uint32_t        uCheeseSpreadSpeed;
    uint32_t        uTimeToPlay;
    uint32_t        uAdditonalTimeCoinTime;                     // zusätzliche Zeit durch Zeitmünze
    uint32_t        uTimeWheelRotation;
    uint32_t        uTimeDoorTime;
    uint32_t        uTimeMagicWall;
    uint32_t        uTimeLight;
    uint32_t        uDynamiteCount;                             // Anzahl Dynamits, die der Man hat
    uint32_t        uHammerCount;                               // Anzahl Hammer, die der Man hat
    uint32_t        uWhiteKeyCount;                             // Anzahl der weißen Schlüssel, die der Man hat
    uint32_t        uMaxYamExplosionIndex;                      // Max. Yam-Explosions-Index in diesem Level
    YAMEXPLOSION    YamExplosions[EMERALD_MAX_YAM_EXPLOSIONS];
    YAM_COORDS      YamCoords[EMERALD_MAX_YAM_EXPLOSIONS];      // Koordinaten für Kästchen des YAM-Editors
} ED;


void SetPanelElements(uint32_t uMenuState);
void ShowEditorStatus(void);
void ClearOldMan(void);
int InitEditor(bool bNewLevel, uint32_t uXdim, uint32_t uYdim, char *pszFilename);
int CopyPlayfieldValueToEditor(void);
void SetLevelBorder(void);
int Editor(SDL_Window *pWindow, SDL_Renderer *pRenderer);
int DrawEditorPanel(SDL_Renderer *pRenderer);
int FillPanel(SDL_Renderer *pRenderer);
uint16_t GetElementByMouseposition(int nMouseXpos, int nMouseYpos);
int RenderEditorLevel(SDL_Renderer *pRenderer, int *pnXpos, int *pnYpos, int nAnimationCount);
int WriteLevelXmlFile(const char *szFilename);
void FillLevelArea(int x,int y,uint16_t uFillElement,uint16_t uGroundElement);
int EditorStateLevel(SDL_Renderer *pRenderer);
int EditorStateYams(SDL_Renderer *pRenderer);
int EditorStateMachines(SDL_Renderer *pRenderer);
int EditorStateTimeAndScores(SDL_Renderer *pRenderer);
void PreCalcYamCoords(void);
 int GetYamExplosionFromMousepointer(uint8_t *puElementIndex);
#endif // EDITOR_H_INCLUDED
