#ifndef TESTSURFACE_H_INCLUDED
#define TESTSURFACE_H_INCLUDED

#include <SDL2/SDL.h>

#define MIN_LEVEL_W     4       // Minumum 4 Elemente in der Breite
#define MIN_LEVEL_H     3       // Minumum 3 Elemente in der Höhe

#define MAX_LEVEL_W     500     // Maximum Elemente in der Breite
#define MAX_LEVEL_H     500     // Maximum Elemente in der Höhe

#define PANEL_H         32      // Spiel-Anzeige (Time, Score usw.)

// Level-Elemente
#define EMERALD_ALIEN                           0x04            // Alien
#define EMERALD_MINE_UP                         0x08            // Mine hoch
#define EMERALD_MINE_RIGHT                      0x09            // Mine rechts
#define EMERALD_MINE_DOWN                       0x0A            // Mine runter
#define EMERALD_MINE_LEFT                       0x0B            // Mine links
#define EMERALD_BEETLE_UP                       0x14            // Käfer hoch
#define EMERALD_BEETLE_RIGHT                    0x15            // Käfer rechts
#define EMERALD_BEETLE_DOWN                     0x16            // Käfer runter
#define EMERALD_BEETLE_LEFT                     0x17            // Käfer links
#define EMERALD_REPLICATOR_RED_TOP_LEFT         0x18        	// Replikator rot, aus, oben links
#define EMERALD_REPLICATOR_RED_TOP_MID          0x19        	// Replikator rot, aus, oben mitte
#define EMERALD_REPLICATOR_RED_TOP_RIGHT        0x1A        	// Replikator rot, aus, oben rechts
#define EMERALD_REPLICATOR_RED_BUTTOM_LEFT      0x1B        	// Replikator rot, aus, unten links
#define EMERALD_REPLICATOR_RED_BUTTOM_RIGHT     0x1C        	// Replikator rot, aus, unten rechts
#define EMERALD_REPLICATOR_RED_SWITCH           0x1D        	// Schalter für roten Replikator
#define EMERALD_MAN                             0x40            // Man
#define EMERALD_SPACE                           0x80            // Space
#define EMERALD_SAND                            0x82            // Sand
#define EMERALD_HARDWALL                        0x83            // harte (Außen)-Mauer
#define EMERALD_KEY_RED                         0x85            // roter Schlüssel
#define EMERALD_KEY_YELLOW                      0x86            // gelber Schlüssel
#define EMERALD_KEY_BLUE                        0x87            // blauer Schlüssel
#define EMERALD_KEY_GREEN                       0x88            // grüner Schlüssel
#define EMERALD_DOOR_RED                        0x89            // rote Tür
#define EMERALD_DOOR_YELLOW                     0x8A            // gelbe Tür
#define EMERALD_DOOR_BLUE                       0x8B            // blaue Tür
#define EMERALD_DOOR_GREEN                      0x8C            // grüne Tür
#define EMERALD_WHEEL                           0x93            // Rad
#define EMERALD_INVALID                         0xFF            // ungültiges Element

#define EMERALD_SCROLL_LEVEL_STAND              0x00000000      // keine Richtung, Stand
#define EMERALD_SCROLL_LEVEL_UP                 0x00000001      // nach oben
#define EMERALD_SCROLL_LEVEL_RIGHT              0x00000002      // nach rechts
#define EMERALD_SCROLL_LEVEL_DOWN               0x00000003      // nach unten
#define EMERALD_SCROLL_LEVEL_LEFT               0x00000004      // nach links

// Animationen
#define EMERALD_ANIM_STAND                      0x00000000      // keine Animation
#define EMERALD_ANIM_UP                         0x00000100      // hoch
#define EMERALD_ANIM_RIGHT                      0x00000200      // rechts
#define EMERALD_ANIM_DOWN                       0x00000300      // runter
#define EMERALD_ANIM_LEFT                       0x00000400      // links
#define EMERALD_ANIM_SPIN_UP_TO_RIGHT           0x00000500      // von oben nach rechts drehen
#define EMERALD_ANIM_SPIN_RIGHT_TO_DOWN         0x00000600      // von rechts nach unten drehen
#define EMERALD_ANIM_SPIN_DOWN_TO_LEFT          0x00000700      // von unten nach links drehen
#define EMERALD_ANIM_SPIN_LEFT_TO_UP            0x00000800      // von links nach oben drehen
#define EMERALD_ANIM_SPIN_UP_TO_LEFT            0x00000900      // von oben nach links drehen
#define EMERALD_ANIM_SPIN_LEFT_TO_DOWN          0x00000A00      // von links nach unten drehen
#define EMERALD_ANIM_SPIN_DOWN_TO_RIGHT         0x00000B00      // von unten nach rechts drehen
#define EMERALD_ANIM_SPIN_RIGHT_TO_UP           0x00000C00      // von rechts nach oben drehen
#define EMERALD_ANIM_UP_DOUBLESPEED             0x00000D00      // hoch, doppelte Geschwindigkeit
#define EMERALD_ANIM_RIGHT_DOUBLESPEED          0x00000E00      // rechts, doppelte Geschwindigkeit
#define EMERALD_ANIM_DOWN_DOUBLESPEED           0x00000F00      // runter, doppelte Geschwindigkeit
#define EMERALD_ANIM_LEFT_DOUBLESPEED           0x00001000      // links, doppelte Geschwindigkeit

#define EMERALD_ANIM_CLEAN_UP                   0x00010000      // über dem invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_RIGHT                0x00020000      // rechts neben invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_DOWN                 0x00030000      // unter dem invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_LEFT                 0x00040000      // links neben invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_UP_DOUBLESPEED       0x00050000      // 2 Felder über dem invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_RIGHT_DOUBLESPEED    0x00060000      // 2 Felder rechts neben invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_DOWN_DOUBLESPEED     0x00070000      // 2 Felder unter dem invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_LEFT_DOUBLESPEED     0x00080000      // 2 Felder links neben invaliden Feld löschen


// "Selbststeuernde" Zustände                                   // Diese Zustände müssen vom Element selbst zurückgesetzt werden
#define EMERALD_NO_ADDITIONAL_ANIMSTATUS        0x00000000      // kein zusätzlicher Status
#define EMERALD_ANIM_LOST_GUIDE                 0x01000000      // Mine oder Käfer haben Führung verloren
#define EMERALD_ANIM_ALIEN_MOVED                0x02000000      // Alien hat sich im letzten Schritt bewegt -> soll keine Doppelbewegungen machen
#define EMERALD_ANIM_WHEEL_RUN                  0x03000000      // Rad soll bewegt werden
#define EMERALD_ANIM_BORN1                      0x04000000      // Replikator erzeugt neues Element über
#define EMERALD_ANIM_BORN2                      0x05000000      // 2 Steuerungsphasen -> halbe Geschwindigekit
#define EMERALD_ANIM_KEY_RED_SHRINK             0x06000000      // roter Schlüssel wird keiner
#define EMERALD_ANIM_KEY_GREEN_SHRINK           0x07000000      // grüner Schlüssel wird keiner
#define EMERALD_ANIM_KEY_BLUE_SHRINK            0x08000000      // blauer Schlüssel wird keiner
#define EMERALD_ANIM_KEY_YELLOW_SHRINK          0x09000000      // gelber Schlüssel wird keiner
#define EMERALD_ANIM_SAND_SHRINK                0x0A000000      // Sand verschwindet



#define EMERALD_STANDARD_SPEED                  false
#define EMERALD_STANDARD_DOUBLESPEED            true

#define EMERALD_MAX_HEADER_SIZE                 512             // memmem begrenzen

typedef struct {
    bool            bInitOK;
    bool            bHasRedKey;
    bool            bHasGreenKey;
    bool            bHasBlueKey;
    bool            bHasYellowKey;
    bool            bReplicatorRedOn;
    bool            bReplicatorGreenOn;
    bool            bReplicatorBlueOn;
    bool            bReplicatorYellowOn;
    char            szVersion[6];           // z.B. "01.00"
    char            szLevelTitle[32 + 1];   // z.B. "Der Bunker"
    char            szLevelAuthor[32 + 1];  // z.B. "Mikiman"
    uint32_t        uScoreEmerald;
    uint32_t        uScoreSaphir;
    uint32_t        uScoreKey;
    uint32_t        uScoreDynamite;
    uint32_t        uScoreNutCracking;
    uint32_t        uScoreStoningBeetle;
    uint32_t        uScoreStoningMine;
    uint32_t        uScoreStoningAlien;
    uint32_t        uScoreStoningYam;
    uint32_t        uEmeraldsToCollect;
    uint32_t        uTimeScoreFactor;
    uint32_t        uCheeseSpreadSpeed;
    uint32_t        uTimeToPlay;
    uint32_t        uTimeWheelRotation;
    uint32_t        uTimeWheelRotationLeft; // Verbleibende Zeit für Wheel-Rotation
    bool            bWheelRunning;
    uint32_t        uWheelRunningXpos;      // nur gültig, wenn bWheelRunning = true
    uint32_t        uWheelRunningYpos;      // nur gültig, wenn bWheelRunning = true
    uint32_t        uTimeBlueWall;
    uint8_t         *pLevel;
    uint32_t        uLevel_X_Dimension;
    uint32_t        uLevel_Y_Dimension;
    uint32_t        *pStatusAnimation;
    int             nXpos;                  // aktuelle X-Pixel-Position
    int             nYpos;                  // aktuelle Y-Pixel-Position
    uint32_t        uTotalScore;
    uint32_t        uVisibleX;              // Anzahl der sichtbaren Elemente-Spalten
    uint32_t        uVisibleY;              // Anzahl der sichtbaren Elemente-Zeilen (ohne unteres Panel)
    uint32_t        uVisibleCenterX;        // Zentrum X des sichtbaren Bereichs (ohne unteres Panel)
    uint32_t        uVisibleCenterY;        // Zentrum X des sichtbaren Bereichs (ohne unteres Panel)
    uint32_t        uManXpos;               // X-Element-Position des Man
    uint32_t        uManYpos;               // Y-Element-Position des Man
    uint32_t        uFrameCounter;          // Bildzähler
} PLAYFIELD;


uint32_t ControlGame(uint32_t uDirection);
int RenderLevel(SDL_Renderer *pRenderer, int *pnXpos, int *pnYpos, int nAnimationCount);
int ShowPanel(SDL_Renderer *pRenderer);
int WritePanelText(SDL_Renderer *pRenderer, const char *szText, int nXpos, int nYpos, int nFontSize, bool bGreenNumbers);
uint32_t GetTextureIndexByElement(uint32_t uElement,int nAnimationCount);
void ScrollAndCenterLevel(uint32_t uManDirection);
void CheckPlayTime(void);
int RunGame(SDL_Window *pWindow, SDL_Renderer *pRenderer);


#endif // TESTSURFACE_H_INCLUDED
