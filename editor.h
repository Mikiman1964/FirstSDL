#ifndef EDITOR_H_INCLUDED
#define EDITOR_H_INCLUDED

#include <stdbool.h>
#include "EmeraldMine.h"
#include "mySDL.h"
#include "mystd.h"

#define MIN_Y_RESOLUTION_FOR_LEVELEDITOR        960

#define MAX_PANEL_ELEMENTS                      264     // (8 * 33)
#define EDITOR_MEM_SIZE                         4096

#define MENUSTATE_LEVEL_STD                     0
#define MENUSTATE_LEVEL_TEXT                    1
#define MENUSTATE_YAMS                          2
#define MENUSTATE_YAMS_TEXT                     3
#define MENUSTATE_MACHINES                      4
#define MENUSTATE_TIME_AND_SCORES               5
#define MENUSTATE_CONFIRM_NEWLEVEL_DIMENSION    6
#define MENUSTATE_TIME_AND_SCORES_MESSAGE       7
#define MENUSTATE_TIME_AND_SCORES_EDIT_TITLE    8
#define MENUSTATE_TIME_AND_SCORES_EDIT_AUTHOR   9
#define MENUSTATE_MAX                           (MENUSTATE_TIME_AND_SCORES_EDIT_AUTHOR + 1)


#define BUTTONLABEL_EDITOR_SAVE      "EDITOR_LEVEL_SAVE"
#define BUTTONLABEL_EDITOR_QUIT      "EDITOR_QUIT"
#define BUTTONLABEL_EDITOR_YAMS      "EDITOR_YAMS"
#define BUTTONLABEL_RETURN_TO_LEVEL  "RETURN_TO_LEVEL"
#define BUTTONLABEL_EDITOR_YAM_MINUS "EDITOR_YAM_MINUS"
#define BUTTONLABEL_EDITOR_YAM_PLUS  "EDITOR_YAM_PLUS"
#define BUTTONLABEL_EDITOR_MACHINES  "EDITOR_MACHINES"
#define BUTTONLABEL_TIME_AND_SCORES  "TIME_AND_SCORES"
#define BUTTONLABEL_EDITOR_TEXT      "EDITOR_TEXT"
#define BUTTONLABEL_EDITOR_STD       "EDITOR_STD"
#define BUTTONLABEL_EDITOR_OPTION_1  "EDITOR_OPTION1"
#define BUTTONLABEL_EDITOR_OPTION_2  "EDITOR_OPTION2"
#define BUTTONLABEL_EDITOR_OPTION_3  "EDITOR_OPTION3"
#define BUTTONLABEL_SAVE_MESSAGE     "SAVE_MESSAGE"
#define BUTTONLABEL_CANCEL_MESSAGE   "CANCEL_MESSAGE"
#define BUTTONLABEL_CALL_GAME        "CALL_GAME"
#define BUTTONLABEL_CALL_DEMO        "CALL_DEMO"
#define BUTTONLABEL_CALL_QUIT        "CALL_QUIT"



typedef struct {
    int             nEditMessage;                               // -1 = ungültig, sonst 0 bis EMERALD_MAX_MESSAGES - 1
    uint32_t        uMessageLen;                                // Länge der aktuellen Nachricht
    char            szMessageEditorMem[EDITOR_MEM_SIZE];        // Speicher für Message-Editor.
    char            cCharAtCursor;                              // Zeichen, das für Cursor zwischengespeichert werden muss;
    uint32_t        uCursorPos;                                 // Cursorposition
    uint32_t        uCursorFlashSpeedFrames;                    // Nach X Frames wird Cursor umgeschaltet
    uint32_t        uFrameCounter;                              // Bildzähler
    uint32_t        uLastToggleCursorFrame;                     // Letztes Frame, bei dem Cursor umgeschaltet wurde
    bool            bCursor;                                    // Cursor statt Zeichen wird angezeigt
    bool            bInsert;                                    // Einfüge-Modus vs. Überschreib-Modus
} MESSAGEEDITOR;


typedef struct {
    uint32_t uXstart;
    uint32_t uXend;
    uint32_t uYstart;
    uint32_t uYend;
} YAM_COORDS;


typedef struct {
    uint8_t *pLevelXml;
    char szLevelTitle[EMERALD_TITLE_LEN + 1];
} CLIPBOARD;


typedef struct {
    // Editor
    bool            bEditorRun;
    bool            bFoundError;                                // Ein Levelfehler (Man fehlt, unvollständiger Replikator, unvollständiges Säurebecken) wurde gefunden
    bool            bHalfSize;
    uint32_t        uPanelXpos;                                 // X-Positionierung des Panels im Editor
    uint32_t        uPanelYpos;                                 // Y-Positionierung des Panels im Editor
    uint32_t        uPanelW;                                    // Breite des Panels im Editor
    uint32_t        uPanelH;                                    // Höhe des Panels im Editor
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
    MESSAGEEDITOR   MessageEditor;                              // Nachrichten-Editor
    // Leveldaten
    uint16_t        *pLevel;
    uint32_t        uLevel_X_Dimension;
    uint32_t        uLevel_Y_Dimension;
    uint32_t        uTmpLevel_X_Dimension;
    uint32_t        uTmpLevel_Y_Dimension;
    bool            bLightBarrierRedOn;
    bool            bLightBarrierGreenOn;
    bool            bLightBarrierBlueOn;
    bool            bLightBarrierYellowOn;
    bool            bReplicatorRedOn;
    uint16_t        uReplicatorRedObject;
    bool            bReplicatorGreenOn;
    uint16_t        uReplicatorGreenObject;
    bool            bReplicatorBlueOn;
    uint16_t        uReplicatorBlueObject;
    bool            bReplicatorYellowOn;
    uint16_t        uReplicatorYellowObject;
    uint8_t         uConveybeltRedState;                        // CONVEYBELT_OFF, CONVEYBELT_LEFT, CONVEYBELT_RIGHT
    uint8_t         uConveybeltRedDirection;                    // EMERALD_CONVEYBELT_TO_LEFT, EMERALD_CONVEYBELT_TO_RIGHT
    uint8_t         uConveybeltGreenState;
    uint8_t         uConveybeltGreenDirection;
    uint8_t         uConveybeltBlueState;
    uint8_t         uConveybeltBlueDirection;
    uint8_t         uConveybeltYellowState;
    uint8_t         uConveybeltYellowDirection;
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
    uint32_t        uGrassSpreadSpeed;
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


void InitClipboard(void);
int PutLevelToClipboard(int nDestLevelNumber);
void FreeClipboard(void);
void ShowClipboard(void);
int UpdateLevelgroupHash(uint8_t *pszInput, uint8_t *puCalculatedHash);
int UpdateCreateTimestamp(uint8_t *pszInput);
uint8_t *GetStartOrEndLevelTag(uint8_t *pszInput, uint8_t *pEndpointer, bool bStart);
int RenumLevelgroup(uint8_t *pszXml);
void SetPanelElements(uint32_t uMenuState);
void ShowEditorStatus(void);
void ClearOldMan(void);
int InitEditor(bool bNewLevel, uint32_t uXdim, uint32_t uYdim, int nLevel);
DYNSTRING *Editor(SDL_Renderer *pRenderer, int nLevel);
void InitMessageEditor(void);
void CalcEditorViewArea(void);
int CreateEditorButtons(void);
void FreeEditorButtons(void);
int CopyPlayfieldValueToEditor(void);
int SetLevelBorder(uint16_t *pLevel, bool bClear, bool bAlwaysSteel);
uint32_t GetLevelnameBeamPosition(void);
uint32_t GetImportFilesBeamPosition(void);
int MenuSelectLevelname(SDL_Renderer *pRenderer, uint32_t *puBeamPosition);
int ImportMenuSelectFile(SDL_Renderer *pRenderer, uint32_t *puBeamPosition);
void InitLevelTitleList(void);
int HandlePreEditorButtons(int nSelectedLevel);
void PreparePreEditormenu(void);
int PreEditorMenu(SDL_Renderer *pRenderer);
int DrawEditorPanel(SDL_Renderer *pRenderer);
int FillPanel(SDL_Renderer *pRenderer);
uint16_t GetElementByMouseposition(int nMouseXpos, int nMouseYpos);
int RenderEditorLevel(SDL_Renderer *pRenderer, int *pnXpos, int *pnYpos, int nAnimationCount);
DYNSTRING *GetLevelXmlFromEditor(void);
int EditorStateLevel(SDL_Renderer *pRenderer);
int EditorStateYams(SDL_Renderer *pRenderer);
int EditorStateMachines(SDL_Renderer *pRenderer);
int EditorStateTimeAndScores(SDL_Renderer *pRenderer);
int EditorStateConfirmNewLevelDimension(SDL_Renderer *pRenderer);
void PreCalcYamCoords(void);
int GetYamExplosionFromMousepointer(uint8_t *puElementIndex);
int GetTimeScoresSwitchfieldAndOffset(uint32_t uSwitch,uint32_t *puSwitchOffset);
void ChangeTimeScoresValue(int nSwitchField,uint32_t uSwitchOffset);
uint32_t IncreaseOrDecreaseTimeScoreValue(uint32_t uValue, uint32_t uSwitchOffset);
int CreateNewLevel(bool bCopyOldLeveldata);
int SaveNewMessage(void);
int ScrollLevelTitleList(int nButton);
#endif // EDITOR_H_INCLUDED
