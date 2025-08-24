#ifndef QUICKSAVELOADGAME_H_INCLUDED
#define QUICKSAVELOADGAME_H_INCLUDED

#include "EmeraldMine.h"
#include "loadlevel.h"


typedef struct {
    uint16_t        *pLevel;
    uint32_t        *pStatusAnimation;
    uint32_t        *pLastStatusAnimation;                      // Wird für Explosionen benötigt, damit entsprechende Felder auf Space gesetzt werden können
    uint16_t        *pInvalidElement;                           // Damit ungültiges Feld später auf richtiges Element gesetzt werden kann
    uint16_t        *pSlimeElement;                             // Element, welches der Schleim gefressen hat
    uint8_t         *pLastYamSlimeDirection;
    bool            bReadyToGo;                                 // Man hat genug Diamanten eingesammelt
    bool            bHasRedKey;
    bool            bHasGreenKey;
    bool            bHasBlueKey;
    bool            bHasYellowKey;
    bool            bHasGeneralKey;
    bool            bWheelRunning;
    bool            bMagicWallRunning;
    bool            bMagicWallWasOn;                            // Magic Wall war mal an
    bool            bLightOn;                                   // Licht ist an -> unsichtbare Mauern/Stahl sind sichtbar
    bool            bPushStone;                                 // wenn true, kann Man sofort (Stone, Nut, Bomb) schieben
    bool            bManDead;
    bool            bWellDone;                                  // Level wurde geschafft
    bool            bManProtected;                              // Man ist in Röhre geschützt
    bool            bLightBarrierRedOn;
    bool            bLightBarrierGreenOn;
    bool            bLightBarrierBlueOn;
    bool            bLightBarrierYellowOn;
    bool            bTimeDoorOpen;                              // Für Zeit-Tür
    bool            bSwitchDoorState;                           // Für Schaltertür, true = umgeschaltet
    bool            bSwitchDoorImpluse;                         // Für Schaltertür
    bool            bReplicatorRedOn;
    bool            bReplicatorGreenOn;
    bool            bReplicatorBlueOn;
    bool            bReplicatorYellowOn;
    uint8_t         uConveybeltRedState;                        // CONVEYBELT_OFF, CONVEYBELT_LEFT, CONVEYBELT_RIGHT
    uint8_t         uConveybeltRedDirection;                    // EMERALD_CONVEYBELT_TO_LEFT, EMERALD_CONVEYBELT_TO_RIGHT
    uint8_t         uConveybeltGreenState;
    uint8_t         uConveybeltGreenDirection;
    uint8_t         uConveybeltBlueState;
    uint8_t         uConveybeltBlueDirection;
    uint8_t         uConveybeltYellowState;
    uint8_t         uConveybeltYellowDirection;
    char            szMd5String[EMERALD_MD5_STRING_LEN + 1];    // MD5-Prüfsumme als String für komprimierte Leveldaten, d.h. ohne Header
    uint32_t        uEmeraldsToCollect;
    uint32_t        uTimeToPlay;
    uint32_t        uTimeWheelRotationLeft;                     // Verbleibende Zeit für Wheel-Rotation
    uint32_t        uTimeDoorTimeLeft;
    uint32_t        uShieldCoinTimeLeft;                        // Verbleibende Zeit für Schild
    uint32_t        uWheelRunningXpos;                          // nur gültig, wenn bWheelRunning = true
    uint32_t        uWheelRunningYpos;                          // nur gültig, wenn bWheelRunning = true
    uint32_t        uTimeMagicWallLeft;                         // Verbleibende Zeit für Magic-Wall
    uint32_t        uTimeLightLeft;                             // Verbleibende Zeit für Licht
    uint32_t        uDynamiteCount;                             // Anzahl Dynamits, die der Man hat
    uint32_t        uHammerCount;                               // Anzahl Hammer, die der Man hat
    uint32_t        uWhiteKeyCount;                             // Anzahl der weißen Schlüssel, die der Man hat
    uint32_t        uManXpos;                                   // X-Element-Position des Man
    uint32_t        uManYpos;                                   // Y-Element-Position des Man
    uint32_t        uYamExplosion;                              // Aktuelle YAM-Explosion
    uint32_t        uDynamitePos;                               // lineare Koordinate des manuell gezündeten Dynamits durch den Man, 0xFFFFFFFF = keine Zündung
    uint32_t        uDynamiteStatusAnim;                        // Status/Animation für manuell gezündetes Dynamit
    uint32_t        uPlayTimeStart;                             // Zeitpunkt, wann Level gestartet wurde
    uint32_t        uPlayTimeEnd;                               // Zeitpunkt, wann Level beendet wurde
    uint32_t        uTotalScore;
} SAVEGAME;


void InitSaveGame(void);                                        // Struktur Savegame.x initialisieren
int32_t QuickSaveGame(void);                                    // Aktuelles Spiel speichern
int32_t QuickLoadGame(void);                                    // Aktuelles Spiel mit gespeichertem Spiel überschreiben
bool IsQuickLoadGameAvailable(void);                            // Steht gespeichertes Game zur Verfügung?
void FreeSaveGame(void);                                        // Gespeichertes Savegame wieder freigeben

#endif // QUICKSAVELOADGAME_H_INCLUDED

